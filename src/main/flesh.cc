 /*@@
   @file      flesh.cc
   @date      Fri Sep 18 14:17:08 1998
   @author    Tom Goodale
   @desc 
   Main program file for cactus.
   @enddesc 
 @@*/
#include <stdio.h>

#include "cctk_Flesh.h"
#include "CactusMainFunctions.h"

static char *rcsid = "$Id$";

 /*@@
   @routine    main
   @date       Fri Sep 18 14:17:37 1998
   @author     Tom Goodale
   @desc 
   Main program for cactus.  This has to be c++ as on some
   architectures you need the main program in c++ if there's
   going to be any c++ at all in your program.
   
   @enddesc 
   @calls     InitialiseCactus Initialise Evolve Shutdown ShutdownCactus
   @calledby   
   @history 
 
   @endhistory 
   @var     argc
   @vdesc   The number of command line arguments
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     argv
   @vdesc   The command line arguments
   @vtype   char *[]
   @vio     in
   @vcomment 
 
   @endvar 

@@*/
int main(int argc, char **argv)
{
  tFleshConfig ConfigData;

  /* Initialise any cactus specific stuff.
   */
  InitialiseCactus(&argc, &argv, &ConfigData);

  /* This is a (c-linkage) routine which has been registered by a thorn.
   */
  CCTK_Initialise(&ConfigData);

  /* This is a (c-linkage) routine which has been registered by a thorn.
   */
  CCTK_Evolve(&ConfigData);

  /* This is a (c-linkage) routine which has been registered by a thorn.
   */
  CCTK_Shutdown(&ConfigData);

  /* Shut down any cactus specific stuff.
   */
  ShutdownCactus(&ConfigData);

  return 0;
}

