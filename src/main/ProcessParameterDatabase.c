 /*@@
   @file      ProcessParameterDatabase.c
   @date      Thu Sep 24 10:34:46 1998
   @author    Tom Goodale
   @desc 
   Routines to determine the parameters and store them.
   @enddesc 
 @@*/

#include <stdio.h>

#include "cctk_Flesh.h"
#include "cctk_Parameter.h"

#include "ParameterData.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_ProcessParameterDatabase_c)

int ParseFile(FILE *ifp, 
              int (*set_function)(const char *, const char *));
void CCTKi_SetParameterSetMask(int mask);


 /*@@
   @routine    CCTKi_ProcessParameterDatabase
   @date       Thu Sep 24 10:37:07 1998
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTKi_ProcessParameterDatabase(tFleshConfig *ConfigData)
{
  int retval;
  FILE *parameter_file;

  CCTKi_SetParameterSetMask(PARAMETER_RECOVERY_PRE);

  if((parameter_file = fopen(ConfigData->parameter_file_name, "r")))
  {
    ParseFile(parameter_file, CCTKi_SetParameter);
    fclose(parameter_file);
    retval = 0;
  }
  else
  {
    fprintf(stderr, "Unable to open parameter file '%s'\n", 
                    ConfigData->parameter_file_name);
    retval = 1;
  }
      
  return 0;
}

