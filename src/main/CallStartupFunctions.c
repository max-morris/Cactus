 /*@@
   @file      CallStartupFunctions.c
   @date      Mon Sep 28 14:16:19 1998
   @author    Tom Goodale
   @desc 
   Contains routines to deal with thorn startup functions.
   @enddesc 
   @version $Header$
 @@*/

static char *rcsid = "$Header$";

#include <stdio.h>

#include "cctk_Flesh.h"
#include "cctk_Schedule.h"

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

  CCTK_ScheduleTraverse("CCTK_STARTUP", NULL, NULL);

  return 0;
}
  
