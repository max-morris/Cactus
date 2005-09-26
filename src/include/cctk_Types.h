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


/* Declare and initialise a variable and tell the compiler that it may
   be unused.  This is used for CCTK_PARAMETERS and CCTK_ARGUMENTS.

   The macro CCTK_DECLARE_INIT (typ, nam, val) is used with
   typ: a type, used to declare the variable (e.g. "const int")
   nam: the variable name (e.g. "x")
   val: the value used to initialise it (e.g. "42")
*/

#if    (! __cplusplus && HAVE_CCTK_C_ATTRIBUTE_UNUSED  )  \
    || (  __cplusplus && HAVE_CCTK_CXX_ATTRIBUTE_UNUSED)

/* We have __attribute__((unused)), so use it */
#define CCTK_DECLARE_INIT(typ,nam,val)          \
  typ nam __attribute__((unused)) = (val);

#else

/* Some fallback, bound to fool most compilers */
#define CCTK_DECLARE_INIT(typ,nam,val)                                  \
  typ nam = (val);                                                      \
  void const * cctki_use_##nam = (cctki_use_##nam = &nam, &cctki_use_##nam);

#endif


typedef void *CCTK_POINTER;
typedef const void *CCTK_POINTER_TO_CONST;
typedef void (*CCTK_FPOINTER)(void);

/* Character types */
typedef char CCTK_CHAR;
typedef const char * CCTK_STRING;

/* Structures for complex types */

#ifdef HAVE_CCTK_REAL16
#define HAVE_CCTK_COMPLEX32 1
typedef struct
{
  CCTK_REAL16 Re;
  CCTK_REAL16 Im;
} CCTK_COMPLEX32;
#endif

#ifdef HAVE_CCTK_REAL8
#define HAVE_CCTK_COMPLEX16 1
typedef struct
{
  CCTK_REAL8 Re;
  CCTK_REAL8 Im;
} CCTK_COMPLEX16;
#endif

#ifdef HAVE_CCTK_REAL4
#define HAVE_CCTK_COMPLEX8 1
typedef struct
{
  CCTK_REAL4 Re;
  CCTK_REAL4 Im;
} CCTK_COMPLEX8;
#endif

/* Small positive integer type */
typedef unsigned char CCTK_BYTE;

#endif /* CCODE */

/* Define stuff for Fortran. */
#ifdef FCODE


/* Declare a variable and tell the compiler that it may be unused.
   This is used for CCTK_ARGUMENTS.

   The macro CCTK_DECLARE (typ, nam, dim) is used with
   typ: a type, used to declare the variable (e.g. "CCTK_REAL")
   nam: the variable name (e.g. "x")
   dim: optional array dimensions, (e.g. "(10,10)")
*/

#ifdef F90CODE

/* Declare it, and use it for a dummy operation  */
#define CCTK_DECLARE(typ,nam,dim)                       \
  typ nam dim &&                                        \
  integer, parameter :: cctki_use_/**/nam = kind(nam)

#else

/* Just declare it; FORTRAN 77 has no good way of marking it as used
   within a block of declarations  */
#define CCTK_DECLARE(typ,nam,dim)               \
  typ nam dim

#endif


#define CCTK_POINTER          integer*SIZEOF_CHAR_P
#define CCTK_POINTER_TO_CONST integer*SIZEOF_CHAR_P
/* TODO: add autoconf for determining the size of function pointers */
#define CCTK_FPOINTER         integer*SIZEOF_CHAR_P

/* Character types */
/* A single character does not exist in Fortran; in Fortran, all
   character types are strings.  Hence we do not define CCTK_CHAR.  */
/* #define CCTK_CHAR   CHARACTER */
/* This is a C-string, i.e., only a pointer */
#define CCTK_STRING CCTK_POINTER_TO_CONST

#ifdef HAVE_CCTK_INT8
#define CCTK_INT8 INTEGER*8
#endif
#ifdef HAVE_CCTK_INT4
#define CCTK_INT4 INTEGER*4
#endif
#ifdef HAVE_CCTK_INT2
#define CCTK_INT2 INTEGER*2
#endif
#ifdef HAVE_CCTK_INT1
#define CCTK_INT1 INTEGER*1
#endif

#ifdef HAVE_CCTK_REAL16
#define CCTK_REAL16 REAL*16
#define HAVE_CCTK_COMPLEX32 1
#define CCTK_COMPLEX32  COMPLEX*32
#endif

#ifdef HAVE_CCTK_REAL8
#define CCTK_REAL8  REAL*8
#define HAVE_CCTK_COMPLEX16 1
#define CCTK_COMPLEX16  COMPLEX*16
#endif

#ifdef HAVE_CCTK_REAL4
#define CCTK_REAL4  REAL*4
#define HAVE_CCTK_COMPLEX8 1
#define CCTK_COMPLEX8   COMPLEX*8
#endif

/* Should be unsigned, but Fortran doesn't have that */
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

