 /*@@
   @header    cctk_GroupsOnGH.h
   @date      Wed Apr 7 1999
   @author    Gabrielle Allen
   @desc 
   Prototypes and constants for group functions which use GH stucture.
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTK_GROUPSONGH_H_
#define _CCTK_GROUPSONGH_H_

/* Prototypes */

#ifdef __cplusplus 
extern "C" {
#endif

void *CCTK_VarDataPtr(cGH *GH, int timelevel, char *fullvarname);

void *CCTK_VarDataPtrI(cGH *GH, int timelevel, int varindex);

void *CCTK_VarDataPtrB(cGH *GH, int timelevel, int varindex, char *fullvarname);

int CCTK_DisableGroupStorageI(void *GH, int group);
int CCTK_DisableGroupCommI(void *GH, int group);
int CCTK_EnableGroupStorageI(void *GH, int group);
int CCTK_EnableGroupCommI(void *GH, int group);

#ifdef __cplusplus 
}
#endif

#endif
