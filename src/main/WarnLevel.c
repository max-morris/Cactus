/*@@
   @file      WarnLevel.c
   @date      Wed Feb 17 00:30:09 1999
   @author    Tom Goodale
   @desc 
   Routines to deal with warning levels.
   @enddesc 
   @version $Header$
 @@*/

#include "cctk_Config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "StoreKeyedData.h"

#include "cctk_Misc.h"
#include "cctk_WarnLevel.h"
#include "cctk_FortranString.h"
#include "cctk_Parameter.h"
#include "cctk_Comm.h"
#include "cctk_Flesh.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_WarnLevel_c)

/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/

/********************************************************************
 ********************* Local Routine Prototypes *********************
 ********************************************************************/

/********************************************************************
 ********************* Other Routine Prototypes *********************
 ********************************************************************/

int CactusDefaultMyProc(void *GH);

/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/

/* Store the parameter checking level */

static int parameter_level = CCTK_PARAMETER_NORMAL;


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


/********************************************************************
 *********************     External Routines   **********************
 ********************************************************************/

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
   @var     level
   @vdesc   The warning level
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     line
   @vdesc   Line number of warning in originating file
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     file
   @vdesc   Name of originating file
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     thorn
   @vdesc   Name of originating thorn
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     message
   @vdesc   Warning message
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   0  - success
   @endreturndesc
@@*/
int CCTK_Warn(int level, int line, const char *file, const char *thorn, const char *message)
{
  int param_type;
  int cctk_full_warnings;

  if(level <= warning_level)
  {
    cctk_full_warnings = *((CCTK_INT *)CCTK_ParameterGet("cctk_full_warnings","Cactus",&param_type));

    if (cctk_full_warnings)
    {
      fprintf(stderr, "WARNING level %d in thorn %s \n  (line %d of %s): \n", 
              level, thorn, line, file);
      fprintf(stderr, "  -> %s\n",message);
        fflush(stderr);
    }
    else
    {
      fprintf(stderr, "WARNING[%d] (%s): %s\n", level, thorn, message);
      fflush(stderr);
    }
  }

  if(level <= error_level)
  {
    CCTK_Abort(NULL,0); 
  }

  return 0;

}

void CCTK_FCALL CCTK_FNAME(CCTK_Warn)
     (int *level, int *line, THREE_FORTSTRINGS_ARGS)
{
  THREE_FORTSTRINGS_CREATE(file,thorn,message)

  CCTK_Warn(*level,*line,file,thorn,message);
  free(thorn);
  free(message); 
  free(file);
}


/*@@
   @routine    CCTK_VWarn
   @date       Sun Nov 14 00:23:29 1999
   @author     Tom Goodale
   @desc 
   Warning routine with variable argument list
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     level
   @vdesc   The warning level
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     line
   @vdesc   Line number of warning in originating file
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     file
   @vdesc   Name of originating file
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     thorn
   @vdesc   Name of originating thorn
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     format
   @vdesc   Format string for following arguments
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     ...
   @vdesc   arguments for format string
   @vtype   multiple arguments
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   0  - success
   @endreturndesc
@@*/

int CCTK_VWarn(int level, int line, const char *file, const char *thorn, const char *format, ...)
{
  int cctk_full_warnings;
  int param_type;

  va_list ap;

  if(level <= warning_level)
  {

    va_start(ap, format);
    
    cctk_full_warnings = *((CCTK_INT *)CCTK_ParameterGet("cctk_full_warnings","Cactus",&param_type));

    if (cctk_full_warnings)
    {
      fprintf(stderr, "WARNING level %d in thorn %s \n  (line %d of %s): \n", 
              level, thorn, line, file);
      fprintf(stderr, "  -> ");
      vfprintf(stderr, format, ap);
      fprintf(stderr, "\n");
      fflush(stderr);
    }
    else
    {
      fprintf(stderr, "WARNING[%d] (%s): ", level, thorn);
      vfprintf(stderr, format, ap);
      fprintf(stderr, "\n");
      fflush(stderr);
    }

    va_end(ap);
  }

  if(level <= error_level)
  {
    CCTK_Abort(NULL,0);
  }

  return 0;
}  

 /*@@
   @routine    CCTK_ParameterLevel
   @date       Wed Feb 21 2001
   @author     Gabrielle Allen
   @desc 
   Returns the parameter checking level
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
 
   @returntype int
   @returndesc
   parameter checking level now being used
   @endreturndesc
@@*/
int CCTK_ParameterLevel(void)
{
  return parameter_level;
}

