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

static char *rcsid = "$Header$";

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
  int test_nprocs = 1;
  int c;

  /* Store the command line */
  argc = *inargc;

  argv = *inargv;

  /* Process the command line - needs some work !*/

  if(argc>1)
  {
    while (1)
    {
      struct option long_options[] =
      {
	{"help", no_argument, NULL, 'h'},
	{"describe-all-parameters", no_argument, NULL, 'O'},
	{"describe-parameter", required_argument, NULL, 'o'},
	{"test-parameters", optional_argument, NULL, 'x'},
	{"warning-level", required_argument, NULL, 'W'},
	{"error-level", required_argument, NULL, 'E'},
	{"redirect-stdout", no_argument, NULL, 'r'},
	{"list-thorns", no_argument, NULL, 'T'},
	{"test-thorn-compiled", required_argument, NULL, 't'},
	{"version", no_argument, NULL, 'v'},
	{0, 0, 0, 0}
      };
      
      c = getopt_long_only (argc, argv, "hOo:x::W:E:rTt:v",
			    long_options, &option_index);
      if (c == -1)
	break;
  
      switch (c)
      {
	case 't': CCTK_CommandLineTestThornCompiled(optarg); break;
	case 'O': CCTK_CommandLineDescribeAllParameters(); break;
	case 'o': CCTK_CommandLineDescribeParameter(optarg); break;
	case 'x': CCTK_CommandLineTestParameters(optarg); break;
	case 'W': CCTK_CommandLineWarningLevel(optarg); break;
	case 'E': CCTK_CommandLineErrorLevel(optarg); break;
	case 'r': CCTK_CommandLineRedirectStdout(); break;
	case 'T': CCTK_CommandLineListThorns(); break;
	case 'v': CCTK_CommandLineVersion(); break;
	case 'h': 
	case '?':
	  CCTK_CommandLineHelp(); break;
	default:
	  printf ("?? getopt returned character code 0%o ??\n", c);
      }
    }

    if(argc > optind)
    {
      ConfigData->parameter_file_name = argv[optind];
    }
    else
    {
      CCTK_CommandLineUsage();
    }
  }
  else
  {
    CCTK_CommandLineUsage();
  }

  CCTK_CommandLineFinished();

  return 0;
}


 /*@@
   @routine    CCTK_GetCommandLine
   @date       Wed Feb 17 00:19:30 1999
   @author     Tom Goodale
   @desc 
   Gets the command line arguments.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_GetCommandLine(char ***outargv)
{
  *outargv = argv;

  return argc;
}
