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
#include "WarnLevel.h"

static char *rcsid = "$Id$";


/* Local data holding info on extensions..*/

static cHandledData *GHExtensions = NULL;
static int num_extensions = 0;

/* The GH extension structure.
 * This contains pointers to functions which the extension requires.
 *
 * To add a new function, you must:
 *
 *  a) Add it to this structure
 *  b) Initialise it to NULL in CCTK_RegisterGHExtension
 *  c) Check its value in CheckAllExtensionsSetup
 *  d) Provide a dummy function for CheckAllExtensionsSetup to use
 *  e) Provide a registration function.
 *  f) Add a prototype for the registration function to GHExtensions.h
 */
struct GHExtension
{
  void *(*SetupGH)(tFleshConfig *, int, cGH *);
  int    (*InitGH)(cGH *);
  int    (*rfrTraverseGH)(cGH *, int);
};


/* Function which checks that all the functions on all extensions have been 
 * filled in.
 */
static int CheckAllExtensionsSetup(void);

/* Dummy function prototypes. */

static void *DummySetupGH(tFleshConfig *config, int convergence_level, cGH *GH);
static int DummyInitGH(cGH *GH);
static int DummyrfrTraverseGH(cGH *GH, int rfrpoint);


/**************************************************************************
 **************************************************************************
 **************************************************************************/


 /*@@
   @routine    CCTK_RegisterGHExtension
   @date       Wed Feb  3 13:33:09 1999
   @author     Tom Goodale
   @desc 
   Registers a new GH extension.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_RegisterGHExtension(const char *name)
{

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
      new_extension->rfrTraverseGH = NULL;
      
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

/***************************************************************************
 *
 *          Function Registration Routines.
 *
 ***************************************************************************/

 /*@@
   @routine    CCTK_RegisterGHExtensionSetupGH
   @date       Wed Feb  3 13:34:12 1999
   @author     Tom Goodale
   @desc 
   Registers a function to setup a GH extension.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_RegisterGHExtensionSetupGH(int handle, 
				    void *(*func)(tFleshConfig *, int, cGH *))
{
  int return_code;
  struct GHExtension *extension;

  /* Get the extension. */
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

 /*@@
   @routine    CCTK_RegisterGHExtensionInitGH
   @date       Wed Feb  3 13:33:36 1999
   @author     Tom Goodale
   @desc 
   Registers a GH extension initialisation routine.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_RegisterGHExtensionInitGH(int handle, int (*func)(cGH *))
{
  int return_code;
  struct GHExtension *extension;

  /* Get the extension. */
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

 /*@@
   @routine    CCTK_RegisterGHExtensionrfrTraverseGH
   @date       Wed Feb  3 14:31:20 1999
   @author     Tom Goodale
   @desc 
   Registers a GH extension rfr traversal routine routine.   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_RegisterGHExtensionrfrTraverseGH(int handle, int (*func)(cGH *, int))
{
  int return_code;
  struct GHExtension *extension;

  /* Get the extension. */
  extension = CCTK_GetHandledData(GHExtensions, handle);

  if(extension)
  {
    extension->rfrTraverseGH = func;
    return_code = 1;
  }
  else
  {
    return_code = 0;
  }

  return return_code;
}


