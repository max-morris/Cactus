 /*@@
   @file      CactusDefaultEvolve.c
   @date      Thu Oct  8 17:28:46 1998
   @author    Tom Goodale
   @desc 
   Default cactus evolution stuff.
   @enddesc 
   @version $Header$
 @@*/

/*#define DEBUG_CCTK*/

#include <stdio.h>

#include "cctk_Flesh.h"
#include "cctk_Parameter.h"

#include "CactusIOFunctions.h"

static char *rcsid="$Header$";

CCTK_FILEVERSION(main_CactusDefaultEvolve_c)

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

int cactus_terminate;

static int cactus_terminate_global = 0;

#define TERMINATION_RAISED_BRDCAST 4

/* Local function prototypes. */
static int DoneMainLoop (CCTK_REAL cctk_time, int iteration);
static int StepGH(cGH *GH);
 
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
  return iteration;
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
   @hdate Fri May 12 2000 @hauthor Thomas Radke
   @hdesc  Moved evolution loop termination check into DoneMainLoop()
   @endhistory 

@@*/
int CactusDefaultEvolve(tFleshConfig *config)
{
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
  /*** a Traverse with CCTK_ANALYSIS      ***/
  ForallConvLevels(iteration, convergence_level)
  {
    CCTK_Traverse(config->GH[convergence_level], "CCTK_ANALYSIS");
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


  while (! DoneMainLoop (config->GH[0]->cctk_time, iteration))
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

      StepGH(config->GH[convergence_level]);
      /*
      CCTK_InfoOutput(config->GH[convergence_level], convergence_level);
      */
    }
    EndForallConvLevels;
  
    /* Dump out checkpoint data on all levels */
    ForallConvLevels(iteration, convergence_level)
    {
      CCTK_Traverse(config->GH[convergence_level], "CCTK_CHECKPOINT");
    }
    EndForallConvLevels;

    /* Output perhaps */
#if 0
    CactusStartTimer(config->timer[OUTPUT]);
#endif
    /*** Call OUTPUT for this GH (this routine    ***/
    /*** checks if output is necessary) and makes ***/
    /*** an Traverse with CCTK_ANALYSIS      ***/
    ForallConvLevels(iteration, convergence_level)
    {
        CCTK_Traverse(config->GH[convergence_level], "CCTK_ANALYSIS");
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

 /*@@
   @routine    DoneMainLoop
   @date       Fri May 12 2000
   @author     Thomas Radke
   @desc
   Check the termination conditions for the evolution loop
   @enddesc
   @calls
   @calledby
   @history
   @endhistory

@@*/
static int DoneMainLoop (CCTK_REAL cctk_time, int iteration)
{
  int param_type;
  CCTK_INT  cctk_itlast;
  CCTK_REAL cctk_initial_time;
  CCTK_REAL cctk_final_time;

  cctk_initial_time = (*(CCTK_REAL *)CCTK_ParameterGet("cctk_initial_time",
						       "Cactus",&param_type));
  cctk_final_time   = (*(CCTK_REAL *)CCTK_ParameterGet("cctk_final_time",
						       "Cactus",&param_type));
  cctk_itlast       = (*(CCTK_INT *)CCTK_ParameterGet("cctk_itlast",
						      "Cactus",&param_type));

  return (! (iteration < cctk_itlast ||
            (cctk_final_time > cctk_initial_time ?
             cctk_time < cctk_final_time : 0)));
}


 /*@@
   @routine    StepGH
   @date       Fri Aug 14 12:39:49 1998
   @author     Gerd Lanfermann
   @desc 
     The full set of routines used to execute all schedule point 
     int the main iteration loop. Makes calls to the individual 
     routines for each schedule point.
   @enddesc 
   @calls  PreStepper, EvolStepper, PostStepper
   @calledby main   
 @@*/

static int StepGH(cGH *GH) 
{

  /* Advance GH->iteration BEFORE evolving */

  GH->cctk_iteration++;

  CCTK_Traverse(GH, "CCTK_PRESTEP");
  CCTK_Traverse(GH, "CCTK_EVOL");

  /* Advance GH->time AFTER evolving */

  GH->cctk_time = GH->cctk_time + GH->cctk_delta_time;

  CCTK_Traverse(GH, "CCTK_POSTSTEP"); 

  return 0;
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
                                 (chkpnt_terminate.c) by the scheduler 
     the raised termiantion signal is caught on 1 PE only and has to be recduced
     on all PEs before a termination sequenced can be launched (I like that)
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

void TerminationStepper(cGH *GH) 
{
  /*  int cactus_terminate_global; */  
  cactus_terminate_global=cactus_terminate;

#if 0

#ifdef MPI
  MPI_Allreduce(&cactus_terminate,&cactus_terminate_global,1,
                MPI_INT,MPI_LOR,GH->PUGH_COMM_WORLD);
#endif
#endif
  if (cactus_terminate_global) 
  { 
    cactus_terminate=TERMINATION_RAISED_BRDCAST;
    printf("RECEIVED GLOBAL TERMINATION SIGNAL \n");
  }
  CCTK_Traverse(GH, "CCTK_TERMINATE");
}
