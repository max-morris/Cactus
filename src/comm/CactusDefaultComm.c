 /*@@
   @file      CactusDefaultComm.c
   @date      Tue Sep 29 15:06:22 1998
   @author    Tom Goodale
   @desc 
   Default communication routines.
   @enddesc 
 @@*/


#include <stdio.h>
#include <stdlib.h>

#include "flesh.h"
#include "CactusMainDefaults.h"
#include "GHExtensions.h"

static char *rcsid = "$Id$";


 /*@@
   @routine    CactusDefaultSetupGH
   @date       Tue Sep 29 15:06:22 1998
   @author     Tom Goodale
   @desc 
   Default cactus SetupGH routine.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
cGH *CactusDefaultSetupGH(tFleshConfig *config, int convergence_level)
{
  cGH *retval;

  cGH *thisGH;

  int n_variables;
  int variable;

  retval = NULL;

  /* Put this in for the moment until parameter stuff is done. */
  if(convergence_level > 0)
  {
    return retval;
  }

  /* Create a new Grid Hierarchy */
  thisGH = (cGH *)malloc(sizeof(cGH));

  if(thisGH)
  {
    thisGH->dim = CCTK_GetMaxDim();
    thisGH->iteration = 0;
    thisGH->local_shape = (int *)malloc(thisGH->dim*sizeof(int));
    thisGH->lower_bound = (int *)malloc(thisGH->dim*sizeof(int));
    thisGH->upper_bound = (int *)malloc(thisGH->dim*sizeof(int));
    thisGH->bbox        = (int *)malloc(2*thisGH->dim*sizeof(int));

    thisGH->levfac = 1;

    n_variables = CCTK_GetNVariables();

    thisGH->data = (void **)malloc(n_variables*sizeof(void *));

    if(thisGH->data)
    {
      for(variable = 0; variable < n_variables; variable++)
      {
	thisGH->data[variable] = NULL;
      }
    }

    thisGH->extensions = NULL;

    thisGH->GroupData = (cGHGroupData *)malloc(CCTK_GetNGroups()*sizeof(cGHGroupData));

  }
  
  if(thisGH && 
     thisGH->local_shape &&
     thisGH->lower_bound &&
     thisGH->upper_bound &&
     thisGH->bbox &&
     thisGH->data &&
     thisGH->GroupData)
  {
    /* Traverse list of GH setup routines. */
    CCTK_SetupGHExtensions(config, convergence_level, thisGH);

    retval = thisGH;
  }

  return thisGH;
}


