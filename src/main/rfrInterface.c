 /*@@
   @file      rfrInterface.c
   @date      Wed Feb  3 14:42:24 1999
   @author    Tom Goodale
   @desc 
   Routine used by cactus to talk to the rfr.
   @enddesc 
   @version $Header$
 @@*/

/*#define RFRDEBUG*/

#include <stdio.h>
#include <stdlib.h>

#include "cctk_WarnLevel.h"
#include "cctk_Flesh.h"
#include "cctk_GHExtensions.h"
#include "CactusrfrInterface.h"
#include "rfrConstants.h"
#include "rfrInterface.h"
#include "cctk_Schedule.h"
#include "cctki_GHExtensions.h"

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

  CCTK_Warn(1,__LINE__,__FILE__,"Cactus",
	    "CCTK_rfrTraverse is deprecated, please use CCTK_Traverse");
  
  CCTKi_rfrTraverseGHExtensions(GH, rfrpoint);

  return 0;
}

#include "cctki_Schedule.h"

#define SCHEDULE(x) case CCTK_ ## x : CCTK_ScheduleTraverse("CCTK_" #x, data, NULL); break

void rfrTraverse(void *rfr_top, void *data, int when) 
{

  CCTK_Warn(1,__LINE__,__FILE__,"Cactus",
	    "rfrTraverse is deprecated, please use CCTK_ScheduleTraverse");

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

 /*@@
   @routine    CCTK_Traverse
   @date       Thu Jan 27 14:42:16 2000
   @author     Tom Goodale
   @desc 
   Routine called to traverse functions 
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_Traverse(cGH *GH, const char *where)
{
  int retcode;

  retcode = CCTKi_ScheduleTraverseGHExtensions(GH, where);

  return retcode;
}
