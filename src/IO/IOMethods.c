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
#include "Groups.h"
#include "StoreHandledData.h"

static char *rcsid = "$Header$";

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
   @calls      CCTK_GetHandle
               CCTK_NewHandle
   @history 
   @endhistory
 
   @var        name
   @vdesc      The name of the method for IO
   @vtype      const char *
   @vio        in
   @vcomment 
   @endvar 

   @returntype int
   @returndesc
               -2  = memory allocation failed
               -1  = method with this name already registered
               i>0 = handle for method
   @endreturndesc

   @version    $Header$
@@*/

int CCTK_RegisterIOMethod(const char *name)
{

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
   @calls      CCTK_GetHandledData
   @calledby   
   @history 
   @endhistory 

   @var        handle
   @vdesc      identifies the method in the stored data
   @vtype      int
   @vio        in
   @vcomment 
   @endvar 

   @var        func
   @vdesc      name of the routine for implementing OutputGH
   @vtype      (* func)(cGH *)
   @vio        in
   @vcomment 
   @endvar 

   @returntype int
   @returndesc
               1 = function successfully registered
               0 = function did not register
   @endreturndesc

   @version    $Header$

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
   @routine    CCTK_RegisterIOMethodOutputVarAs
   @date       Wed Feb  3 13:34:12 1999
   @author     Tom Goodale
   @desc 
               Registers a function to register a routine for OutputVarAs
   @enddesc 
   @calls      CCTK_GetHandledData
   @calledby   
   @history 
   @endhistory 

   @var        handle
   @vdesc      identifies the method in the stored data
   @vtype      int
   @vio        in
   @vcomment 
   @endvar 

   @var        func
   @vdesc      name of the routine for implementing OutputVarAs
   @vtype      (* func)(cGH *, const char *,const char *)
   @vio        in
   @vcomment 
   @endvar 

   @returntype int
   @returndesc
               1 = function successfully registered
               0 = function did not register
   @endreturndesc

   @version    $Header$

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

int CCTK_RegisterIOMethodTriggerOutput(int handle, int (*func)(cGH *, int))
{
  int return_code;
  struct IOMethod *method;

  /* Get the extension. */
  method = CCTK_GetHandledData(IOMethods, handle);

  if(method)
  {
    method->TriggerOutput = func;
    return_code = 1;
  }
  else
  {
    return_code = 0;
  }

  return return_code;
}

 /*@@
   @routine    CCTK_RegisterIOMethodTimeToOutput
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc 
               Registers a function to register a routine for TimeToOutput
   @enddesc 
   @calls      CCTK_GetHandledData
   @calledby   
   @history 
   @endhistory 

   @var        handle
   @vdesc      identifies the method in the stored data
   @vtype      int
   @vio        in
   @vcomment 
   @endvar 

   @var        func
   @vdesc      name of the routine for implementing TimeToOutput
   @vtype      (* func)(cGH *, int)
   @vio        in
   @vcomment 
   @endvar 

   @returntype int
   @returndesc
               1 = function successfully registered
               0 = function did not register
   @endreturndesc

   @version    $Header$

@@*/

int CCTK_RegisterIOMethodTimeToOutput(int handle, int (*func)(cGH *, int))
{
  int return_code;
  struct IOMethod *method;

  /* Get the extension. */
  method = CCTK_GetHandledData(IOMethods, handle);

  if(method)
  {
    method->TimeToOutput = func;
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
@@*/

static int DummyOutputGH(cGH *GH)
{
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
  return 0;
}

static int DummyTriggerOutput(cGH *GH, int var)
{
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
   @calledby   
   @history 
   @endhistory 

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @vcomment 
   @endvar 

   @returntype int
   @returndesc
              -1    = No IO methods were found
               i>=0 = Last handle used for output (number of IO methods-1)
   @endreturndesc

   @version    $Header$

@@*/

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
   @calls      
   @history 
   @endhistory 

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @vcomment 
   @endvar 

   @var        var
   @vdesc      Name of variable to output
   @vtype      const char *
   @vio        in
   @vcomment 
   @endvar 

   @var        method
   @vdesc      Name of method to use for output
   @vtype      const char *
   @vio        in
   @vcomment 
   @endvar 

   @var        alias
   @vdesc      Name variable should be output as
   @vtype      const char *alias
   @vio        in
   @vcomment   Note that the IO method may not use this feature
   @endvar 

   @returntype int
   @returndesc
   @endreturndesc

   @version    $Header$

@@*/

int CactusDefaultOutputVarAsByMethod(cGH *GH, 
                             const char *var,   
                             const char *methodname, 
                             const char *alias)
{
  printf("In default method CactusDefaultOutputVarAsByMethod\n");
  return 0;
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
   @calls      CCTK_GetHandledData
               IOMethod->OutputVarAs
   @history 
   @endhistory 

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @vcomment 
   @endvar 

   @var        var
   @vdesc      Name of variable to output
   @vtype      const char *
   @vio        in
   @vcomment 
   @endvar 

   @var        alias
   @vdesc      Name variable should be output as
   @vtype      const char *alias
   @vio        in
   @vcomment   Note that the IO method may not use this feature
   @endvar 

   @returntype int
   @returndesc
              -1    = No IO methods were found
               i>=0 = Last handle used for output (number of IO methods-1)
   @endreturndesc

   @version    $Header$

@@*/


int CCTK_OutputVarAs(cGH *GH, const char *var, const char *alias)
{
  int handle;
  struct IOMethod *method;
  
  for (handle = 0;;handle++)
  {
    method = (struct IOMethod *)CCTK_GetHandledData(IOMethods, handle);
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
   @history 
   @endhistory 

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @vcomment 
   @endvar 

   @var        var
   @vdesc      Name of variable to output
   @vtype      const char *
   @vio        in
   @vcomment   This is also the name the variable will be output as
   @endvar 

   @returntype int
   @returndesc
               0    = always returns this
   @endreturndesc

   @version    $Header$

@@*/

int CCTK_OutputVar(cGH *GH, const char *var)
{
  CCTK_OutputVarAs(GH, var, var);

  return 0;
}


 /*@@
   @routine    CactusOutputVarByMethods
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc 
               Outputs a variable using one given method and no alias
   @enddesc 
   @calls      CCTK_OutputVarAsByMethod
   @history 
   @endhistory 

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @vcomment 
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
   @vcomment 
   @endvar 

   @returntype int
   @returndesc
               0    = always returns this
   @endreturndesc

   @version    $Header$

@@*/

int CCTK_OutputVarByMethod(cGH *GH, const char *var, const char *method)
{
  CCTK_OutputVarAsByMethod(GH, var, method, var);

  return 0;
}


/************************************************************************
 *
 *  These routines are here because they need to get at IOMethods
 *
 ************************************************************************/

 /*@@
   @routine    CCTK_rfrTriggerSaysGo
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc 
               Checks if a trigger registered for a routine is
               due for output by any IO method.
   @enddesc 
   @calls      CCTK_GetHandledData
               IOMethod->TimeToOutput
   @history  
   @endhistory 

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @vcomment 
   @endvar 

   @var        variable
   @vdesc      GH variable index
   @vtype      int
   @vio        in
   @vcomment 
   @endvar 

   @returntype int
   @returndesc
               0    = Trigger says don't output me this iteration
	       1    = Trigger says output me this iteration
   @endreturndesc

   @version    $Header$
 */

int CCTK_rfrTriggerSaysGo(cGH *GH, int variable)
{
  int handle;
  int flag;
  struct IOMethod *method;

  /* Loop over all registered IO methods */
  for (handle = 0;;handle++)
  {
    method = (struct IOMethod *)CCTK_GetHandledData(IOMethods, handle);
    if (method)
    {
      /* Check if it is time to output this variable for this method*/
      flag = method->TimeToOutput(GH, variable);
      if (flag) 
	return 1;
    }
    else
    {
      /* There are no more methods left, so the variable will not be output */
      return 0;
    }
  }
  
}


 /*@@
   @routine    CCTK_rfrTriggerAction
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc 
               Essentially the same as CCTK_rfrTriggerSaysGo, 
               but now calls each IO method for which it is 
               time to output the trigger
   @enddesc 
   @calls      CCTK_GetHandledData
               CCTK_GetFullName
	       CCTK_GetVarName
               IOMethod->TimeToOutput
	       IOMethod->OutputVarAs
   @history  
   @endhistory 

   @var        GH
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @vcomment 
   @endvar 

   @var        variable
   @vdesc      GH variable index
   @vtype      int
   @vio        in
   @vcomment 
   @endvar 

   @returntype int
   @returndesc
               0    = This should never happen, since at least
                      one IO method should have been found by
		      CCTK_rfrTriggerSaysGo
	       >0   = Number of IO methods called for output for
                      this variable
   @endreturndesc

   @version    $Header$
 */

int CCTK_rfrTriggerAction(void *GH, int variable)
{
  char *varname;
  char *fullvarname;
  int handle;
  int nmethods;
  struct IOMethod *method;

  fullvarname = CCTK_GetFullName(variable);
  varname = CCTK_GetVarName(variable);

  /* Count the number of methods used */
  nmethods = 0;

  /* Loop over all registered IO methods */
  for (handle = 0;;handle++)
  {
    method = (struct IOMethod *)CCTK_GetHandledData(IOMethods, handle);
    if (method)
    {
      /* Check if it is time to output this variable for this method*/
      if (method->TimeToOutput(GH, variable))
      {
	/* And if so do call the output routine for the method */
	method->TriggerOutput(GH,variable);
	nmethods++;
      }
    }
    else
    {
      /* There are no methods left */

      /* This was assigned by CCTK_GetFullName */
      free(fullvarname);

      return nmethods;
    }
  }
}
