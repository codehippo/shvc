#ifndef SHV_RPCCLIENT_H
#define SHV_RPCCLIENT_H

#include <stdbool.h>
#include <shv/chainpack.h>
#include <shv/rpcurl.h>

struct rpcclient;
typedef struct rpcclient *rpcclient_t;


rpcclient_t rpcclient_connect(const struct rpcurl *url);

rpcclient_t rpcclient_stream_new(int readfd, int writefd);
rpcclient_t rpcclient_stream_tcp_connect(const char *location, int port);
rpcclient_t rpcclient_stream_unix_connect(const char *location);

rpcclient_t rpcclient_datagram_neq(void);
rpcclient_t rpcclient_datagram_udp_connect(const char *location, int port);

rpcclient_t rpcclient_serial_new(int fd);
rpcclient_t rpcclient_serial_connect(const char *path);

bool rpcclient_login(rpcclient_t, const char *username, const char *password,
	enum rpc_login_type type);

void rpcclient_disconnect(rpcclient_t client);



bool rpcclient_next_message(rpcclient_t client);

struct cpcp_item *rpcclient_next_item(rpcclient_t client);


struct cpcp_pack_context *rpcclient_pack_context(rpcclient_t client);


#endif
