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
   @version $Header$
 @@*/

#include <stdlib.h>

#include "cctk_Flesh.h"
#include "cctk_FortranString.h"
#include "cctk_Comm.h"
#include "cctk_Groups.h"
#include "cctk_Sync.h"

static char *rcsid = "$Header$";

 /*@@
   @routine    CCTK_SyncGroupI
   @date       Thu Sep  18 14:27:18 1999
   @author     Gerd Lanfermann
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTK_SyncGroupI(cGH *GH, 
                     int groupi) 
{
  char *groupname = CCTK_GroupName(groupi);

  CCTK_SyncGroup(GH,groupname);

  free(groupname);
}

void FMODIFIER FORTRAN_NAME(CCTK_SyncGroupI)(cGH *GH, int *groupi) 
{
  CCTK_SyncGroupI(GH, *groupi);
}



 /*@@
   @routine    CCTK_SyncGroupWithVar
   @date       Thu Sep  18 14:27:18 1999
   @author     Gerd Lanfermann
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTK_SyncGroupWithVar(cGH *GH, 
                           const char *varn) 
{
  int groupi;

  groupi = CCTK_GroupIndexFromVarI(CCTK_VarIndex(varn));

  CCTK_SyncGroupI(GH,groupi);
}

void FMODIFIER FORTRAN_NAME(CCTK_SyncGroupWithVar)(cGH *GH, ONE_FORTSTRING_ARG) 
{
  ONE_FORTSTRING_CREATE(varn);
  CCTK_SyncGroupWithVar(GH,varn);
  free(varn);
}



 /*@@
   @routine    CCTK_SyncGroupWithVarI
   @date       Thu Sep  18 14:27:18 1999
   @author     Gerd Lanfermann
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTK_SyncGroupWithVarI(cGH *GH, 
                            int vari) 
{
  int groupi;

  groupi = CCTK_GroupIndexFromVarI(vari);

  CCTK_SyncGroupI(GH,groupi);
}

void FMODIFIER FORTRAN_NAME(CCTK_SyncGroupWithVarI)(cGH *GH, int *vari) 
{
  CCTK_SyncGroupWithVarI(GH,*vari);
}

 /*@@
   @routine    CCTK_SyncGroupsI
   @date       Thu Jan 27 18:00:15 2000
   @author     Tom Goodale
   @desc 
   Synchronises a list of groups by group index. 
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_SyncGroupsI(cGH *GH, 
                     int n_groups, 
                     int *groups)
{
  int i;

  for(i = 0; i < n_groups; i++)
  {
    CCTK_SyncGroupI(GH, i);
  }

  return 0;
}

