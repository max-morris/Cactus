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

#include "cctk_Flesh.h"
#include "cctk_ActiveThorns.h"
#include "cctki_ActiveThorns.h"
#include "cctki_Cache.h"

/* FIXME - remove this when ActiveThorns doesn't need it */
#include "SKBinTree.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_InitialiseDataStructures_c)

 /*@@
   @routine    CCTKi_InitialiseDataStructures
   @date       Wed Jan 20 09:27:56 1999
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTKi_InitialiseDataStructures(tFleshConfig *ConfigData)
{

  CCTKi_RegisterDefaultTimerFunctions();

  ConfigData->nGHs = 0;
  ConfigData->GH = NULL;

#if 0
  ConfigData->timer[INITIALISATION] = CactusNewTimer();
  ConfigData->timer[EVOLUTION] = CactusNewTimer();
  ConfigData->timer[ELLIPTIC] = CactusNewTimer();
#endif


  /* Initialise appropriate subsystems. */

  /*  CCTKi_BindingsImplementationsInitialise();*/
  /*  CCTKi_BindingsParametersInitialise();*/
  CCTKi_BindingsVariablesInitialise();
  /*CCTKi_BindingsScheduleInitialise();*/

  CCTKi_ActivateThorn("Cactus");
  CCTKi_SetupCache();

  return 0;
}

