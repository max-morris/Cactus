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
#include "Groups.h"
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

  int n_groups;
  int n_variables;
  int variable;
  int ntimelevels;
  int level;

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
    thisGH->data = (void ***)malloc((n_variables ? n_variables:1)*sizeof(void **));

    if(thisGH->data)
    {
      for(variable = 0; variable < n_variables; variable++)
      {
	ntimelevels = CCTK_GetNumTimeLevels(variable);

	thisGH->data[variable] = (void **)malloc(ntimelevels*sizeof(void *));
	if(thisGH->data[variable])
	{
	  for(level = 0; level < ntimelevels; level++)
	  {
	    thisGH->data[variable][level] = NULL;
	  }
	}
	else
	{
	  break;
	}
      }
    }

    thisGH->extensions = NULL;

    /* Allocate memory for the group data pointers.
     * Note we want at least one to prevent memory allocattion from failing !
     */
    n_groups = CCTK_GetNumGroups();
    thisGH->GroupData = (cGHGroupData *)malloc((n_groups ? n_groups:1)*sizeof(cGHGroupData));

  }
  
  if(thisGH && 
     thisGH->global_shape &&
     thisGH->local_shape &&
     thisGH->lower_bound &&
     thisGH->upper_bound &&
     thisGH->bbox &&
     thisGH->data &&
     variable == n_variables &&
     thisGH->GroupData)
  {
    /* Traverse list of GH setup routines. */
    CCTK_SetupGHExtensions(config, convergence_level, thisGH);

    retval = thisGH;
  }

  return retval;
}

 /*@@
   @routine    CactusDefaultGetMyProc
   @date       Tue Jan 23 1999
   @author     Gabrielle Allen
   @desc 
   Default cactus GetMyProc routine.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CactusDefaultGetMyProc(cGH *GH)
{
  return 0;
}

 /*@@
   @routine    CactusDefaultGetnProcs
   @date       Tue Jan 23 1999
   @author     Gabrielle Allen
   @desc 
   Default cactus GetnProcs routine.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CactusDefaultGetnProcs(cGH *GH)
{
  return 1;
}
