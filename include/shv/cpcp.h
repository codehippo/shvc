#ifndef SHV_CPCP_H
#define SHV_CPCP_H

#include <shv/cpcp_pack.h>
#include <shv/cpcp_unpack.h>


typedef enum {
	CPCP_ChainPack = 1,
	CPCP_Cpon,
} cpcp_pack_format;

typedef enum {
	CPCP_RC_OK = 0,
	CPCP_RC_MALLOC_ERROR,
	CPCP_RC_BUFFER_OVERFLOW,
	CPCP_RC_BUFFER_UNDERFLOW,
	CPCP_RC_MALFORMED_INPUT,
	CPCP_RC_LOGICAL_ERROR,
	CPCP_RC_CONTAINER_STACK_OVERFLOW,
	CPCP_RC_CONTAINER_STACK_UNDERFLOW,
} cpcp_error_codes;

const char *cpcp_error_string(int err_no);


#endif
