 /*@@
   @file      CactusDefaultInitialise.c
   @date      Tue Sep 29 12:45:04 1998
   @author    Tom Goodale
   @desc 
   Default cactus initialisation routine.
   @enddesc 
 @@*/


#include <stdio.h>
#include <stdlib.h>

#include "flesh.h"
#include "CactusMainDefaults.h"
#include "CactusCommFunctions.h"
#include "parameters.h"
#include "rfr_constants.h"
#include "Dummies.h"

static char *rcsid = "$Id$";


 /*@@
   @routine    CactusDefaultInitialise
   @date       Tue Sep 29 12:45:04 1998
   @author     Tom Goodale
   @desc 
   Default initialisation routine.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CactusDefaultInitialise(tFleshConfig *config)
{
  cGH *GH;
  int convergence_level;

  CactusStartTimer(config->timer[INITIALISATION]);

  CactusResetTimer(config->timer[ELLIPTIC]);

  convergence_level = 0;
  while((GH = CCTK_SetupGH(config, convergence_level)))
  {
    CCTK_AddGH(config, convergence_level, GH);

    Cactus_InitialiseGH(GH);

    convergence_level++;
  };

  CactusStopTimer(config->timer[INITIALISATION]);
  
  return 0;
}



 /*@@
   @routine    CactusDefaultShutdown
   @date       Tue Sep 29 12:45:04 1998
   @author     Tom Goodale
   @desc 
   DEfault shutdown routine.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CactusDefaultShutdown(tFleshConfig *config)
{
  printf("I'm in the default shutdown routine\n");
  
  return 0;
}


 /*@@
   @routine    Cactus_InitialiseGH
   @date       Mon Feb  1 12:13:09 1999
   @author     Tom Goodale
   @desc 
   Responsible for initialising a GH.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int Cactus_InitialiseGH(cGH *GH)
{

  int Rstep;

  /*
  SetupFortranArrays(GH);
  */
  
  
  /* Setup the rfr_top on this GH */

  GH->rfr_top = NULL;

  rfrInitTree(&(GH->rfr_top), 
	      CCTK_DummyStorageOn,
	      CCTK_DummyStorageOff,
	      CCTK_DummyCommunicationOn,
	      CCTK_DummyCommunicationOff,
	      CCTK_DummyTriggerable,
	      CCTK_DummyTriggerSaysGo,
	      CCTK_DummyTriggerAction,
	      CCTK_DummyCallFunc);

  /* Do the rfr initialisation on this GH */
  CCTK_BindingsScheduleRegister("RFRINIT", (void *)GH);


  /* Initialise all the extensions. */
  CCTK_InitGHExtensions(GH);


  /* Do various rfr traversals.  Will tidy up later. */

  CCTK_rfrTraverse(GH, CACTUS_BASEGRID); 
  CCTK_rfrTraverse(GH,CACTUS_INITIAL0);

  /* Loops like this should go eventually... */
  for (Rstep = CACTUS_INITIAL; Rstep <= CACTUS_INITIAL9; Rstep++)
  {
    CCTK_rfrTraverse(GH,Rstep);
  }

  /* Ignore checkpointing for now.
   * CCTK_rfrTraverse(GH,CACTUS_RECOVER);
   * CCTK_rfrTraverse(GH,CACTUS_CPINITIAL);
   */

  for (Rstep = CACTUS_POSTSTEP; Rstep <= CACTUS_POSTSTEP10; Rstep++)
  {
    CCTK_rfrTraverse(GH,Rstep);
  }

}
