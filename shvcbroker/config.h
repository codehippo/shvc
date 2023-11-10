/*****************************************************************************
* Copyright (C) 2022 Elektroline Inc. All rights reserved.
* K Ladvi 1805/20
* Prague, 184 00
* info@elektroline.cz (+420 284 021 111)
*****************************************************************************/
#ifndef _FOO_CONFIG_H_
#define _FOO_CONFIG_H_
#include <stdio.h>
#include <stdbool.h>

/* Parse arguments and load configuration file
* Returned pointer is to statically allocated (do not call free on it).
 */

struct user {
	char* name;
	char* password;
	char* sha1pass;
	char* role;
};

struct method {
	char* name;
	char* path;
};

struct path_role {
	struct role* role;
	const char* path;
};


struct role {
	char* name;
	size_t num_methods;
	char* other_roles;
	char* access;
	char* txt_methods;
	struct method** methods;
};

struct config {
	char* internet;
	char* unixx;
	struct user* users;
	struct role* roles;
	size_t num_users;
	size_t num_roles;
	struct method* methods;
	size_t num_methods;
};

void load_config(int argc, char **argv) __attribute__((nonnull));
static int handler(void* user, const char* section, const char* name,
	const char* value);
bool config_listen(struct config* conf, const char* section,
	const char* name);
bool config_users(struct config* conf, const char* section,
	const char* name, const char* value);
bool config_role(struct config* conf, const char* section,
	const char* name, const char* value);
bool config_method(struct config* conf);
void config_method_path(const char* meth_name, struct method* method, struct path_role* path_role, size_t* all_path_cnt, struct role* role);
void prepare_user(struct config* conf, const char* section);
void prepare_role(struct config* conf, const char* section);
void free_config(struct config* conf);

#endif

