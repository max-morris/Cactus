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

typedef struct GROUPDYNAMICDATA
{
  int dim;
  const int *gsh;
  const int *lsh;
  const int *lbnd;
  const int *ubnd;
  const int *bbox;
  const int *nghostzones;
} cGroupDynamicData;

/* Prototypes */

#ifdef __cplusplus 
extern "C" {
#endif

void *CCTK_VarDataPtr(cGH *GH, int timelevel, const char *fullvarname);
void *CCTK_VarDataPtrI(cGH *GH, int timelevel, int varindex);
void *CCTK_VarDataPtrB(cGH *GH, int timelevel, int varindex, char *fullvarname);

int CCTK_DisableGroupStorageI(void *GH, int group);
int CCTK_DisableGroupCommI(void *GH, int group);
int CCTK_EnableGroupStorageI(void *GH, int group);
int CCTK_EnableGroupCommI(void *GH, int group);

int CCTK_GrouplbndGN(cGH *GH, int dim, int *lbnd, const char *groupname);
int CCTK_GrouplbndVN(cGH *GH, int dim, int *lbnd, const char *varname);
int CCTK_GrouplbndGI(cGH *GH, int dim, int *lbnd, int groupindex);
int CCTK_GrouplbndVI(cGH *GH, int dim, int *lbnd, int varindex);

int CCTK_GroupubndGN(cGH *GH, int dim, int *ubnd, const char *groupname);
int CCTK_GroupubndVN(cGH *GH, int dim, int *ubnd, const char *varname);
int CCTK_GroupubndGI(cGH *GH, int dim, int *ubnd, int groupindex);
int CCTK_GroupubndVI(cGH *GH, int dim, int *ubnd, int varindex);

int CCTK_GrouplshGN(cGH *GH, int dim, int *lsh, const char *groupname);
int CCTK_GrouplshVN(cGH *GH, int dim, int *lsh, const char *varname);
int CCTK_GrouplshGI(cGH *GH, int dim, int *lsh, int groupindex);
int CCTK_GrouplshVI(cGH *GH, int dim, int *lsh, int varindex);

int CCTK_GroupgshGN(cGH *GH, int dim, int *gsh, const char *groupname);
int CCTK_GroupgshVN(cGH *GH, int dim, int *gsh, const char *varname);
int CCTK_GroupgshGI(cGH *GH, int dim, int *gsh, int groupindex);
int CCTK_GroupgshVI(cGH *GH, int dim, int *gsh, int varindex);

int CCTK_GroupbboxGI(cGH *cctkGH, int size, int *bbox, int groupindex);
int CCTK_GroupbboxGN(cGH *cctkGH, int size, int *bbox, const char *groupname);
int CCTK_GroupbboxVI(cGH *cctkGH, int size, int *bbox, int varindex);
int CCTK_GroupbboxVN(cGH *cctkGH, int size, int *bbox, const char *varname);

int CCTK_GroupnghostzonesGN(cGH *GH, int dim, int *nghostzones, const char *groupname);
int CCTK_GroupnghostzonesVN(cGH *GH, int dim, int *nghostzones, const char *varname);
int CCTK_GroupnghostzonesGI(cGH *GH, int dim, int *nghostzones, int groupindex);
int CCTK_GroupnghostzonesVI(cGH *GH, int dim, int *nghostzones, int varindex);

#ifdef __cplusplus 
}
#endif
 
#endif
