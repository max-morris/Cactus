 /*@@
   @file      CactusDefaultEvolve.c
   @date      Thu Oct  8 17:28:46 1998
   @author    Tom Goodale
   @desc 
   Default cactus evolution stuff.
   @enddesc 
 @@*/

/*#define DEBUG_CCTK*/

#include <stdio.h>

#include "cctk_Flesh.h"
#include "cctk.h"
#include "cctk_parameters.h"
#include "rfrConstants.h"
#include "rfrInterface.h"

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
static int cactus_terminate_global = 0;
#define TERMINATION_RAISED_BRDCAST 4

/* Local function prototypes. */

int CCTK_StepGH(cGH *GH);
 

/* the iteration counter used in the evolution loop */
static int iteration = 0;


 /*@@
   @routine    CCTK_SetMainLoopIndex
   @date       Sep 22 1999
   @author     Thomas Radke
   @desc 
               Sets the iteration counter variable of the evolution loop.
               This is used for recovery.
   @enddesc 
   @calls     
   @calledby   

@@*/
int CCTK_SetMainLoopIndex (int main_loop_index)
{
  iteration = main_loop_index;
  return 0;
}


 /*@@
   @routine    CCTK_MainLoopIndex
   @date       Sep 22 1999
   @author     Thomas Radke
   @desc 
               Returns the iteration counter variable of the evolution loop.
               This is used for checkpointing.
   @enddesc 
   @calls     
   @calledby   

@@*/
int CCTK_MainLoopIndex (void)
{
  return (iteration);
}


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

  DECLARE_CCTK_PARAMETERS

  int convergence_level;

#ifdef DEBUG_CCTK
  CCTK_PRINTSEPARATOR
  printf("In CactusDefaultEvolve\n----------------------\n");
  printf("  Initializing iteration = %d\n",iteration);
  CCTK_PRINTSEPARATOR
#endif

#if 0
  CactusStartTimer(config->timer[OUTPUT]);
#endif

  /*** Call OUTPUT for this GH (this routine    ***/
  /*** checks if output is necessary) and makes ***/
  /*** an rfrTraverse with CCTK_ANALYSIS      ***/
  ForallConvLevels(iteration, convergence_level)
  {
      CCTK_rfrTraverse(config->GH[convergence_level],CCTK_ANALYSIS);
      CCTK_OutputGH(config->GH[convergence_level]);
  }
  EndForallConvLevels;

#if 0      
  CactusStopTimer(config->timer[OUTPUT]);

  CactusStartTimer(config->timer[EVOLUTION]);
#endif
  /*
  CCTK_InfoHeader(config);
  */


  while (iteration<cctk_itlast || (cctk_final_time>cctk_initial_time?config->GH[0]->cctk_time<cctk_final_time:0)) 
  {

#ifdef DEBUG_CCTK
    CCTK_PRINTSEPARATOR
    printf("In CactusDefaultEvolve\n----------------------\n");
    printf("  Advancing iteration %d = %d + 1\n",iteration+1,
	 iteration); 
    CCTK_PRINTSEPARATOR
#endif

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
      CCTK_rfrTraverse(config->GH[convergence_level],CCTK_CHECKPOINT);
    }
    EndForallConvLevels;

    /* Output perhaps */
#if 0
    CactusStartTimer(config->timer[OUTPUT]);
#endif
    /*** Call OUTPUT for this GH (this routine    ***/
    /*** checks if output is necessary) and makes ***/
    /*** an rfrTraverse with CCTK_ANALYSIS      ***/
    ForallConvLevels(iteration, convergence_level)
    {
        CCTK_rfrTraverse(config->GH[convergence_level],CCTK_ANALYSIS);
	CCTK_OutputGH(config->GH[convergence_level]);
    }
    EndForallConvLevels;
      
#if 0
    CactusStopTimer(config->timer[OUTPUT]);
#endif

#if 0
    ConvergenceReport(config->GH, iteration);

    TerminationStepper(config->GH[0]);

#endif
    /* Termination has been raised and broadcasted, exit loop*/
    if (cactus_terminate==TERMINATION_RAISED_BRDCAST) break;

  } /*** END OF MAIN ITERATION LOOP ***/

#if 0
  CactusStopTimer(config->timer[EVOLUTION]);
#endif

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

int CCTK_StepGH(cGH *GH) 
{

  void PreStepper(cGH *GH);
  void EvolStepper(cGH *GH);
  void PostStepper(cGH *GH);
  void BoundStepper(cGH *GH);


  /* Advance GH->iteration BEFORE evolving */
#ifdef DEBUG_CCTK
  CCTK_PRINTSEPARATOR
  printf("In CCTK_StepGH\n--------------\n");
  printf("  Advancing GH->iteration to %lu = %lu + 1\n",(GH->cctk_iteration+1),
	 GH->cctk_iteration);
  CCTK_PRINTSEPARATOR
#endif

  GH->cctk_iteration++;

  PreStepper(GH);
  EvolStepper(GH);
  BoundStepper(GH);

  /* Advance GH->time AFTER evolving */
#ifdef DEBUG_CCTK
  CCTK_PRINTSEPARATOR
  printf("In CCTK_StepGH\n--------------\n");
  printf("  Advancing GH->cctk_time %f = %f + %f\n",GH->cctk_time+GH->cctk_delta_time,
	 GH->cctk_time,GH->cctk_delta_time);
  CCTK_PRINTSEPARATOR
#endif

  GH->cctk_time = GH->cctk_time + GH->cctk_delta_time;

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

  /* Call the rfr with CCTK_PRESTEP */
  CCTK_rfrTraverse(GH, CCTK_PRESTEP);
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

   @endhistory
@@*/

void EvolStepper(cGH *GH) 
{

  /* Call the rfr with Evolution */
  CCTK_rfrTraverse(GH, CCTK_EVOL);
  /* after Evolution check for NANs */

}


 /*@@
   @routine    BoundStepper
   @date       Fri Aug 14 12:44:58 1998
   @author     Gerd Lanfermann
   @desc 
     calls RFR-CCTK_BOUND applies boundary conditions
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

void BoundStepper(cGH *GH) {
  CCTK_rfrTraverse(GH,CCTK_BOUND);
}

 /*@@
   @routine    PostStepper
   @date       Fri Aug 14 12:45:39 1998
   @author     Gerd Lanfermann
   @desc 
     calls the routines rgeistered as CCTK_POSSTEPS
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void PostStepper(cGH *GH) {
  int Rstep;  
   /* Call the rfr with post step */
    CCTK_rfrTraverse(GH, CCTK_POSTSTEP); 
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

#if 0

#ifdef MPI
  MPI_Allreduce(&cactus_terminate,&cactus_terminate_global,1,
		MPI_INT,MPI_LOR,GH->PUGH_COMM_WORLD);
#endif
#endif
  if (cactus_terminate_global) { 
    cactus_terminate=TERMINATION_RAISED_BRDCAST;
    printf("RECEIVED GLOBAL TERMINATION SIGNAL \n");
  }
  CCTK_rfrTraverse(GH,CCTK_TERMINATE);
}
