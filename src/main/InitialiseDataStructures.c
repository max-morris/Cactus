 /*@@
   @file      InitialiseDataStructures.c
   @date      Wed Jan 13 20:28:08 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flesh.h"

static char *rcsid = "$Id$";


 /*@@
   @routine    InitialiseDataStructures
   @date       Wed Jan 20 09:27:56 1999
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int InitialiseDataStructures(tFleshConfig *ConfigData)
{
  ConfigData->nGHs = 0;
  ConfigData->GH = NULL;

  ConfigData->timer[INITIALISATION] = CactusNewTimer();
  ConfigData->timer[EVOLUTION] = CactusNewTimer();
  ConfigData->timer[ELLIPTIC] = CactusNewTimer();



  /* Initialise appropriate subsystems. */

  CCTK_BindingsImplementationsInitialise();
  CCTK_BindingsParametersInitialise();
  CCTK_BindingsVariablesInitialise();
  CCTK_BindingsScheduleInitialise();


  return 0;
}

