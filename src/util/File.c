
#include "cctk.h"

 /*@@
   @file      File.c
   @date      September 6th 1999
   @author    File handling routines
   @desc
   Miscellaneuous routines.
   @enddesc
 @@*/             

int CCTK_mkdir(char *dir)
{
  int retval;
  char *command;
  char *message;

  command = (char *)malloc(1024*sizeof(char));

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

  retval = system(command);

  free(command);

  return retval;

}

