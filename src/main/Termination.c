 /*@@
   @file      Termination.c
   @date      Thu March 15 7pm
   @author    Gabrielle Allen
   @desc 
   Termination conditions 
   @enddesc 
   @version $Header$
 @@*/

#include <stdio.h>
#include <stdlib.h>

#include "cctk_Flesh.h"
#include "cctk_Parameter.h"

#include "cctk_Main.h"
#include "cctk_IO.h"

static const char *rcsid="$Header$";

CCTK_FILEVERSION(main_Termination_c)

/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/

/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/

static int termination_reached = 0;

/********************************************************************
 ********************* Local Routine Prototypes *********************
 ********************************************************************/

/********************************************************************
 *********************     External Routines   **********************
 ********************************************************************/

 /*@@
   @routine    CCTK_TerminationReached
   @date       Thu March 15 7pm
   @author     Gabrielle Allen
   @desc 
   Returns true if Cactus will terminate on the next iteration
   @enddesc 
   @calls     
   @calledby   
   @history 
   @hdate @hauthor 
   @hdesc 
   @endhistory 

@@*/
int CCTK_TerminationReached(cGH *GH)
{
  return termination_reached;
}
  
 /*@@
   @routine    CCTK_TerminateNext
   @date       Thu March 15 7pm
   @author     Gabrielle Allen
   @desc 
   Sets termination for next iteration
   @enddesc 
   @calls     
   @calledby   
   @history 
   @hdate @hauthor 
   @hdesc 
   @endhistory 

@@*/
void CCTK_TerminateNext(cGH *GH)
{
  termination_reached = 1;
}
  
/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/

