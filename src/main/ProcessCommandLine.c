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
#include "getopt.h"

static char *rcsid = "$Id$";

static int argc;

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
int ProcessCommandLine(int *inargc, char ***inargv, tFleshConfig *ConfigData)
{

  int option_index = 0;
  int c;

  /* Store the command line */
  argc = *inargc;

  argv = *inargv;

  /* Process the command line */

  if(argc>1)
  {
    while (1)
    {
      struct option long_options[] =
      {
	{"help", 0, 0, 'h'},
	{0, 0, 0, 0}
      };
      
      c = getopt_long_only (argc, argv, "h",
			    long_options, &option_index);
      if (c == -1)
	break;
  
      switch (c)
      {
	case 'h': 
	case '?':
	  printf("Usage: %s <parameter_file_name>\n", argv[0]);
	  exit(1);
	default:
	  printf ("?? getopt returned character code 0%o ??\n", c);
      }
    }

    ConfigData->parameter_file_name = argv[optind];
  }
  else
  {
    printf("Usage: %s <parameter_file_name>\n", argv[0]);

    exit(1);
  }

  return 0;
}

