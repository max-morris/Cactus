 /*@@
   @file      MainUtils.c
   @date      Sep 22 1999
   @author    Thomas Radke, Gabrielle Allen
   @desc 
   Utility Flesh routines
   @enddesc 
   @version $Header$
 @@*/

/* the iteration counter used in the evolution loop */
static int iteration = 0;


 /*@@
   @routine    CCTK_SetMainLoopIndex
   @date       Sep 22 1999
   @author     Thomas Radke
   @desc 
               Sets the iteration counter variable of the evolution loop.
               This is used for recovery.
   @enddesc 
   @calls     
   @calledby   

@@*/
int CCTK_SetMainLoopIndex (int main_loop_index)
{
  iteration = main_loop_index;
  return iteration;
}


 /*@@
   @routine    CCTK_MainLoopIndex
   @date       Sep 22 1999
   @author     Thomas Radke
   @desc 
               Returns the iteration counter variable of the evolution loop.
               This is used for checkpointing.
   @enddesc 
   @calls     
   @calledby   

@@*/
int CCTK_MainLoopIndex (void)
{
  return (iteration);
}

