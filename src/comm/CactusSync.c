 /*@@
   @file      CactusSync.c
   @date      Thu Sep  18 14:27:18 1999
   @author    Gerd Lanfermann
   @desc
              A collection of SyncGroup routines:
              Sync a group by the GROUP INDEX, by the group's VARIABLE NAME,
              by the group's VARIABLE INDEX.

              It ends up calling CCTK_SyncGroup(GH,groupname),
              which is overloaded (currently by PUGH).
   @enddesc
   @version   $Id$
 @@*/

#include <stdlib.h>

#include "cctk_Flesh.h"
#include "cctk_FortranString.h"
#include "cctk_Comm.h"
#include "cctk_Groups.h"
#include "cctk_Sync.h"

static char *rcsid = "$Header$";
CCTK_FILEVERSION(comm_CactusSync_c)


/********************************************************************
 ********************    External Routines   ************************
 ********************************************************************/
/* prototypes for external C routines are declared in header cctk_Groups.h
   here only follow the fortran wrapper prototypes */
void CCTK_FCALL CCTK_FNAME (CCTK_SyncGroupI)
                           (cGH *GH, const int *group);
void CCTK_FCALL CCTK_FNAME (CCTK_SyncGroupWithVar)
                           (cGH *GH, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (CCTK_SyncGroupWithVarI)
                           (cGH *GH, const int *var);


 /*@@
   @routine    CCTK_SyncGroupI
   @date       Thu Sep  18 14:27:18 1999
   @author     Gerd Lanfermann
   @desc
               Synchronizes a group given by its index.
   @enddesc
   @calls      CCTK_GroupName
               CCTK_SyncGroup

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @endvar
   @var        group
   @vdesc      group index
   @vtype      int
   @vio        in
   @endvar
@@*/
void CCTK_SyncGroupI (cGH *GH,
                      int group)
{
  char *groupname;


  groupname = CCTK_GroupName (group);

  if (groupname)
  {
    CCTK_SyncGroup (GH, groupname);

    free (groupname);
  }
}

void CCTK_FCALL CCTK_FNAME (CCTK_SyncGroupI)
                           (cGH *GH, const int *group)
{
  CCTK_SyncGroupI (GH, *group);
}


 /*@@
   @routine    CCTK_SyncGroupWithVar
   @date       Thu Sep  18 14:27:18 1999
   @author     Gerd Lanfermann
   @desc
               Synchronizes a group which contains the given variable
               (given by its name).
   @enddesc
   @calls      CCTK_GroupIndexFromVarI
               CCTK_VarIndex
               CCTK_SyncGroupI

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @endvar
   @var        varname
   @vdesc      full variable name
   @vtype      const char *
   @vio        in
   @endvar
@@*/
void CCTK_SyncGroupWithVar (cGH *GH,
                            const char *varname)
{
  CCTK_SyncGroupI (GH, CCTK_GroupIndexFromVarI (CCTK_VarIndex (varname)));
}

void CCTK_FCALL CCTK_FNAME (CCTK_SyncGroupWithVar)
                           (cGH *GH, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (varname);
  CCTK_SyncGroupWithVar (GH, varname);
  free (varname);
}


 /*@@
   @routine    CCTK_SyncGroupWithVarI
   @date       Thu Sep  18 14:27:18 1999
   @author     Gerd Lanfermann
   @desc
               Synchronizes a group which contains the given variable
               (given by its index).
   @enddesc
   @calls      CCTK_GroupIndexFromVarI
               CCTK_SyncGroupI

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @endvar
   @var        var
   @vdesc      variable index
   @vtype      int
   @vio        in
   @endvar
@@*/
void CCTK_SyncGroupWithVarI (cGH *GH,
                             int var)
{
  CCTK_SyncGroupI (GH, CCTK_GroupIndexFromVarI (var));
}

void CCTK_FCALL CCTK_FNAME (CCTK_SyncGroupWithVarI)
                           (cGH *GH, const int *var)
{
  CCTK_SyncGroupWithVarI (GH, *var);
}


 /*@@
   @routine    CCTK_SyncGroupsI
   @date       Thu Jan 27 18:00:15 2000
   @author     Tom Goodale
   @desc
               Synchronises a list of groups given by their group indices.
   @enddesc
   @calls      CCTK_SyncGroupI

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @endvar
   @var        n_groups
   @vdesc      number of groups to synchronize
   @vtype      int
   @vio        in
   @endvar
   @var        groups
   @vdesc      list of group indices
   @vtype      const int *
   @vio        in
   @endvar

   @returntype int
   @returndesc
               0
   @endreturndesc
@@*/
int CCTK_SyncGroupsI (cGH *GH,
                      int n_groups,
                      const int *groups)
{
  int i;


  for (i = 0; i < n_groups; i++)
  {
    CCTK_SyncGroupI (GH, groups[i]);
  }

  return (0);
}
