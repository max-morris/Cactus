 /*@@
   @file      Overload.c
   @date      Wed Feb  3 23:27:18 1999
   @author    Tom Goodale
   @desc
              Contains routines to overload the communication functions.
              Uses the overload macros to make sure of consistency and
              to save typing !
   @enddesc
   @version   $Id$
 @@*/

#include <stdio.h>
#include <stdlib.h>

#include "cctk_Flesh.h"
#include "cctk_FortranString.h"
#include "cctk_WarnLevel.h"
#include "CactusRegister.h"
#include "OverloadMacros.h"

static const char *rcsid = "$Header$";
CCTK_FILEVERSION(comm_OverloadComm_c)


/* Define the prototypes for the dummy functions. */
#define OVERLOADABLE(name) OVERLOADABLE_DUMMYPROTOTYPE(name)

  /* Deal seperately with the SetupGH routine */
#define CCTKi_DummyAbort    CactusDefaultAbort
#define CCTKi_DummyBarrier  CactusDefaultBarrier
#define CCTKi_DummyExit     CactusDefaultExit
#define CCTKi_DummyMyProc   CactusDefaultMyProc
#define CCTKi_DummynProcs   CactusDefaultnProcs
#define CCTKi_DummySetupGH  CactusDefaultSetupGH

#include "CommOverloadables.h"

  /* Reset the #define to prevent complications. */
#undef CCTKi_DummySetupGH  
#undef CCTKi_DummyMyProc  
#undef CCTKi_DummynProcs
#undef CCTKi_DummyBarrier
#undef CCTKi_DummyExit
#undef CCTKi_DummyAbort

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

#define OVERLOADABLE(name) OVERLOADABLE_INITIALISE(name)

  /* Deal seperately with the SetupGH routine */
#define CCTKi_DummyAbort   CactusDefaultAbort
#define CCTKi_DummyBarrier CactusDefaultBarrier
#define CCTKi_DummyExit    CactusDefaultExit
#define CCTKi_DummyMyProc  CactusDefaultMyProc
#define CCTKi_DummynProcs  CactusDefaultnProcs
#define CCTKi_DummySetupGH CactusDefaultSetupGH

#include "CommOverloadables.h"

  /* Reset the #define to prevent complications. */
#undef CCTKi_DummyAbort
#undef CCTKi_DummyBarrier
#undef CCTKi_DummyExit
#undef CCTKi_DummyMyProc  
#undef CCTKi_DummynProcs
#undef CCTKi_DummySetupGH  

#undef OVERLOADABLE

  return 0;
}

/* Create the dummy functions. */
#define OVERLOADABLE(name) OVERLOADABLE_DUMMY(name)

#include "CommOverloadables.h"

#undef OVERLOADABLE



/* Fortran bindings prototypes for the comm functions */
int CCTK_FCALL CCTK_FNAME (CCTK_nProcs) (const cGH *GH);
int CCTK_FCALL CCTK_FNAME (CCTK_MyProc) (const cGH *GH);
#if 0
void CCTK_FCALL CCTK_FNAME (CCTK_Barrier) (int *ierr, const cGH *GH);
#else
int CCTK_FCALL CCTK_FNAME(CCTK_Barrier)(const cGH *GH);
#endif
void CCTK_FCALL CCTK_FNAME (CCTK_Exit) (int *ierr, cGH *GH, const int *retval);
void CCTK_FCALL CCTK_FNAME (CCTK_Abort) (int *ierr, cGH *GH, const int *retval);
#if 0
void CCTK_FCALL CCTK_FNAME (CCTK_SyncGroup) (int *ierr, cGH *GH, ONE_FORTSTRING_ARG);
#else
int CCTK_FCALL CCTK_FNAME(CCTK_SyncGroup)(cGH *GH, ONE_FORTSTRING_ARG);
#endif
void CCTK_FCALL CCTK_FNAME (CCTK_EnableGroupComm) (int *ierr, cGH *GH, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (CCTK_DisableGroupComm) (int *ierr, cGH *GH, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (CCTK_EnableGroupStorage) (int *ierr, cGH *GH, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (CCTK_DisableGroupStorage) (int *ierr, cGH *GH, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (CCTK_QueryGroupStorage) (int *ierr, const cGH *GH, ONE_FORTSTRING_ARG);


/* Fortran bindings definitions for the comm functions */
int CCTK_FCALL CCTK_FNAME (CCTK_nProcs) (const cGH *GH)
{
  return (CCTK_nProcs (GH));
}
 
int CCTK_FCALL CCTK_FNAME (CCTK_MyProc) (const cGH *GH)
{
  return (CCTK_MyProc (GH));
}
 
#if 0
void CCTK_FCALL CCTK_FNAME (CCTK_Barrier) (int *ierr, const cGH *GH)
{
  *ierr = CCTK_Barrier (GH);
}
#else
int CCTK_FCALL CCTK_FNAME(CCTK_Barrier)(const cGH *GH)
{
  return CCTK_Barrier(GH);
}
#endif

void CCTK_FCALL CCTK_FNAME (CCTK_Exit) (int *ierr, cGH *GH, const int *retval)
{
  *ierr = CCTK_Exit (GH, *retval);
}

void CCTK_FCALL CCTK_FNAME (CCTK_Abort) (int *ierr, cGH *GH, const int *retval)
{
  *ierr = CCTK_Abort (GH, *retval);
}

#if 0
void CCTK_FCALL CCTK_FNAME (CCTK_SyncGroup) (int *ierr, cGH *GH, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (group_name)
  *ierr = CCTK_SyncGroup (GH, group_name);
  free (group_name); 
}
#else
int CCTK_FCALL CCTK_FNAME(CCTK_SyncGroup)(cGH *GH, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(group_name)
  CCTK_SyncGroup(GH,group_name);
  free(group_name);
  return 0;
}
#endif

void CCTK_FCALL CCTK_FNAME (CCTK_EnableGroupComm) (int *ierr, cGH *GH, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (group_name)
  *ierr = CCTK_EnableGroupComm (GH, group_name); 
  free (group_name);
}

void CCTK_FCALL CCTK_FNAME (CCTK_DisableGroupComm) (int *ierr, cGH *GH, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (group_name)
  *ierr = CCTK_DisableGroupComm (GH, group_name); 
  free (group_name);
}

void CCTK_FCALL CCTK_FNAME (CCTK_EnableGroupStorage) (int *ierr, cGH *GH, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (group_name)
  *ierr = CCTK_EnableGroupStorage (GH, group_name);
  free (group_name);
}

void CCTK_FCALL CCTK_FNAME (CCTK_DisableGroupStorage) (int *ierr, cGH *GH, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (group_name)
  *ierr = CCTK_DisableGroupStorage (GH, group_name);
  free (group_name);
}

void CCTK_FCALL CCTK_FNAME (CCTK_QueryGroupStorage) (int *ierr, const cGH *GH, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (group_name)
  *ierr = CCTK_QueryGroupStorage (GH, group_name);
  free (group_name);
}
