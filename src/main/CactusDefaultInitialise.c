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

static char *rcsid = "$Id$";


int CactusDefaultInitialise(tFleshConfig *config)
{
  printf("I'm in the default initialisation routine\n");
  
  return 0;
}


int CactusDefaultEvolve(tFleshConfig *config)
{
  printf("I'm in the default evolution routine\n");
  
  return 0;
}

int CactusDefaultShutdown(tFleshConfig *config)
{
  printf("I'm in the default shutdown routine\n");
  
  return 0;
}
