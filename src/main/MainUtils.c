 /*@@
   @file      MainUtils.c
   @date      Sep 22 1999
   @author    Thomas Radke, Gabrielle Allen
   @desc 
   Utility Flesh routines
   @enddesc 
   @version $Header$
 @@*/

#include <string.h>

#include "cctk_Flesh.h"
#include "cctk_Misc.h"
#include "cctk_Schedule.h"
#include "cctk_Parameter.h"

static char *rcsid = "$Header$";

static const char *startuptime = "How do I do this";

CCTK_FILEVERSION(main_MainUtils_c)


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

 /*@@
   @routine    CCTK_RunTitle
   @date       Sun Sep 17 2000
   @author     Gabrielle Allen
   @desc 
   Returns the simulation description
   @enddesc 
   @calls     
   @calledby   

@@*/
int CCTK_RunTitle(int len, char *title)
{
  int retval; 
  int param_type;
  const char *cctk_title;

  cctk_title = (*(const char **)
		CCTK_ParameterGet("cctk_run_title",
				  "Cactus",
				  &param_type));
  
  if (cctk_title)
  {
    if (CCTK_Equals(cctk_title,""))
    {
      strcpy(title,"Cactus Simulation");
    }
    else
    {
      strncpy(title,cctk_title,len-1);
    }
    retval = strlen(title);
    retval=retval > len ? 0 : retval;
  }
  else
  {
    retval = -1;
  }
  return retval;
}

