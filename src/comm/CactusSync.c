#include <stdlib.h>
#include "cctk_Flesh.h"
#include "cctk_FortranString.h"
#include "cctk_Comm.h"
#include "cctk_Groups.h"


 /*@@
   @file      CactusSync.c
   @date      Thu Sep  18 14:27:18 1999
   @author    Gerd Lanfermann
   @desc 
   A collection of SyncGroup routines: Sync a group by the GROUP INDEX, 
   by the group's VARIABLE NAME, by the group's VARIABLE INDEX.

   It ends up calling CCTK_SyncGroup(GH,groupname), which is overloaded (currently
   by PUGH).
   
   @enddesc 
 @@*/

void CCTK_SyncGroupI(cGH *GH, int groupi) {
  char *groupname = CCTK_GroupName(groupi);

  CCTK_SyncGroup(GH,groupname);
  free(groupname);
}

void FMODIFIER FORTRAN_NAME(CCTK_SyncGroupI)(cGH *GH, int *groupi) {
  CCTK_SyncGroupI(GH, *groupi);
}



void CCTK_SyncGroupWithVar(cGH *GH, const char *varn) {
  int groupi;
  groupi = CCTK_GroupIndexFromVarI(CCTK_VarIndex(varn));
  CCTK_SyncGroupI(GH,groupi);
}

void FMODIFIER FORTRAN_NAME(CCTK_SyncGroupWithVar)(cGH *GH, ONE_FORTSTRING_ARG) {
  ONE_FORTSTRING_CREATE(varn);
  CCTK_SyncGroupWithVar(GH,varn);
  free(varn);
}



void CCTK_SyncGroupWithVarI(cGH *GH, int vari) {
  int groupi;
  groupi = CCTK_GroupIndexFromVarI(vari);
  CCTK_SyncGroupI(GH,groupi);
}

void FMODIFIER FORTRAN_NAME(CCTK_SyncGroupWithVarI)(cGH *GH, int *vari) {
  CCTK_SyncGroupWithVarI(GH,*vari);
}


