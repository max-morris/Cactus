 /*@@
   @file      GHExtensions.c
   @date      Fri Jan 15 13:22:47 1999
   @author    Tom Goodale
   @desc 
   Functions to deal with GH extensions
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>

#include "flesh.h"
#include "GHExtensions.h"
#include "StoreHandledData.h"

static char *rcsid = "$Id$";

static cHandledData *GHExtensions = NULL;
static int num_extensions = 0;

struct GHExtension
{
  int number;
  void *(*SetupGH)(tFleshConfig *, int, cGH *);
  int    (*InitGH)(cGH *);
};

static int CheckAllExtensionsSetup(void);
static void *DummySetupGH(tFleshConfig *config, int convergence_level, cGH *GH);
static int DummyInitGH(cGH *GH);


int CCTK_TraverseGHExtensions(cGH *GH, const char *when)
{
  return 0;
}

int CCTK_SetupGHExtensions(tFleshConfig *config, 
			   int convergence_level, 
			   cGH *GH)
{
  int return_code;
  int handle;
  struct GHExtension *extension;

  if(CheckAllExtensionsSetup())
  {
    GH->extensions = (void **)malloc(num_extensions*sizeof(void *));

    if(GH->extensions)
    {
      for(handle = 0; handle < num_extensions; handle++)
      {
	extension =  (struct GHExtension *)CCTK_GetHandledData(GHExtensions, handle);
	GH->extensions[handle] = extension->SetupGH(config, 
						    convergence_level, 
						    GH);
      }
      return_code = 0;
    }
    else
    {
      return_code = 1;
    }
  }
  else
  {
    return_code = 2;
  }
  
  return return_code;
}

int CCTK_RegisterGHExtension(const char *name)
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
      /* Get a handle for it. */
      handle = CCTK_NewHandle(&GHExtensions, name, new_extension);

      /* Initialise the extension structure. */
      new_extension->InitGH = NULL;
      new_extension->SetupGH = NULL;
      
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

int CCTK_RegisterGHExtensionInitGH(int handle, int (*func)(cGH *))
{
  int return_code;
  struct GHExtension *extension;

  extension = CCTK_GetHandledData(GHExtensions, handle);

  if(extension)
  {
    extension->InitGH = func;
    return_code = 1;
  }
  else
  {
    return_code = 0;
  }

  return return_code;
}

int CCTK_RegisterGHExtensionSetupGH(int handle, 
				    void *(*func)(tFleshConfig *, int, cGH *))
{
  int return_code;
  struct GHExtension *extension;

  extension = CCTK_GetHandledData(GHExtensions, handle);

  if(extension)
  {
    extension->SetupGH = func;
    return_code = 1;
  }
  else
  {
    return_code = 0;
  }

  return return_code;
}

static int CheckAllExtensionsSetup(void)
{
  int return_code;
  int handle;
  struct GHExtension *extension;

  return_code = 1;

  for(handle = 0; handle < num_extensions; handle++)
  {
    extension =  (struct GHExtension *)CCTK_GetHandledData(GHExtensions, handle);

    if(!extension->SetupGH)
    {
      fprintf(stderr, 
	      "Warning: GH Extension '%s' has not regidtered a SetupGH routine.\n",
	      CCTK_GetHandleName(GHExtensions, handle));
      extension->SetupGH=DummySetupGH;
    }

    if(!extension->InitGH)
    {
      fprintf(stderr, 
	      "Warning: GH Extension '%s' has not regidtered an InitGH routine.\n",
	      CCTK_GetHandleName(GHExtensions, handle));
      extension->SetupGH=DummySetupGH;
    }

  }

  return return_code;
}

static void *DummySetupGH(tFleshConfig *config, int convergence_level, cGH *GH)
{
  return NULL;
}

static int DummyInitGH(cGH *GH)
{
  return 0;
}

