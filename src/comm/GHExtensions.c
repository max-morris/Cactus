 /*@@
   @file      GHExtensions.c
   @date      Fri Jan 15 13:22:47 1999
   @author    Tom Goodale
   @desc 
   Functions to deal with GH extensions
   @enddesc 
 @@*/

#include "flesh.h"

static char *rcsid = "$Id$";

int CCTK_TraverseGHExtensions(cGH *GH, const char *when)
{
  return 0;
}

int CCTK_RegisterGHExtension(cGH *GH, const char *name)
{
  return -1;
}

int CCTK_RegisterGHExtensionInitialiser(int handle, void (*func)())
{
  return -1;
}
