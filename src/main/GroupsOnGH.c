 /*@@
   @file      GroupsOnGH.c
   @date      Tues April 6
   @author    Gabrielle Allen
   @desc 
   GH specific Routines to deal with groups.
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "cctk_Flesh.h"
#include "cctk_Misc.h"
#include "cctk_Groups.h"
#include "cctk_WarnLevel.h"
#include "cctk_GroupsOnGH.h"
#include "cctk_Comm.h"

/*#define DEBUG_GROUPS*/

static char *rcsid = "$Header$";


 /*@@
   @routine    CCTK_VarDataPtr
   @date       Tue 6th April 1999
   @author     Gabrielle Allen
   @desc 
   Passes back a variable data pointer, given a full name and timelevel
   @enddesc 
   @calls     
   @history 
 
   @endhistory 

   @var        GH 
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @vcomment 
   @endvar 

   @var        varn
   @vdesc      Full name of the grid variable
   @vtype      char *
   @vio        in
   @vcomment   Format <implementation>::<variable>
   @endvar 

   @var        timelevel
   @vdesc      Index of timelevel on which data is required
   @vtype      int
   @vio        in
   @vcomment 
   @endvar 

   @returntype void *
   @returndesc Pointer to the required data, should be cast to required type
   @endreturndesc

   @version    $Header$

@@*/

void *CCTK_VarDataPtr(cGH *GH, int timelevel, char *varn)
{
  int index;
  void *retval=NULL;

  index = CCTK_VarIndex(varn);
  if (index >= 0)
  {
     retval = GH->data[index][timelevel];
  }
  else
     CCTK_Warn(1,__LINE__,__FILE__,"Cactus","Invalid index in CCTK_VarDataPtr");

#ifdef DEBUG_GROUPS
  printf("In CCTK_VarDataPtr\n----------------------------\n");
  printf("  Data pointer for %s (%d) is %x\n",varn,index,retval);
#endif

  return retval;

}

 /*@@
   @routine    CCTK_VarDataPtrI
   @date       Tue 6th April 1999
   @author     Gabrielle Allen
   @desc 
   Passes back a variable data pointer, given a variable index and timelevel
   @enddesc 
   @calls     
   @history 
      A check for a valid index (i>=0) included by Gerd Lanfermann
   @endhistory 

   @var        GH 
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @vcomment 
   @endvar 

   @var        vari
   @vdesc      Index of grid variable
   @vtype      int
   @vio        in
   @vcomment   Assumed to be in correct range
   @endvar 

   @var        timelevel
   @vdesc      Index of timelevel on which data is required
   @vtype      int
   @vio        in
   @vcomment 
   @endvar 

   @returntype void *
   @returndesc Pointer to the required data, should be cast to required type
   @endreturndesc

   @version    $Header$

@@*/

void *CCTK_VarDataPtrI(cGH *GH, int timelevel, int vari)
{ 
  if (vari < 0) 
    CCTK_Warn(1,__LINE__,__FILE__,"Cactus",
              "WARNING: calling CCTK_VarDataPtrI with negative index");
  return GH->data[vari][timelevel];
}

 /*@@
   @routine    CCTK_VarDataPtrB
   @date       Tue 6th April 1999
   @author     Gabrielle Allen
   @desc 
   Passes back a variable data pointer, given either a  variable index 
   or a full name and timelevel
   @enddesc 
   @calls     
   @history 
 
   @endhistory 
   @var        GH 
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @vcomment 
   @endvar 

   @var        vari
   @vdesc      Index of grid variable
   @vtype      int
   @vio        in
   @vcomment   Assumed to be in correct range
   @endvar 

   @var        varn
   @vdesc      Full name of the grid variable
   @vtype      char *
   @vio        in
   @vcomment   Format <implementation>::<variable>
   @endvar 

   @var        timelevel
   @vdesc      Index of timelevel on which data is required
   @vtype      int
   @vio        in
   @vcomment 
   @endvar 

   @returntype void *
   @returndesc Pointer to the required data, should be cast to required type
   @endreturndesc

   @version    $Header$

@@*/
void *CCTK_VarDataPtrB(cGH *GH, int timelevel, int vari, char *varn)
{
  if (varn) 
  {
    return CCTK_VarDataPtr(GH, timelevel, varn);
  }
  else
  {
    return CCTK_VarDataPtrI(GH, timelevel, vari);
  }
}

 /*@@
   @routine    CCTK_EnableGroupCommI
   @date       Sat Feb 13 17:06:30 1999
   @author     Tom Goodale
   @desc 
   Enables communication for a group based upon its name.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_EnableGroupCommI(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GroupName(group);
  if(group_name)
  {
#ifdef DEBUG_GROUPS
    printf("Turning on comm in %s for group %s (%d)\n",__FILE__,group_name,group);
#endif
    retcode = CCTK_EnableGroupComm(GH, group_name);

    free(group_name);
  }
  else
  {
    retcode = 0;
  }

  return retcode;
}

 /*@@
   @routine    CCTK_EnableGroupStorageI
   @date       Sat Feb 13 17:06:30 1999
   @author     Tom Goodale
   @desc 
   Enables storage for a group based upon its name.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_EnableGroupStorageI(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GroupName(group);
  if(group_name)
  {
#ifdef DEBUG_GROUPS
    printf("Turning on storage in %s for group %s (%d)\n",__FILE__,group_name,group);
#endif
    retcode = CCTK_EnableGroupStorage(GH, group_name);

    free(group_name);
  }
  else
  {
    retcode = 0;
  }

  return retcode;
}

 /*@@
   @routine    CCTK_DisableGroupCommI
   @date       Sat Feb 13 17:06:30 1999
   @author     Tom Goodale
   @desc 
   Routine to switch communication off for a group based upon its index
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_DisableGroupCommI(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GroupName(group);
  if(group_name)
  {
#ifdef DEBUG_GROUPS
    printf("Turning off comm in %s for group %s (%d)\n",__FILE__,group_name,group);
#endif
    retcode = CCTK_DisableGroupComm(GH, group_name);

    free(group_name);
  }
  else
  {
    retcode = 0;
  }

  return retcode;
}

 /*@@
   @routine    CCTK_DisableGroupStorageI
   @date       Sat Feb 13 17:06:30 1999
   @author     Tom Goodale
   @desc 
   Routine to switch storage off for a group based upon its index
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_DisableGroupStorageI(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GroupName(group);
  if(group_name)
  {
#ifdef DEBUG_GROUPS
    printf("Turning off storage in %s for group %s (%d)\n",__FILE__,group_name,group);
#endif
    retcode = CCTK_DisableGroupStorage(GH, group_name);

    free(group_name);
  }
  else
  {
    retcode = 0;
  }

  return retcode;
}


int *CCTK_ArrayGroupSizeI(cGH *GH, int dir, int groupi)
{
  return CCTK_ArrayGroupSizeB(GH,dir,groupi,NULL);
}

int *CCTK_ArrayGroupSize(cGH *GH, int dir, const char *groupn)
{
  return CCTK_ArrayGroupSizeB(GH,dir,-1,groupn);
}

int CCTK_QueryGroupStorageI(cGH *GH, int groupi)
{
  return CCTK_QueryGroupStorageB(GH,groupi,NULL);
}

int CCTK_QueryGroupStorage(cGH *GH, const char *groupn)
{
  return CCTK_QueryGroupStorageB(GH, -1, groupn);
}
