 /*@@
   @file      rfrInterface.c
   @date      Wed Feb  3 14:42:24 1999
   @author    Tom Goodale
   @desc 
   Routine used by cactus to talk to the rfr.
   @enddesc 
 @@*/

/*#define RFRDEBUG*/

#include "flesh.h"
#include "GHExtensions.h"
#include "Groups.h"
#include "CactusrfrInterface.h"
#include "rfr_constants.h"
#include "CactusCommFunctions.h"
#include "IOMethods.h"

static char *rcsid = "$Header$";


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
  
  CCTK_rfrTraverseGHExtensions(GH, rfrpoint);

  return 0;
}


 /*@@
   @routine    CCTK_rfrStorageOn
   @date       Sat Feb 13 17:06:30 1999
   @author     Tom Goodale
   @desc 
   Routine called by the rfr to switch storage on for a group
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_rfrStorageOn(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GetGroupName(group);

  if(group_name)
  {
#ifdef RFRDEBUG
    printf("Turning on storage in rfrInterface.c for group %s (%d)\n",group_name,group);
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
   @routine    CCTK_rfrStorageOff
   @date       Sat Feb 13 17:06:30 1999
   @author     Tom Goodale
   @desc 
   Routine called by the rfr to switch storage off for a group
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_rfrStorageOff(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GetGroupName(group);
  if(group_name)
  {
#ifdef RFRDEBUG
    printf("Turning off storage in rfrInterface.c for group %s (%d)\n",group_name,group);
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

 /*@@
   @routine    CCTK_rfrCommunicationOn
   @date       Sat Feb 13 17:06:30 1999
   @author     Tom Goodale
   @desc 
   Routine called by the rfr to switch communication on for a group
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_rfrCommunicationOn(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GetGroupName(group);
  if(group_name)
  {
#ifdef RFRDEBUG
    printf("Turning on comm in rfrInterface.c for group %s (%d)\n",group_name,group);
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
   @routine    CCTK_rfrCommunicationOff
   @date       Sat Feb 13 17:06:30 1999
   @author     Tom Goodale
   @desc 
   Routine called by the rfr to switch communication off for a group
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_rfrCommunicationOff(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GetGroupName(group);
  if(group_name)
  {
#ifdef RFRDEBUG
    printf("Turning off comm in rfrInterface.c for group %s (%d)\n",group_name,group);
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
   @routine    CCTK_rfrTriggerable
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc 
   Returns true if this rfr entry point should be triggerable
   for some event, otherwise returns false in which case none 
   of the triggers stuff is done.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     entrypoint 
   @vdesc   describes the entrypoint of the RFR
   @vtype   int
   @vio     in
   @vcomment RFR entrypoint macros are in src/include/rfr_constants.h
   @endvar 
@@*/

int CCTK_rfrTriggerable(int entrypoint)
{
  if (entrypoint == CACTUS_ANALYSIS)
    {
      return 1;
    }
  else 
    return 0;
}



 /*@@
   @routine    CCTK_rfrCallFunc
   @date       Sat Feb 13 17:08:39 1999
   @author     Tom Goodale
   @desc 
   Routine called by the rfr to call a function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_rfrCallFunc(void *GH, int language, void *function)
{

  void (*calledfunc)(void *);

  calledfunc = (void (*)(void *))function;

  /* Call the function. */
  
  calledfunc(GH);

  return 0;
}