/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/

 /*@@
   @routine    CCTKi_SetParameterLevel
   @date       Wed Feb 21 2001
   @author     Gabrielle Allen
   @desc 
   Sets the parameter checking level
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     level
   @vdesc   level to set
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   parameter checking level now being used
   @endreturndesc
@@*/
int CCTKi_SetParameterLevel(int level)
{
  if (level == CCTK_PARAMETER_STRICT || level == CCTK_PARAMETER_NORMAL
      || level == CCTK_PARAMETER_RELAXED)
  {
    parameter_level = level;
  }
  else
  {
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus","CCTKi_SetParameterLevel: "
	       "Parameter checking level %d not recognised, level unchanged",
	       level);
  }

  return parameter_level;
}

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
   @var     level
   @vdesc   level to set
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   1  - increased warning level
   0  - warning level unchanged
   -1 - decreased warning level
   @endreturndesc
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
   @routine    CCTKi_ExpectError
   @date       Thanksgiving 99
   @author     Gerd Lanfermann
   @desc 
      Used by CCTKi_EXPCTERR macro (src/include/cctk.h)
      allows testing for error return value, will return a 
      warning statement if error is found.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     in
   @vdesc   FIXME
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     err
   @vdesc   FIXME
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     warnonerr
   @vdesc   The warning level
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     line
   @vdesc   Line number of warning in originating file
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     file
   @vdesc   Name of originating file
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     thorn
   @vdesc   Name of originating thorn
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     message
   @vdesc   Warning message
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
@@*/

void CCTKi_ExpectError(int in, int err, int warnonerr, int line,
                       const char *file, const char *thorn,
                       const char *message) 
{
  if (in==err) 
  {
    CCTK_Warn(warnonerr, line, file, thorn, message);
  }
}


void CCTK_FCALL CCTK_FNAME(CCTKi_ExpectError)
     (int *in, int *err, int *warnonerr, int *line, THREE_FORTSTRINGS_ARGS)
{
  THREE_FORTSTRINGS_CREATE(file,thorn,message)

  CCTKi_ExpectError(*in, *err, *warnonerr, *line,
                    file,thorn,message);
  free(file);
  free(thorn);
  free(message);
}


/*@@
   @routine    CCTKi_ExpectOK
   @date       Thanksgiving 99
   @author     Gerd Lanfermann
   @desc 
      Used by CCTKi_EXPCTOK macro (src/include/cctk.h)
      allows testing for success return value, will return a 
      warning statement otherwise
   @enddesc 
   @calls     
   @calledby   CCTKi_EXPCTOK
   @history 
 
   @endhistory 
   @var     in
   @vdesc   FIXME
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     ok
   @vdesc   FIXME
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     warnonerr
   @vdesc   The warning level
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     line
   @vdesc   Line number of warning in originating file
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     file
   @vdesc   Name of originating file
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     thorn
   @vdesc   Name of originating thorn
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     message
   @vdesc   Warning message
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
@@*/

void CCTKi_ExpectOK(int in, int ok, int warnonerr, int line,
                    const char *file, const char *thorn,
                    const char *message) 
{
  if (in != ok) 
  {
    CCTK_Warn(warnonerr, line, file, thorn, message);
  }
}

void CCTK_FCALL CCTK_FNAME(CCTKi_ExpectOK)
     (int *in, int *ok, int *warnonerr, int *line, THREE_FORTSTRINGS_ARGS)
{
  THREE_FORTSTRINGS_CREATE(file,thorn,message)

  CCTKi_ExpectOK(*in, *ok, *warnonerr, *line,
                   file,thorn,message);

  free(file);
  free(thorn);
  free(message);
}



/*@@
   @routine    CCTK_ParamWarn
   @date       Wed Feb 17 00:45:07 1999
   @author     Tom Goodale
   @desc 
   Warn the user if a parameter error is found
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     thorn
   @vdesc   Name of originating thorn
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     message
   @vdesc   Warning message
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   0  - success
   @endreturndesc
@@*/
int CCTK_ParamWarn(const char *thorn, const char *message)
{
  int cctk_strong_param_check;
  int param_type;

  cctk_strong_param_check = *((CCTK_INT *)CCTK_ParameterGet("cctk_strong_param_check","Cactus",&param_type));

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

  return 0;
}

