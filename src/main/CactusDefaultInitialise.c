 /*@@
   @file      CactusDefaultInitialise.c
   @date      Tue Sep 29 12:45:04 1998
   @author    Tom Goodale
   @desc 
   Default cactus initialisation routine.
   @enddesc 
 @@*/

/*#define DEBUG_CCTK*/

#include <stdio.h>
#include <stdlib.h>

#include "flesh.h"
#include "CactusMainDefaults.h"
#include "CactusCommFunctions.h"
#include "GHExtensions.h"
#include "rfr_constants.h"
#include "CactusrfrInterface.h"
#include "CCTK_Bindings.h"
#include "rfrInterface.h"

#include "cctk_parameters.h"

static char *rcsid = "$Id$";

/* Local function prototypes */
int CactusInitialiseGH(cGH *GH);


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

  CactusResetTimer(config->timer[INITIALISATION]);
  CactusResetTimer(config->timer[EVOLUTION]);
  CactusResetTimer(config->timer[ELLIPTIC]);

  CactusStartTimer(config->timer[INITIALISATION]);

  convergence_level = 0;
  while((GH = CCTK_SetupGH(config, convergence_level)))
  {
    CCTK_AddGH(config, convergence_level, GH);

    CactusInitialiseGH(GH);

    convergence_level++;
  };

  CactusStopTimer(config->timer[INITIALISATION]);
  
  return 0;
}


 /*@@
   @routine    CactusInitialiseGH
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
int CactusInitialiseGH(cGH *GH)
{
  
  DECLARE_CCTK_PARAMETERS

  int Rstep;

  /*
  SetupFortranArrays(GH);
  */
  
  /* Initialise time */
  GH->cctk_time = cctk_initial_time;

  /* Initialise iteration number */
  GH->cctk_iteration = 0;
 
#ifdef DEBUG_CCTK
  CCTK_PRINTSEPARATOR
  printf("In Cactus_Initialise\n--------------------\n");
  printf("  Initializing GH->cctk_time = %f\n",GH->cctk_time);
  printf("  Initializing GH->cctk_iteration = %u\n",GH->cctk_iteration);
  CCTK_PRINTSEPARATOR
#endif
  
  /* Setup the rfr_top on this GH */

  GH->rfr_top = NULL;

  rfrInitTree(&(GH->rfr_top), 
	      CCTK_rfrStorageOn,
	      CCTK_rfrStorageOff,
	      CCTK_rfrCommunicationOn,
	      CCTK_rfrCommunicationOff,
	      CCTK_rfrTriggerable,
	      CCTK_rfrTriggerSaysGo,
	      CCTK_rfrTriggerAction,
	      CCTK_rfrCallFunc);

  /* Do the rfr initialisation on this GH */
  CCTK_BindingsScheduleRegister("RFRINIT", (void *)GH);

  /* Report the rfr tree */
  CCTK_rfrPrintTree(GH,GH->rfr_top);

  /* Initialise all the extensions. */
  CCTK_InitGHExtensions(GH);

#define PUGH

#ifdef PUGH
  /* Do various rfr traversals.  Will tidy up later. */

  /* FIXME : PARAM_CHECK SHOULD BE BEFORE HERE */
  CCTK_rfrTraverse(GH, CCTK_PARAMCHECK);
  CCTKi_FinaliseParamWarn();

  CCTK_rfrTraverse(GH, CCTK_BASEGRID); 
  CCTK_rfrTraverse(GH, CCTK_INITIAL0);

  /* Loops like this should go eventually... */
  for (Rstep = CCTK_INITIAL; Rstep <= CCTK_INITIAL9; Rstep++)
  {
    CCTK_rfrTraverse(GH,Rstep);
  }

  /* Ignore checkpointing for now.
   * CCTK_rfrTraverse(GH,CCTK_RECOVER);
   * CCTK_rfrTraverse(GH,CCTK_CPINITIAL);
   */

  for (Rstep = CCTK_POSTSTEP; Rstep <= CCTK_POSTSTEP10; Rstep++)
  {
    CCTK_rfrTraverse(GH,Rstep);
  }

#endif

  return 1;

}
