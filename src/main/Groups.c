 /*@@
   @file      Groups.c
   @date      Mon Feb  1 12:16:28 1999
   @author    Tom Goodale
   @desc 
   Routines to deal with groups.
   @enddesc 
 @@*/

#include"flesh.h"

static char *rcsid = "$Id$";

int CCTK_EnableGroupStorage(cGH *GH, const char *group)
{
  return 0;
}

int CCTK_DisableGroupStorage(cGH *GH, const char *group)
{
  return 0;
}

int CCTK_EnableGroupCommunication(cGH *GH, const char *group)
{
  return 0;
}

int CCTK_DisableGroupCommunication(cGH *GH, const char *group)
{
  return 0;
}
