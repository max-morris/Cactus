 /*@@
   @file      InitialiseCactus.c
   @date      Fri Sep 18 14:04:02 1998
   @author    Tom Goodale
   @desc 
   Responsible for doing any cactus specific initialisations
   @enddesc 
 @@*/

#include <stdio.h>

#include "flesh.h"

static char *rcsid = "$Id$";


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

  ProcessCommandLine(argc, argv, ConfigData);

  InitialiseDataStructures(ConfigData);

  ProcessParameterDatabase(ConfigData);

  CallStartupFunctions(ConfigData);

  return 0;
}





