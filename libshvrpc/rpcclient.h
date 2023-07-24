#ifndef LIBSHV_RPCCLIENT_H
#define LIBSHV_RPCCLIENT_H

#include <shv/rpcclient.h>

struct rpcclient {
	void (*nextmsg)(rpcclient_t);
	void (*disconnect)(rpcclient_t);
	cpcp_pack_context pack;
	cpcp_unpack_context unpack;
};

#endif
