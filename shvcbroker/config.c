#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ini.h>
#include <string.h>

//TODO reallocating
static int handler(void* user, const char* section, const char* name,
	const char* value) {
	if(!strlen(section) || !strlen(name) || !strlen(value))
		return 0;
	//	printf("%s....%s....%s\n",section,name,value);
	struct config* conf = (struct config*)user;

	if(!strcmp(section,"listen")){
		if(!config_listen(conf,name, value)) { //TODO Error message and exit
			return 0;
		}
	} else if(!strncmp(section,"users.",6)){
		if(conf->num_users>0 ? strcmp(section+6,conf->users[conf->num_users-1].name ) : true){
			prepare_user(conf,section);
		}

		if(!config_users(conf, section, name, value)) {
			return 0;
		}
	}
	else if(!strncmp(section,"roles",5)){
		if(conf->num_roles>0 ? strcmp(section+6,conf->roles[conf->num_roles-1].name ) : true){
			prepare_role(conf, section);
		}

		if(!config_role(conf,section,name, value)){
			return 0;
		}

	}


	return 1;
}

bool config_listen(struct config* conf, const char* name, const char* value) {
	if(!strcmp(name,"internet")){
		conf->internet= strdup(value);
		return 1;
	}
	if(!strcmp(name,"unix")){
		conf->unixx= strdup(value);
		return 1;
	}

	return 0;
}

bool config_users(struct config* conf, const char* section, const char* name, const char* value) {

	if(!strcmp(name,"password")){
		conf->users[conf->num_users-1].password= strdup(value);
		return 1;
	}
	if(!strcmp(name,"sha1pass")){
		conf->users[conf->num_users-1].sha1pass= strdup(value);
		printf("%s\n\n",conf->users[conf->num_users-1].sha1pass);
		return 1;
	}
	if(!strcmp(name,"roles")){
		conf->users[conf->num_users-1].role=strdup(value);
		return 1;
	}
	return 0;
}

bool config_role(struct config* conf, const char* section,
	const char* name, const char* value){

	if(!strcmp(name,"roles")){
		conf->roles[conf->num_roles-1].other_roles=strdup(value);
		return 1;
	}
	if(!strcmp(name,"methods")){
		conf->roles[conf->num_roles-1].txt_methods=strdup(value);
		return 1;
	}
	if(!strcmp(name,"access")){
		conf->roles[conf->num_roles-1].access=strdup(value);
		return 1;
	}
	return 0;
}

void prepare_user(struct config* conf, const char* section) {
	conf->num_users++;
	conf->users[conf->num_users-1].name= strdup(section+6);
	conf->users[conf->num_users-1].role=NULL;
	conf->users[conf->num_users-1].password=NULL;
	conf->users[conf->num_users-1].sha1pass=NULL;
}

void prepare_role(struct config* conf, const char* section) {
	conf->num_roles++;
	conf->roles[conf->num_roles-1].name= strdup(section+6);
	conf->roles[conf->num_roles-1].access=NULL;
	conf->roles[conf->num_roles-1].txt_methods=NULL;
	conf->roles[conf->num_roles-1].other_roles=NULL;
}

static void print_usage(const char *argv0) {
	fprintf(stderr, "%s [-Vh] [-c FILE]\n", argv0);
}

static void print_help(const char *argv0) {
	print_usage(argv0);
	fprintf(stderr, "\n");
	fprintf(stderr, "Arguments:\n");
	fprintf(stderr, "  -c FILE  Configuration file\n");
	fprintf(stderr, "  -V       Print version and exit\n");
	fprintf(stderr, "  -h       Print this help text\n");
}

static void parse_opts(const char **confpath, int argc, char **argv) {
	int c;
	while ((c = getopt(argc, argv, "c:Vh")) != -1) {
		switch (c) {
			case 'c':
				*confpath = argv[optind];
				break;
			case 'V':
				printf("%s " PROJECT_VERSION, argv[0]);
				exit(0);
			case 'h':
				print_help(argv[0]);
				exit(0);
			default:
				print_usage(argv[0]);
				fprintf(stderr, "Invalid option: -%c\n", c);
				exit(2);
		}
	}
	if (optind < argc) {
		fprintf(stderr, "Invalid argument: %s\n", argv[optind + 1]);
		exit(2);
	}
}

