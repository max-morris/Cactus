 /*@@
   @file      RegisterMainFunctions.c
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
#include "CactusMainDefaults.h"

static char *rcsid = "$Id$";


/* Definitions of how many functions */
#define CACTUS_MAIN_MIN 0
#define CACTUS_MAIN_MAX 2

/* The functions. */

int (*Initialise)(tFleshConfig *);

int (*Evolve)(tFleshConfig *);

int (*Shutdown)(tFleshConfig *);

/* Array of functions */

static void (**functions)() = NULL;


 /*@@
   @routine    RegisterMainFunction
   @date       Tue Sep 29 14:11:07 1998
   @author     Tom Goodale
   @desc 
   Registers a function for use by the cactus flesh.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int RegisterMainFunction(int key, int (*func)(tFleshConfig *))
{  
  int return_code;

  /* Allocate memory for the array of functions. */
  if(!functions) functions = CreateKeyedFunctionArray(CACTUS_MAIN_MAX - CACTUS_MAIN_MIN+1);

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
					CACTUS_MAIN_MIN, 
					CACTUS_MAIN_MAX, 
					key, 
					(void (*)())func);
  };

  return return_code;
}


 /*@@
   @routine    SetupMainFunctions
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
int SetupMainFunctions(void)
{
  
  if(functions&&functions[0])
  {
    Initialise = (int (*)(tFleshConfig *))functions[0];
  }
  else
  {
    Initialise = CactusDefaultInitialise;
  }

  if(functions&&functions[1])
  {
    Evolve     = (int (*)(tFleshConfig *))functions[1];
  }
  else
  {
    Evolve = CactusDefaultEvolve;
  }

  if(functions&&functions[2])
  {
    Shutdown   = (int (*)(tFleshConfig *))functions[2];
  }
  else
  {
    Shutdown = CactusDefaultShutdown;
  }

  return 0;
}
