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
#define CCTK_FARGS  dim,sh, lb, ub, bbox, levfac, GH
#define DECLARE_CCTK_FARGS INTEGER dim&&\
                           INTEGER sh(dim), lb(dim), ub(dim), bbox(2*dim)&&\
                           INTEGER levfac&&\
                           POINTER GH

#endif /*FCODE*/

#ifdef CCODE
#define INITIALISE_CCTK_C2F
#define DECLARE_CCTK_C2F 
#define PASS_CCTK_C2F(xGH) &((xGH)->dim),\
                           (xGH)->local_shape, (xGH)->lower_bound, (xGH)->upper_bound, (xGH)->bbox,\
                           &((xGH)->levfac),\
                           (xGH)
#define CCTK_C2F_PROTO     int *,\
                           int *,int *, int *, int *,\
                           int *,\
                           cGH *

#define CCTK_STORAGESIZE(xGH, group, dim) (CCTK_QueryGroupStorage(xGH,group) ?\
                           (CCTK_ArrayGroupSize(xGH, group, dim)) : &(_cctk_one))


extern int _cctk_one;

#endif /*CCODE*/

#endif
