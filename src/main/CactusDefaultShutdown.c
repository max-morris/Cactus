 /*@@
   @file      CactusDefaultShutdown.c
   @date      Fri Feb 26 16:53:58 1999
   @author    Tom Goodale
   @desc 
   The default shutdown routines.
   @enddesc 
 @@*/

#include <stdio.h>

#include "cctk.h"

#include "cctk_Flesh.h"

#include "cctk_Comm.h"

#ifdef MPI
#include "mpi.h"
#endif

static char *rcsid = "$Header$";

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

 /*@@
   @routine    CactusDefaultShutdown
   @date       Tue Sep 29 12:45:04 1998
   @author     Tom Goodale
   @desc 
   DEfault shutdown routine.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CactusDefaultShutdown(tFleshConfig *config)
{
  int myproc;

  myproc = CCTK_MyProc(config->GH[0]);

#ifdef MPI
  if(MPI_Active)
  {
    CACTUS_MPI_ERROR(MPI_Finalize());
  }
#endif

  if(myproc == 0)
  {
    /* printf("Goodbye! Adeu! Adios! Tchuess! Au Revoir! Ciao! Math sin leat!\n"); */
    printf("--------------------------------------------------------------------------------\n");
    printf("Done.\n");
  }

  return 0;
}
