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

static char *rcsid = "$Header$";

CCTK_FILEVERSION(util_File_c)

/* some systems (eg. Windows NT) don't define this macro */
#ifndef S_ISDIR
#define S_ISDIR(mode)   ((mode) & S_IFMT) == S_IFDIR)
#endif


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

  return retval;

}

void FMODIFIER FORTRAN_NAME(CCTK_mkdir)(int *ierr, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(arg1)
  *ierr = CCTK_mkdir(arg1);
  free(arg1); 
}
