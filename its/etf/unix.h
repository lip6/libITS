#ifndef UNIX_H
#define UNIX_H

#include <stdlib.h>

#if defined(HAVE_DECL_STRNDUP) && !HAVE_DECL_STRNDUP
extern char *strndup(const char *str, size_t n);
#endif
#if defined(HAVE_DECL_ASPRINTF) && !HAVE_DECL_ASPRINTF
extern int asprintf(char **ret, const char *format, ...);
#endif
#if defined(HAVE_DECL_STRSEP) && !HAVE_DECL_STRSEP
extern char *strsep(char **stringp, const char *delim);
#endif
#if defined(HAVE_DECL_MKDTEMP) && !HAVE_DECL_MKDTEMP
extern char *mkdtemp(char *);
#endif

#endif

