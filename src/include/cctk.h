 /*@@
   @header    cctk.h
   @date      Tue Jan 26 17:29:34 1999
   @author    Tom Goodale
   @desc 
   Main include file for the CCTK.  All thorns should include this...
   @enddesc
   @version $Header$
 @@*/

#ifndef _CCTK_H_
#define _CCTK_H_

/* Grab the main configuration info. */

#include "cctk_Config.h"

/* Define which thorn the file is in */

#include "definethisthorn.h"

/* Include the constants */

#include "cctk_Constants.h"

/* Define some stuff */

#ifdef FCODE

#define CCTK_PRINTSEPARATOR\
  print *,"-----------------------------------------------------------"

#define _CCTK_FARGUMENTS  cctk_dim, cctk_gsh, cctk_lsh, cctk_lbnd,\
                          cctk_ubnd, cctk_lssh, cctk_from, cctk_to, cctk_bbox,\
                          cctk_delta_time, cctk_time, cctk_delta_space,\
                          cctk_origin_space, cctk_levfac, cctk_convlevel,\
                          cctk_nghostzones, cctk_iteration, cctkGH

#define _DECLARE_CCTK_FARGUMENTS INTEGER cctk_dim&&\
                           INTEGER cctk_gsh(cctk_dim),cctk_lsh(cctk_dim)&&\
                           INTEGER cctk_lbnd(cctk_dim),cctk_ubnd(cctk_dim)&&\
                           INTEGER cctk_lssh(CCTK_NSTAGGER*cctk_dim)&&\
                           INTEGER cctk_from(cctk_dim),cctk_to(cctk_dim)&&\
                           INTEGER cctk_bbox(2*cctk_dim)&&\
                           CCTK_REAL cctk_delta_time, cctk_time&&\
                           CCTK_REAL cctk_delta_space(cctk_dim)&&\
                           CCTK_REAL cctk_origin_space(cctk_dim)&&\
                           INTEGER cctk_levfac(cctk_dim)&&\
                           INTEGER cctk_convlevel&&\
                           INTEGER cctk_nghostzones(cctk_dim)&&\
                           INTEGER cctk_iteration&&\
                           CCTK_POINTER cctkGH&&\

#define CCTK_WARN(a,b) CCTK_Warn(a,__LINE__,__FORTRANFILE__,CCTK_THORNSTRING,b)

#define CCTKi_EXPECTERR(in,err,warnonerr,message) CCTKi_ExpectError(in,err,warnonerr,__LINE__,__FORTRANFILE__,CCTK_THORNSTRING,message)

#define CCTKi_EXPECTOK(in,ok,warnonerr,message)  CCTKi_ExpectOK(in,ok,warnonerr,__LINE__,__FORTRANFILE__,CCTK_THORNSTRING,message)

#define CCTK_EQUALS(a,b) (CCTK_Equals(a,b).eq.1)

#define CCTK_PASS_FTOF CCTK_FARGUMENTS

#define CCTK_DELTA_SPACE(x) (cctk_delta_space(x)/cctk_levfac(x))
#define CCTK_DELTA_TIME cctk_delta_time
#define CCTK_LSSH(stag,dim) cctk_lssh(stag*CCTK_NSTAGGER+dim)
#define CCTK_LSSH_IDX(stag,dim) (stag*CCTK_NSTAGGER+dim)

#define DECLARE_CCTK_FUNCTIONS integer CCTK_Equals, CCTK_MyProc, CCTK_nProcs

#endif /*FCODE*/

#ifdef CCODE

#include "cGH.h"
#include "cctk_Flesh.h"
#include "cctk_IO.h"
#include "cctk_Comm.h"
#include "cctk_GHExtensions.h"

#include "cctk_ActiveThorns.h"
#include "cctk_Banner.h"
#include "cctk_Cache.h"
#include "cctk_CommandLine.h"
#include "cctk_Complex.h"
#include "cctk_Coord.h"
#include "cctk_File.h"
#include "cctk_Groups.h"
#include "cctk_Stagger.h"
#include "cctk_GroupsOnGH.h"
#include "cctk_GHExtensions.h"
#include "cctk_Interp.h"
#include "cctk_IOMethods.h"
#include "cctk_Misc.h"
#include "cctk_Parameter.h"
#include "cctk_Reduction.h"
#include "cctk_WarnLevel.h"

#define CCTK_GFINDEX3D(GH,i,j,k) ((i) + (GH)->cctk_lsh[0]*((j)+(GH)->cctk_lsh[1]*(k)))
#define CCTK_GFINDEX2D(GH,i,j)   ((i) + (GH)->cctk_lsh[0]*((j)))
#define CCTK_GFINDEX1D(GH,i)      (i)


#define CCTK_PRINTSEPARATOR \
  printf("--------------------------------------------------------------------------------\n");

