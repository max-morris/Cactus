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


  CCTK_BindingsScheduleRegister("STARTUP", NULL);

  /*  These used to be here to set the defaults afterwards.  Now set the default before. 
   *   SetupMainFunctions();
   *   SetupCommFunctions();
   *   SetupIOFunctions();
   */

  return 0;
}
  