/***************************************************************************
 *
 *          Function Calling Routines.
 *
 ***************************************************************************/

 /*@@
   @routine    CCTK_SetupGHExtensions
   @date       Wed Feb  3 13:32:26 1999
   @author     Tom Goodale
   @desc 
   Sets up all registered GH extensions on a GH.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_SetupGHExtensions(tFleshConfig *config, 
			   int convergence_level, 
			   cGH *GH)
{
  int return_code;
  int handle;
  struct GHExtension *extension;

  if(CheckAllExtensionsSetup())
  {
    /* Create GHExtension array on the GH. */
    GH->extensions = (void **)malloc(num_extensions*sizeof(void *));

    if(GH->extensions)
    {
      for(handle = 0; handle < num_extensions; handle++)
      {
	/* Call the SetupGH routines for each extension. */
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





 /*@@
   @routine    CCTK_InitGHExtensions
   @date       Wed Feb  3 14:12:18 1999
   @author     Tom Goodale
   @desc 
   Calls the initialisation routine for a GH extension.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_InitGHExtensions(cGH *GH)
{
  int handle;
  struct GHExtension *extension;

  for(handle = 0; handle < num_extensions; handle++)
  {
    extension =  (struct GHExtension *)CCTK_GetHandledData(GHExtensions, handle);
    extension->InitGH(GH);
  }

  return 0;
}


 /*@@
   @routine    CCTK_rfrTraverseGHExtensions
   @date       Wed Feb  3 14:16:17 1999
   @author     Tom Goodale
   @desc 
   Calls the routines which an extension needs called at an rfr traversal.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_rfrTraverseGHExtensions(cGH *GH, int rfrpoint)
{
  int handle;
  struct GHExtension *extension;

  for(handle = 0; handle < num_extensions; handle++)
  {
    extension =  (struct GHExtension *)CCTK_GetHandledData(GHExtensions, handle);
    extension->rfrTraverseGH(GH, rfrpoint);
  }

  return 0;
}


/***************************************************************************
 *
 *                 Checking routines
 *
 ***************************************************************************/


 /*@@
   @routine    CheckAllExtensionsSetup
   @date       Wed Feb  3 13:34:58 1999
   @author     Tom Goodale
   @desc 
   Checks the state of all extensions.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int CheckAllExtensionsSetup(void)
{
  int return_code;
  int handle;
  struct GHExtension *extension;

  return_code = 1;

  /* Check all registered extensions. */
  for(handle = 0; handle < num_extensions; handle++)
  {
    extension =  (struct GHExtension *)CCTK_GetHandledData(GHExtensions, handle);

    /* Check that each function has been registered.
     * Print a warning if not, and then register a dummy function.
     */

    /* SetupGH */
    if(!extension->SetupGH)
    {
      const char *handlename = CCTK_GetHandleName(GHExtensions, handle);
      char *message = (char *)malloc(300*sizeof(char));
      sprintf(message,"GH Extension '%s' has not registered a SetupGH routine",handlename);
      CCTK_Warn(4,__LINE__,CCTK_THORNSTRING,message,__FILE__) ;
      free(message);
      extension->SetupGH=DummySetupGH;
    }

    /*  InitGH */
    if(!extension->InitGH)
    {   
      const char *handlename = CCTK_GetHandleName(GHExtensions, handle);
      char *message = (char *)malloc(300*sizeof(char));
      sprintf(message,"GH Extension '%s' has not registered a InitGH routine",handlename);
      CCTK_Warn(4,__LINE__,CCTK_THORNSTRING,message,__FILE__) ;
      free(message);
      extension->InitGH=DummyInitGH;
    }

    /* rfrTraverse */
    if(!extension->rfrTraverseGH)
    {
      const char *handlename = CCTK_GetHandleName(GHExtensions, handle);
      char *message = (char *)malloc(300*sizeof(char));
      sprintf(message,"GH Extension '%s' has not registered a rfrTraverse routine",handlename);
      CCTK_Warn(4,__LINE__,CCTK_THORNSTRING,message,__FILE__) ;
      free(message);
      extension->rfrTraverseGH=DummyrfrTraverseGH;
    }

  }

  return return_code;
}

/************************************************************************
 *
 *                      Query functions.
 *
 ************************************************************************/

 /*@@
   @routine    CCTK_GetGHExtensionHandle
   @date       Tue Feb  9 18:23:41 1999
   @author     Tom Goodale
   @desc 
   Gets the handle to the GH extension.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_GetGHExtensionHandle(const char *name)
{
  return CCTK_GetHandle(GHExtensions, name, NULL);
}


/************************************************************************
 *
 *  Dummy functions.  Registered if no real function registered.
 *
 ************************************************************************/


 /*@@
   @routine    DummySetupGH
   @date       Wed Feb  3 13:36:52 1999
   @author     Tom Goodale
   @desc 
   Dummy for SetupGH functions.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static void *DummySetupGH(tFleshConfig *config, int convergence_level, cGH *GH)
{
  return NULL;
}


 /*@@
   @routine    DummyInitGH
   @date       Wed Feb  3 13:37:31 1999
   @author     Tom Goodale
   @desc 
   Dummy for InitGH functions.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int DummyInitGH(cGH *GH)
{
  return 0;
}


 /*@@
   @routine    DummyrfrTraverseGH
   @date       Wed Feb  3 14:17:57 1999
   @author     Tom Goodale
   @desc 
   Dummy for rfrTraverseGH functions.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int DummyrfrTraverseGH(cGH *GH, int rfrpoint)
{
  return 0;
}

