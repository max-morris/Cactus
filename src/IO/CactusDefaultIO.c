 /*@@
   @file      CactusDefaultIO.c
   @date      Tue Sep 29 12:45:04 1998
   @author    Tom Goodale
   @desc 
   Default cactus IO routine.
   @enddesc 
 @@*/


#include <stdio.h>
#include <stdlib.h>

#include "flesh.h"
#include "CactusIODefaults.h"

static char *rcsid = "$Id$";


int CactusDefaultOutput1D(cGH *GH, cGF *GF)
{
  printf("I'm in the default 1d output routine\n");
  
  return 0;
}

int CactusDefaultOutput2D(cGH *GH, cGF *GF)
{
  printf("I'm in the default 2d output routine\n");
  
  return 0;
}

int CactusDefaultOutput3D(cGH *GH, cGF *GF)
{
  printf("I'm in the default 3d output routine\n");
  
  return 0;
}
