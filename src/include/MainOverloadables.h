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

#undef RETURN_TYPE
#define RETURN_TYPE int
#undef ARGUMENTS
#define ARGUMENTS void

OVERLOADABLE(MainLoopIndex)

#undef RETURN_TYPE
#define RETURN_TYPE int
#undef ARGUMENTS
#define ARGUMENTS int main_loop_index

OVERLOADABLE(SetMainLoopIndex)

#undef ARGUMENTS
#undef RETURN_TYPE
