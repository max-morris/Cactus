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
  while((GH = SetupGH(config, convergence_level)))
  {
    CCTK_AddGH(config, convergence_level, GH);
#ifdef 0
    Initialise(GH);
#endif 
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
