 /*@@
   @file      RegisterCommFunctions.c
   @date      Tue Sep 29 10:38:41 1998
   @author    Tom Goodale
   @desc 
   Routines to register the main routines.
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>

#include "flesh.h"
#include "RegisterKeyedFunction.h"
#include "CactusCommDefaults.h"

static char *rcsid = "$Id$";


/* Definitions of how many functions */
#define CACTUS_COMM_MIN 0
#define CACTUS_COMM_MAX 7

/* The functions. */

cGH * (*SetupGH)(tFleshConfig *, int);
int (*SetupGF)(cGH *, cGF *);

int (*SyncAllFuncs)(cGH *);
int (*SyncGroupFuncs)(cGH *, const char *group);
int (*SyncOneFunc)(cGH*, int );

int (*ParallelInit)(tFleshConfig *);
int (*ParallelFinalise)(tFleshConfig *);

int (*Reduce)(cGH *, int , int operation, void *result);

/* Array of functions */

static void (**functions)() = NULL;


 /*@@
   @routine    RegisterCommFunction
   @date       Tue Sep 29 14:11:07 1998
   @author     Tom Goodale
   @desc 
   Registers a function for use by the cactus communication layer.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int RegisterCommFunction(int key, int (*func)())
{  
  int return_code;

  /* Allocate memory for the array of functions. */
  if(!functions) functions = CreateKeyedFunctionArray(CACTUS_COMM_MAX - CACTUS_COMM_MIN+1);

  /* Check if all is well. */
  if(!functions) 
  {
    fprintf(stderr, "Memory allocation error at line %d in file %s\n", __LINE__, __FILE__);
    exit(1);
  }
  else
  {
    /* Register the function. */
    return_code = RegisterKeyedFunction(functions, 
					CACTUS_COMM_MIN, 
					CACTUS_COMM_MAX, 
					key, 
					(void (*)())func);
  };

  return return_code;
}


 /*@@
   @routine    SetupCommFunctions
   @date       Tue Sep 29 14:19:51 1998
   @author     Tom Goodale
   @desc 
   Assigns the correct names to the registered functions.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int SetupCommFunctions(void)
{
  
  if(functions&&functions[0])
  {
    SetupGH = (cGH * (*)(tFleshConfig *, int))functions[0];
  }
  else
  {
    SetupGH = CactusDefaultSetupGH;
  }

  if(functions&&functions[0])
  {
    SyncAllFuncs = (int (*)(cGH *))functions[0];
  }
  else
  {
    SyncAllFuncs = CactusDefaultSyncAllFuncs;
  }

  if(functions&&functions[0])
  {
    SyncGroupFuncs = (int (*)(cGH *, const char *group))functions[0];
  }
  else
  {
    SyncGroupFuncs = CactusDefaultSyncGroupFuncs;
  }

  if(functions&&functions[0])
  {
    SyncOneFunc = (int (*)(cGH*, int ))functions[0];
  }
  else
  {
    SyncOneFunc = CactusDefaultSyncOneFunc;
  }


  if(functions&&functions[0])
  {
    ParallelInit = (int (*)(tFleshConfig *))functions[0];
  }
  else
  {
    ParallelInit = CactusDefaultParallelInit;
  }

  if(functions&&functions[0])
  {
    ParallelFinalise = (int (*)(tFleshConfig *))functions[0];
  }
  else
  {
    ParallelFinalise = CactusDefaultParallelFinalise;
  }


  if(functions&&functions[0])
  {
    Reduce = (int (*)(cGH *, int , int operation, void *result))functions[0];
  }
  else
  {
    Reduce = CactusDefaultReduce;
  }

  return 0;
}