void CCTK_FCALL CCTK_FNAME(CCTK_ParamWarn)
     (TWO_FORTSTRINGS_ARGS)
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
   @var     thorn
   @vdesc   Name of originating thorn
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     message
   @vdesc   Warning message
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   0  - success
   @endreturndesc
@@*/

int CCTK_Info(const char *thorn, const char *message)
{
  fprintf(stdout, "INFO (%s): %s\n", thorn, message);
 
  return 0;
}

void CCTK_FCALL CCTK_FNAME(CCTK_Info)
     (TWO_FORTSTRINGS_ARGS)
{
  TWO_FORTSTRINGS_CREATE(thorn,message)
    

  CCTK_Info(thorn,message);
  free(thorn);
  free(message); 
}


/*@@
   @routine    CCTK_VInfo
   @date       Mon Apr 10
   @author     Thomas Radke
   @desc 
   Info routine with variable argument list
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     thorn
   @vdesc   Name of originating thorn
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     format
   @vdesc   format string for message
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     ...
   @vdesc   arguments for format string
   @vtype   multiple arguments
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   0  - success
   @endreturndesc
@@*/

int CCTK_VInfo (const char *thorn, const char *format, ...)
{
  va_list ap;

  va_start(ap, format);

  fprintf(stdout, "INFO (%s): ", thorn);
  vfprintf(stdout, format, ap);
  fprintf(stdout, "\n");
  fflush(stdout);

  va_end(ap);

  return 0;
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
   @var     level
   @vdesc   New level
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   1  - increased error level
   0  - error error unchanged
   -1 - decreased error level
   @endreturndesc
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

  int param_type;
  int cctk_strong_param_check;

  if (param_errors)
  {

    cctk_strong_param_check = *((CCTK_INT *)CCTK_ParameterGet("cctk_strong_param_check","Cactus",&param_type));

    if(cctk_strong_param_check)
    {
      fprintf(stderr,"\nFailed parameter check (%d errors)\n\n",param_errors);
      fflush(stderr);
      exit(99);
    }
    else
    {
      if (param_errors==1)
      {
        fprintf(stderr, "\nThere was 1 parameter warning\n\n");
      }
      else
      {
        fprintf(stderr, "\nThere were %d parameter warnings\n\n",param_errors);
      }
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
   @var     ONE_FORTRSTRING_ARG
   @vdesc   Format string
   @vtype   Fortran string macro
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   number of previous formats
   @endreturndesc
@@*/
int CCTK_FCALL CCTK_FNAME(CCTK_MessageFormat)
     (ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(format)
  
  StoreKeyedData(&formatlist, n_formats++ , (void *)format);

  return n_formats-1;
}

 /*@@
   @routine    CCTK_VINFO
   @date       Wed Oct  4 21:03:08 2000
   @author     Tom Goodale
   @desc 
   Fortran version of CCTK_VInfo
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     format_number
   @vdesc   format string handle
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     ...
   @vdesc   arguments for format string
   @vtype   multiple arguments
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   FIXME - UNFINISHED
   @endreturndesc
@@*/
void CCTK_FCALL CCTK_FNAME(CCTK_VInfo)
     (int format_number, ...)
{
  char *format_string;
  char *message;
  int message_length;
  int current_place;
  
  if(format_number < n_formats)
  {
    format_string = (char *)GetKeyedData(formatlist, format_number);

    /* Pick an arbitrary starting length for the message */
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
        /* FIXME */
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
   @var     message
   @vdesc   Message to be printed
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

@@*/

void CCTKi_NotYetImplemented(const char *message)
{

  char *out = malloc((300+strlen(message))*sizeof(char));
  sprintf(out,"\n\n This planned feature is not yet implemented in the code.\n If you need this feature please contact the Cactus maintainers.\n %s",message);
  CCTK_Warn(0,__LINE__,__FILE__,"Cactus","Feature not implemented");
  free(out);

}

void CCTK_FCALL CCTK_FNAME(CCTKi_NotYetImplemented)
     (ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(message)

  CCTKi_NotYetImplemented(message);
  free(message); 
}
