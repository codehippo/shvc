#include "rpcclient.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>


struct rpcclient *rpcclient_connect(const struct rpcurl *url) {
	struct rpcclient *res;
	switch (url->protocol) {
		case RPC_PROTOCOL_TCP:
			res = rpcclient_stream_tcp_connect(url->location, url->port);
			break;
		case RPC_PROTOCOL_LOCAL_SOCKET:
			res = rpcclient_stream_unix_connect(url->location);
			break;
		case RPC_PROTOCOL_UDP:
			res = rpcclient_datagram_udp_connect(url->location, url->port);
			break;
		case RPC_PROTOCOL_SERIAL_PORT:
			res = rpcclient_serial_connect(url->location);
			break;
	};
	/*if (rpcclient_login(res, url->username, url->password, url->login_type))*/
		/*return res;*/
	rpcclient_disconnect(res);
	return NULL;
}


void rpcclient_disconnect(struct rpcclient *client) {
	client->disconnect(client);
}



bool rpcclient_next_message(struct rpcclient *client) {}

struct cpcp_item *rpcclient_next_item(struct rpcclient *client) {}



struct cpcp_pack_context *rpcclient_pack_context(struct rpcclient *client) {}
