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

#include "cctk_Flesh.h"
#include "cctk_Groups.h"
#include "cctk_Constants.h"
#include "CactusMainDefaults.h"
#include "cctk_GHExtensions.h"

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

static char *rcsid = "$Header$";


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
  int i;

  retval = NULL;

  /* Put this in for the moment until parameter stuff is done. */
  if(convergence_level > 0)
  {
    return retval;
  }

  /* Initialise this since it is used later and in exceptional
   * circumstances might not be initialsed beforehand. 
   */

  variable = -1;

  /* Create a new Grid Hierarchy */
  thisGH = (cGH *)malloc(sizeof(cGH));

  if(thisGH)
  {
    thisGH->cctk_dim = CCTK_MaxDim();

    /* Need this to be at least one otherwise the memory allocation will fail. */
    if(thisGH->cctk_dim == 0) thisGH->cctk_dim = 1;
    thisGH->cctk_iteration    = 0;
    thisGH->cctk_gsh          = (int *)malloc(thisGH->cctk_dim*sizeof(int));
    thisGH->cctk_lsh          = (int *)malloc(thisGH->cctk_dim*sizeof(int));
    thisGH->cctk_lbnd         = (int *)malloc(thisGH->cctk_dim*sizeof(int));
    thisGH->cctk_ubnd         = (int *)malloc(thisGH->cctk_dim*sizeof(int));
    
    thisGH->cctk_lssh         = (int *)malloc(CCTK_NSTAGGER*thisGH->cctk_dim*sizeof(int));
    thisGH->cctk_to           = (int *)malloc(thisGH->cctk_dim*sizeof(int));
    thisGH->cctk_from         = (int *)malloc(thisGH->cctk_dim*sizeof(int));
    thisGH->cctk_bbox         = (int *)malloc(2*thisGH->cctk_dim*sizeof(int));
    thisGH->cctk_nghostzones  = (int *)malloc(2*thisGH->cctk_dim*sizeof(int));
    thisGH->cctk_levfac       = (int *)malloc(thisGH->cctk_dim*sizeof(int));
    thisGH->cctk_delta_space  = (CCTK_REAL *)malloc(thisGH->cctk_dim*sizeof(CCTK_REAL));
    /* FIXME : Next line goes when coords are done properly */
    thisGH->cctk_origin_space = (CCTK_REAL *)malloc(thisGH->cctk_dim*sizeof(CCTK_REAL));

    thisGH->cctk_delta_time = 1;
    thisGH->cctk_convlevel = 0;

    n_variables = CCTK_NumVars();

    /* Allocate memory for the variable data pointers.
     * Note we want at least one to prevent memory allocattion from failing !
     */
    thisGH->data = (void ***)malloc((n_variables ? n_variables:1)*sizeof(void **));

    if(thisGH->data)
    {
      for(variable = 0; variable < n_variables; variable++)
      {
        ntimelevels = CCTK_NumTimeLevelsFromVarI(variable);

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
    n_groups = CCTK_NumGroups();
    thisGH->GroupData = (cGHGroupData *)malloc((n_groups ? n_groups:1)*sizeof(cGHGroupData));

  }
  
  if(thisGH && 
     thisGH->cctk_gsh &&
     thisGH->cctk_lsh &&
     thisGH->cctk_lbnd &&
     thisGH->cctk_ubnd &&
     thisGH->cctk_lssh &&
     thisGH->cctk_from &&
     thisGH->cctk_to &&
     thisGH->cctk_bbox &&
     thisGH->cctk_nghostzones &&
     thisGH->cctk_levfac &&
     thisGH->cctk_delta_space &&
     thisGH->cctk_origin_space &&
     thisGH->data &&
     variable == n_variables &&
     thisGH->GroupData)
  {
    /* Traverse list of GH setup routines. */
    CCTKi_SetupGHExtensions(config, convergence_level, thisGH);

    retval = thisGH;
  }

  return retval;
}

 /*@@
   @routine    CactusDefaultMyProc
   @date       Tue Jan 23 1999
   @author     Gabrielle Allen
   @desc 
   Default cactus MyProc routine.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CactusDefaultMyProc(cGH *GH)
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
   @routine    CactusDefaultnProcs
   @date       Tue Jan 23 1999
   @author     Gabrielle Allen
   @desc 
   Default cactus nProcs routine.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CactusDefaultnProcs(cGH *GH)
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
