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
#include <stdarg.h>

#include "StoreKeyedData.h"

#include "cctk_Misc.h"
#include "cctk_WarnLevel.h"
#include "cctk_FortranString.h"

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


/* Store a list of format strings */

static int n_formats = 0;
static pKeyedData *formatlist = NULL;


 /*@@
   @routine    CCTKi_SetWarnLevel
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
int CCTKi_SetWarnLevel(int level)
{
  int retval;
  int old_level;
  char warning_message[1001];

  old_level = warning_level;

  warning_level = level;

  if(level > old_level)
  {
    sprintf(warning_message, "Increasing warning level from %d to %d\n", old_level, level);
    CCTK_Warn(1, __LINE__,__FILE__,"Cactus",warning_message);
    retval = 1;
  }
  else if(level == old_level)
  {
    sprintf(warning_message, "Warning level is already %d\n", level);
    CCTK_Warn(1, __LINE__,__FILE__,"Cactus",warning_message);
    retval = 0;
  }
  else
  {
    sprintf(warning_message, "Decreasing warning level from %d to %d\n", old_level, level);
    CCTK_Warn(1,__LINE__,__FILE__,"Cactus", warning_message);
    retval = -1;
  }

  /* FIXME Is this right? Gab. */
  if(warning_level < error_level)
  {
    error_level = warning_level;
    sprintf(warning_message, "Decreasing error level to warning_level\n");
    CCTK_Warn(2, __LINE__,__FILE__,"Cactus",warning_message);
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
void CCTK_Warn(int level, int line, const char *file, const char *thorn, const char *message)
{

  DECLARE_CCTK_PARAMETERS

  if(level <= warning_level)
  {
    if (cctk_full_warnings)
    {
      fprintf(stderr, "WARNING level %d in thorn %s (line %d of %s): \n", level, thorn, line, file);
      fprintf(stderr, "  -> %s\n",message);
      fflush(stderr);
    }
    else
    {
      fprintf(stderr, "WARNING (%s): %s\n", thorn, message);
      fflush(stderr);
    }
  }

  if(level <= error_level)
  {
    exit(99);
  }

}

void FMODIFIER FORTRAN_NAME(CCTK_Warn)(int *level, int *line, THREE_FORTSTRINGS_ARGS)
{
  THREE_FORTSTRINGS_CREATE(file,thorn,message)
  CCTK_Warn(*level,*line,file,thorn,message);
  free(thorn);
  free(message); 
  free(file);
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
  fprintf(stdout, "INFO (%s): %s\n", thorn, message);
}

void FMODIFIER FORTRAN_NAME(CCTK_Info)(TWO_FORTSTRINGS_ARGS)
{
  TWO_FORTSTRINGS_CREATE(thorn,message)
  CCTK_Info(thorn,message);
  free(thorn);
  free(message); 
}


 /*@@
   @routine    CCTKi_SetErrorLevel
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
int CCTKi_SetErrorLevel(int level)
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
      CCTK_Warn(1, __LINE__,__FILE__,"Cactus",warning_message);
      retval = 1;
    }
    else if(level == old_level)
    {
      sprintf(warning_message, "Error level is already %d\n", level);
      CCTK_Warn(3, __LINE__,__FILE__,"Cactus",warning_message);
      retval = 0;
    }
    else
    {
      sprintf(warning_message, "Decreasing error level from %d to %d\n", old_level, level);
      CCTK_Warn(1,__LINE__,__FILE__,"Cactus", warning_message);
      retval = -1;
    }
  }
  else
  {
    sprintf(warning_message, "Error level cannot be higher than warning level\n");
    CCTK_Warn(1,__LINE__,__FILE__,"Cactus", warning_message);
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

 /*@@
   @routine    CCTK_MessageFormat
   @date       Mon Jul 26 19:51:26 1999
   @author     Tom Goodale
   @desc 
   Stores a format for messages from Fortran.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int FMODIFIER FORTRAN_NAME(CCTK_MessageFormat)(ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(format)
  
  StoreKeyedData(&formatlist, n_formats++ , (void *)format);

  return n_formats-1;
}

void FMODIFIER FORTRAN_NAME(CCTK_VInfo)(int format_number, ...)
{
  char *format_string;
  char *message;
  char format[100];
  int message_length;
  int current_place;
  va_list args;
  
  if(format_number < n_formats)
  {
    format_string = (char *)GetKeyedData(formatlist, format_number);

    /* Pick an aribitrary starting length for the message */
    message_length=5*strlen(format_string);

    message = (char *)malloc(message_length);

    /* Loop through the format string */
    for(current_place=0; format_string; format_string++)
    {
      if(*format_string != '%')
      {
	message[current_place] = *format_string;
	current_place++;
	if(current_place >= message_length)
	{
	  message = (char *)realloc(message, message_length*2);
	  message_length *=2;
	}
      }
      else
      {
	
      }
    }

  }

}

 /*@@
   @routine    CCTK_NotYetImplemented
   @routine    CCTKi_NotYetImplemented
   @date       July 1999
   @author     Gabrielle Allen
   @desc 
               Report on features not yet added to code
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

void CCTKi_NotYetImplemented(const char *message)
{

  char *out = malloc((300+strlen(message))*sizeof(char));
  sprintf(out,"\n\n This planned feature is not yet implemented in the code.\n If you need this feature please contact the Cactus maintainers.\n %s",message);
  CCTK_Warn(0,__LINE__,__FILE__,"Cactus","Feature not implemented");
  free(out);

}

void FMODIFIER FORTRAN_NAME(CCTKi_NotYetImplemented)(ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(message)
  CCTKi_NotYetImplemented(message);
  free(message); 
}
