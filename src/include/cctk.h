 /*@@
   @header    cctk.h
   @date      Tue Jan 26 17:29:34 1999
   @author    Tom Goodale
   @desc 
   Main include file for the CCTK.  All thorns should include this...
   @enddesc
   @version $Id$
 @@*/

#ifndef _CCTK_H_
#define _CCTK_H_

/* Grab the main configuration info. */

#include "config.h"


/* The thorn definitions #define <PACKAGE>_<THORN> */

#include "definethorn.h"

/* Define which thorn the file is in */

#include "definethisthorn.h"

/* Define some stuff */

#ifdef FCODE

#define CCTK_PRINTSEPARATOR\
  print *,"----------------------------------------------------------------"


#define _CCTK_FARGUMENTS  dim, global_sh, sh, lb, ub, bbox, delta_time, time, delta_space, origin_space, levfac, convlevel, nghostzones, iteration, GH
#define _DECLARE_CCTK_FARGUMENTS INTEGER dim&&\
                           INTEGER global_sh(dim)&&\
                           INTEGER sh(dim), lb(dim), ub(dim), bbox(2*dim)&&\
                           CCTK_REAL delta_time, time, delta_space(dim)&&\
			   CCTK_REAL origin_space(dim)&&\
                           INTEGER levfac&&\
                           INTEGER convlevel&&\
                           INTEGER nghostzones&&\
                           INTEGER iteration&&\
                           CCTK_POINTER GH&&\

#endif /*FCODE*/

#ifdef CCODE

#include "cGH.h"

#define CCTK_PRINTSEPARATOR \
  printf("----------------------------------------------------------------\n");

#define _INITIALISE_CCTK_C2F
#define _DECLARE_CCTK_C2F 
#define _PASS_CCTK_C2F(xGH) &((xGH)->dim),\
                            (xGH)->global_shape,\
                            (xGH)->local_shape, (xGH)->lower_bound, (xGH)->upper_bound, (xGH)->bbox,\
                            &((xGH)->delta_time), &((xGH)->time), (xGH)->delta_space,\
			    (xGH)->origin_space,\
                            &((xGH)->levfac),\
                            &((xGH)->convlevel),\
                            &((xGH)->nghostzones),\
                            &((xGH)->iteration),\
                            (xGH)
#define _CCTK_C2F_PROTO     int *,\
                            int *,\
                            int *,int *, int *, int *,\
                            CCTK_REAL *, CCTK_REAL *, CCTK_REAL *,\
			    CCTK_REAL *,\
                            int *,\
                            int *,\
                            int *,\
                            int *,\
                            cGH *

#define CCTK_STORAGESIZE(xGH, group, dim) (CCTK_QueryGroupStorage(xGH,group) ?\
                           (CCTK_ArrayGroupSize(xGH, group, dim)) : &(_cctk_one))


extern int _cctk_one;

#endif /*CCODE*/

#define CCTK_VARIABLE_CHAR    1
#define CCTK_VARIABLE_INTEGER 2
#define CCTK_VARIABLE_REAL    3
#define CCTK_VARIABLE_COMPLEX 4

/*#define CCTK_MAKESTRING(x) CCTK_REALSTRING(x)
#define CCTK_REALSTRING(x) #x
#define CCTK_WARN(a,b) CCTK_Warn(a,CCTK_MAKESTRING(CCTK_THORN),b)
*/
#define CCTK_WARN(a,b) CCTK_Warn(a,CCTK_THORNSTRING,b)

#endif

