 /*@@
   @file      rfrInterface.c
   @date      Wed Feb  3 14:42:24 1999
   @author    Tom Goodale
   @desc 
   Routine used by cactus to talk to the rfr.
   @enddesc 
 @@*/

/*#define RFRDEBUG*/

#include <stdio.h>
#include <stdlib.h>

#include "cctk_Flesh.h"
#include "cctk_GHExtensions.h"
#include "cctk_Groups.h"
#include "cctk_WarnLevel.h"
#include "cctk_IOMethods.h"
#include "cctk_Parameters.h"

#include "CactusCommFunctions.h"
#include "CactusrfrInterface.h"
#include "rfrConstants.h"
#include "rfrInterface.h"

static char *rcsid = "$Header$";


 /*@@
   @routine    CCTK_rfrTraverse
   @date       Wed Feb  3 14:45:57 1999
   @author     Tom Goodale
   @desc 
   Calls the rfrTraverse, then deals with extensions.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_rfrTraverse(cGH *GH, int rfrpoint)
{
  
  CCTKi_rfrTraverseGHExtensions(GH, rfrpoint);

  return 0;
}

#include "cctki_Schedule.h"

/*#define SCHEDULE(x) case CCTK_ ## x : CCTKi_ScheduleTraverse("CCTK_" #x, data); break*/

#define SCHEDULE(x) case CCTK_ ## x : CCTKi_ScheduleTraverse("CCTK_" #x, data, NULL); break

void rfrTraverse(void *rfr_top, void *data, int when) 
{
  switch(when)
  {
    SCHEDULE(PARAMCHECK);
    SCHEDULE(BASEGRID);
    SCHEDULE(RECOVER_PARAMETERS);
    SCHEDULE(RECOVER_VARIABLES);
    SCHEDULE(RECOVER);
    SCHEDULE(INITIAL);
    SCHEDULE(POSTINITIAL);
    SCHEDULE(CPINITIAL);
    SCHEDULE(PRESTEP);
    SCHEDULE(POSTSTEP);
    SCHEDULE(EVOL);
    SCHEDULE(CHECKPOINT);
    SCHEDULE(ANALYSIS);
    SCHEDULE(TERMINATE);
    SCHEDULE(CONVERGENCE);
    default :
      CCTK_Warn(0,__LINE__,__FILE__,"Cactus","Unknown scheduling point"); 
  }

  return;
}
