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


#ifdef OVERLOADABLE_CALL
#undef OVERLOADABLE_CALL
#endif

#ifdef OVERLOABLE_PREFIX
#undef OVERLOADABLE_PREFIX
#endif

#ifdef OVERLOABLE_DUMMY_PREFIX
#undef OVERLOADABLE_DUMMY_PREFIX
#endif

#define OVERLOADABLE_CALL CCTK_
#define OVERLOADABLE_PREFIX CCTK_
#define OVERLOADABLE_DUMMY_PREFIX CCTKi_Dummy

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

#undef OVERLOADABLE_CALL
#undef OVERLOADABLE_PREFIX
#undef OVERLOADABLE_DUMMY_PREFIX
