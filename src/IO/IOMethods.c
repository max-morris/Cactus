 /*@@
   @file      IOMethods.c
   @date      Mon Jan 8 1999
   @author    Gabrielle Allen
   @desc
              Functions to deal with IO registration
   @enddesc
   @version   $Id$
 @@*/

/* #define DEBUG_IO 1 */

#include <stdio.h>
#include <stdlib.h>

#include "cGH.h"
#include "cctk_Flesh.h"
#include "cctk_IOMethods.h"
#include "cctk_Groups.h"
#include "StoreHandledData.h"
#include "cctk_FortranString.h"
#include "cctk_IO.h"

static const char *rcsid = "$Header$";

CCTK_FILEVERSION (IO_IOMethods_c)

/* Local data holding info on IO methods.*/
static cHandledData *IOMethods = NULL;
static int num_methods = 0;

/* Dummy registerable function prototypes. */
static int DummyOutputGH(cGH *GH);
static int DummyOutputVarAs(cGH *GH,
                            const char *var,
                            const char *alias);
static int DummyTriggerOutput(cGH *GH, int var);
static int DummyTimeToOutput(cGH *GH, int var);

int CactusDefaultOutputGH (cGH *GH);
int CactusDefaultOutputVarAsByMethod (cGH *GH,
                                      const char *var,
                                      const char *methodname,
                                      const char *alias);
void CCTK_FCALL CCTK_FNAME (CCTK_OutputVarAsByMethod)
                           (int *ierr, cGH *GH, THREE_FORTSTRINGS_ARGS);
void CCTK_FCALL CCTK_FNAME (CCTK_OutputVarByMethod)
                           (int *ierr, cGH *GH, TWO_FORTSTRINGS_ARGS);
int CCTK_OutputVarAs (cGH *GH, const char *var, const char *alias);
int CCTK_OutputVar (cGH *GH, const char *var);
int CCTK_OutputVarByMethod (cGH *GH, const char *var, const char *method);
int CCTKi_TriggerSaysGo (cGH *GH, int variable);
int CCTKi_TriggerAction (void *GH, int variable);


