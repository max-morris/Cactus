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

void CCTK_SyncGroupI(cGH *GH, int gi) {
  char *groupname = CCTK_GroupName(gi);

  CCTK_SyncGroup(GH,groupname);
  free(groupname);
}

void FMODIFIER FORTRAN_NAME(CCTK_SyncGroupI)(cGH *GH, int *gi) {
  CCTK_SyncGroupI(GH, *gi);
}



void CCTK_SyncGroupWithVar(cGH *GH, const char *impvarname) {
  int gi;
  gi = CCTK_GroupIndexFromVarI(CCTK_VarIndex(impvarname));
  CCTK_SyncGroupI(GH,gi);
}

void FMODIFIER FORTRAN_NAME(CCTK_SyncGroupWithVar)(cGH *GH, ONE_FORTSTRING_ARG) {
  ONE_FORTSTRING_CREATE(impvarname);
  CCTK_SyncGroupWithVar(GH,impvarname);
  free(impvarname);
}



void CCTK_SyncGroupWithVarI(cGH *GH, int vi) {
  int gi;
  gi = CCTK_GroupIndexFromVarI(vi);
  CCTK_SyncGroupI(GH,gi);
}

void FMODIFIER FORTRAN_NAME(CCTK_SyncGroupWithVarI)(cGH *GH, int *vi) {
  CCTK_SyncGroupWithVarI(GH,*vi);
}


