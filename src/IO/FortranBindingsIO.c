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
#include "cctk_IO.h"
#include "cctk_IOMethods.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(IO_FortranBindingsIO_c)

void CCTK_FCALL CCTK_FNAME(CCTK_OutputGH)
     (int *istat,cGH *GH)
{
  *istat = CCTK_OutputGH(GH);
}

void CCTK_FCALL CCTK_FNAME(CCTK_RegisterIOMethod)
     (int *handle, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name);
  *handle = CCTK_RegisterIOMethod(name);
  free(name);
}
 