void load_config(int argc, char **argv) {
	const char *confpath = NULL;
	struct config *conf = malloc(sizeof(struct config));
	conf->users = malloc(10*sizeof(struct user));
	conf->roles = malloc(10*sizeof(struct role));
	conf->num_users=0;
	conf->num_roles=0;
	parse_opts(&confpath, argc, argv);
	const char* config_file = argv[2];

	if (argc != 3) {
		return ;
	}

	if (ini_parse(config_file, handler, conf) < 0) {
		printf("Can't load '%s'\n", config_file);
		return ;
	}
	//    printf("%s\n%s",conf->internet, conf->unixx);
	//    printf("\n-----------\n");
	//	for(int i=0; i<conf->num_users;i++){
	//		printf("%s--%s--%s--%s\n",conf->users[i].name,conf->users[i].password,conf->users[i].sha1pass,conf->users[i].role);
	//	}
	//    printf("\n-----------\n");
	//    for(int i=0; i<conf->num_roles;i++){
	//        printf("%s--%s--%s--%s\n",conf->roles[i].name,conf->roles[i].access, conf->roles[i].other_roles, conf->roles[i].txt_methods);
	//    }
	//	printf("\n-----------\n");
	//	printf("\n-----------\n");
	//	printf("\n-----------\n");
	config_method(conf);
	//	printf("\n-----------\n");
	//	printf("\n-----------\n");
	//	printf("\n-----------\n");
//	for(int s=0;s<conf->num_roles;s++){
//		for(int i=0;i<conf->roles[s].num_methods;i++){
//			printf("%s - %s\n",conf->roles[s].methods[i]->path,conf->roles[s].methods[i]->name);
//		}
//		printf("\n----------\n");
//
//	}
	for(int i=0;i<conf->num_methods;i++){
		printf("%s\n",conf->methods[i].name);
	}
	free_config(conf);
}

void free_config(struct config* conf) { //TODO finish
	for (size_t i = 0; i < conf->num_users; i++) {
		free(conf->users[i].name);
		free(conf->users[i].password);
		free(conf->users[i].sha1pass);
		free(conf->users[i].role);

	}
	free(conf->users);

	for (size_t i = 0; i < conf->num_roles; ++i) {
		free(conf->roles[i].name);

		free(conf->roles[i].other_roles);
		free(conf->roles[i].access);
		free(conf->roles[i].txt_methods);
	}
	free(conf->roles);

	free(conf->internet);
	free(conf->unixx);
	free(conf);
}

bool config_method(struct config* conf) { // TODO jenom ":" pomocí counter
	conf->methods = malloc(10 * sizeof(struct method));
	struct role** all_methods_role = malloc(10 * sizeof(struct role*));
	struct path_role* all_path_role = malloc(10 * sizeof(struct path_role));
	size_t all_methods_cnt=0;
	size_t all_path_cnt=0;
	conf->num_methods = 0;

	for (int i = 0; i < conf->num_roles; i++) {
		conf->roles[i].num_methods=0;
		conf->roles[i].methods= malloc(10*sizeof (struct method*));
		char* ptr = strdup(conf->roles[i].txt_methods);
		if(ptr[0]==':') {
			all_methods_cnt++;
			all_methods_role[all_methods_cnt-1]=&conf->roles[i];
			continue;
		}
		char* token_1 = strtok(ptr, " ");
		while (token_1 != NULL) {
			conf->num_methods++;
			conf->roles[i].num_methods++;
			config_method_path(token_1,&conf->methods[conf->num_methods-1],all_path_role,&all_path_cnt,&conf->roles[i]);
			conf->roles[i].methods[conf->roles[i].num_methods-1]=&conf->methods[conf->num_methods-1];
			token_1=strtok(NULL," ");
		}
		free(ptr);
	}
//	for(int i=0;i<all_methods_cnt;i++){
//		printf(" %s\n",all_methods_role[i]->name);
//	}
	config_path_method_all(all_methods_role,all_path_role,all_path_cnt,all_methods_cnt);
}

void config_method_path(const char* meth_name, struct method* method, struct path_role* path_role, size_t* all_path_cnt,struct role* role){
	int path_len;
	for(path_len=0;path_len<strlen(meth_name);path_len++){
		if(meth_name[path_len]==':') {
			break;
		}
	}
	method->path= strndup(meth_name,path_len);
	if(meth_name[path_len]==':' && strlen(meth_name)==path_len+1){
			method->name=NULL;
		(*all_path_cnt)++;
		path_role[*all_path_cnt-1].path=strdup(meth_name);
		path_role[*all_path_cnt-1].role=role;
		return;
	}
	meth_name +=path_len+1;
	method->name= strdup(meth_name);
}
void config_path_method_all(struct role** roles_all, struct path_role* path_role,  size_t all_path_cnt, size_t all_methods_cnt){
	for(int i=0;i<all_path_cnt;i++){
		printf("%s %s\n",path_role[i].path, path_role[i].role->name);
	}
	printf("---------------\n");
	for(int i=0;i<all_methods_cnt;i++){
		printf("%s \n",roles_all[i]->name);
	}
}

