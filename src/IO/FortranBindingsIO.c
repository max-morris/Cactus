 /*@@
   @file      FortranBindingsIO.c
   @date      Thu Feb  18 14:27:18 1999
   @author    Gabrielle Allen
   @desc 
   Fortran bindings for the IO functions
   @enddesc 
 @@*/ 

#include <stdlib.h>
#include "cctk_Flesh.h"
#include "cctk_FortranString.h"
#include "cctk_Comm.h"

void FMODIFIER FORTRAN_NAME(CCTK_OutputVarAsByMethod)(int *istat, cGH *GH, THREE_FORTSTRINGS_ARGS)
{
  THREE_FORTSTRINGS_CREATE(variable,method,alias);
  *istat = CCTK_OutputVarAsByMethod(GH,variable,method,alias);
  free(variable);
  free(method);
  free(alias);
}

void FMODIFIER FORTRAN_NAME(CCTK_OutputGH)(int *istat,cGH *GH)
{
  *istat = CCTK_OutputGH(GH);
}

void FMODIFIER FORTRAN_NAME(CCTK_RegisterIOMethod)(int *handle, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name);
  *handle = CCTK_RegisterIOMethod(name);
  free(name);
}
 
