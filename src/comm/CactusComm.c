

#include <stdio.h>

#include "cctk.h"
#include "flesh.h"
#include "Comm.h"
#include "GHExtensions.h"

int *CCTK_ArrayGroupSize_ByIndex(cGH *GH, int index, int dir)
{
  return CCTK_ArrayGroupSize_ByBoth(GH,index,NULL,dir);
}

int *CCTK_ArrayGroupSize(cGH *GH, const char *groupname, int dir)
{
  return CCTK_ArrayGroupSize_ByBoth(GH,-1,groupname,dir);
}
