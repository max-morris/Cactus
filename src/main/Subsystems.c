 /*@@
   @file      Subsystems.c
   @date      Fri Jul 23 14:38:25 1999
   @author    Tom Goodale
   @desc 
   Misc stuff for the subsystems.
   @enddesc 
   @version $Header$
 @@*/

#include "cctk_Flesh.h"
#include "CactusRegister.h"

static const char *rcsid = "$Header$";

CCTK_FILEVERSION(main_Subsystems_c)

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
   @routine    CCTKi_InitialiseSubsystemDefaults
   @date       Fri Jul 23 14:39:53 1999
   @author     Tom Goodale
   @desc 
   Sets up the defaults for the overloadable functions in the subsystems.
   @enddesc 
   @calls     CCTKi_SetupMainFunctions CCTKi_SetupCommFunctions CCTKi_SetupIOFunctions CCTKi_BindingsImplementationsInitialise CCTKi_BindingsParametersInitialise
   @calledby   
   @history 
 
   @endhistory 

   @returntype int
   @returndesc
   0  - success
   @endreturndesc

@@*/

int CCTKi_InitialiseSubsystemDefaults(void)
{
  CCTKi_SetupMainFunctions();
  CCTKi_SetupCommFunctions();
  CCTKi_SetupIOFunctions();
  CCTKBindings_SetupThornFunctions();
  CCTKi_BindingsImplementationsInitialise();
  CCTKi_BindingsParametersInitialise();
  
  return 0;
}

/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/

