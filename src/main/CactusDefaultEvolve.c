 /*@@
   @file      CactusDefaultEvolve.c
   @date      Thu Oct  8 17:28:46 1998
   @author    Tom Goodale
   @desc 
   Default cactus evolution stuff.
   @enddesc 
 @@*/
#include <stdio.h>

#include "flesh.h"
#include "cctk.h"
#include "declare_parameters.h"
#include "rfr_constants.h"
#include "CactusIOFunctions.h"

static char *rcsid="$Id$";

/* Define some macros for convenience. */

#define ForallConvLevels(iteration, conv_level)  {                     \
                                        int factor = 1;                \
					for(conv_level = 0 ;           \
					    conv_level < config->nGHs; \
					    conv_level++)              \
					{                              \
					  if(iteration%factor == 0)    \
					  {
                            
#define EndForallConvLevels                                            \
					  };                           \
					  factor *=2;                  \
					};                             \
                                     }

 /* Quick stuff for testing purposes. */
#define EVOLUTION 1
#define OUTPUT    2
int cactus_terminate;
/*int cctk_itfirst = 0;
  int cctk_itlast = 50;*/
static int cactus_terminate_global = 0;
#define TERMINATION_RAISED_BRDCAST 4
 

 /*@@
   @routine    CactusDefaultEvolve
   @date       Thu Oct  8 17:30:15 1998
   @author     Tom Goodale
   @desc 
   The default cactus evolution routine.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CactusDefaultEvolve(tFleshConfig *config)
{

  DECLARE_PARAMETERS

  int iteration;
  int convergence_level;

  CactusStartTimer(config->timer[EVOLUTION]);

  /*
  CCTK_InfoHeader(config);
  */

  iteration = cctk_itfirst;

  while (iteration<=cctk_itlast) 
  {
    iteration++;

    /* Step each convergence level */


    ForallConvLevels(iteration, convergence_level)
    {
      CCTK_StepGH(config->GH[convergence_level]);
      /*
      CCTK_InfoOutput(config->GH[convergence_level], convergence_level);
      */
    }
    EndForallConvLevels;
  
    /* Dump out checkpoint data on all levels */
    ForallConvLevels(iteration, convergence_level)
    {
      CCTK_rfrTraverse(config->GH[convergence_level],CACTUS_CHECKPOINT);
    }
    EndForallConvLevels;

    /* Output perhaps */
    CactusStartTimer(config->timer[OUTPUT]);
    /*** Call OUTPUT for this GH (this routine    ***/
    /*** checks if output is necessary) and makes ***/
    /*** an rfrTraverse with CACTUS_ANALYSIS      ***/
    ForallConvLevels(iteration, convergence_level)
      {
        CCTK_rfrTraverse(config->GH[convergence_level],CACTUS_ANALYSIS);
	CCTK_OutputGH(config->GH[convergence_level]);
      }
    EndForallConvLevels;
      
    CactusStopTimer(config->timer[OUTPUT]);

#ifdef 0
    ConvergenceReport(config->GH, iteration);

    TerminationStepper(config->GH[0]);

#endif
    /* Termination has been raised and broadcasted, exit loop*/
    if (cactus_terminate==TERMINATION_RAISED_BRDCAST) break;

  } /*** END OF MAIN ITERATION LOOP ***/

  CactusStopTimer(config->timer[EVOLUTION]);

  return 0;
}

/************************************************************************/

/* The following routines have been nicked from 3.0 for the moment. */





 /*@@
   @routine    CStepper
   @date       Fri Aug 14 12:39:49 1998
   @author     Gerd Lanfermann
   @desc 
     The full set of routines used to execute all rfr steps 
     int the main iteration loop. Makes calls to the individual 
     routines for each rfr step.
   @enddesc 
   @calls  PreStepper, EvolStepper, PostStepper, BoundStepper
   @calledby main   
 @@*/

int CCTK_StepGH(cGH *GH) {
  void PreStepper(cGH *GH);
  void EvolStepper(cGH *GH);
  void PostStepper(cGH *GH);
  void BoundStepper(cGH *GH);
  PreStepper(GH);
  EvolStepper(GH);
  BoundStepper(GH);
  PostStepper(GH);

  return 0;
}

 /*@@
   @routine    PreStepper
   @date       Fri Aug 14 12:43:20 1998
   @author     Gerd Lanfermann
   @desc 
     calls RFR-PRESTEP
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

void PreStepper(cGH *GH) {
  int Rstep;  

  /* Call the rfr with CACTUS_PRESTEP */
  for (Rstep = CACTUS_PRESTEP;Rstep <= CACTUS_PRESTEP5; Rstep++)
    CCTK_rfrTraverse(GH, Rstep);
}
 /*@@
   @routine    EvolStepper
   @date       Fri Aug 14 12:44:00 1998
   @author     Gerd Lanfermann
   @desc 
     calls RFR-EVOLUTION, checks for nans, increases physical time
   @enddesc 
   @calls     
   @calledby   
   @history
   @hauthor Gabrielle Allen
   @hdate Sep 98 @hdesc Advance GHiteration
   @endhistory
@@*/

void EvolStepper(cGH *GH) {
  /* Call the rfr with Evolution */
  CCTK_rfrTraverse(GH, CACTUS_EVOL);
  /* after Evolution check for NANs */

#ifdef 0
  /* Increment physical time now */
  GH->phys_time = GH->phys_time + GH->dt0;
#endif

  GH->iteration++;


}
 /*@@
   @routine    BoundStepper
   @date       Fri Aug 14 12:44:58 1998
   @author     Gerd Lanfermann
   @desc 
     calls RFR-CACTUS_BOUND applies boudnary conditions
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

void BoundStepper(cGH *GH) {
  CCTK_rfrTraverse(GH,CACTUS_BOUND);
}

 /*@@
   @routine    PostStepper
   @date       Fri Aug 14 12:45:39 1998
   @author     Gerd Lanfermann
   @desc 
     calls the routines rgeistered as CACTUS_POSSTEPS
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void PostStepper(cGH *GH) {
  int Rstep;  
   /* Call the rfr with post step */
  for (Rstep = CACTUS_POSTSTEP; Rstep <= CACTUS_POSTSTEP10; Rstep++)
    CCTK_rfrTraverse(GH, Rstep); 
}
 /*@@
   @routine    TerminationStepper
   @date       Fri Aug 14 13:07:11 1998
   @author     Gerd Lanfermann
   @desc 
     catctus_terminate is a global variable with these values: 
     TERMINATION_NOT_RAISED    : not signaled yet (cactus_initial.c)
     TERMINATION_RAISED_LOCAL  : signaled on one PE, not reduced (MPI_LOR) 
                                 to all PEs yet (main.c)
     TERMINATION_RAISED_BRDCAST: reduced -> can now be used to terminate 
                                 (chkpnt_terminate.c) by RFR 
     the raised termiantion signal is caught on 1 PE only and has to be recduced
     on all PEs before a termination sequenced can be launched (I like that)
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

void TerminationStepper(cGH *GH) {
  int cactus_terminate_global; 
  
  cactus_terminate_global=cactus_terminate;
#ifdef MPI
  MPI_Allreduce(&cactus_terminate,&cactus_terminate_global,1,
		MPI_INT,MPI_LOR,GH->PUGH_COMM_WORLD);
#endif
  if (cactus_terminate_global) { 
    cactus_terminate=TERMINATION_RAISED_BRDCAST;
    printf("RECEIVED GLOBAL TERMINATION SIGNAL \n");
  }
  CCTK_rfrTraverse(GH,CACTUS_TERMINATE);
}
