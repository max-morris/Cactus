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


/* Define some stuff */

#ifdef FCODE
#define _CCTK_FARGUMENTS  dim, global_sh, sh, lb, ub, bbox, delta_time, time, delta_space, origin_space, levfac, convlevel, nghostzones, iteration, GH
#define _DECLARE_CCTK_FARGUMENTS INTEGER dim&&\
                           INTEGER global_sh(dim)&&\
                           INTEGER sh(dim), lb(dim), ub(dim), bbox(2*dim)&&\
                           REAL delta_time, time, delta_space(dim)&&\
			   REAL origin_space(dim)&&\
                           INTEGER levfac&&\
                           INTEGER convlevel&&\
                           INTEGER nghostzones&&\
                           INTEGER iteration&&\
                           POINTER GH&&\

#endif /*FCODE*/

#ifdef CCODE
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
                            Double *, Double *, Double *,\
			    Double *,\
                            int *,\
                            int *,\
                            int *,\
                            unsigned long *,\
                            cGH *

#define CCTK_STORAGESIZE(xGH, group, dim) (CCTK_QueryGroupStorage(xGH,group) ?\
                           (CCTK_ArrayGroupSize(xGH, group, dim)) : &(_cctk_one))

typedef struct
{
  Double Re;
  Double Im;
} Complex;

extern int _cctk_one;

#endif /*CCODE*/

#endif
