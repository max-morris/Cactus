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

/* Dummy function prototypes. */

static int DummyOutputGH(cGH *GH);
static int DummyOutputVarAs(cGH *GH, 
                            const char *var, 
                            const char *alias);
static int DummyTimeToOutput(cGH *GH, int);


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
      new_method->TimeToOutput = DummyTimeToOutput;

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

 /*@@
   @routine    CCTK_RegisterIOMethodTimeToOutput
   @date       Sat 6 March 1999
   @author     Gabrielle Allen
   @desc 
   Registers a IO method TimeToOutput function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

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
   @routine    DummyTimeToOutput
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc 
   Dummy for TimeToOutput function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

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
                             const char *methodname, 
                             const char *alias)
{
  printf("In default method CactusDefaultOutputVarAsByMethod\n");
  return 0;
}


int CCTK_OutputVarAs(cGH *GH, const char *var, const char *alias)
{
  int handle;
  char *method;
  
  for (handle = 0;;handle++)
  {
    method = CCTK_GetHandleName(IOMethods, handle);
    printf("Method is %s\n",method);
    CCTK_OutputVarAsByMethod(GH, var, method, alias);
  }
  
  return 0;
}


int CCTK_OutputVar(cGH *GH, const char *var)
{
  CCTK_OutputVarAs(GH, var, var);

  return 0;
}


int CCTK_OutputVarByMethod(cGH *GH, const char *var, const char *method)
{
  CCTK_OutputVarAsByMethod(GH, var, method, var);

  return 0;
}


 /*@@
   @routine    CCTK_rfrTriggerSaysGo
   @date       Sat March 6 1999
   @author     Gabrielle Allen
   @desc 
   Checks if a triggers registered for a routine is
   due for output by any IO method.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     variable
   @vdesc   GH variable index
   @vtype   int
   @vio     in
   @vcomment 
   @endvar 
 */

int CCTK_rfrTriggerSaysGo(cGH *GH, int variable)
{
  int handle;
  int flag;
  struct IOMethod *method;

  for (handle = 0;;handle++)
  {
    method = (struct IOMethod *)CCTK_GetHandledData(IOMethods, handle);
    if (method)
    {
      flag = method->TimeToOutput(GH, variable);
      if (flag) 
	return 1;
    }
    else
    {
      return 0;
    }
  }
  
}




int CCTK_rfrTriggerAction(void *GH, int variable)
{
  char *varname;
  char *fullvarname;
  int handle;
  int flag;
  struct IOMethod *method;

  fullvarname = CCTK_GetFullName(variable);
  varname = CCTK_GetVarName(variable);

  for (handle = 0;;handle++)
  {
    method = (struct IOMethod *)CCTK_GetHandledData(IOMethods, handle);
    if (method)
    {
      flag = method->TimeToOutput(GH, variable);
      if (flag) 
      {
	method->OutputVarAs(GH,fullvarname,varname);
      }
    }
    else
    {
      free(fullvarname);
      return 0;
    }
  }
}