/************************************************************************
 *
 *  Registration routines functions provided by IO methods
 *
 ************************************************************************/

 /*@@
   @routine    CCTK_RegisterIOMethod
   @date       Wed Feb  3 13:33:09 1999
   @author     Tom Goodale
   @desc
               Registers a new IO method.
   @enddesc
   @calls      Util_GetHandle
               Util_NewHandle

   @var        name
   @vdesc      The name of the method for IO
   @vtype      const char *
   @vio        in
   @endvar

   @returntype int
   @returndesc -2  = memory allocation failed
               -1  = method with this name already registered
               i>0 = handle for method
   @endreturndesc
@@*/
int CCTK_RegisterIOMethod(const char *name)
{
  int handle;
  struct IOMethod *new_method;


  /* Check that the method hasn't already been registered */
  handle = Util_GetHandle(IOMethods, name, NULL);

  if(handle < 0)
  {
    /* New extension. */
    new_method = (struct IOMethod *)malloc(sizeof(struct IOMethod));

    if(new_method)
    {
      /* Get a handle for it. */
      handle = Util_NewHandle(&IOMethods, name, new_method);

      /* Initialise the IO method structure with dummy routines */
      new_method->OutputGH      = DummyOutputGH;
      new_method->OutputVarAs   = DummyOutputVarAs;
      new_method->TriggerOutput = DummyTriggerOutput;
      new_method->TimeToOutput  = DummyTimeToOutput;

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


 /*@@
   @routine    CCTK_RegisterIOMethodOutputGH
   @date       Wed Feb  3 13:34:12 1999
   @author     Tom Goodale
   @desc
               Registers a function to register a routine for OutputGH.
   @enddesc
   @calls      Util_GetHandledData

   @var        handle
   @vdesc      identifies the method in the stored data
   @vtype      int
   @vio        in
   @endvar
   @var        func
   @vdesc      name of the routine for implementing OutputGH
   @vtype      (* func)(cGH *)
   @vio        in
   @endvar

   @returntype int
   @returndesc 1 = function successfully registered
               0 = function did not register
   @endreturndesc
@@*/
int CCTK_RegisterIOMethodOutputGH(int handle, int (*func)(cGH *))
{
  struct IOMethod *method;


  /* Get the method. */
  method = Util_GetHandledData(IOMethods, handle);
  if(method)
  {
    method->OutputGH = func;
  }

  return (method != NULL);
}


 /*@@
   @routine    CCTK_RegisterIOMethodOutputVarAs
   @date       Wed Feb  3 13:34:12 1999
   @author     Tom Goodale
   @desc
               Registers a function to register a routine for OutputVarAs
   @enddesc
   @calls      Util_GetHandledData

   @var        handle
   @vdesc      identifies the method in the stored data
   @vtype      int
   @vio        in
   @endvar
   @var        func
   @vdesc      name of the routine for implementing OutputVarAs
   @vtype      (* func)(cGH *, const char *,const char *)
   @vio        in
   @endvar

   @returntype int
   @returndesc 1 = function successfully registered
               0 = function did not register
   @endreturndesc
@@*/
int CCTK_RegisterIOMethodOutputVarAs(int handle, int (*func)(cGH *,
                                    const char *,const char *))
{
  struct IOMethod *method;


  /* Get the extension. */
  method = Util_GetHandledData(IOMethods, handle);
  if (method)
  {
    method->OutputVarAs = func;
  }

  return (method != NULL);
}


int CCTK_RegisterIOMethodTriggerOutput(int handle, int (*func)(cGH *, int))
{
  struct IOMethod *method;


  /* Get the extension. */
  method = Util_GetHandledData(IOMethods, handle);
  if (method)
  {
    method->TriggerOutput = func;
  }

  return (method != NULL);
}


 /*@@
   @routine    CCTK_RegisterIOMethodTimeToOutput
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc
               Registers a function to register a routine for TimeToOutput
   @enddesc
   @calls      Util_GetHandledData

   @var        handle
   @vdesc      identifies the method in the stored data
   @vtype      int
   @vio        in
   @endvar
   @var        func
   @vdesc      name of the routine for implementing TimeToOutput
   @vtype      (* func)(cGH *, int)
   @vio        in
   @endvar

   @returntype int
   @returndesc 1 = function successfully registered
               0 = function did not register
   @endreturndesc
@@*/
int CCTK_RegisterIOMethodTimeToOutput(int handle, int (*func)(cGH *, int))
{
  struct IOMethod *method;


  /* Get the extension. */
  method = Util_GetHandledData (IOMethods, handle);
  if (method)
  {
    method->TimeToOutput = func;
  }

  return (method != NULL);
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
@@*/
static int DummyOutputGH(cGH *GH)
{
  GH = GH;
  return 0;
}


 /*@@
   @routine    DummyTimeToOutput
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc
               Dummy for TimeToOutput function.
   @enddesc
@@*/
static int DummyTimeToOutput(cGH *GH, int var)
{
  GH = GH;
  var = var;
  return 0;
}


 /*@@
   @routine    DummyOutputVarAs
   @date       Wed Feb  3 13:37:31 1999
   @author     Tom Goodale
   @desc
               Dummy for OutputVarAs functions.
   @enddesc
@@*/
static int DummyOutputVarAs(cGH *GH,
                            const char *var,
                            const char *alias)
{
  GH = GH;
  var = var;
  alias = alias;
  return 0;
}


static int DummyTriggerOutput(cGH *GH, int var)
{
  GH = GH;
  var = var;
  return 0;
}


/************************************************************************
 *
 *  Default functions for overloadable functions provided by IO methods
 *
 ************************************************************************/

 /*@@
   @routine    CactusDefaultOutputGH
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc
               Loops over all methods calling each methods OutputGH
               routine
   @enddesc
   @calls      IOMethod->OutputGH

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @endvar

   @returntype int
   @returndesc -1    = No IO methods were found
               i>=0 = Last handle used for output (number of IO methods-1)
   @endreturndesc
@@*/
int CactusDefaultOutputGH (cGH *GH)
{
  int handle;
  struct IOMethod *method;


  for (handle = 0;;handle++)
  {
    method = (struct IOMethod *)Util_GetHandledData(IOMethods, handle);
    if (method)
    {

#ifdef DEBUG_IO
    printf("Calling IO method with handle %d \n",handle);
#endif

      method->OutputGH(GH);
    }
    else
    {
      return handle--;
    }
  }
}


 /*@@
   @routine    CactusDefaultOutputVarAsByMethod
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc
               Outputs one chosen variable by one chosen method
   @enddesc

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @endvar
   @var        var
   @vdesc      Name of variable to output
   @vtype      const char *
   @vio        in
   @endvar
   @var        method
   @vdesc      Name of method to use for output
   @vtype      const char *
   @vio        in
   @endvar
   @var        alias
   @vdesc      Name variable should be output as
   @vtype      const char *alias
   @vio        in
   @vcomment   Note that the IO method may not use this feature
   @endvar

   @returntype int
   @returndesc -1    = No such IO method found
               0    = success
   @endreturndesc
@@*/
int CactusDefaultOutputVarAsByMethod (cGH *GH,
                                      const char *var,
                                      const char *methodname,
                                      const char *alias)
{
  struct IOMethod *method = NULL;


  Util_GetHandle (IOMethods, methodname, (void **) &method);
  if (! method)
  {
    return -1;
  }

  method->OutputVarAs(GH, var, alias);

  return 0;
}


void CCTK_FCALL CCTK_FNAME (CCTK_OutputVarAsByMethod)
                           (int *ierr, cGH *GH, THREE_FORTSTRINGS_ARGS)
{
  THREE_FORTSTRINGS_CREATE(var, methodname, alias);

  *ierr = CCTK_OutputVarAsByMethod (GH, var, methodname, alias);

  free (var);
  free (methodname);
  free (alias);
}


/************************************************************************
 *
 *  More IO functions which perhaps should be overloadable
 *
 ************************************************************************/


 /*@@
   @routine    CactusOutputVarAs
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc
               Loops over all methods for a given variable,
               calling each methods OutputVarAs routine
   @enddesc
   @calls      Util_GetHandledData
               IOMethod->OutputVarAs

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @endvar
   @var        var
   @vdesc      Name of variable to output
   @vtype      const char *
   @vio        in
   @endvar
   @var        alias
   @vdesc      Name variable should be output as
   @vtype      const char *alias
   @vio        in
   @vcomment   Note that the IO method may not use this feature
   @endvar

   @returntype int
   @returndesc -1    = No IO methods were found
               i>=0 = Last handle used for output (number of IO methods-1)
   @endreturndesc
@@*/
int CCTK_OutputVarAs (cGH *GH, const char *var, const char *alias)
{
  int handle;
  struct IOMethod *method;


  for (handle = 0;;handle++)
  {
    method = (struct IOMethod *)Util_GetHandledData(IOMethods, handle);
    if (method)
    {
      method->OutputVarAs(GH, var, alias);
    }
    else
    {
      return handle--;
    }
  }
}


 /*@@
   @routine    CactusOutputVar
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc
               Outputs a variable using all methods and no alias
   @enddesc
   @calls      CCTK_OutputVarAs

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @endvar
   @var        var
   @vdesc      Name of variable to output
   @vtype      const char *
   @vio        in
   @endvar

   @returntype int
   @returndesc 0    = always returns this
   @endreturndesc
@@*/
int CCTK_OutputVar (cGH *GH, const char *var)
{
  CCTK_OutputVarAs(GH, var, var);

  return 0;
}


 /*@@
   @routine    CactusOutputVarByMethod
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc
               Outputs a variable using one given method and no alias
   @enddesc
   @calls      CCTK_OutputVarAsByMethod

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @endvar
   @var        var
   @vdesc      Name of variable to output
   @vtype      const char *
   @vio        in
   @vcomment   This is also the name the variable will be output as
   @endvar
   @var        method
   @vdesc      Name of method to use for output
   @vtype      const char *
   @vio        in
   @endvar

   @returntype int
   @returndesc 0    = always returns this
   @endreturndesc
@@*/
int CCTK_OutputVarByMethod (cGH *GH, const char *var, const char *method)
{
  CCTK_OutputVarAsByMethod(GH, var, method, var);

  return 0;
}

void CCTK_FCALL CCTK_FNAME (CCTK_OutputVarByMethod)
                           (int *ierr, cGH *GH, TWO_FORTSTRINGS_ARGS)
{
  TWO_FORTSTRINGS_CREATE(var,method);
  *ierr = CCTK_OutputVarByMethod(GH, var, method);
  free(var);
  free(method);
}


/************************************************************************
 *
 *  These routines are here because they need to get at IOMethods
 *
 ************************************************************************/

 /*@@
   @routine    CCTKi_TriggerSaysGo
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc
               Checks if a trigger registered for a routine is
               due for output by any IO method.
   @enddesc
   @calls      Util_GetHandledData
               IOMethod->TimeToOutput

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @endvar
   @var        variable
   @vdesc      GH variable index
   @vtype      int
   @vio        in
   @endvar

   @returntype int
   @returndesc 0    = Trigger says don't output me this iteration
               1    = Trigger says output me this iteration
   @endreturndesc
@@*/
int CCTKi_TriggerSaysGo (cGH *GH, int variable)
{
  int handle;
  struct IOMethod *method;


  /* Loop over all registered IO methods */
  for (handle = 0; handle < num_methods; handle++)
  {
    /* Check if it is time to output this variable for this method */
    method = (struct IOMethod *) Util_GetHandledData (IOMethods, handle);
    if (method && method->TimeToOutput (GH, variable))
    {
      return (1);
    }
  }
  return (0);
}


 /*@@
   @routine    CCTKi_TriggerAction
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc
               Essentially the same as CCTKi_TriggerSaysGo,
               but now calls each IO method for which it is
               time to output the trigger
   @enddesc
   @calls      Util_GetHandledData
               CCTK_FullName
               CCTK_VarName
               IOMethod->TimeToOutput
               IOMethod->OutputVarAs

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @endvar
   @var        variable
   @vdesc      GH variable index
   @vtype      int
   @vio        in
   @endvar

   @returntype int
   @returndesc 0    = This should never happen, since at least
                      one IO method should have been found by
                      CCTKi_TriggerSaysGo
               >0   = Number of IO methods called for output for
                      this variable
   @endreturndesc
@@*/
int CCTKi_TriggerAction (void *GH, int variable)
{
  int handle;
  int nmethods;
  struct IOMethod *method;


  /* Count the number of methods used */
  nmethods = 0;

  /* Loop over all registered IO methods */
  for (handle = 0; handle < num_methods; handle++)
  {
    /* Check if it is time to output this variable for this method */
    method = (struct IOMethod *) Util_GetHandledData (IOMethods, handle);
    if (method && method->TimeToOutput (GH, variable))
    {
      /* And if so do call the output routine for the method */
      method->TriggerOutput (GH, variable);
      nmethods++;
    }
  }

  return (nmethods);
}
