#include "cctk.h"

 /*@@
   @file      File.c
   @date      September 6th 1999
   @author    Gabrielle Allen
   @desc
              File Handling routines
   @enddesc
 @@*/             

#include <malloc.h>
#include <stdio.h>

#include "cctk_WarnLevel.h"
#include "cctk_FortranString.h"
#include "StoreHandledData.h"

static cHandledData *DirNames = NULL;   

 /*@@
   @routine   CCTK_mkdir
   @date      September 6th 1999
   @author    Gabrielle Allen
   @desc
              Create a directory, if we haven't already tried 
              to create it.
   @enddesc
 @@*/             

int CCTK_mkdir(char *dir)
{
  int retval=-1;
  int handle;
  char *command;
  char *message;
 
  command = (char *)malloc(1024*sizeof(char));

  /* Store directory name */
  handle = Util_GetHandle(DirNames, dir, NULL);


  if (handle < 0)
  {

    /* New directory name */
    handle = Util_NewHandle(&DirNames, dir, NULL);     

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

    message = (char *)malloc(1024*sizeof(char));
    sprintf(message,"Creating directory: \"%s\"",command);
    CCTK_Info("Cactus",message);
    free(message);

    /*** FIXME: not sure what a successfull system call look like
	 across all architecures - ignore for now
	 and set zero for Cactus success ***/
    retval = system(command);
    retval = 0;

    free(command);

  } else 
    retval = -1;

  return retval;

}

void FMODIFIER FORTRAN_NAME(CCTK_mkdir)(int *ierr, ONE_FORTSTRING_ARG)
{
  int retval;
  ONE_FORTSTRING_CREATE(arg1)
  *ierr = CCTK_mkdir(arg1);
  free(arg1); 
}

