 /*@@
   @file      CCTKi_CallStartupFunctions.c
   @date      Mon Sep 28 14:16:19 1998
   @author    Tom Goodale
   @desc 
   Contains routines to deal with thorn startup functions.
   @enddesc 
   @version $Header$
 @@*/

#include <stdio.h>
#include <stdlib.h>

#include "cctk_Flesh.h"
#include "cctk_Schedule.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_CallStartupFunctions_c)

/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/

/********************************************************************
 ********************* Local Routine Prototypes *********************
 ********************************************************************/

/********************************************************************
 ********************* Other Routine Prototypes *********************
 ********************************************************************/

/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/

/********************************************************************
 *********************     External Routines   **********************
 ********************************************************************/

 /*@@
   @routine    CCTKi_CallStartupFunctions
   @date       Mon Sep 28 14:24:39 1998
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     CCTK_ScheduleTraverse
   @calledby   
   @history 
 
   @endhistory 
   @var     ConfigData
   @vdesc   Flesh configuration data
   @vtype   tFleshConfig
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   0  - success
   @endreturndesc
@@*/

int CCTKi_CallStartupFunctions(tFleshConfig *ConfigData)
{

  CCTK_ScheduleTraverse("CCTK_STARTUP", NULL, NULL);

  return 0;
}
  
/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/

