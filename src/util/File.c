
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

  sprintf(command, MKDIR MKDIRFLAGS " %s",dir);

  message = (char *)malloc(1024*sizeof(char));
  sprintf(message,"Creating directory: \"%s\"",command);
  CCTK_Info("Cactus",message);
  free(message);

  retval = system(command);

  free(command);

  return retval;

}

