 /*@@
   @file      CactusDefaultMainLoopIndex.c
   @date      Wed Oct 04 16:53:58 2000
   @author    Gabrielle Allen
   @desc 
   The default main loop index routines
   @enddesc 
 @@*/

#include "cctk.h"

static const char *rcsid = "$Header$";

CCTK_FILEVERSION(main_CactusDefaultMainLoopIndex_c)

static int iteration = 0;

 /*@@
   @routine    CactusDefaultMainLoopIndex
   @date       Wed Oct 04 16:53:58 2000
   @author     Gabrielle Allen
   @desc 
   Default main loop index routine
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CactusDefaultMainLoopIndex(void)
{
  return (iteration);
}


 /*@@
   @routine    CactusDefaultSetMainLoopIndex
   @date       Sep 22 1999
   @author     Thomas Radke
   @desc 
               Sets the iteration counter variable of the evolution loop.
               This is used for recovery.
   @enddesc 
   @calls     
   @calledby   

@@*/
int CactusDefaultSetMainLoopIndex (int main_loop_index)
{
  iteration = main_loop_index;
  return iteration;
}

