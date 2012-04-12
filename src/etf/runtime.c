#include "runtime.h"
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
// #include "unix.h"
#include <libgen.h>
//#include <git_version.h>
//#include <hre-main.h>

int RTverbosity=1;

int linear_search(si_map_entry map[],const char*key){
	while(map[0].key){
		if(!strcmp(map[0].key,key)) return map[0].val;
		map++;
	}
	return -1;
}

void (*RThandleFatal)(const char*file,int line,int errnum,int code);

void RTinit(int *argcp,char**argvp[]){
    RThandleFatal=NULL;
//    HREinitBare(argcp,argvp);
}


void RTexitUsage(int exit_code){
    exit(exit_code);
}

void RTexitHelp(int exit_code){
    exit(exit_code);
}

void* RTmalloc(size_t size){
	if(size==0) return NULL;
	void *tmp=malloc(size);
	if (tmp==NULL) Fatal(0,error,"out of memory trying to get %zu",size);
	return tmp;
}

void* RTmallocZero(size_t size){
	void *p=RTmalloc(size);
	memset(p, 0, size);
	return p;
}

void* RTrealloc(void *rt_ptr, size_t size){
    void *tmp=realloc(rt_ptr,size);
    if (tmp==NULL) Fatal(0,error,"out of memory trying to resize to %zu",size);
    return tmp;
}

char* RTstrdup(const char *str){
    if (str == NULL) return NULL;
    char *tmp = strdup (str);
    if (tmp == NULL) Fatal(0, error, "out of memory trying to get %zu",
                           strlen (str)+1);
    return tmp;
}

void RTfree(void *rt_ptr){
	if(rt_ptr != NULL)
            free (rt_ptr);
}
