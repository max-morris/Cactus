 /*@@
   @file      File.c
   @date      September 6th 1999
   @author    Gabrielle Allen
   @desc
   File Handling routines
   @enddesc
   @version $Header$
 @@*/             

#include "cctk.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "cctk_WarnLevel.h"
#include "cctk_FortranString.h"

#include "util_String.h"

static const char *rcsid = "$Header$";

CCTK_FILEVERSION(util_File_c)

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

/* some systems (eg. Windows NT) don't define this macro */
#ifndef S_ISDIR
#define S_ISDIR(mode)   (((mode) & S_IFMT) == S_IFDIR)
#endif

/* Some systems don't have mode_t and only pass one argument to mkdir. */
#ifdef HAVE_MODE_T
#define MKDIR_WRAPPER(a,b) mkdir(a,b)
#else
#define MKDIR_WRAPPER(a,b) mkdir(a)
#endif /* HAVE_MODE_T */

/********************************************************************
 *********************     External Routines   **********************
 ********************************************************************/

 /*@@
   @routine    CCTK_CreateDirectory
   @date       Tue May  2 21:38:48 2000
   @author     Tom Goodale
   @desc 
   Makes all directories necessary for the path to exist.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     mode
   @vdesc   permissions of the directory to be created
   @vtype   int
   @vio     in
   @vcomment 
   This is the unix mode, e.g. 0755
   @endvar 
   @var     pathname
   @vdesc   Name of directory to create
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   0  - success
   -1 - failure
   @endreturndesc
@@*/
int CCTK_CreateDirectory(int mode, const char *pathname)
{
  int retval;
  const char *path;
  char *current;
  const char *token;
  struct stat statbuf;

  path = pathname;

  retval = 0;
 
  current = (char *)malloc((strlen(pathname)+1)*sizeof(char));
  
  if(current)
  {
    current[0] = '\0';

    while((token = Util_StrSep(&path, "/")))
    {
      /* Treat first token carefully. */
      if(*current == '\0')
      {
        if(*token == '\0')
        {
          strcpy(current, "/");
        }
        else
        {
          strcpy(current,token);
        }
      }
      else
      {
        sprintf(current,"%s/%s", current, token);
      }

      if(stat(current, &statbuf))
      {
        CCTK_VInfo("Cactus","Creating directory: \"%s\"", current);
        if(MKDIR_WRAPPER(current, mode) == -1)
        {
          CCTK_VWarn(0,__LINE__,__FILE__,"Cactus","Failed to create directory \"%s\"", current);
          retval = -2;
        }
      }
      else
      {
        if(! S_ISDIR(statbuf.st_mode))
        {
          CCTK_VWarn(0,__LINE__,__FILE__,"Cactus","\"%s\" exists but is not a directory", current);
          retval = -3;
        }
      }

      if(retval)
      {
        break;
      }
    }

    if(! retval)
    {
      /* Deal with last component of path */
      if((size_t)(path - pathname) < strlen(pathname))
      {
        if(stat(pathname, &statbuf))
        {
          CCTK_VInfo("Cactus","Creating directory: \"%s\"", pathname);
          if(MKDIR_WRAPPER(pathname, mode) == -1)
          {
            CCTK_VWarn(0,__LINE__,__FILE__,"Cactus","Failed to create directory \"%s\"", pathname);
            retval = -2;
          }
        }
        else
        {
          if(! S_ISDIR(statbuf.st_mode))
          {
            CCTK_VWarn(0,__LINE__,__FILE__,"Cactus", "\"%s\" exists but is not a directory", pathname);
            retval = -3;
          }
        }
      }
    }

    free(current);

  }     
  else
  {
    CCTK_Warn(0,__LINE__,__FILE__,"Cactus", "Failed to allocate some temporary memory");
    retval = -1;
  }

  return retval;

}

void CCTK_FCALL CCTK_FNAME(CCTK_CreateDirectory)
     (int *ierr, int *mode, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(arg1)
  *ierr = CCTK_CreateDirectory(*mode,arg1);
  free(arg1); 
}

/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/


