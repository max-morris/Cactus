 /*@@
   @header    Comm.h
   @date      Sat Feb  13 19:42:29 1999
   @author    Tom Goodale
   @desc 
   Header defining the variables holding the overloaded communication functions
   @enddesc 
   @version $Header$
 @@*/

#ifndef _COMM_H_
#define _COMM_H_

#include "OverloadMacros.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Define the prototypes for the functions. */
#define OVERLOADABLE(name) OVERLOADABLE_PROTOTYPE(name)

#include "CommOverloadables.h"

#undef OVERLOADABLE

#ifdef __cplusplus
}
#endif

#endif
