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

#ifdef MPI
#include "mpi.h"
#endif

#ifdef MPI
extern char MPI_Active;
#endif

#ifdef MPI
#define CACTUS_MPI_ERROR(xf)  do {int errcode; \
                                    if((errcode = xf) != MPI_SUCCESS)                     \
				    {                                                     \
				      char mpi_error_string[MPI_MAX_ERROR_STRING+1];      \
				      int resultlen;                                      \
				      MPI_Error_string(errcode, mpi_error_string, &resultlen);\
				      fprintf(stderr, "MPI Call %s returned error code %d (%s)\n", \
                                      #xf, errcode, mpi_error_string);                    \
				      fprintf(stderr, "At line %d of file %s\n",                   \
					     __LINE__, __FILE__);                         \
				    }                                                     \
				  } while (0)
#endif

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
    thisGH->delta_space = (Double *)malloc(thisGH->dim*sizeof(Double));
    /* FIXME : Next line goes when coords are done properly */
    thisGH->origin_space = (Double *)malloc(thisGH->dim*sizeof(Double));

    thisGH->delta_time = 1;
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
     thisGH->delta_space &&
     thisGH->origin_space &&
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
  int myproc;

#ifdef MPI
  if(MPI_Active)
  {
    CACTUS_MPI_ERROR(MPI_Comm_rank(MPI_COMM_WORLD, &myproc));
  }
#else
  myproc = 0;
#endif

  return myproc;
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
  int nprocs;

#ifdef MPI
  if(MPI_Active)
  {
    CACTUS_MPI_ERROR(MPI_Comm_size(MPI_COMM_WORLD, &nprocs));
  }
#else
  nprocs = 1;
#endif

  return nprocs;
}
