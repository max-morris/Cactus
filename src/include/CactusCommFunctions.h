 /*@@
   @header    CactusCommFunctions.h
   @date      Thu Jan 14 18:37:58 1999
   @author    Tom Goodale
   @desc 
   Overloadable communication functions
   @enddesc 
 @@*/
 
/* $Id$ */

#ifndef _CACTUSCOMMFUNCTIONS_H_
#define _CACTUSCOMMFUNCTIONS_H_

#include <stdarg.h>

#include "OverloadMacros.h"

#ifdef _cplusplus
extern "C" {
#endif

/* The functions. */

#define OVERLOADABLE(name) OVERLOADABLE_PROTOTYPE(name)

#include "CommOverloadables.h"

#undef OVERLOADABLE(name)

#ifdef _cplusplus
	   }
#endif

#endif
