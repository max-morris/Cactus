 /*@@
   @file      CactusDefaultIO.c
   @date      Mon Jan 8 1999
   @author    Gabrielle Allen
   @desc 
   Default cactus IO routines.
   @enddesc 
 @@*/


#include "flesh.h"

static char *rcsid = "$Header:";

int CactusDefaultOutputGH(cGH *GH)
{
  printf("I'm in the default OutputGH routine\n");
  
  return 0;
}

int CactusDefaultOutputVarAs(cGH *GH, 
                             const char *var,   
                             const char *method, 
                             const char *alias)
{
  printf("I'm in the default OutputVarAs routine\n");
  
  return 0;
}
