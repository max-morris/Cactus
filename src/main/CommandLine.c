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
#include "cctki_ActiveThorns.h"
#include "cctk_ActiveThorns.h"
#include "cctki_WarnLevel.h"

#include "CactusCommFunctions.h"

#define NEED_PARAMETER_SCOPE_STRINGS
#define NEED_PARAMETER_TYPE_STRINGS

#include "cctk_Parameter.h"

static char *rcsid = "$Header$";

/*Prototypes for some functions */

char *compileTime(void);
char *compileDate(void);
char *CCTK_FullVersion(void);
int CCTK_CommandLine(char ***outargv);

static void CommandLinePrintParameter(const cParamData *properties);

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
  char mess[512];

  if((retval = CCTK_IsThornCompiled(optarg)))
  {
    sprintf(mess,"Thorn '%s' available.\n", optarg);
  }
  else
  {
    sprintf(mess,"Thorn '%s' unavailable.\n", optarg);
  }
  CCTK_Info("CommandlineParser", mess);
  CCTK_Exit(retval,NULL);
}


void CCTKi_CommandLineDescribeAllParameters(const char *optarg)
{
  int first;
  int n_thorns;
  int thorn;
  const char *thornname;
  char *param;
  const cParamData *properties;
  char mess[1024];

  n_thorns = CCTK_NumCompiledThorns ();

  for(thorn = 0; thorn < n_thorns; thorn++)
  {
    thornname = CCTK_CompiledThorn (thorn);
    sprintf(mess,"\nParameters of thorn '%s' providing implementation '%s':\n",
           thornname, CCTK_ThornImplementation(thornname));

    first = 1;
    while (CCTK_ParameterWalk (first, thornname, &param, &properties) == 0)
    {
      if(optarg)
      {
        switch(*optarg)
        {
          case 'v':
            CommandLinePrintParameter(properties);
            break;
          default :
            sprintf(mess,"%sUnknown verbosity option %s\n", mess, optarg);
	    CCTK_Info("CommandlineParser", mess);
            CCTK_Exit(2,NULL);
        }
      }
      else
      {
        sprintf(mess,"%s%s\n", mess, param);
      }

      free(param);
      first = 0;
    }
  }

  /*  CCTKi_BindingsParameterHelp(NULL,"%s",stdout);*/
  CCTK_Info("CommandlineParser", mess);
  CCTK_Exit(0,NULL);
}

void CCTKi_CommandLineDescribeParameter(const char *optarg)
{
  char *thorn;
  char *param;
  const cParamData *properties;
  const char *cthorn;

  Util_SplitString(&thorn, &param, optarg, "::");

  if(!param)
  {
    properties = CCTK_ParameterData(optarg, NULL);
  }
  else
  {
    properties = CCTK_ParameterData(param, thorn);

    if(!properties)
    {
      cthorn = CCTK_ImplementationThorn(thorn);
      properties = CCTK_ParameterData(param, cthorn);
    }

    free(thorn);
    free(param);
  }

  CommandLinePrintParameter(properties);
    
  CCTK_Exit(0,NULL);
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
  char mess[2048];
  CCTK_Info("CommandlineParser", "\n---------------Compiled Thorns-------------\n");
  CCTKi_PrintThorns(stdout, "  %s\n", 0);
  /*$CCTK_Info ("CommandlineParser", "-------------------------------------------\n\n");$*/
  CCTK_Exit(1,NULL);
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
  char mess[512];

  const char *version=NULL;

  argc = CCTK_CommandLine(&argv);

  version = (const char *)CCTK_FullVersion();

  sprintf(mess,"%s: Version %s.  Compiled on %s at %s\n", argv[0], version, 
          compileDate(), compileTime());
  CCTK_Info("CommandlineParser", mess);
  CCTK_Exit(1,NULL);
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
  char *mess;

  argc = CCTK_CommandLine(&argv);
  mess = (char*) malloc (2048*sizeof(char));

  sprintf(mess,"\n%s, compiled on %s at %s\n", argv[0], compileDate(), compileTime());
  sprintf(mess,"%sUsage: %s [-h] [-O] [-o paramname] [-x [nprocs]] [-W n] [-E n] [-r] [-T] [-t name] [-v] <parameter_file_name>\n", mess, argv[0]);

  sprintf(mess,"%s\n", mess);
  sprintf(mess,"%sValid options:\n", mess);
  sprintf(mess,"%s-h, -help                           : gets this help.\n", mess);
  sprintf(mess,"%s-O, -describe-all-parameters        : describes all the parameters.\n", mess);
  sprintf(mess,"%s-o, -describe-parameter <paramname> : describe the given parameter.\n", mess);
  sprintf(mess,"%s-x, -test-parameters [nprocs]       : does a quick test of the parameter file\n"
         "                                      pretending to be on nprocs processors, \n"
         "                                      or 1 if not given.\n", mess);
  sprintf(mess,"%s-W, -warning-level <n>              : Sets the warning level to n.\n", mess);
  sprintf(mess,"%s-E, -error-level <n>                : Sets the error level to n.\n", mess);
  sprintf(mess,"%s-r, -redirect-stdout                : Redirects standard output to files.\n", mess);
  sprintf(mess,"%s-T, -list-thorns                    : Lists the compiled-in thorns.\n", mess);
  sprintf(mess,"%s-t, -test-thorn-compiled <name>     : Tests for the presence of thorn <name>.\n", mess);
  sprintf(mess,"%s-v, -version                        : Prints the version.\n", mess);
  CCTK_Info("CommandlineParser",mess);
  CCTK_Exit(1,NULL);
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
  char mess[512];

  argc = CCTK_CommandLine(&argv);

  sprintf(mess,"Usage: %s [-h] [-O] [-o paramname] [-x [nprocs]] [-W n] [-E n] [-r] [-T] [-t name] [-v] <parameter_file_name>\n", argv[0]);
  CCTK_Info("CommandlineParser", mess);
  CCTK_Exit(1,NULL);
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
   @routine    CommandLinePrintParameter
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
static void CommandLinePrintParameter(const cParamData *properties)
{
  t_range *range;
  char mess[2048];

  sprintf(mess,"");
  if(properties)
  {
    sprintf(mess, "%sParameter:   %s::%s", mess, properties->thorn, properties->name);
    if(properties->scope != SCOPE_PRIVATE)
    {
      sprintf(mess, "%s, %s::%s", mess,CCTK_ThornImplementation(properties->thorn),
                         properties->name);
    }
    sprintf(mess, "%s\n");
    sprintf(mess, "%sDescription: \"%s\"\n", mess,properties->description);
    sprintf(mess, "%sType:        %s\n", mess,cctk_parameter_type_names[properties->type-1]);
    sprintf(mess, "%sDefault:     %s\n", mess,properties->defval);
    sprintf(mess, "%sScope:       %s\n", mess,cctk_parameter_scopes[properties->scope-1]);
    
    for(range=properties->range; range; range=range->next)
    {
      sprintf(mess, "%s  Range:     %s\n", mess,range->range);
      sprintf(mess, "%s    Origin:      %s\n", mess,range->origin);
      sprintf(mess, "%s    Description: %s\n", mess,range->description);
    }
  }

  return;
}
