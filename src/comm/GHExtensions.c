 /*@@
   @file      GHExtensions.c
   @date      Fri Jan 15 13:22:47 1999
   @author    Tom Goodale
   @desc 
   Functions to deal with GH extensions
   @enddesc 
 @@*/

#include <stdlib.h>

#include "flesh.h"
#include "StoreHandledData.h"

static char *rcsid = "$Id$";

static cHandledData *GHExtensions = NULL;
static int num_extensions = 0;

struct GHExtension
{
  int number;
  void *(*init)(cGH *);
};

int CCTK_TraverseGHExtensions(cGH *GH, const char *when)
{
  return 0;
}

int CCTK_RegisterGHExtension(cGH *GH, const char *name)
{
  int return_val;

  int handle;

  struct GHExtension *new_extension;

  /* Check that the extension hasn't already been registered */
  handle = CCTK_GetHandle(GHExtensions, name, NULL);

  if(handle < 0)
  {
    /* New extension. */
    new_extension = (struct GHExtension *)malloc(sizeof(struct GHExtension));

    if(new_extension)
    {
      handle = CCTK_NewHandle(&GHExtensions, name, new_extension);

      new_extension->init = NULL;
      
      /* Remember how many extensions there are */
      num_extensions++;
    }
    else
    {
      /* Memory failure. */
      handle = -2;
    }
  }
  else
  {
    /* Extension already exists. */
    handle = -1;
  }
    
  return handle;
}

int CCTK_RegisterGHExtensionInitialiser(int handle, void *(*func)(cGH *))
{
  int return_code;
  struct GHExtension *extension;

  extension = CCTK_GetHandledData(GHExtensions, handle);

  if(extension)
  {
    extension->init = func;
    return_code = 1;
  }
  else
  {
    return_code = 0;
  }

  return return_code;
}
