 /*@@
   @header    CommOverloadables.h
   @date      Thu Feb  4 08:11:41 1999
   @author    Tom Goodale
   @desc 
   The overloadable functions for the comm layer.
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
#define ARGUMENTS cGH *GH, const char *group
OVERLOADABLE(SyncGroup)

OVERLOADABLE(EnableGroupStorage)
OVERLOADABLE(DisableGroupStorage)

OVERLOADABLE(EnableGroupComm)
OVERLOADABLE(DisableGroupComm)

#undef ARGUMENTS
#define ARGUMENTS cGH *GH
OVERLOADABLE(Barrier)
OVERLOADABLE(GetMyProc)
OVERLOADABLE(GetnProcs)

#undef ARGUMENTS
#define ARGUMENTS cGH *GH,                \
		   const char *operation, \
		   int n_infields,        \
		   int n_outfields,       \
		   int out_type,          \
		   void **outarray,       \
		   ...

OVERLOADABLE(Reduce)

#undef ARGUMENTS
#define ARGUMENTS  cGH *GH,                \
		   const char *operation,  \
		   int n_coords,           \
		   int n_infields,         \
		   int n_outfields,        \
		   int n_points,           \
		   int type,               \
		   ...

OVERLOADABLE(Interp)

#undef ARGUMENTS
#define ARGUMENTS cGH *GH
OVERLOADABLE(ParallelInit)
OVERLOADABLE(Exit)
OVERLOADABLE(Abort)

#undef ARGUMENTS
#define ARGUMENTS tFleshConfig *config, int convergence_level
#undef RETURN_TYPE 
#define RETURN_TYPE cGH *
OVERLOADABLE(SetupGH)

#undef ARGUMENTS
#define ARGUMENTS cGH *GH, int group, const char *groupname, int dir
#undef RETURN_TYPE 
#define RETURN_TYPE int *
OVERLOADABLE(ArrayGroupSize_ByBoth)

#undef ARGUMENTS
#define ARGUMENTS cGH *GH, int group, const char *groupname
#undef RETURN_TYPE
#define RETURN_TYPE int
OVERLOADABLE(QueryGroupStorage_ByBoth)

#undef ARGUMENTS
#undef RETURN_TYPE




