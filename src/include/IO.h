 /*@@
   @header    IO.h
   @date      
   @author    Tom Goodale
   @desc 
   Header defining the variables holding the overloaded communication functions
   @enddesc 
   @version $Header$
 @@*/

#ifndef _IO_H_
#define _IO_H_

#include "OverloadMacros.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Define the prototypes for the functions. */
#define OVERLOADABLE(name) OVERLOADABLE_PROTOTYPE(name)

#include "IOOverloadables.h"

#undef OVERLOADABLE

#ifdef __cplusplus
}
#endif

#endif




