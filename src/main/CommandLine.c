 /*@@
   @file      CommandLine.c
   @date      Wed Feb 17 00:11:26 1999
   @author    Tom Goodale
   @desc 
   Routines to deal with command line arguments.
   @enddesc 
 @@*/

/* Joan */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CommandLine.h"
#include "thornlist.h"
#include "WarnLevel.h"

static char *rcsid = "$Header$";

/*Prototypes for some functions */

char *compileTime();
char *compileDate();
int CCTK_GetCommandLine(char ***outargv);

/* The functions used to deal with each option. */

 /*@@
   @routine    CCTK_CommandLineTestThornActive
   @date       Wed Feb 17 10:25:30 1999
   @author     Gabrielle Allen
   @desc 
   Tests if a given thorn has been compiled. 
   At the moment the given thorn must be in the format
   <package name>/<thorn name>
   @enddesc 
   @calls      CCTK_IsThornActive 
   @calledby   
   @history 
 
   @endhistory 

@@*/

void CCTK_CommandLineTestThornActive(const char *optarg)
{
  if(CCTK_IsThornActive(optarg))
  {
    printf("Thorn '%s' available.\n", optarg);
  }
  else
  {
    printf("Thorn '%s' unavailable.\n", optarg);
  }
  exit(1);
}

void CCTK_CommandLineDescribeAllParameters(void)
{

}

void CCTK_CommandLineDescribeParameter(const char *optarg)
{
  CCTK_BindingsParameterHelp(optarg,NULL,NULL);
  exit(1);
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
  int i;
  printf ("\n---------------Active Thorns---------------\n");
  for(i=0; i < nthorns; i++)
  {
    fprintf(stdout, "%s\n", thorn_name[i]);
  }
  printf ("-------------------------------------------\n\n");
  exit(1);
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





 /*@@
   @routine    CCTK_IsThornActive
   @date       Sat May 16 14:47:14 1998
   @author     Tom Goodale
   @desc 
   Determines if a thorn was compiled into cactus.
   Returns 1 if the thorn is available, 0 otherwise.
   @enddesc 
   @calls     
   @calledby   
   @history 
   @endhistory 

@@*/

int CCTK_IsThornActive(const char *thorn) 
{
  int i;
  char full_thorn_name[507];

  for(i=0; i < nthorns; i++)
  {
    if(!strcmp(thorn_name[i], thorn)) return 1;
    sprintf(full_thorn_name, "thorn_%s", thorn_name[i]);
    if(!strcmp(full_thorn_name, thorn)) return 1;    
  };
  
  return 0;
}

