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

#include "cctk_ParamCheck.h"

#define NEED_PARAMETER_SCOPE_STRINGS
#define NEED_PARAMETER_TYPE_STRINGS

#include "cctk_Parameter.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_CommandLine_c)


/********************************************************************
 ********************* Local Routine Prototypes *********************
 ********************************************************************/

static void CommandLinePrintParameter(const cParamData *properties);

/********************************************************************
 ********************* Other Routine Prototypes *********************
 ********************************************************************/

/* FIXME: these should be put in a header somewhere */

char *compileTime(void);
char *compileDate(void);
char *CCTK_FullVersion(void);
int CCTK_CommandLine(char ***outargv);

/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/

static int redirectsubs = 0;
static int paramchecking = 0;

/********************************************************************
 *********************     Global Data   *****************************
 ********************************************************************/

int cctki_paramchecking;
int cctki_paramcheck_nprocs;

/********************************************************************
 *********************     External Routines   **********************
 ********************************************************************/

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

  CCTK_Exit(retval,NULL);
}


 /*@@
   @routine    CCTKi_CommandLineDescribeAllParameters
   @date       Tue Apr 18 15:00:12 2000
   @author     Tom Goodale
   @desc 
   Describe all the parameters
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_CommandLineDescribeAllParameters(const char *optarg)
{
  int first;
  int n_thorns;
  int thorn;
  const char *thornname;
  char *param;
  const cParamData *properties;

  n_thorns = CCTK_NumCompiledThorns ();

  for(thorn = 0; thorn < n_thorns; thorn++)
  {
    thornname = CCTK_CompiledThorn (thorn);
    printf("\nParameters of thorn '%s' providing implementation '%s':\n",
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
            fprintf(stderr, "Unknown verbosity option %s\n", optarg);
            CCTK_Exit(2,NULL);
        }
      }
      else
      {
        printf("%s\n", param);
      }

      free(param);
      first = 0;
    }
  }

  /*  CCTKi_BindingsParameterHelp(NULL,"%s",stdout);*/
 
  CCTK_Exit(0,NULL);
}

 /*@@
   @routine    CCTKi_CommandLineDescribeParameter
   @date       Tue Apr 18 15:00:33 2000
   @author     Tom Goodale
   @desc 
   Describe a particular parameter.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
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

 /*@@
   @routine    CCTKi_CommandLineTestParameters
   @date       Tue Apr 18 15:00:45 2000
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
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

  paramchecking = 1;

  cctki_paramchecking = 1;
  cctki_paramcheck_nprocs = nprocs;
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

 /*@@
   @routine    CCTKi_CommandLineListThorns
   @date       Tue Apr 18 15:05:00 2000
   @author     Tom Goodale
   @desc 
   List the thorns which are compiled in.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_CommandLineListThorns(void)
{
  printf ("\n---------------Compiled Thorns-------------\n");
  CCTKi_PrintThorns(stdout, "  %s\n", 0);
  printf ("-------------------------------------------\n\n");
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
  char **argv;

  const char *version=NULL;

  CCTK_CommandLine(&argv);

  version = (const char *)CCTK_FullVersion();

  printf("%s: Version %s.  Compiled on %s at %s\n", argv[0], version, 
          compileDate(), compileTime());

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
  char **argv;

  CCTK_CommandLine(&argv);

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
  char **argv;

  CCTK_CommandLine(&argv);

  printf("Usage: %s [-h] [-O] [-o paramname] [-x [nprocs]] [-W n] [-E n] [-r] [-T] [-t name] [-v] <parameter_file_name>\n", argv[0]);
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

  /* Are we in a paramcheck run ? */
  if(! paramchecking)
  {
    cctki_paramchecking = 0;
  }

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
      /* FIXME:  remove this #ifdef in beta 8 */
#ifdef NULL_DEVICE
      sprintf(fname,NULL_DEVICE);
#else
#ifdef WIN32
      /* hack for Windows which doesn't know about /dev/null */
      sprintf(fname,"NUL");
#else
      sprintf(fname,"/dev/null");
#endif
#endif /* NULL_DEVICE */
    }

    freopen(fname,"w",stdout);
  }


}

/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/

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

  if(properties)
  {
    printf("Parameter:   %s::%s", properties->thorn, properties->name);
    if(properties->scope != SCOPE_PRIVATE)
    {
      printf(", %s::%s", CCTK_ThornImplementation(properties->thorn),
                         properties->name);
    }
    printf("\n");
    printf("Description: \"%s\"\n", properties->description);
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
