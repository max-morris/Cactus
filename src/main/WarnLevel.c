 /*@@
   @file      WarnLevel.c
   @date      Wed Feb 17 00:30:09 1999
   @author    Tom Goodale
   @desc 
   Routines to deal with warning levels.
   @enddesc 
 @@*/


#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "Misc.h"
#include "WarnLevel.h"
#include "FortranString.h"
#include "cctk_parameters.h"

static char *rcsid = "$Header$";

/* Store the number of parameter errors */

static int param_errors = 0;


/* Store the warning level - warnings of this severity or worse will
 * be reported
 */

static int warning_level = 1;

/* Store the error level - warnings of this severity or worse will stop
 * the code. 
 */

static int error_level    = 0;

 /*@@
   @routine    CCTK_SetWarnLevel
   @date       Wed Feb 17 00:42:16 1999
   @author     Tom Goodale
   @desc 
   Sets the warning level
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_SetWarnLevel(int level)
{
  int retval;
  int old_level;
  char warning_message[1001];

  old_level = warning_level;

  warning_level = level;

  if(level > old_level)
  {
    sprintf(warning_message, "Increasing warning level from %d to %d\n", old_level, level);
    CCTK_Warn(1,"CCTK", warning_message);
    retval = 1;
  }
  else if(level == old_level)
  {
    sprintf(warning_message, "Warning level is already %d\n", level);
    CCTK_Warn(1,"CCTK", warning_message);
    retval = 0;
  }
  else
  {
    sprintf(warning_message, "Decreasing warning level from %d to %d\n", old_level, level);
    CCTK_Warn(1,"CCTK", warning_message);
    retval = -1;
  }

  /* FIXME Is this right? Gab. */
  if(warning_level < error_level)
  {
    error_level = warning_level;
    sprintf(warning_message, "Decreasing error level to warning_level\n");
    CCTK_Warn(2, "CCTK", warning_message);
  }
  return retval;
}

 /*@@
   @routine    CCTK_Warn
   @date       Wed Feb 17 00:45:07 1999
   @author     Tom Goodale
   @desc 
   Warn the user of something if the warning level is suitable.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_Warn(int level, const char *thorn, const char *message)
{
  int retval;

  if(level <= warning_level)
  {
    fprintf(stderr, "WARNING (%s): %s\n", thorn, message);
    fflush(stderr);
    retval = 1;
  }
  else
  {
    retval = 0;
  }

  if(level <= error_level)
  {
    exit(99);
  }

  return retval;
}

int FMODIFIER FORTRAN_NAME(CCTK_Warn)(int *level, TWO_FORTSTRINGS_ARGS)
{
  TWO_FORTSTRINGS_CREATE(thorn,message)
  int retval;
  retval = CCTK_Warn(*level,thorn,message);
  free(thorn);
  free(message); 
  return(retval);
}

 /*@@
   @routine    CCTK_ParamWarn
   @date       Wed Feb 17 00:45:07 1999
   @author     Tom Goodale
   @desc 
   Warn the user is a parameter error is found
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTK_ParamWarn(const char *thorn, const char *message)
{

  DECLARE_CCTK_PARAMETERS

  if(cctk_strong_param_check)
  {
    fprintf(stderr, "PARAM ERROR (%s): %s\n", thorn, message);
    fflush(stderr);
  }
  else
  {
    fprintf(stderr, "PARAM WARNING (%s): %s\n", thorn, message);
    fflush(stderr);
  }

  param_errors++;

}

void FMODIFIER FORTRAN_NAME(CCTK_ParamWarn)(TWO_FORTSTRINGS_ARGS)
{
  TWO_FORTSTRINGS_CREATE(thorn,message)
  CCTK_ParamWarn(thorn,message);
  free(thorn);
  free(message); 
}



 /*@@
   @routine    CCTK_Info
   @date       Tue Mar 30 1999
   @author     Gabrielle Allen
   @desc 
   Print information
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

void CCTK_Info(const char *thorn, const char *message)
{
  fprintf(stderr, "INFO (%s): %s\n", thorn, message);
}

int FMODIFIER FORTRAN_NAME(CCTK_Info)(TWO_FORTSTRINGS_ARGS)
{
  TWO_FORTSTRINGS_CREATE(thorn,message)
  CCTK_Info(thorn,message);
  free(message); 
}


 /*@@
   @routine    CCTK_SetErrorLevel
   @date       Wed Feb 17 00:48:02 1999
   @author     Tom Goodale
   @desc 
   Sets the error level
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_SetErrorLevel(int level)
{
  int retval;
  int old_level;
  char warning_message[1001];

  old_level = error_level;

  if(level <= warning_level)
  {
    error_level = level;

    if(level > old_level)
    {
      sprintf(warning_message, "Increasing error level from %d to %d\n", old_level, level);
      CCTK_Warn(1,"CCTK", warning_message);
      retval = 1;
    }
    else if(level == old_level)
    {
      sprintf(warning_message, "Error level is already %d\n", level);
      CCTK_Warn(3, "CCTK", warning_message);
      retval = 0;
    }
    else
    {
      sprintf(warning_message, "Decreasing error level from %d to %d\n", old_level, level);
      CCTK_Warn(1, "CCTK", warning_message);
      retval = -1;
    }
  }
  else
  {
    sprintf(warning_message, "Error level cannot be higher than warning level\n");
    CCTK_Warn(1, "CCTK", warning_message);
    retval = 0;
  }

  return retval;
}

 /*@@
   @routine    CCTKi_FinaliseParamWarn
   @date       June 1999
   @author     Gabrielle Allen
   @desc 
               Die if required after param check
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

void CCTKi_FinaliseParamWarn(void)
{

  DECLARE_CCTK_PARAMETERS

  if (param_errors)
  {
    if(cctk_strong_param_check)
    {
      fprintf(stderr,"\nFailed parameter check (%d errors)\n\n",param_errors);
      fflush(stderr);
      exit(99);
    }
    else
    {
      if (param_errors==1)
	fprintf(stderr, "\nThere was 1 parameter warning\n\n");
      else
	fprintf(stderr, "\nThere were %d parameter warnings\n\n",param_errors);
      fflush(stderr);
    }

  }

}

