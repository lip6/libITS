/** @file runtime.h
 *  @brief Runtime support library.
 */

#ifndef RUNTIME_H
#define RUNTIME_H

/**
\defgroup runtime Runtime support library.
 */
///@{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <errno.h>
#include <stdint.h>

typedef struct {
	char* key;
	int val;
} si_map_entry;

/**
\brief Find the value for a given key, or -1 if it does not exist.
 */
extern int linear_search(si_map_entry map[],const char*key);

#define RTstackBottom HREstackBottom

extern void* RTmalloc(size_t size);

extern void* RTmallocZero(size_t size);

extern void* RTrealloc(void *rt_ptr, size_t size);

extern char* RTstrdup(const char *str);

extern void RTfree(void *rt_ptr);

#define RT_NEW(sort) ((sort*)RTmallocZero(sizeof(sort)))

extern void *RTdlsym (const char *libname, void *handle, const char *symbol);

extern int RTverbosity;

extern void RTinit(int *argc, char **argv[]);
/**< @brief Initializes the runtime library.

Some platform do not like it is you change argv. Thus this
call makes a copy of argv allowing subsequent calls
to make changes to argv without copying again.
*/


extern void (*RThandleFatal)(const char*file,int line,int errnum,int code);

#define Warning(log,...)  fprintf(stderr,__VA_ARGS__)
#define WarningCall(log,...) fprintf(stderr,__VA_ARGS__)
#define Log(log,...) fprintf(stderr,__VA_ARGS__)
#define LogCall(log,...) fprintf(stderr,__VA_ARGS__)
#define Fatal(code,log,...) {\
        fprintf(stderr,__VA_ARGS__);			\
	if (RThandleFatal) RThandleFatal(__FILE__,__LINE__,0,code);\
	if (code==0) {\
		fprintf(stderr,"exit with FAILURE instead of 0");\
		exit(EXIT_FAILURE);\
	} else {\
		exit(code);\
	}\
}
#define FatalCall(code,log,...) {\
	fprintf(stderr,__FILE__,__LINE__,errno,__VA_ARGS__);\
	if (RThandleFatal) RThandleFatal(__FILE__,__LINE__,errno,code);\
	if (code==0) {\
		fprintf(stderr,"exit with FAILURE instead of 0");\
		exit(EXIT_FAILURE);\
	} else {\
		exit(code);\
	}\
}

/**
\brief Check if an integer is between a minimum and a maximum.
*/
#define RangeCheckInt(val,min,max) if ((val) < (min) || (val) > (max)) \
    Fatal(1,error,"value %d is out of range [%d,%d]",val,min,max)

///@}


#endif

