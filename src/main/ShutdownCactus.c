 /*@@
   @file      ShutdownCactus.c
   @date      Mon Sep 28 14:25:48 1998
   @author    Tom Goodale
   @desc 
   Contains routines to shutdown cactus.
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>

#include "cctk_Flesh.h"
#include "cctk_Misc.h"
#include "cctk_Parameter.h"
#include "cctk_Schedule.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_ShutdownCactus_c)

 /*@@
   @routine    CCTKi_ShutdownCactus
   @date       Mon Sep 28 14:50:50 1998
   @author     Tom Goodale
   @desc 
   Cactus specific shutdown stuff.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTKi_ShutdownCactus(tFleshConfig *ConfigData)
{
  int param_type;
  const char *cctk_timer_output;

  cctk_timer_output = (*(const char **)CCTK_ParameterGet("cctk_timer_output","Cactus",&param_type));

  if (CCTK_Equals(cctk_timer_output,"full"))
  {
    CCTK_SchedulePrintTimes(NULL);
  }

  return 0;

}
