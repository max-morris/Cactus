 /*@@
   @file      Functions.c
   @date      Sat Dec  4 13:39:12 1999
   @author    Tom Goodale
   @desc 
   File to hold all CCTK overloadable or registerable functions.  
   In principle all should be registered through here, but probably 
   need a bit more functionality first.
   @enddesc 
   @version $Header$
 @@*/

static char *rcsid = "$Header$";

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "cctk.h"

#include "CactusTimers.h"
#include "StoreHandledData.h"

#include "cctk_Functions.h"
#include "cctki_Functions.h"

static cHandledData *functions;


struct iFunctionData
{
  int timer;

  cFunctionType ftype;

  void (*function)();
};


 /*@@
   @routine    CCTK_FunctionRegister
   @date       Sat Dec  4 15:05:42 1999
   @author     Tom Goodale
   @desc 
   Registers a function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

   @var     name
   @vdesc   Function name
   @vtype   const char *
   @vio     in
   @vcomment 
   The name of the function being registered.
   @endvar 
   @var     function
   @vdesc   Function being registered
   @vtype   void (*)(void)
   @vio     in
   @vcomment 
   Pointer to the function being registered.
   Doesn't matter what the real type is as long as it returns void.
   @endvar 
   @var     ftype
   @vdesc   The type of the function.
   @vtype   cFunctionType *
   @vio     in
   @vcomment 
   This is a pointer to a structure which should be 
   filled out with 
       the number of arguments 
       the function takes the return type of the function
   @endvar 

   @returntype void *
   @returndesc
     NULL on failure
     pointer to data about function on success
   @endreturndesc

@@*/
void *CCTK_FunctionRegister(const char *name, 
                            void (*function)(void), 
                            cFunctionType *ftype)
{
  int handle;
  struct iFunctionData *fdata;

  fdata = NULL;

  if(ftype->n_args <= CCTK_MAX_ARGS)
  {
    fdata = (struct iFunctionData *)malloc(sizeof(struct iFunctionData));

    if(fdata)
    {
      fdata->function = function;
      fdata->ftype.n_args   = ftype->n_args;
      fdata->ftype.type     = ftype->type;
      fdata->timer    = CCTK_TimerCreateI();

      handle = Util_NewHandle(&functions, name, fdata);
    }
    else
    {
      handle = -1;
    }
  }
  else
  {
    handle = -2;
  }

  return (void *)fdata;
}

 /*@@
   @routine    CCTK_FunctionCall
   @date       Sat Dec  4 15:10:36 1999
   @author     Tom Goodale
   @desc 
   Calls a function with the appropriate number of arguments.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

   @var     fpointer
   @vdesc   The pointer to the function
   @vtype   void *
   @vio     in
   @vcomment 
   This is the data returned by CCTK_FunctionRegister
   @endvar 
   @var     ...
   @vdesc   Variable argument list
   @vtype   any number of void *s
   @vio     inout
   @vcomment 
   This is 0 or more void *s which will be passed unchanged to the
   function.  The number here should match the number given to
   CCTK_FunctionRegister
   @endvar 

@@*/
int CCTK_FunctionCall(void *data, ...)
{
  int retval;
  int i;
  va_list ap;
  struct iFunctionData *fdata;
  char  *retchar;
  CCTK_REAL *retreal;
  CCTK_INT  *retint;
  
  void *array[CCTK_MAX_ARGS];
  
  fdata = (struct iFunctionData *)data;

  if(fdata)
  {
    va_start(ap, data);
      
    if(fdata->ftype.type != CCTK_VARIABLE_VOID)
    {
      switch(fdata->ftype.type)
      {
        case CCTK_VARIABLE_REAL : retreal = va_arg(ap, CCTK_REAL *); break;
        case CCTK_VARIABLE_INT  : retint  = va_arg(ap, CCTK_INT *); break;
        default :
          fprintf(stderr, "Unsupported return type for function at line %d of %s", 
                  __LINE__, __FILE__);
      }
    }

    if(fdata->ftype.n_args > 0)
    {
      for(i = 0; i < fdata->ftype.n_args; i++)
      {
        array[i] = va_arg(ap, void *);
      }
    }

    va_end(ap);

    CCTK_TimerStartI(fdata->timer);

    switch(fdata->ftype.type)
    {
      case CCTK_VARIABLE_VOID : CCTK_CALLVOIDFUNC(fdata->ftype.n_args, array, fdata->function); break;
      case CCTK_VARIABLE_REAL : CCTK_CALLRETFUNC(*retreal, CCTK_REAL, fdata->ftype.n_args, array, fdata->function); break;
      case CCTK_VARIABLE_INT  : CCTK_CALLRETFUNC(*retint, CCTK_INT, fdata->ftype.n_args, array, fdata->function); break;
      default :
        fprintf(stderr, "Unsupported return type for function at line %d of %s", 
                __LINE__, __FILE__);
    }

    CCTK_TimerStopI(fdata->timer);

    retval = 0;
  }
  else
  {
    retval = -1;
  }

  return retval;
}
