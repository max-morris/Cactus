 /*@@
   @header    cctk_Interp.h
   @date      July 07 1999
   @author    Thomas Radke
   @desc 
              Header file for using interpolation operators
   @enddesc 
   @history
   @date      July 07 1999
   @author    Thomas Radke
   @hdesc     Just copied from cctk_Reduction.h
   @endhistory
 @@*/


#ifndef _CCTK_INTERP_H_
#define _CCTK_INTERP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define REGISTER_INTERP_ARGLIST  \
                         cGH *,  \
                         int,    \
                         int,    \
                         int,    \
                         int,    \
                         int *,  \
                         void **,\
                         int *,  \
                         void *, \
                         void *, \
                         void **,\
                         int *,  \
                         void **,\
                         int *    

int CCTK_InterpArray(cGH *GH,
                int operation_handle,
                int nPoints,
                int nCoords,
                int nInFields,
                int nOutFields,
                ...);

int CCTK_GetInterpHandle(const char *interp);

int CCTK_RegisterInterpOperator(int (*function)(REGISTER_INTERP_ARGLIST),
                                const char *name);

int CCTK_Interp(cGH *GH,
                 int operation_handle,
                 int nPoints,
                 int nDims,
                 int nInFields,
                 int nOutFields,
                 ...);

#ifdef __cplusplus
           }
#endif

#endif  /* _INTERP_H_ */
