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

void *CCTK_GetVarDataPtr_ByName(cGH *GH, int timelevel, char *fullvarname);

void *CCTK_GetVarDataPtr_ByIndex(cGH *GH, int timelevel, int varindex);

void *CCTK_GetVarDataPtr(cGH *GH, int timelevel, int varindex, char *fullvarname);

#ifdef __cplusplus 
}
#endif

#endif
