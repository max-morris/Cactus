 /*@@
   @header    cctk_IO.h
   @date      
   @author    Tom Goodale
   @desc 
   Header defining the variables holding the overloaded communication functions
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTK_IO_H_
#define _CCTK_IO_H_

#include "OverloadMacros.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Define the prototypes for the overloading functions. */
#define OVERLOADABLE(name) OVERLOADABLE_OVERLOADPROTO(name)

#include "IOOverloadables.h"

#undef OVERLOADABLE


/* Define the prototypes for the functions. */
#define OVERLOADABLE(name) OVERLOADABLE_PROTOTYPE(name)

#include "IOOverloadables.h"

#undef OVERLOADABLE

#ifdef __cplusplus
}
#endif

#endif




