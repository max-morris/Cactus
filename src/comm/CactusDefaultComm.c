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

    /* Need this to be at least one otherwise the memory allocation will fail. */
    if(thisGH->dim == 0) thisGH->dim = 1;
    thisGH->iteration = 0;
    thisGH->global_shape = (int *)malloc(thisGH->dim*sizeof(int));
    thisGH->local_shape = (int *)malloc(thisGH->dim*sizeof(int));
    thisGH->lower_bound = (int *)malloc(thisGH->dim*sizeof(int));
    thisGH->upper_bound = (int *)malloc(thisGH->dim*sizeof(int));
    thisGH->bbox        = (int *)malloc(2*thisGH->dim*sizeof(int));

    thisGH->levfac = 1;
    thisGH->convlevel = 1;
    thisGH->nghostzones = 0;

    n_variables = CCTK_GetNumVars();

    /* Allocate memory for the variable data pointers.
     * Note we want at least one to prevent memory allocattion from failing !
     */
    thisGH->data = (void **)malloc((n_variables||1)*sizeof(void *));

    if(thisGH->data)
    {
      for(variable = 0; variable < n_variables; variable++)
      {
	thisGH->data[variable] = NULL;
      }
    }

    thisGH->extensions = NULL;

    /* Allocate memory for the group data pointers.
     * Note we want at least one to prevent memory allocattion from failing !
     */
    thisGH->GroupData = (cGHGroupData *)malloc((CCTK_GetNumGroups()||1)*sizeof(cGHGroupData));

  }
  
  if(thisGH && 
     thisGH->global_shape &&
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

  return retval;
}


