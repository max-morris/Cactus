 /*@@
   @header    cctk_Comm.h
   @date      Sat Feb  13 19:42:29 1999
   @author    Tom Goodale
   @desc 
   Header defining the variables holding the overloaded communication functions
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTK_COMM_H_
#define _CCTK_COMM_H_

#include "OverloadMacros.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Define the prototypes for the functions. */
#define OVERLOADABLE(name) OVERLOADABLE_PROTOTYPE(name)

#include "CommOverloadables.h"

#undef OVERLOADABLE

/* Define the prototypes for the overloading functions. */
#define OVERLOADABLE(name) OVERLOADABLE_OVERLOADPROTO(name)

#include "CommOverloadables.h"

#undef OVERLOADABLE

int CCTK_QueryGroupStorage(cGH *,const char *);
int CCTK_QueryGroupStorageI(cGH *,int);
const int *CCTK_ArrayGroupSize(cGH *,int, const char *);
const int *CCTK_ArrayGroupSizeI(cGH *,int,int);

#ifdef __cplusplus
}
#endif

#endif




