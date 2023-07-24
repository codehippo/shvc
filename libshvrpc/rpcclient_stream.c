#include "rpcclient.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


struct rpcclient_stream {
	struct rpcclient c;
	int rfd, wfd;
	uint8_t packbuf[SHV_PACK_BUFSIZ];
	uint8_t unpackbuf[SHV_UNPACK_BUFSIZ];
	ssize_t msglen;
};


static void overflow_handler(struct cpcp_pack_context *pack, size_t size_hint) {
	struct rpcclient_stream *c = (struct rpcclient_stream *)(pack -
		offsetof(struct rpcclient_stream, c.pack));

	size_t to_send = pack->current - pack->start;
	char *ptr_data = pack->start;
	while (to_send > 0) {
		ssize_t i = write(c->wfd, ptr_data, to_send);
		if (i == -1) {
			if (errno == EINTR)
				continue;
			printf("Overflow error: %s\n", strerror(errno));
			pack->current = pack->end; /* signal error */
			return;
		}
		ptr_data += i;
		to_send -= i;
	}
	pack->current = pack->start;
}

static size_t underflow_handler(struct cpcp_unpack_context *unpack) {
	struct rpcclient_stream *c = (struct rpcclient_stream *)(unpack -
		offsetof(struct rpcclient_stream, c.unpack));

	c->msglen -= unpack->end - unpack->start;
	ssize_t i;
	do
		i = read(c->rfd, (void *)unpack->start,
			c->msglen >= 0 && c->msglen < SHV_UNPACK_BUFSIZ ? c->msglen
															: SHV_UNPACK_BUFSIZ);
	while (i == -1 && errno == EINTR);
	if (i < 0) {
		printf("Underflow error: %s\n", strerror(errno));
		return 0;
	}
	unpack->current = unpack->start;
	unpack->end = unpack->start + i;
	return i;
}

static void nextmsg(rpcclient_t client) {
	struct rpcclient_stream *sclient = (struct rpcclient_stream *)client;
	sclient->msglen = -1;
	sclient->msglen = chainpack_unpack_uint_data(&client->unpack, NULL);
	if (client->unpack.err_no != CPCP_RC_OK)
		return; // TODO error
}

static void disconnect(rpcclient_t client) {
	struct rpcclient_stream *sclient = (struct rpcclient_stream *)client;
	close(sclient->rfd);
	if (sclient->rfd != sclient->wfd)
		close(sclient->wfd);
	free(sclient);
}

rpcclient_t rpcclient_stream_new(int readfd, int writefd) {
	struct rpcclient_stream *res = malloc(sizeof *res);
	res->c.nextmsg = &nextmsg;
	res->c.disconnect = &disconnect;
	// TODO possibly just do not initialize pack context
	cpcp_pack_context_init(&res->c.pack, &res->packbuf, 0, overflow_handler);
	cpcp_unpack_context_init(&res->c.unpack, &res->unpackbuf, SHV_PACK_BUFSIZ,
		underflow_handler, NULL);
	res->rfd = readfd;
	res->wfd = writefd;
	res->msglen = 0;
	return &res->c;
}

rpcclient_t rpcclient_stream_tcp_connect(const char *location, int port) {
	int fd = socket(AF_INET6, SOCK_STREAM, 0);
	// TODO connect
	return rpcclient_stream_new(fd, fd);
}

rpcclient_t rpcclient_stream_unix_connect(const char *location) {
	int sock = socket(AF_UNIX, SOCK_STREAM, 0);

	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	const size_t addrlen =
		sizeof(struct sockaddr_un) - offsetof(struct sockaddr_un, sun_path) - 1;
	strncpy(addr.sun_path, location, addrlen);
	addr.sun_path[addrlen] = '\0';
	if (connect(sock, &addr, sizeof addr) == -1) {
		close(sock);
		return NULL;
	}

	return rpcclient_stream_new(sock, sock);
}
