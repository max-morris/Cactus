 /*@@
   @file      InitialiseCactus.c
   @date      Fri Sep 18 14:04:02 1998
   @author    Tom Goodale
   @desc 
   Responsible for doing any cactus specific initialisations
   @enddesc 
 @@*/

static char *rcsid = "$Header$";

#include <stdio.h>

#include "cctk.h"
#include "cctk_Flesh.h"

#include "cctk_schedule.h"
#include "cctki_schedule.h"

int ProcessCommandLine(int *inargc, char ***inargv, tFleshConfig *ConfigData);
int ProcessEnvironment(int *argc, char ***argv,tFleshConfig *ConfigData);



 /*@@
   @routine    InitialiseCactus
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
int InitialiseCactus(int *argc, char ***argv, tFleshConfig *ConfigData)
{

  InitialiseSubsystemDefaults();

  ProcessEnvironment(argc, argv, ConfigData);

  ProcessCommandLine(argc, argv, ConfigData);

  CactusBanner();

  InitialiseDataStructures(ConfigData);

  ProcessParameterDatabase(ConfigData);

  InitialiseScheduler(ConfigData);

  CallStartupFunctions(ConfigData);

  CCTK_PrintBanners();

  return 0;
}

 /*@@
   @routine    InitialiseScheduler
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
int InitialiseScheduler(tFleshConfig *ConfigData)
{
  int retcode;
  
  CCTKi_BindingsScheduleInitialise();

  retcode = CCTKi_ScheduleSortAllGroups();

  CCTK_PRINTSEPARATOR
  CCTK_SchedulePrint(NULL);
  CCTK_PRINTSEPARATOR

  return retcode;
}



