 /*@@
   @file      rfrInterface.c
   @date      Wed Feb  3 14:42:24 1999
   @author    Tom Goodale
   @desc 
   Routine used by cactus to talk to the rfr.
   @enddesc 
 @@*/

#include "flesh.h"
#include "GHExtensions.h"
#include "Groups.h"
#include "CactusrfrInterface.h"

static char *rcsid = "$Id$";


 /*@@
   @routine    CCTK_rfrTraverse
   @date       Wed Feb  3 14:45:57 1999
   @author     Tom Goodale
   @desc 
   Calls the rfrTraverse, then deals with extensions.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_rfrTraverse(cGH *GH, int rfrpoint)
{
  rfrTraverse(GH->rfr_top, GH, rfrpoint);
  
  CCTK_rfrTraverseGHExtensions(GH, rfrpoint);

  return 0;
}


int CCTK_rfrStorageOn(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GetGroupName(group);
  if(group_name)
  {
    retcode = CCTK_EnableGroupStorage(GH, group_name);

    free(group_name);
  }
  else
  {
    retcode = 0;
  }

  return retcode;
}

int CCTK_rfrStorageOff(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GetGroupName(group);
  if(group_name)
  {
    retcode = CCTK_DisableGroupStorage(GH, group_name);

    free(group_name);
  }
  else
  {
    retcode = 0;
  }

  return retcode;
}

int CCTK_rfrCommunicationOn(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GetGroupName(group);
  if(group_name)
  {
    retcode = CCTK_EnableGroupComm(GH, group_name);

    free(group_name);
  }
  else
  {
    retcode = 0;
  }

  return retcode;
}

int CCTK_rfrCommunicationOff(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GetGroupName(group);
  if(group_name)
  {
    retcode = CCTK_DisableGroupComm(GH, group_name);

    free(group_name);
  }
  else
  {
    retcode = 0;
  }

  return retcode;
}

int CCTK_rfrTriggerable(int variable)
{
  return 0;
}

int CCTK_rfrTriggerSaysGo(int variable)
{
  return 0;
}

int CCTK_rfrTriggerAction(void *GH, int group)
{
  return 0;
}

int CCTK_rfrCallFunc(void *GH, int language, void *function)
{

  void (*calledfunc)(void *);

  calledfunc = (void (*)(void *))function;

  /* Call the function. */
  
  calledfunc(GH);

  return 0;
}
