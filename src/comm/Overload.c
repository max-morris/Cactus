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

#include "flesh.h"
#include "OverloadMacros.h"

static char *rcsid="$Header$";

/* Define the prototypes for the dummy functions. */
#define OVERLOADABLE(name) OVERLOADABLE_DUMMYPROTOTYPE(name)

  /* Deal seperately with the SetupGH routine */
#define CCTK_DummySetupGH  CactusDefaultSetupGH

#include "CommOverloadables.h"

  /* Reset the #define to prevent complications. */
#undef CCTK_DummySetupGH  

#undef OVERLOADABLE(name)

/* Create the overloadable function variables and the 
 * functions allowing the variables to be set.
 */
#define OVERLOADABLE(name) OVERLOADABLE_FUNCTION(name)

#include "CommOverloadables.h"

#undef OVERLOADABLE(name)


 /*@@
   @routine    SetupCommFunctions(void)
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
int SetupCommFunctions(void)
{

#define OVERLOADABLE(name) OVERLOADABLE_CHECK(name)

  /* Deal seperately with the SetupGH routine */
#define CCTK_DummySetupGH  CactusDefaultSetupGH

#include "CommOverloadables.h"

  /* Reset the #define to prevent complications. */
#undef CCTK_DummySetupGH  

#undef OVERLOADABLE(name)

  return 0;
}

/* Create the dummy functions. */
#define OVERLOADABLE(name) OVERLOADABLE_DUMMY(name)

#include "CommOverloadables.h"

#undef OVERLOADABLE(name)

