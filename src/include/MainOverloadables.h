 /*@@
   @header    MainOverloadables.h
   @date      Thu Feb  4 08:58:52 1999
   @author    Tom Goodale
   @desc 
   The overloadable functions for the main layer.
   See OverloadMacros.h to see how to use these.
   @enddesc 
   @version $Header$
 @@*/

#ifdef ARGUMENTS
#undef ARGUMENTS
#endif

#ifdef RETURN_TYPE
#undef RETURN_TYPE
#endif

#define RETURN_TYPE int
#define ARGUMENTS tFleshConfig *config

OVERLOADABLE(Initialise)
OVERLOADABLE(Evolve)
OVERLOADABLE(Shutdown)

#undef ARGUMENTS
#undef RETURN_TYPE