#define _DECLARE_CCTK_CARGUMENTS \
            void      *cctk_dummy_pointer;\
            CCTK_INT   cctk_dummy_int;\
            CCTK_REAL  cctk_dummy_real;\
            int        cctk_dim=cctkGH->cctk_dim;\
            int       *cctk_gsh=cctkGH->cctk_gsh;\
            int       *cctk_lsh=cctkGH->cctk_lsh;\
            int       *cctk_lbnd=cctkGH->cctk_lbnd;\
            int       *cctk_ubnd=cctkGH->cctk_ubnd;\
            int       *cctk_lssh=cctkGH->cctk_lssh;\
            int       *cctk_from=cctkGH->cctk_from;\
            int       *cctk_to=cctkGH->cctk_to;\
            int       *cctk_bbox=cctkGH->cctk_bbox;\
            CCTK_REAL  cctk_delta_time=cctkGH->cctk_delta_time;\
            CCTK_REAL  cctk_time=cctkGH->cctk_time;\
            CCTK_REAL *cctk_delta_space=cctkGH->cctk_delta_space;\
            CCTK_REAL *cctk_origin_space=cctkGH->cctk_origin_space;\
            int       *cctk_levfac=cctkGH->cctk_levfac;\
            int        cctk_convlevel=cctkGH->cctk_convlevel;\
            int       *cctk_nghostzones=cctkGH->cctk_nghostzones;\
            int        cctk_iteration=cctkGH->cctk_iteration;


#define _USE_CCTK_CARGUMENTS\
            cctk_dummy_int=0;\
            cctk_dummy_int+=0;\
            cctk_dummy_real=0;\
            cctk_dummy_real+=0;\
            cctk_dummy_pointer=NULL;\
            cctk_dummy_pointer=(CCTK_REAL *)cctk_dummy_pointer;\
            cctk_dummy_int=cctk_dim;\
            cctk_dummy_pointer=(void *)cctk_gsh;\
            cctk_dummy_pointer=(void *)cctk_lsh;\
            cctk_dummy_pointer=(void *)cctk_lbnd;\
            cctk_dummy_pointer=(void *)cctk_ubnd;\
            cctk_dummy_pointer=(void *)cctk_lssh;\
            cctk_dummy_pointer=(void *)cctk_from;\
            cctk_dummy_pointer=(void *)cctk_to;\
            cctk_dummy_pointer=(void *)cctk_bbox;\
            cctk_dummy_real=cctk_delta_time;\
            cctk_dummy_real=cctk_time;\
            cctk_dummy_pointer=(void *)cctk_delta_space;\
            cctk_dummy_pointer=(void *)cctk_origin_space;\
            cctk_dummy_pointer=(void *)cctk_levfac;\
            cctk_dummy_pointer=(void *)cctk_convlevel;\
            cctk_dummy_pointer=(void *)cctk_nghostzones;\
            cctk_dummy_pointer=(void *)cctk_iteration;

#define _INITIALISE_CCTK_C2F
#define _DECLARE_CCTK_C2F 
#define _PASS_CCTK_C2F(xGH) &((xGH)->cctk_dim),\
                            (xGH)->cctk_gsh,(xGH)->cctk_lsh, \
                            (xGH)->cctk_lbnd,(xGH)->cctk_ubnd, \
                            (xGH)->cctk_lssh,\
                            (xGH)->cctk_from,(xGH)->cctk_to, \
                            (xGH)->cctk_bbox,\
                            &((xGH)->cctk_delta_time), \
                            &((xGH)->cctk_time), (xGH)->cctk_delta_space,\
                            (xGH)->cctk_origin_space,\
                            (xGH)->cctk_levfac,\
                            &((xGH)->cctk_convlevel),\
                            (xGH)->cctk_nghostzones,\
                            &((xGH)->cctk_iteration),\
                            (xGH)
#define _CCTK_C2F_PROTO     int *,\
                            int *,\
                            int *,int *, int *, int *, int *,int *,int *,\
                            CCTK_REAL *, CCTK_REAL *, CCTK_REAL *,\
                            CCTK_REAL *,\
                            int *,\
                            int *,\
                            int *,\
                            int *,\
                            cGH *

#define CCTK_STORAGESIZE(xGH, cctk_dim, group) \
                  (CCTK_QueryGroupStorage(xGH,group) ?\
                  (CCTK_ArrayGroupSize(xGH, cctk_dim, group)) : &(_cctk_one))


extern int _cctk_one;

#define CCTK_EQUALS(a,b) (CCTK_Equals((a),(b))==1)

#define CCTK_PASS_CTOC cctkGH

#define CCTK_DELTA_SPACE(x) (cctk_delta_space[x]/cctk_levfac[x])
#define CCTK_DELTA_TIME cctk_delta_time
#define CCTK_LSSH(stag,dim) cctk_lssh[stag*CCTK_NSTAGGER+dim]
#define CCTK_LSSH_IDX(stag,dim) (stag*CCTK_NSTAGGER+dim)

#define CCTK_WARN(a,b) CCTK_Warn(a,__LINE__,__FILE__,CCTK_THORNSTRING,b)

#define CCTKi_EXPCTERR(in,err,warnonerr,message) CCTKi_ExpectError(in,err,warnonerr,__LINE__,__FORTRANFILE__,CCTK_THORNSTRING,message)

#define CCTKi_EXPCTOK(in,ok,onerr,message) CCTKi_ExpectOK(in,ok,warnonerr,__LINE__,__FORTRANFILE__,CCTK_THORNSTRING,message)

#endif /*CCODE*/

#define CCTK_INFO(a) CCTK_Info(CCTK_THORNSTRING,(a))
#define CCTK_PARAMWARN(a) CCTK_ParamWarn(CCTK_THORNSTRING,(a))

/*
#define CCTK_MAKESTRING(x) CCTK_REALSTRING(x)
#define CCTK_REALSTRING(x) #x
#define CCTK_WARN(a,b) CCTK_Warn(a,CCTK_MAKESTRING(CCTK_THORN),b,__LINE__,__FILE__)
*/


#endif

