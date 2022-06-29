/*
 * $Id: array.h,v 1.6 2002/12/08 03:37:19 fabio Exp $
 *
 */
#ifndef ARRAY_H
#define ARRAY_H

#include "util.h"

/* Return value when memory allocation fails */
#define ARRAY_OUT_OF_MEM -10000

/*
 * In between accesses, the "index" field of the array_t structure
 * holds the negative of the size of the objects stored in the array.
 * This allows the functions and macros to perform a rudimentary form
 * of type checking.
 */

typedef struct array_t {
    char *space;
    int	 num;		/* number of array elements.		*/
    int	 n_size;	/* size of 'data' array (in objects)	*/
    int	 obj_size;	/* size of each array object.		*/
    int	 index;		/* combined index and locking flag.	*/
} array_t;

EXTERN array_t *array_do_alloc ARGS((int, int));
EXTERN array_t *array_dup ARGS((array_t *));
EXTERN array_t *array_join ARGS((array_t *, array_t *));
EXTERN void array_free ARGS((array_t *));
EXTERN int array_append ARGS((array_t *, array_t *));
EXTERN void array_sort ARGS((array_t *, int (*)(const void *, const void *)));
EXTERN void array_uniq ARGS((array_t *, int (*)(char **, char **), void (*)(char *)));
EXTERN int array_abort ARGS((array_t *, int));
EXTERN int array_resize ARGS((array_t *, int));
EXTERN char *array_do_data ARGS((array_t *));

extern int unsigned array_global_index;
extern int array_global_insert;

#define array_alloc(type, number)		\
    array_do_alloc(sizeof(type), number)

#define array_insert(type, a, i, datum)         \
    (  -(a)->index != sizeof(type) ? array_abort((a),4) : 0,\
        (a)->index = (i),\
        (a)->index < 0 ? array_abort((a),0) : 0,\
        (a)->index >= (a)->n_size ?\
	array_global_insert = array_resize(a, (a)->index + 1) : 0,\
        array_global_insert != ARRAY_OUT_OF_MEM ?\
        *((type *) ((a)->space + (a)->index * (a)->obj_size)) = datum : datum,\
        array_global_insert != ARRAY_OUT_OF_MEM ?\
        ((a)->index >= (a)->num ? (a)->num = (a)->index + 1 : 0) : 0,\
        array_global_insert != ARRAY_OUT_OF_MEM ?\
        ((a)->index = -(int)sizeof(type)) : ARRAY_OUT_OF_MEM )

#define array_insert_last(type, array, datum)	\
    array_insert(type, array, (array)->num, datum)

/* RB, added this without understanding locking */
#define array_remove_last(a) \
    (  -(a)->index != (a)->obj_size ? array_abort((a),4) : 0,\
       (a)->index = -(a)->index,\
       (a)->num ? (a)->num-- : array_abort((a),5),\
       (a)->index = -(a)->index )

#define array_fetch(type, a, i)			\
    (array_global_index = (i),				\
      (array_global_index >= (unsigned) ((a)->num)) ? array_abort((a),1) : 0,\
      *((type *) ((a)->space + array_global_index * (a)->obj_size)))

#define array_fetch_p(type, a, i)                       \
    (array_global_index = (i),                             \
      (array_global_index >= (unsigned) ((a)->num)) ? array_abort((a),1) : 0,\
      ((type *) ((a)->space + array_global_index * (a)->obj_size)))

#define array_fetch_last(type, array)		\
    array_fetch(type, array, ((array)->num)-1)

#define array_n(array)				\
    (array)->num

#define array_data(type, array)			\
    (type *) array_do_data(array)

#define arrayForEachItem(                                      \
  type,  /* type of object stored in array */                  \
  array, /* array to iterate */                                \
  i,     /* int, local variable for iterator */                \
  data   /* object of type */                                  \
)                                                              \
  for((i) = 0;                                                 \
      (((i) < array_n((array)))                                \
       && (((data) = array_fetch(type, (array), (i))), 1));    \
      (i)++)

#endif






