 /*@@
   @file      InitialiseCactus.c
   @date      Fri Sep 18 14:04:02 1998
   @author    Tom Goodale
   @desc 
   Responsible for doing any cactus specific initialisations
   @enddesc 
 @@*/

#include <stdio.h>
#include <time.h>

#include "cctk_Flesh.h"
#include "cctk_Parameter.h"
#include "cctk_Schedule.h"
#include "cctk_WarnLevel.h"

#include "cctki_Banner.h"
#include "cctki_Bindings.h"
#include "cctki_Schedule.h"

int CCTKi_InitialiseScheduler(tFleshConfig *ConfigData);
int CCTKi_InitialiseSubsystemDefaults(void);
int CCTKi_ProcessEnvironment(int *argc, char ***argv,tFleshConfig *ConfigData);
int CCTKi_BindingsParameterRecoveryInitialise(void);

#define CCTK_PRINTSEPARATOR \
  printf("--------------------------------------------------------------------------------\n");

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_InitialiseCactus_c)

int ProcessCommandLine(int *inargc, char ***inargv, tFleshConfig *ConfigData);
int ProcessEnvironment(int *argc, char ***argv,tFleshConfig *ConfigData);

static time_t startuptime;
  

 /*@@
   @routine    CCTKi_InitialiseCactus
   @date       Fri Sep 18 14:05:21 1998
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls      ProcessCommandLine ProcessParameterDatabase CallStartupFunctions
   @calledby   main  
   @history 
 
   @endhistory 
   @var     argc
   @vdesc   The number of command line arguments
   @vtype   int *
   @vio     inout
   @vcomment 

   @endvar 
   @var     argv
   @vdesc   The command line arguments
   @vtype   char **
   @vio     inout
   @vcomment 
 
   @endvar 
   @var     ConfigData
   @vdesc   Flesh configuration data
   @vtype   tFleshConfig
   @vio     inout
   @vcomment 
 
   @endvar 

@@*/
int CCTKi_InitialiseCactus(int *argc, char ***argv, tFleshConfig *ConfigData)
{
  startuptime = time(NULL);

  CCTKi_InitialiseSubsystemDefaults();

  CCTKi_ProcessEnvironment(argc, argv, ConfigData);

  CCTKi_ProcessCommandLine(argc, argv, ConfigData);

  CCTKi_CactusBanner();

  CCTKi_InitialiseDataStructures(ConfigData);

  CCTKi_ProcessParameterDatabase(ConfigData);

  CCTKi_BindingsVariablesInitialise();

  CCTKi_InitialiseScheduler(ConfigData);

  CCTKi_CallStartupFunctions(ConfigData);

  CCTKi_PrintBanners();

  return 0;
}


 /*@@
   @routine    CCTKi_InitialiseScheduler
   @date       Fri Sep 17 19:34:55 1999
   @author     Tom Goodale
   @desc 
   Initialise all scheduled items
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTKi_InitialiseScheduler(tFleshConfig *ConfigData)
{
  int retcode;
  extern void CCTKi_SetParameterSetMask(int mask);

  CCTKi_SetParameterSetMask(PARAMETER_RECOVERY_IN);

  if(CCTKi_BindingsParameterRecoveryInitialise() < 0)
    CCTK_Warn(0,__LINE__,__FILE__,"Cactus","Failed to recover parameters");

  CCTKi_SetParameterSetMask(PARAMETER_RECOVERY_POST);

  CCTKi_BindingsScheduleInitialise();

  retcode = CCTKi_DoScheduleSortAllGroups();

  CCTK_PRINTSEPARATOR
  CCTK_SchedulePrint(NULL);
  CCTK_PRINTSEPARATOR

  return retcode;
}



 /*@@
   @routine    CCTK_RunTime
   @date       Tue Oct 3 2000
   @author     Gabrielle Allen
   @desc 
   Seconds since startup
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_RunTime()
{
  int retval;
  time_t currenttime;

  currenttime = time(NULL);
  
  retval = (int)(currenttime-startuptime);

  return retval;
}



