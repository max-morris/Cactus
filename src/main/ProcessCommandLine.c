 /*@@
   @file      ProcessCommandLine.c
   @date      Thu Sep 24 10:32:28 1998
   @author    Tom Goodale
   @desc 
   Routines to deal with the command line arguments.
   @enddesc 
 @@*/
#include <stdio.h>
#include <stdlib.h>

#include "flesh.h"

static char *rcsid = "$Id$";

static int *argc;

static char **argv;


 /*@@
   @routine    ProcessCommandLine
   @date       Thu Sep 24 10:33:31 1998
   @author     Tom Goodale
   @desc 
   Processes the command line arguments.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int ProcessCommandLine(int *inargc, char *inargv[], tFleshConfig *ConfigData)
{
  argc = inargc;

  argv = inargv;

  if(*argc>1)
  {
    ConfigData->parameter_file_name = argv[1];
  }
  else
  {
    printf("Usage: %s <parameter_file_name>\n", argv[0]);

    exit(1);
  }

  return 0;
}


