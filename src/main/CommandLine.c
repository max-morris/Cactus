 /*@@
   @file      CommandLine.c
   @date      Wed Feb 17 00:11:26 1999
   @author    Tom Goodale
   @desc 
   Routines to deal with command line arguments.
   @enddesc 
 @@*/

#include <stdio.h>
#include <string.h>

#include "CommandLine.h"
#include "WarnLevel.h"

static char *rcsid = "$Header$";

/*Prototypes for some functions */

char *compileTime();
char *compileDate();
int CCTK_GetCommandLine(char ***outargv);

/* The functions used to deal with each option. */

void CCTK_CommandLineTestThornActive(const char *optarg)
{

}
void CCTK_CommandLineDescribeAllParameters(void)
{

}
void CCTK_CommandLineDescribeParameter(const char *optarg)
{

}
void CCTK_CommandLineTestParameters(const char *optarg)
{
  int nprocs;

  if(optarg == NULL)
  {
    nprocs = 1;
  }
  else
  {
    nprocs = atoi(optarg);
  }

}

 /*@@
   @routine    CCTK_CommandLineWarningLevel
   @date       Wed Feb 17 00:58:56 1999
   @author     Tom Goodale
   @desc 
   Sets the CCTK warning level from a command line argument. 
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTK_CommandLineWarningLevel(const char *optarg)
{
  int warninglevel;

  warninglevel = atoi(optarg);

  CCTK_SetWarnLevel(warninglevel);

}

 /*@@
   @routine    CCTK_CommandLineErrorLevel
   @date       Wed Feb 17 00:58:56 1999
   @author     Tom Goodale
   @desc 
   Sets the CCTK error level from a command line argument. 
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTK_CommandLineErrorLevel(const char *optarg)
{
  int errorlevel;

  errorlevel = atoi(optarg);

  CCTK_SetErrorLevel(errorlevel);

}

void CCTK_CommandLineRedirectStderr(void)
{

}
void CCTK_CommandLineListActiveThorns(void)
{

}
void CCTK_CommandLineVersion(void)
{
  int argc;
  char **argv;

  argc = CCTK_GetCommandLine(&argv);

  printf("%s, compiled on %s at %s\n", argv[0], compileDate(), compileTime());

  exit(1);
}

void CCTK_CommandLineHelp(void)
{
  CCTK_CommandLineUsage();
}

void CCTK_CommandLineUsage(void)
{
  int argc;
  char **argv;

  argc = CCTK_GetCommandLine(&argv);

  printf("Usage: %s <parameter_file_name>\n", argv[0]);
  exit(1);
}  
