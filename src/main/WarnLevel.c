 /*@@
   @file      WarnLevel.c
   @date      Wed Feb 17 00:30:09 1999
   @author    Tom Goodale
   @desc 
   Routines to deal with warning levels.
   @enddesc 
 @@*/

#include <stdio.h>

#include "Misc.h"
#include "WarnLevel.h"
#include "FortranString.h"

static char *rcsid = "$Header$";

/* Store the warning level.  Default to 1 */

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
    CCTK_Warn(1, warning_message);
    retval = 1;
  }
  else if(level == old_level)
  {
    sprintf(warning_message, "Warning level is already %d\n", level);
    CCTK_Warn(1, warning_message);
    retval = 0;
  }
  else
  {
    sprintf(warning_message, "Decreasing warning level from %d to %d\n", old_level, level);
    CCTK_Warn(1, warning_message);
    retval = -1;
  }

  if(warning_level < error_level)
  {
    error_level = warning_level;
    sprintf(warning_message, "Decreasing error level to warning_level\n");
    CCTK_Warn(2, warning_message);
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
int CCTK_Warn(int level, const char *message)
{
  int retval;

  if(level <= warning_level)
  {
    fprintf(stderr, "WARNING: %s\n", message);
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

int FORTRAN_NAME(CCTK_Warn)(int *level, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(message)
  int retval;
  retval = CCTK_Warn(*level,message);
  free(message); 
  return(retval);
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
      CCTK_Warn(1, warning_message);
      retval = 1;
    }
    else if(level == old_level)
    {
      sprintf(warning_message, "Error level is already %d\n", level);
      CCTK_Warn(3, warning_message);
      retval = 0;
    }
    else
    {
      sprintf(warning_message, "Decreasing error level from %d to %d\n", old_level, level);
      CCTK_Warn(1, warning_message);
      retval = -1;
    }
  }
  else
  {
    sprintf(warning_message, "Error level cannot be higher than warning level\n");
    CCTK_Warn(1, warning_message);
    retval = 0;
  }

  return retval;
}
