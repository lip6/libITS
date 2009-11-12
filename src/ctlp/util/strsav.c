/*
 * Revision Control Information
 *
 * $Id: strsav.c,v 1.7 2009/01/05 21:04:04 fabio Exp $
 *
 */
/* LINTLIBRARY */

#include <stdio.h>
#include <string.h>
#include "util.h"


/*
 *  util_strsav -- save a copy of a string
 */
char *
util_strsav(char const *s)
{
    if(s == NIL(char)) {  /* added 7/95, for robustness */
       return NIL(char);
    }
    else {
       return strcpy(ALLOC(char, strlen(s)+1), s);
    }
}

/*
 * util_inttostr -- converts an integer into a string
 */
char *
util_inttostr(int i)
{
  unsigned int mod, len;
  char *s;
  
  if (i == 0)
    len = 1;
  else {
    if (i < 0) {
      len = 1;
      mod = -i;
    }
    else {
      len = 0;
      mod = i;
    }
    len += (unsigned) floor(log10(mod)) + 1;
  }

  s = ALLOC(char, len + 1);
  sprintf(s, "%d", i);
  
  return s;
}

/*
 * util_strcat3 -- Creates a new string which is the concatenation of 3
 *    strings. It is the responsibility of the caller to free this string
 *    using FREE.
 */
char *
util_strcat3(
  char const * str1,
  char const * str2,
  char const * str3)
{
  char *str = ALLOC(char, strlen(str1) + strlen(str2) + strlen(str3) + 1);
  
  (void) strcpy(str, str1);
  (void) strcat(str, str2);
  (void) strcat(str, str3);

  return (str);
}

/*
 * util_strcat4 -- Creates a new string which is the concatenation of 4
 *    strings. It is the responsibility of the caller to free this string
 *    using FREE.
 */
char *
util_strcat4(
  char const * str1,
  char const * str2,
  char const * str3,
  char const * str4)
{
  char *str = ALLOC(char, strlen(str1) + strlen(str2) + strlen(str3) +
                    strlen(str4) + 1);
  
  (void) strcpy(str, str1);
  (void) strcat(str, str2);
  (void) strcat(str, str3);
  (void) strcat(str, str4);

  return (str);
}


