 /*@@
   @header    cctk_types.h
   @date      Mon Jun 21 21:03:27 1999
   @author    Tom Goodale
   @desc 
   Defines the appropriate types based upon the precision.
   Should be included by cctk_Config.h .
   @enddesc 
 @@*/

#ifndef _CCTK_TYPES_H_
#define _CCTK_TYPES_H_

/* Make sure that cctk_config.h is available in case someone includes
 * this by hand.
 */
#ifndef _CCTK_CONFIG_H_
#include "cctk_Config.h"
#endif

/* Define stuff for C. */
#ifdef CCODE

typedef void *CCTK_POINTER;
typedef const void *CCTK_POINTER_TO_CONST;
typedef void (*CCTK_FPOINTER)(void);

#define CCTK_STRING const char *

/* Structures for complex types */

#if CCTK_HAVE_REAL16
#define CCTK_HAVE_COMPLEX32 1
typedef struct
{
  CCTK_REAL16 Re;
  CCTK_REAL16 Im;
} CCTK_COMPLEX32;
#endif

#if CCTK_HAVE_REAL8
#define CCTK_HAVE_COMPLEX16 1
typedef struct
{
  CCTK_REAL8 Re;
  CCTK_REAL8 Im;
} CCTK_COMPLEX16;
#endif

#if CCTK_HAVE_REAL4
#define CCTK_HAVE_COMPLEX8 1
typedef struct
{
  CCTK_REAL4 Re;
  CCTK_REAL4 Im;
} CCTK_COMPLEX8;
#endif

/* Character type */
/* DEPRECATED IN BETA 10 */
typedef unsigned char CCTK_CHAR;

typedef unsigned char CCTK_BYTE;

#endif /* CCODE */

/* Define stuff for fortran. */
#ifdef FCODE

#define CCTK_POINTER          integer*SIZEOF_CHAR_P
#define CCTK_POINTER_TO_CONST integer*SIZEOF_CHAR_P
/* TODO: add autoconf for determining the size of function pointers */
#define CCTK_FPOINTER         integer*SIZEOF_CHAR_P

#define CCTK_STRING CCTK_POINTER

#ifdef CCTK_HAVE_INT8
#define CCTK_INT8 INTEGER*8
#endif
#ifdef CCTK_HAVE_INT4
#define CCTK_INT4 INTEGER*4
#endif
#ifdef CCTK_HAVE_INT2
#define CCTK_INT2 INTEGER*2
#endif
#ifdef CCTK_HAVE_INT1
#define CCTK_INT1 INTEGER*1
#endif

#ifdef CCTK_HAVE_REAL16
#define CCTK_REAL16 REAL*16
#define CCTK_HAVE_COMPLEX32 1
#define CCTK_COMPLEX32  COMPLEX*32
#endif
#ifdef CCTK_HAVE_REAL8
#define CCTK_REAL8  REAL*8
#define CCTK_HAVE_COMPLEX16 1
#define CCTK_COMPLEX16  COMPLEX*16
#endif
#ifdef CCTK_HAVE_REAL4
#define CCTK_REAL4  REAL*4
#define CCTK_HAVE_COMPLEX8 1
#define CCTK_COMPLEX8   COMPLEX*8
#endif

/* DEPRECATED IN BETA 10 */
#define CCTK_CHAR CHARACTER

#define CCTK_BYTE INTEGER*1

#endif /*FCODE */

/* Now pick the types based upon the precision variable. */

/* Floating point precision */
#ifdef CCTK_REAL_PRECISION_16
#define CCTK_REAL CCTK_REAL16
#endif

#ifdef CCTK_REAL_PRECISION_8
#define CCTK_REAL CCTK_REAL8
#endif

#ifdef CCTK_REAL_PRECISION_4
#define CCTK_REAL CCTK_REAL4
#endif

/* Integer precision */

#ifdef CCTK_INTEGER_PRECISION_8
#define CCTK_INT CCTK_INT8
#endif

#ifdef CCTK_INTEGER_PRECISION_4
#define CCTK_INT CCTK_INT4
#endif

#ifdef CCTK_INTEGER_PRECISION_2
#define CCTK_INT CCTK_INT2
#endif

#ifdef CCTK_INTEGER_PRECISION_1
#define CCTK_INT CCTK_INT1
#endif

/* Complex precision */
#ifdef CCTK_REAL_PRECISION_16
#define CCTK_COMPLEX CCTK_COMPLEX32
#endif

#ifdef CCTK_REAL_PRECISION_8
#define CCTK_COMPLEX CCTK_COMPLEX16
#endif

#ifdef CCTK_REAL_PRECISION_4
#define CCTK_COMPLEX CCTK_COMPLEX8
#endif

#endif /*_CCTK_TYPES_H_ */

