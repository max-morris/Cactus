 /*@@
   @header    cctk_types.h
   @date      Mon Jun 21 21:03:27 1999
   @author    Tom Goodale
   @desc 
   Defines the appropriate types based upon the precision.
   Should be included by config.h .
   @enddesc 
 @@*/

#ifndef _CCTK_TYPES_H_
#define _CCTK_TYPES_H_

/* Make sure that config.h is available in case someone includes
 * this by hand.
 */
#ifndef _CONFIG_H_
#include "config.h"
#endif

/* Define stuff for fortran. */
#ifdef FCODE

#define CCTK_POINTER integer*SIZEOF_CHAR_P

#define CCTK_STRING CCTK_POINTER

#define CCTK_REAL16 REAL*16
#define CCTK_REAL8  REAL*8
#define CCTK_REAL4  REAL*4

#define CCTK_INT8 INTEGER*8
#define CCTK_INT4 INTEGER*4
#define CCTK_INT2 INTEGER*2

#endif /*FCODE */

/* Now pick the types based upon the precision variable. */

/* Floating point precision */
#ifdef CCTK_PRECISION_QUAD
#define CCTK_REAL CCTK_REAL16
#endif

#ifdef CCTK_PRECISION_DOUBLE
#define CCTK_REAL CCTK_REAL8
#endif

#ifdef CCTK_PRECISION_SINGLE
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

#endif /*_CCTK_TYPES_H_ */

