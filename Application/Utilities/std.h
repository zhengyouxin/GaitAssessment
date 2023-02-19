#ifndef _STD_H
#define _STD_H

#include <inttypes.h>
#include <math.h>

/* some convenience macros to print debug/config messages at compile time */

/* stringify a define, e.g. one that was not quoted */
#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY(s)

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

/* Boolean values */
typedef uint8_t bool_t;

/* Unit (void) values */
typedef uint8_t unit_t;

#ifndef bool
#define bool uint8_t
#endif

#define Min(x,y) (x < y ? x : y)
#define Max(x,y) (x > y ? x : y)

#ifndef ABS
#define ABS(val) ((val) < 0 ? -(val) : (val))
#endif


#endif
