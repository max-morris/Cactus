#include "cctk.h"

 /*@@
   @file      File.c
   @date      September 6th 1999
   @author    Gabrielle Allen
   @desc
              File Handling routines
   @enddesc
 @@*/             

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "cctk_WarnLevel.h"
#include "cctk_FortranString.h"

#include "util_String.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(util_File_c)

/* some systems (eg. Windows NT) don't define this macro */
#ifndef S_ISDIR
#define S_ISDIR(mode)   (((mode) & S_IFMT) == S_IFDIR)
#endif

/* Some systems don't have mode_t and only pass one argument to mkdir. */
#ifdef HAVE_MODE_T
#define mkdir(a,b) mkdir(a,b)
#else

/* FIXME: Temporary kludge until everyone has reconfigured */ 
#ifdef WIN32
#define mkdir(a,b) mkdir(a)
#else
#define mkdir(a,b) mkdir(a,b)
/* Should just be the clause with one argument, this one should go. */
#endif /* WIN32 */

#endif /* HAVE_MODE_T */

int CCTK_MkDirs(const char *pathname, int mode);

 /*@@
   @routine   CCTK_mkdir
   @date      September 6th 1999
   @author    Gabrielle Allen
   @desc
              Create a directory, if we haven't already tried 
              to create it.
   @enddesc
   @var        dir
   @vdesc      directory name to create
   @vtype      const char *
   @vio        in
   @endvar

   @returntype int
   @returndesc negative: failed to create dir for some reason
               0         dir was successfully created
               positive  directory already exists
   @endreturndesc

 @@*/             

int CCTK_mkdir(const char *dir)
{

  int retval;

#if 0

  struct stat statbuf;
  char *command;
 
  if(stat(dir, &statbuf) == 0)
    return(S_ISDIR(statbuf.st_mode) ? +1 : -1);

  command = (char *) malloc(strlen(MKDIR MKDIRFLAGS) + strlen(dir) + 2);

  /***
    TR 13/09/99: disabled use of MKDIRFLAGS until it also works for NT
    This means either
      - we make sure to always use the bash's mkdir command (which accepts
        option '-p')
      - call the dos shell with MKDIRFLAGS set to an empty string
      - use the POSIX mkdir routine
    The disadvantage for calling mkdir without '-p' under UNIX is now
    that we can always create one subdirectory only which needs to be
    relative to an existing directory.
   ***/
#if 0
  sprintf(command, MKDIR MKDIRFLAGS " %s",dir);
#else
  sprintf(command, MKDIR " %s",dir);
#endif

  CCTK_VInfo("Cactus","Creating directory: \"%s\"",command);

  /*** FIXME: not sure what a successful system call looks like
       across all architecures - ignore for now
       and set zero for Cactus success ***/
  retval = system(command);

  free(command);

  retval = 0;
#endif

  retval = CCTK_MkDirs(dir, 0755);

  return retval;

}

void FMODIFIER FORTRAN_NAME(CCTK_mkdir)(int *ierr, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(arg1)
  *ierr = CCTK_mkdir(arg1);
  free(arg1); 
}

 /*@@
   @routine    CCTK_MkDirs
   @date       Tue May  2 21:38:48 2000
   @author     Tom Goodale
   @desc 
   Makes all directories necessary for the path to exist.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_MkDirs(const char *pathname, int mode)
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
        if(mkdir(current, mode) == -1)
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
      if(path - pathname < strlen(pathname))
      {
        if(stat(pathname, &statbuf))
        {
          CCTK_VInfo("Cactus","Creating directory: \"%s\"", pathname);
          if(mkdir(pathname, mode) == -1)
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
