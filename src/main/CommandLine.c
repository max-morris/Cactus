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
#include "cctk_Bindings.h"
#include "cctk_Misc.h"
#include "cctk_ActiveThorns.h"
#include "cctk_ParameterFunctions.h"
#include "cctki_WarnLevel.h"

static char *rcsid = "$Header$";

/*Prototypes for some functions */

char *compileTime(void);
char *compileDate(void);
int CCTK_GetCommandLine(char ***outargv);

static void CCTKi_CommandLinePrintParameter(t_param_prop *properties);

static int redirectsubs;

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
  int retval;

  if((retval = CCTK_IsThornCompiled(optarg)))
  {
    printf("Thorn '%s' available.\n", optarg);
  }
  else
  {
    printf("Thorn '%s' unavailable.\n", optarg);
  }

  exit(retval);
}


void CCTKi_CommandLineDescribeAllParameters(const char *optarg)
{
  int n_thorns;
  char **thornlist;
  int thorn;
  int n_parameters;
  char **parameterlist;
  int parameter;
  const char *implementation;
  t_param_prop *properties;

  CCTK_ThornList(0, &thornlist, &n_thorns);

  for(thorn = 0; thorn < n_thorns; thorn++)
  {
    implementation = CCTK_ThornImplementation(thornlist[thorn]);
    CCTK_ParameterList(thornlist[thorn], &parameterlist, &n_parameters);

    for(parameter = 0 ; parameter < n_parameters; parameter++)
    {
      properties = CCTK_ParameterInfo(parameterlist[parameter], thornlist[thorn]);

      if(optarg)
      {
        switch(*optarg)
        {
          case 'v':
            CCTKi_CommandLinePrintParameter(properties);
            break;
          default :
            fprintf(stderr, "Unknown verbosity option %s\n", optarg);
            exit(2);
        }
      }
      else
      {
        if(properties->scope == SCOPE_PRIVATE)
        {
          printf("%s::%s\n", thornlist[thorn], parameterlist[parameter]);
        }
        else
        {
          printf("%s::%s\n", implementation, parameterlist[parameter]);
        }
      }

      free(parameterlist[parameter]);
    }
    free(parameterlist);
    free(thornlist[thorn]);
  }
  free(thornlist);

  /*  CCTKi_BindingsParameterHelp(NULL,"%s",stdout);*/
 
  exit(0);
}

void CCTKi_CommandLineDescribeParameter(const char *optarg)
{
  char *thorn;
  char *param;
  t_param_prop *properties;
  const char *cthorn;

  Util_SplitString(&thorn, &param, optarg, "::");

  if(!param)
  {
    properties = CCTK_ParameterInfo(optarg, NULL);
  }
  else
  {
    properties = CCTK_ParameterInfo(param, thorn);

    if(!properties)
    {
      cthorn = CCTK_ImplementationThorn(thorn);
      properties = CCTK_ParameterInfo(param, cthorn);
    }

    free(thorn);
    free(param);
  }

  CCTKi_CommandLinePrintParameter(properties);
    
  exit(0);
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
  printf ("\n---------------Compiled Thorns-------------\n");
  CCTKi_ListThorns(stdout, "  %s\n", 0);
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
  printf("Usage: %s [-h] [-O] [-o paramname] [-x [nprocs]] [-W n] [-E n] [-r] [-T] [-t name] [-v] <parameter_file_name>\n", argv[0]);

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
  printf("-r, -redirect-stdout                : Redirects standard output to files.\n");
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

  printf("Usage: %s [-h] [-O] [-o paramname] [-x [nprocs]] [-W n] [-E n] [-r] [-T] [-t name] [-v] <parameter_file_name>\n", argv[0]);
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
#ifdef WIN32
      /* hack for Windows which doesn't know about /dev/null */
      sprintf(fname,"NUL");
#else
      sprintf(fname,"/dev/null");
#endif
    }

    freopen(fname,"w",stdout);
  }
}


 /*@@
   @routine    CCTKi_CommandLinePrintParameter
   @date       Sun Oct 17 22:11:31 1999
   @author     Tom Goodale
   @desc 
   Prints a parameter.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static void CCTKi_CommandLinePrintParameter(t_param_prop *properties)
{
  t_range *range;

  if(properties)
  {
    printf("Parameter:   %s::%s - \"%s\"\n", properties->thorn, properties->name, properties->description);
    printf("Type:        %s\n", cctk_parameter_type_names[properties->type-1]);
    printf("Default:     %s\n", properties->defval);
    printf("Scope:       %s\n", cctk_parameter_scopes[properties->scope-1]);
    
    for(range=properties->range; range; range=range->next)
    {
      printf("  Range:     %s\n", range->range);
      printf("    Origin:      %s\n", range->origin);
      printf("    Description: %s\n", range->description);
    }
  }

  return;
}
