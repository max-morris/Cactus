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

#include "cctki_Schedule.h"

#define SCHEDULE(x) case CCTK_ ## x : CCTK_ScheduleTraverse("CCTK_" #x, data, NULL); break

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
