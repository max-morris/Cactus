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
#include "cctk_Flesh.h"
#include "cGH.h"
#include "cctk_Comm.h"
#include "cctk_WarnLevel.h"
#include "CCTK_Bindings.h"

/* FIXME. This shouldn't be here !*/
#include "thornlist.h"

static char *rcsid = "$Header$";

/*Prototypes for some functions */

char *compileTime(void);
char *compileDate(void);
int CCTK_GetCommandLine(char ***outargv);

static int redirectsubs;

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

void CCTKi_CommandLineTestThornCompiled(const char *optarg)
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

void CCTKi_CommandLineDescribeAllParameters(void)
{
  CCTKi_BindingsParameterHelp(NULL,"%s",stdout);
  exit(1);
}

void CCTKi_CommandLineDescribeParameter(const char *optarg)
{
  CCTKi_BindingsParameterHelp(optarg,"%s",stdout);
  exit(1);
}

void CCTKi_CommandLineTestParameters(const char *optarg)
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
   @routine    CCTKi_CommandLineWarningLevel
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

void CCTKi_CommandLineWarningLevel(const char *optarg)
{
  int warninglevel;

  warninglevel = atoi(optarg);

  CCTKi_SetWarnLevel(warninglevel);

}

 /*@@
   @routine    CCTKi_CommandLineErrorLevel
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
void CCTKi_CommandLineErrorLevel(const char *optarg)
{
  int errorlevel;

  errorlevel = atoi(optarg);

  CCTKi_SetErrorLevel(errorlevel);

}

 /*@@
   @routine    CCTKi_CommandLineRedirectStdout
   @date       Fri Jul 23 11:32:46 1999
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_CommandLineRedirectStdout(void)
{
  /* Set the flag to say we need to redirect the stdout. */

  redirectsubs = 1;

}

void CCTKi_CommandLineListThorns(void)
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

 /*@@
   @routine    CCTKi_CommandLineVersion
   @date       Fri Jul 23 12:57:45 1999
   @author     Tom Goodale
   @desc 
   Prints version info
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_CommandLineVersion(void)
{
  int argc;
  char **argv;

  const char *version=NULL;

  argc = CCTK_GetCommandLine(&argv);

  version = (const char *)CCTK_FullVersion();

  printf("%s: Version %s.  Compiled on %s at %s\n", argv[0], version, 
	  compileDate(), compileTime());

  exit(1);
}

 /*@@
   @routine    CCTKi_CommandLineHelp
   @date       Fri Jul 23 12:57:23 1999
   @author     Tom Goodale
   @desc 
   Prints a help message
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_CommandLineHelp(void)
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

 /*@@
   @routine    CCTKi_CommandLineUsage
   @date       Fri Jul 23 12:57:04 1999
   @author     Tom Goodale
   @desc 
   Prints a usage message.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_CommandLineUsage(void)
{
  int argc;
  char **argv;

  argc = CCTK_GetCommandLine(&argv);

  printf("Usage: %s [-h] [-O] [-o paramname] [-x [nprocs]] [-W n] [-E n] [-r] [-T] [-t name] [-v] parameter_file_name>\n", argv[0]);
  exit(1);
}  

 /*@@
   @routine    CCTKi_CommandLineFinished
   @date       Fri Jul 23 12:55:39 1999
   @author     Tom Goodale
   @desc 
   Subroutine to do anything which has to be done based upon the 
   commandline, but needs to be have a default.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_CommandLineFinished(void)
{
  int myproc;

    /* Redirect output from sub-processors ... */

  if ((myproc = CCTK_MyProc(NULL)) != 0) 
  {
    char fname[256];
    if (redirectsubs)
    {
      sprintf(fname,"CCTK_Proc%d.out",myproc);
    }
    else
    {
      sprintf(fname,"/dev/null");
    }

    freopen(fname,"w",stdout);
  }
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



