 /*@@
   @file      CommandLine.c
   @date      Wed Feb 17 00:11:26 1999
   @author    Tom Goodale
   @desc 
   Routines to deal with command line arguments.
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CommandLine.h"
#include "WarnLevel.h"
#include "CCTK_Bindings.h"

/* FIXME. This shouldn't be here !*/
#include "thornlist.h"

static char *rcsid = "$Header$";

/*Prototypes for some functions */

char *compileTime(void);
char *compileDate(void);
int CCTK_GetCommandLine(char ***outargv);

/* FIXME. This shouldn't be in this file */
int CCTK_IsThornCompiled(const char *thorn) ;


/* The functions used to deal with each option. */

 /*@@
   @routine    CCTK_CommandLineTestThorncompiled
   @date       Wed Feb 17 10:25:30 1999
   @author     Gabrielle Allen
   @desc 
   Tests if a given thorn has been compiled. 
   At the moment the given thorn must be in the format
   <package name>/<thorn name>
   @enddesc 
   @calls      CCTK_IsThornCompiled 
   @calledby   
   @history 
 
   @endhistory 

@@*/

void CCTK_CommandLineTestThornCompiled(const char *optarg)
{
  if(CCTK_IsThornCompiled(optarg))
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
  CCTK_BindingsParameterHelp(NULL,"%s",stdout);
  exit(1);
}

void CCTK_CommandLineDescribeParameter(const char *optarg)
{
  CCTK_BindingsParameterHelp(optarg,"%s",stdout);
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
void CCTK_CommandLineListThorns(void)
{
  int i;
  printf ("\n---------------Compiled Thorns-------------\n");
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
  int argc;
  char **argv;

  argc = CCTK_GetCommandLine(&argv);

  printf("%s, compiled on %s at %s\n", argv[0], compileDate(), compileTime());
  printf("Usage: %s [-h] [-O] [-o paramname] [-x [nprocs]] [-W n] [-E n] [-r] [-T] [-t name] [-v] parameter_file_name>\n", argv[0]);

  printf("\n");
  printf("Valid options:\n");
  printf("-h, -help                           : gets this help.\n");
  printf("-O, -describe-all-parameters        : describes all the parameters.\n");
  printf("-o, -describe-parameter <paramname> : describe the given parameter.\n");
  printf("-x, -test-parameters [nprocs]       : does a quick test of the parameter file\n"
	 "                                      pretending to be on nprocs processors, \n"
	 "                                      or 1 if not given.\n");
  printf("-W, -warning-level <n>              : Sets the warning level to n.\n");
  printf("-E, -error-level <n>                : Sets the error level to n.\n");
  printf("-r, -redirect-stderr                : Redirects standard error to files.\n");
  printf("-T, -list-thorns                    : Lists the compiled-in thorns.\n");
  printf("-t, -test-thorn-compiled <name>     : Tests for the presence of thorn <name>.\n");
  printf("-v, -version                        : Prints the version.\n");

  exit(1);
}

void CCTK_CommandLineUsage(void)
{
  int argc;
  char **argv;

  argc = CCTK_GetCommandLine(&argv);

  printf("Usage: %s [-h] [-O] [-o paramname] [-x [nprocs]] [-W n] [-E n] [-r] [-T] [-t name] [-v] parameter_file_name>\n", argv[0]);
  exit(1);
}  





 /*@@
   @routine    CCTK_IsThornCompiled
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

int CCTK_IsThornCompiled(const char *thorn) 
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

