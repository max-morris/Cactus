 /*@@
   @file      GHExtensions.c
   @date      Fri Jan 15 13:22:47 1999
   @author    Tom Goodale
   @desc 
   Functions to deal with GH extensions
   @enddesc 
 @@*/

/*#define DEBUG*/

#include <stdio.h>
#include <stdlib.h>

#include "cctk_Flesh.h"
#include "cctk_FortranString.h"
#include "cctk_GHExtensions.h"
#include "StoreHandledData.h"
#include "cctk_WarnLevel.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_GHExtensions_c)

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
 *  f) Add a prototype for the registration function to cctk_GHExtensions.h
 */
struct GHExtension
{
  void *(*SetupGH)(tFleshConfig *, int, cGH *);
  int    (*InitGH)(cGH *);
  int    (*ScheduleTraverseGH)(cGH *, const char *);
};


/* Function which checks that all the functions on all extensions have been 
 * filled in.
 */
static int CheckAllExtensionsSetup(void);

/* Dummy function prototypes. */

static void *DummySetupGH(tFleshConfig *config, 
                          int convergence_level, 
                          cGH *GH);
static int DummyInitGH(cGH *GH);
static int DummyScheduleTraverseGH(cGH *GH, 
                                   const char *where);


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
  handle = Util_GetHandle(GHExtensions, name, NULL);

  if(handle < 0)
  {
    /* New extension. */
    new_extension = (struct GHExtension *)malloc(sizeof(struct GHExtension));

    if(new_extension)
    {
      /* Get a handle for it. */
      handle = Util_NewHandle(&GHExtensions, name, new_extension);

#ifdef DEBUG
      printf("CCTK_RegisterGHExtension: Extension %s gets handle %d\n",name,handle);
#endif

      /* Initialise the extension structure. */
      new_extension->InitGH = NULL;
      new_extension->SetupGH = NULL;
      new_extension->ScheduleTraverseGH = NULL;
      
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

 /*@@
   @routine    CCTK_UnregisterGHExtension
   @date       Tue May 09 2000
   @author     Thomas Radke
   @desc 
   Unregisters a GH extension.
   @enddesc 
   @calls     
   @calledby   
   @var        name
   @vdesc      The name of the GH extension to unregister
   @vtype      const char *
   @vio        in
   @vcomment
   @endvar
   @returntype int
   @returndesc 0 for success, -1 for failure
   @endreturndesc
   @history 
 
   @endhistory 

@@*/
int CCTK_UnregisterGHExtension(const char *name)
{
  int handle, ret_val = -1;
  void *extension;

  /* Check that the extension exists */
  handle = Util_GetHandle(GHExtensions, name, &extension);

  if(handle >= 0)
  {
    /* Delete extension handle and free its associated structure */
    if (Util_DeleteHandle(GHExtensions, handle) == 0)
    {
      free (extension);
      /* Remember how many extensions there are left */
      num_extensions--;
      ret_val = 0;

#ifdef DEBUG
      printf("CCTK_UnregisterGHExtension: unregistered extension %s with "
             "handle %d\n", name, handle);
#endif

    }
  }

  return ret_val;
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
  extension = Util_GetHandledData(GHExtensions, handle);

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
int CCTK_RegisterGHExtensionInitGH(int handle, 
                                   int (*func)(cGH *))
{
  int return_code;
  struct GHExtension *extension;

  /* Get the extension. */
  extension = Util_GetHandledData(GHExtensions, handle);

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
   @routine    CCTK_RegisterGHExtensionScheduleTraverseGH
   @date       Thu Jan 27 14:37:09 2000
   @author     Tom Goodale
   @desc 
   Registers a GH extension Schedule traversal routine routine.      
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_RegisterGHExtensionScheduleTraverseGH(int handle, 
                                               int (*func)(cGH *, const char *))
{
  int return_code;
  struct GHExtension *extension;

  /* Get the extension. */
  extension = Util_GetHandledData(GHExtensions, handle);

  if(extension)
  {
    extension->ScheduleTraverseGH = func;
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
   @routine    CCTKi_SetupGHExtensions
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
int CCTKi_SetupGHExtensions(tFleshConfig *config, 
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
        extension =  (struct GHExtension *)Util_GetHandledData(GHExtensions, handle);
        GH->extensions[handle] = extension->SetupGH(config, 
                                                    convergence_level, 
                                                    GH);
#ifdef DEBUG
        printf("CCTKi_SetupGHExtensions: Set up extension for handle %d\n",handle);
#endif
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
   @routine    CCTKi_InitGHExtensions
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
int CCTKi_InitGHExtensions(cGH *GH)
{
  int handle;
  struct GHExtension *extension;

  for(handle = 0; handle < num_extensions; handle++)
  {
    extension =  (struct GHExtension *)Util_GetHandledData(GHExtensions, handle);
    extension->InitGH(GH);
  }

  return 0;
}

 /*@@
   @routine    CCTKi_ScheduleTraverseGHExtensions
   @date       Thu Jan 27 14:47:06 2000
   @author     Tom Goodale
   @desc 
   Calls the routines which an extension needs called at a schedule traversal.   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTKi_ScheduleTraverseGHExtensions(cGH *GH, 
                                       const char *where)
{
  int handle;
  struct GHExtension *extension;

  for(handle = 0; handle < num_extensions; handle++)
  {
    extension =  (struct GHExtension *)Util_GetHandledData(GHExtensions, handle);
    extension->ScheduleTraverseGH(GH, where);
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
    extension =  (struct GHExtension *)Util_GetHandledData(GHExtensions, handle);

    /* Check that each function has been registered.
     * Print a warning if not, and then register a dummy function.
     */

    /* SetupGH */
    if(!extension->SetupGH)
    {
      CCTK_VWarn(4,__LINE__,__FILE__,"Cactus",
                 "GH Extension '%s' has not registered a SetupGH routine",
                 Util_GetHandleName(GHExtensions, handle));
      extension->SetupGH=DummySetupGH;
    }

    /*  InitGH */
    if(!extension->InitGH)
    {   
      CCTK_VWarn(4,__LINE__,__FILE__,"Cactus",
                 "GH Extension '%s' has not registered a InitGH routine",
                 Util_GetHandleName(GHExtensions, handle));
      extension->InitGH=DummyInitGH;
    }

    /* ScheduleTraverse */
    if(!extension->ScheduleTraverseGH)
    {
      CCTK_VWarn(4,__LINE__,__FILE__,"Cactus",
                 "GH Extension '%s' has not registered a ScheduleTraverse routine",
                 Util_GetHandleName(GHExtensions, handle));
      extension->ScheduleTraverseGH=DummyScheduleTraverseGH;
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
   @routine    CCTK_GHExtensionHandle
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
int CCTK_GHExtensionHandle(const char *name)
{
  return Util_GetHandle(GHExtensions, name, NULL);
}

void  CCTK_FCALL CCTK_FNAME(CCTK_GHExtensionHandle)
     (int *handle,ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name)
  *handle = CCTK_GHExtensionHandle(name);
  free(name);
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
static void *DummySetupGH(tFleshConfig *config, 
                          int convergence_level, 
                          cGH *GH)
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
   @routine    DummyScheduleTraverseGH
   @date       Thu Jan 27 14:34:41 2000
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int DummyScheduleTraverseGH(cGH *GH, 
                                   const char *where)
{
  return 0;
}
