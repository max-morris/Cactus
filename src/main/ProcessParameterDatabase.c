 /*@@
   @file      ProcessParameterDatabase.c
   @date      Thu Sep 24 10:34:46 1998
   @author    Tom Goodale
   @desc 
   Routines to determine the parameters and store them.
   @enddesc 
 @@*/

#include <stdio.h>

#include "flesh.h"

#include "ParameterData.h"

static char *rcsid = "$Id$";

int ParseFile(FILE *ifp, 
	      int (*set_function)(const char *, const char *));


 /*@@
   @routine    ProcessParameterDatabase
   @date       Thu Sep 24 10:37:07 1998
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int ProcessParameterDatabase(tFleshConfig *ConfigData)
{
  int retval;
  FILE *parameter_file;

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

