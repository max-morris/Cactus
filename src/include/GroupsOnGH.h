 /*@@
   @header    GroupsOnGH.h
   @date      Wed Apr 7 1999
   @author    Gabrielle Allen
   @desc 
   Prototypes and constants for group functions which use GH stucture.
   @enddesc 
   @version $Header$
 @@*/

#ifndef _GROUPSONGH_H_
#define _GROUPSONGH_H_

/* Prototypes */

#ifdef __cplusplus 
extern "C" {
#endif

void *CCTK_GetVarDataPtr_ByName(cGH *GH, char *fullvarname, int timelevel);

void *CCTK_GetVarDataPtr_ByIndex(cGH *GH, int varindex, int timelevel);

void *CCTK_GetVarDataPtr(cGH *GH, int varindex, char *fullvarname, int timelevel);

int *CCTK_ArrayGroupSize(cGH *GH, const char *group, int dim);

int CCTK_QueryGroupStorage(cGH *GH, const char *group);

#ifdef __cplusplus 
}
#endif

#endif
