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
#include "parameters.h"

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
int itlast;
int cactus_terminate;
#define CACTUS_CHECKPOINT 3
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
  int iteration;
  int convergence_level;

  CactusStartTimer(config->timer[EVOLUTION]);

  InfoHeader(config);
  while (iteration<itlast) 
  {
    iteration++;

    /* Step each convergence level */


    ForallConvLevels(iteration, convergence_level)
      {
	StepGH(config->GH[convergence_level]);
	InfoOutput(config->GH[convergence_level], convergence_level);
      }
    EndForallConvLevels;
  
    /* Dump out checkpoint data on all levels */
    ForallConvLevels(iteration, convergence_level)
      {
	rfrTraverse(config->GH[convergence_level],CACTUS_CHECKPOINT);
      }
    EndForallConvLevels;

    /* Output perhaps */
    CactusStartTimer(config->timer[OUTPUT]);
    /*** Call OUTPUT for this GH (this routine    ***/
    /*** checks if output is necessary) and makes ***/
    /*** an rfrTraverse with CACTUS_ANALYSIS      ***/
    ForallConvLevels(iteration, convergence_level)
      {
	outputGH(config->GH[convergence_level]);
      }
    EndForallConvLevels;
      
    CactusStopTimer(config->timer[OUTPUT]);

    ConvergenceReport(config->GH, iteration);

    TerminationStepper(config->GH[0]);
    /* Termination has been raised and broadcasted, exit loop*/
    if (cactus_terminate==TERMINATION_RAISED_BRDCAST) break;

  } /*** END OF MAIN ITERATION LOOP ***/

  CactusStopTimer(config->timer[EVOLUTION]);

  return 0;
}

