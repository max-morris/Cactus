 /*@@
   @file      IOMethods.c
   @date      Mon Jan 8 1999
   @author    Gabrielle Allen
   @desc 
   Functions to deal with IO registration
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>

#include "flesh.h"
#include "IOMethods.h"
#include "StoreHandledData.h"

static char *rcsid = "$Header$";


/* Local data holding info on IO methods.*/

static cHandledData *IOMethods = NULL;
static int num_methods = 0;

/* Function which checks that all the functions on all extensions have been 
 * filled in.
 */
static int CheckAllMethodsSetup(void);

/* Dummy function prototypes. */

static int DummyOutputGH(cGH *GH);
static int DummyOutputVarAs(cGH *GH, 
                            const char *var, 
                            const char *alias);


/**************************************************************************
 **************************************************************************
 **************************************************************************/


 /*@@
   @routine    CCTK_RegisterIOMethod
   @date       Wed Feb  3 13:33:09 1999
   @author     Tom Goodale
   @desc 
   Registers a new IO method.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_RegisterIOMethod(const char *name)
{
  int return_val;

  int handle;

  struct IOMethod *new_method;

  /* Check that the method hasn't already been registered */
  handle = CCTK_GetHandle(IOMethods, name, NULL);

  if(handle < 0)
  {
    /* New extension. */
    new_method = (struct IOMethod *)malloc(sizeof(struct IOMethod));

    if(new_method)
    {
      /* Get a handle for it. */
      handle = CCTK_NewHandle(&IOMethods, name, new_method);

      /* Initialise the IO method structure. */
      new_method->OutputGH = DummyOutputGH;
      new_method->OutputVarAs = DummyOutputVarAs;
      
      /* Remember how many methods there are */
      num_methods++;
    }
    else
    {
      /* Memory failure. */
      handle = -2;
    }
  }
  else
  {
    /* Method already exists. */
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
   @routine    CCTK_RegisterIOMethodOutputGH
   @date       Wed Feb  3 13:34:12 1999
   @author     Tom Goodale
   @desc 
   Registers a function to register a routine for OutputGH.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_RegisterIOMethodOutputGH(int handle, int (*func)(cGH *))
{
  int return_code;
  struct IOMethod *method;

  /* Get the method. */
  method = CCTK_GetHandledData(IOMethods, handle);

  if(method)
  {
    method->OutputGH = func;
    return_code = 1;
  }
  else
  {
    return_code = 0;
  }

  return return_code;
  }

 /*@@
   @routine    CCTK_RegisterIOMethodOutputVar
   @date       Wed Feb  3 13:33:36 1999
   @author     Tom Goodale
   @desc 
   Registers a IO method OutputVarAs function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_RegisterIOMethodOutputVarAs(int handle, int (*func)(cGH *, 
                                    const char *,const char *))
{
  int return_code;
  struct IOMethod *method;

  /* Get the extension. */
  method = CCTK_GetHandledData(IOMethods, handle);

  if(method)
  {
    method->OutputVarAs = func;
    return_code = 1;
  }
  else
  {
    return_code = 0;
  }

  return return_code;
}


/************************************************************************
 *
 *  Dummy functions.  Registered if no real function registered.
 *
 ************************************************************************/


 /*@@
   @routine    DummyOutputGH
   @date       Wed Feb  3 13:36:52 1999
   @author     Tom Goodale
   @desc 
   Dummy for OutputGH functions.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int DummyOutputGH(cGH *GH)
{
  return 0;
}


 /*@@
   @routine    DummyOutputVar
   @date       Wed Feb  3 13:37:31 1999
   @author     Tom Goodale
   @desc 
   Dummy for OutputVarAs functions.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int DummyOutputVarAs(cGH *GH, 
                            const char *var, 
                            const char *alias)
{
  return 0;
}




int CactusDefaultOutputGH(cGH *GH)
{
  int handle;
  struct IOMethod *method;

  for (handle = 0;;handle++)
  {
    method = (struct IOMethod *)CCTK_GetHandledData(IOMethods, handle);
    if (method)
    {
      method->OutputGH(GH);
    }
    else
    {
      return handle;
    }
  }
}

int CactusDefaultOutputVarAsByMethod(cGH *GH, 
                             const char *var,   
                             const char *method, 
                             const char *alias)
{
  printf("I'm in the default OutputVarAsByMethod routine\n");
  
  return 0;
}
