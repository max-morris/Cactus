 /*@@
   @file      FortranBindings.c
   @date      Thu Feb  18 14:27:18 1999
   @author    Gabrielle Allen
   @desc 
   Fortran bindings for the comm functions
   @enddesc 
 @@*/

#include "flesh.h"
#include "FortranString.h"
#include "Comm.h"

int FORTRAN_NAME(CCTK_Exit)(cGH *GH)
{
  CCTK_Exit(GH);
  return 0;
}

int FORTRAN_NAME(CCTK_ParallelInit)(cGH *GH)
{
  CCTK_ParallelInit(GH);
  return 0;
}

int FORTRAN_NAME(CCTK_Abort)(cGH *GH)
{
  CCTK_Abort(GH);
  return 0;
}

int FORTRAN_NAME(CCTK_SyncGroup)(cGH *GH, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(group_name)
  CCTK_SyncGroup(GH,group_name);
  free(group_name); 
  return 0;
}
