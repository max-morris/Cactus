 /*@@
   @header    CommOverloadables.h
   @date      Thu Feb  4 08:11:41 1999
   @author    Tom Goodale
   @desc
              The overloadable functions for the comm layer.
              See OverloadMacros.h to see how to use these.
   @enddesc
   @version   $Header$
 @@*/

#include "cctk_Flesh.h"
#include "cctk_GroupsOnGH.h"

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
#define ARGUMENTS cGH *GH, const char *group
OVERLOADABLE(SyncGroup)

OVERLOADABLE(EnableGroupStorage)
OVERLOADABLE(DisableGroupStorage)

OVERLOADABLE(EnableGroupComm)
OVERLOADABLE(DisableGroupComm)

#undef ARGUMENTS
#define ARGUMENTS const cGH *GH
OVERLOADABLE(Barrier)
OVERLOADABLE(MyProc)
OVERLOADABLE(nProcs)

#undef ARGUMENTS
#define ARGUMENTS cGH *GH
OVERLOADABLE(ParallelInit)

#undef ARGUMENTS
#define ARGUMENTS cGH *GH, int retval
#undef RETURN_TYPE
#define RETURN_TYPE int
OVERLOADABLE(Exit)
OVERLOADABLE(Abort)

#undef ARGUMENTS
#define ARGUMENTS tFleshConfig *config, int convergence_level
#undef RETURN_TYPE
#define RETURN_TYPE cGH *
OVERLOADABLE(SetupGH)

#undef ARGUMENTS
#define ARGUMENTS const cGH *GH, int dir, int group, const char *groupname
#undef RETURN_TYPE
#define RETURN_TYPE const int *
OVERLOADABLE(ArrayGroupSizeB)

#undef ARGUMENTS
#define ARGUMENTS const cGH *GH, int group, const char *groupname
#undef RETURN_TYPE
#define RETURN_TYPE int
OVERLOADABLE(QueryGroupStorageB)

#undef ARGUMENTS
#define ARGUMENTS const cGH *GH, int group, cGroupDynamicData *data
#undef RETURN_TYPE
#define RETURN_TYPE int
OVERLOADABLE(GroupDynamicData)

#undef ARGUMENTS
#undef RETURN_TYPE

#undef OVERLOADABLE_CALL
#undef OVERLOADABLE_PREFIX
#undef OVERLOADABLE_DUMMY_PREFIX
