 /*@@
   @file      Overload.c
   @date      Wed Feb  3 23:27:18 1999
   @author    Tom Goodale
   @desc 
   Contains routines to overload the communication functions.
   Uses the overload macros to make sure of consistency and
   to save typing !
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "cctk_Flesh.h"
#include "cctk_FortranString.h"
#include "OverloadMacros.h"
#include "cctk_WarnLevel.h"

static char *rcsid="$Header$";

/* Define the prototypes for the dummy functions. */
#define OVERLOADABLE(name) OVERLOADABLE_DUMMYPROTOTYPE(name)

  /* Deal seperately with the SetupGH routine */
#define CCTKi_DummySetupGH  CactusDefaultSetupGH
#define CCTKi_DummyMyProc  CactusDefaultMyProc
#define CCTKi_DummynProcs  CactusDefaultnProcs
#define CCTKi_DummyExit    CactusDefaultExit

#include "CommOverloadables.h"

  /* Reset the #define to prevent complications. */
#undef CCTKi_DummySetupGH  
#undef CCTKi_DummyMyProc  
#undef CCTKi_DummynProcs
#undef CCTKi_DummyExit

#undef OVERLOADABLE

/* Create the overloadable function variables and the 
 * functions allowing the variables to be set.
 */
#define OVERLOADABLE(name) OVERLOADABLE_FUNCTION(name)

#include "CommOverloadables.h"

#undef OVERLOADABLE


 /*@@
   @routine    CCTKi_SetupCommFunctions
   @date       Thu Feb  4 08:21:26 1999
   @author     Tom Goodale
   @desc 
   Set any comm function which hasn't been overloaded to the default.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTKi_SetupCommFunctions(void)
{

#define OVERLOADABLE(name) OVERLOADABLE_CHECK(name)

  /* Deal seperately with the SetupGH routine */
#define CCTKi_DummySetupGH CactusDefaultSetupGH
#define CCTKi_DummyMyProc  CactusDefaultMyProc
#define CCTKi_DummynProcs  CactusDefaultnProcs
#define CCTKi_DummyExit    CactusDefaultExit

#include "CommOverloadables.h"

  /* Reset the #define to prevent complications. */
#undef CCTKi_DummySetupGH  
#undef CCTKi_DummyMyProc  
#undef CCTKi_DummynProcs
#undef CCTKi_DummyExit

#undef OVERLOADABLE

  return 0;
}

/* Create the dummy functions. */
#define OVERLOADABLE(name) OVERLOADABLE_DUMMY(name)

#include "CommOverloadables.h"

#undef OVERLOADABLE



/*    Fortran bindings for the comm functions */



int FMODIFIER FORTRAN_NAME(CCTK_Exit)(int *retval, cGH *GH)
{
  CCTK_Exit(*retval, GH);
}

int FMODIFIER FORTRAN_NAME(CCTK_ParallelInit)(cGH *GH)
{
  CCTK_ParallelInit(GH);
  return 0;
}

int FMODIFIER FORTRAN_NAME(CCTK_Abort)(cGH *GH)
{
  CCTK_Abort(GH);
  return 0;
}

int FMODIFIER FORTRAN_NAME(CCTK_SyncGroup)(cGH *GH, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(group_name)
  CCTK_SyncGroup(GH,group_name);
  free(group_name); 
  return 0;
}

int FMODIFIER FORTRAN_NAME(CCTK_nProcs)(cGH *GH)
{
  return CCTK_nProcs(GH);
}
 
int FMODIFIER FORTRAN_NAME(CCTK_MyProc)(cGH *GH)
{
  return CCTK_MyProc(GH);
}
 
