

#include <stdio.h>

#include "cctk.h"
#include "cctk_Flesh.h"
#include "cctk_Comm.h"
#include "cctk_GHExtensions.h"

int *CCTK_ArrayGroupSizeI(cGH *GH, int dir, int index)
{
  return CCTK_ArrayGroupSizeB(GH,dir,index,NULL);
}

int *CCTK_ArrayGroupSize(cGH *GH, int dir, const char *groupname)
{
  return CCTK_ArrayGroupSizeB(GH,dir,-1,groupname);
}

int CCTK_QueryGroupStorageI(cGH *GH, int index)
{
  return CCTK_QueryGroupStorageB(GH,index , NULL);
}

int CCTK_QueryGroupStorage(cGH *GH, const char *groupname)
{
  return CCTK_QueryGroupStorageB(GH, -1, groupname);
}
