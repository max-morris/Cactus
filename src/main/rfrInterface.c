 /*@@
   @file      rfrInterface.c
   @date      Wed Feb  3 14:42:24 1999
   @author    Tom Goodale
   @desc 
   Routine used by cactus to talk to the rfr.
   @enddesc 
 @@*/

/*#define RFRDEBUG*/

#include <stdio.h>
#include <stdlib.h>

#include "flesh.h"
#include "GHExtensions.h"
#include "Groups.h"
#include "CactusrfrInterface.h"
#include "rfr_constants.h"
#include "CactusCommFunctions.h"
#include "IOMethods.h"
#include "cctk_parameters.h"
#include "rfrInterface.h"

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
   @routine    rfrPrintTree
   @date       14 Mar 1999
   @author     Gabrielle Allen
   @desc 
               Prints the order of the RFR calling tree
   @enddesc 
   @calls      
   @history 
   @endhistory
 
   @var        
   @vdesc      
   @vtype      
   @vio        
   @vcomment 
   @endvar 

   @returntype int
   @returndesc
      0 = routine did not print rfr tree
      1 = routine did print rfr tree
   @endreturndesc

   @version    $Header$
@@*/

int CCTK_rfrPrintTree(cGH *GH,void *rfr_top)
{
  DECLARE_CCTK_PARAMETERS

  static int showed_tree = 0;

  /* Return if the users doesn't want to see the tree */
  if (!cctk_show_rfr_tree || cctk_brief_output) return 0;

  if (!showed_tree) 
  {

    showed_tree = 1;

    printf ("\nEvolution tree from the RFR\n\n");

    rfrPrintDescs(rfr_top,GH,CCTK_BASEGRID,"CCTK_BASEGRID","");
    
    rfrPrintDescs(rfr_top,GH,CCTK_INITIAL0,"CCTK_INITIAL0","");
    rfrPrintDescs(rfr_top,GH,CCTK_INITIAL, "CCTK_INITIAL","");
    rfrPrintDescs(rfr_top,GH,CCTK_INITIAL2,"CCTK_INITIAL2","");
    rfrPrintDescs(rfr_top,GH,CCTK_INITIAL3,"CCTK_INITIAL3","");
    rfrPrintDescs(rfr_top,GH,CCTK_INITIAL4,"CCTK_INITIAL4","");
    rfrPrintDescs(rfr_top,GH,CCTK_INITIAL5,"CCTK_INITIAL5","");
    rfrPrintDescs(rfr_top,GH,CCTK_INITIAL6,"CCTK_INITIAL6","");
    rfrPrintDescs(rfr_top,GH,CCTK_INITIAL7,"CCTK_INITIAL7","");
    rfrPrintDescs(rfr_top,GH,CCTK_INITIAL8,"CCTK_INITIAL8","");
    rfrPrintDescs(rfr_top,GH,CCTK_INITIAL9,"CCTK_INITIAL9","");
    printf ("\n  (Don't forget CCTK_POSTSTEP* is here too...)\n");
    printf ("\n  do loop over timesteps\n");
    rfrPrintDescs(rfr_top,GH,CCTK_PRESTEP,"CCTK_PRESTEP","   ");
    rfrPrintDescs(rfr_top,GH,CCTK_PRESTEP2,"CCTK_PRESTEP2","   ");
    rfrPrintDescs(rfr_top,GH,CCTK_PRESTEP3,"CCTK_PRESTEP3","   ");
    rfrPrintDescs(rfr_top,GH,CCTK_PRESTEP4,"CCTK_PRESTEP4","   ");
    rfrPrintDescs(rfr_top,GH,CCTK_PRESTEP5,"CCTK_PRESTEP5","   ");
    
    rfrPrintDescs(rfr_top,GH,CCTK_EVOL,"CCTK_EVOL","   ");
    rfrPrintDescs(rfr_top,GH,CCTK_BOUND,"CCTK_BOUND","   ");
    
    printf ("     t = t+dt\n");
    
    rfrPrintDescs(rfr_top,GH,CCTK_POSTSTEP,"CCTK_POSTSTEP","   ");
    rfrPrintDescs(rfr_top,GH,CCTK_POSTSTEP2,"CCTK_POSTSTEP2","   ");
    rfrPrintDescs(rfr_top,GH,CCTK_POSTSTEP3,"CCTK_POSTSTEP3","   ");
    rfrPrintDescs(rfr_top,GH,CCTK_POSTSTEP4,"CCTK_POSTSTEP4","   ");
    rfrPrintDescs(rfr_top,GH,CCTK_POSTSTEP5,"CCTK_POSTSTEP5","   ");
    rfrPrintDescs(rfr_top,GH,CCTK_POSTSTEP6,"CCTK_POSTSTEP6","   ");
    rfrPrintDescs(rfr_top,GH,CCTK_POSTSTEP7,"CCTK_POSTSTEP7","   ");
    rfrPrintDescs(rfr_top,GH,CCTK_POSTSTEP8,"CCTK_POSTSTEP8","   ");
    rfrPrintDescs(rfr_top,GH,CCTK_POSTSTEP9,"CCTK_POSTSTEP9","   ");
    rfrPrintDescs(rfr_top,GH,CCTK_POSTSTEP10,"CCTK_POSTSTEP10","   ");
    
    printf ("     if (time-for-output)\n");
    rfrPrintDescs(rfr_top,GH,CCTK_ANALYSIS,"CCTK_ANALYSIS","      ");
    printf ("     endif\n");
    printf ("   enddo\n");
    printf ("End of tree from RFR\n\n");
  }

  return 1;

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

  group_name = CCTK_GroupName(group);

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

  group_name = CCTK_GroupName(group);
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

  group_name = CCTK_GroupName(group);
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

  group_name = CCTK_GroupName(group);
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
  if (entrypoint == CCTK_ANALYSIS)
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
