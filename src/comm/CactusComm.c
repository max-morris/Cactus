

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

int CCTK_QueryGroupStorage_ByIndex(cGH *GH, int index)
{
  return CCTK_QueryGroupStorage_ByBoth(GH,index , NULL);
}

int CCTK_QueryGroupStorage(cGH *GH, const char *groupname)
{
  return CCTK_QueryGroupStorage_ByBoth(GH, -1, groupname);
}
