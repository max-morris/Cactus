 /*@@
   @file      Subsystems.c
   @date      Fri Jul 23 14:38:25 1999
   @author    Tom Goodale
   @desc 
   Misc stuff for the subsystems.
   @enddesc 
 @@*/

static char *rcsid = "$Header$";

#include "cctk_Flesh.h"


 /*@@
   @routine    CCTKi_InitialiseSubsystemDefaults
   @date       Fri Jul 23 14:39:53 1999
   @author     Tom Goodale
   @desc 
   Sets up the defaults for the overloadable functions in the subsystems.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTKi_InitialiseSubsystemDefaults(void)
{
  CCTKi_SetupMainFunctions();
  CCTKi_SetupCommFunctions();
  SetupIOFunctions();

  CCTKi_BindingsImplementationsInitialise();
  CCTKi_BindingsParametersInitialise();

  return 0;
}

