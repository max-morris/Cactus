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


#define _CCTK_FARGUMENTS  cctk_dim, cctk_gsh, cctk_lsh, cctk_lbnd, cctk_ubnd, cctk_from, cctk_to, cctk_bbox, cctk_delta_time, cctk_time, cctk_delta_space, cctk_origin_space, cctk_levfac, cctk_convlevel, cctk_nghostzones, cctk_iteration, GH
#define _DECLARE_CCTK_FARGUMENTS CCTK_INT cctk_dim&&\
                           INTEGER cctk_gsh(cctk_dim),cctk_lsh(cctk_dim)&&\
                           INTEGER cctk_lbnd(cctk_dim),cctk_ubnd(cctk_dim)&&\
                           INTEGER cctk_from(cctk_dim),cctk_to(cctk_dim),cctk_bbox(2*cctk_dim)&&\
                           CCTK_REAL cctk_delta_time, cctk_time&&\
                           CCTK_REAL cctk_delta_space(cctk_dim)&&\
			   CCTK_REAL cctk_origin_space(cctk_dim)&&\
                           INTEGER cctk_levfac(cctk_dim)&&\
                           INTEGER cctk_convlevel&&\
                           INTEGER cctk_nghostzones(cctk_dim)&&\
                           INTEGER cctk_iteration&&\
                           CCTK_POINTER GH&&\

#endif /*FCODE*/

#ifdef CCODE

#include "cGH.h"

#define CCTK_PRINTSEPARATOR \
  printf("----------------------------------------------------------------\n");

#define _INITIALISE_CCTK_C2F
#define _DECLARE_CCTK_C2F 
#define _PASS_CCTK_C2F(xGH) &((xGH)->cctk_dim),\
                            (xGH)->cctk_gsh,\
                            (xGH)->cctk_lsh, (xGH)->cctk_lbnd, (xGH)->cctk_ubnd, (xGH)->cctk_from,(xGH)->cctk_to,(xGH)->cctk_bbox,\
                            &((xGH)->cctk_delta_time), &((xGH)->cctk_time), (xGH)->cctk_delta_space,\
			    (xGH)->cctk_origin_space,\
                            (xGH)->cctk_levfac,\
                            &((xGH)->cctk_convlevel),\
                            (xGH)->cctk_nghostzones,\
                            &((xGH)->cctk_iteration),\
                            (xGH)
#define _CCTK_C2F_PROTO     int *,\
                            int *,\
                            int *,int *, int *, int *,int *,int *,\
                            CCTK_REAL *, CCTK_REAL *, CCTK_REAL *,\
			    CCTK_REAL *,\
                            int *,\
                            int *,\
                            int *,\
                            int *,\
                            cGH *

#define CCTK_STORAGESIZE(xGH, group, cctk_dim) (CCTK_QueryGroupStorage(xGH,group) ?\
                           (CCTK_ArrayGroupSize(xGH, group, cctk_dim)) : &(_cctk_one))


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

