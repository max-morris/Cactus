 /*@@
   @file      CallStartupFunctions.c
   @date      Mon Sep 28 14:16:19 1998
   @author    Tom Goodale
   @desc 
   Contains routines to deal with thorn startup functions.
   @enddesc 
 @@*/

#include <stdio.h>

#include "flesh.h"

#include "CactusRegister.h"

static char *rcsid = "$Id$";

int dummy(tFleshConfig *);

 /*@@
   @routine    CallStartupFunctions
   @date       Mon Sep 28 14:24:39 1998
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CallStartupFunctions(tFleshConfig *ConfigData)
{

  /*
  RegisterMainFunction(0, dummy);
  RegisterMainFunction(1, dummy);
  RegisterMainFunction(2, dummy);

  */
  SetupMainFunctions();
  SetupCommFunctions();
  return 0;
}
  

int dummy(tFleshConfig *foo)
{
  printf("I'm in dummy\n");

  return 0;
}
