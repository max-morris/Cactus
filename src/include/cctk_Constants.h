 /*@@
   @header    cctk_Constants.h
   @date      Fri Oct 15 21:29:23 CEST 1999
   @author    Gabrielle Allen
   @desc 
   Constants used by Cactus
   @enddesc
   @version $Header$
 @@*/

#ifndef _CCTK_CONSTANTS_H_
#define _CCTK_CONSTANTS_H_

#define CCTK_VARIABLE_VOID       100
#define CCTK_VARIABLE_BYTE       101
#define CCTK_VARIABLE_INT        102
#define CCTK_VARIABLE_INT1       103
#define CCTK_VARIABLE_INT2       104
#define CCTK_VARIABLE_INT4       105
#define CCTK_VARIABLE_INT8       106
#define CCTK_VARIABLE_REAL       107
#define CCTK_VARIABLE_REAL4      108
#define CCTK_VARIABLE_REAL8      109
#define CCTK_VARIABLE_REAL16     110
#define CCTK_VARIABLE_COMPLEX    111
#define CCTK_VARIABLE_COMPLEX8   112
#define CCTK_VARIABLE_COMPLEX16  113
#define CCTK_VARIABLE_COMPLEX32  114
#define CCTK_VARIABLE_STRING     115
#define CCTK_VARIABLE_POINTER	 116
#define CCTK_VARIABLE_FPOINTER   117

/* DEPRECATED IN BETA 12 */
#define CCTK_VARIABLE_FN_POINTER CCTK_VARIABLE_FPOINTER

/* DEPRECATED IN BETA 10 */
#define CCTK_VARIABLE_CHAR       CCTK_VARIABLE_BYTE


/* steerable status of parameters */
#define CCTK_STEERABLE_NEVER   200
#define CCTK_STEERABLE_ALWAYS  201
#define CCTK_STEERABLE_RECOVER 202

/* number of staggerings */
#define CCTK_NSTAGGER      3


#endif /* _CCTK_CONSTANTS_ */

