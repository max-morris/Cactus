 /*@@
   @file      RegisterIOFunction.c
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
#include "CactusIODefaults.h"

static char *rcsid = "$Id$";


/* Definitions of how many functions */
#define CACTUS_IO_MIN 0
#define CACTUS_IO_MAX 2

/* The functions. */

int (*Output1D)(cGH *, cGF *);

int (*Output2D)(cGH *, cGF *);

int (*Output3D)(cGH *, cGF *);

/* Array of functions */

static void (**functions)() = NULL;


 /*@@
   @routine    RegisterIOFunction
   @date       Tue Sep 29 14:11:07 1998
   @author     Tom Goodale
   @desc 
   Registers a function for use by the cactus IO.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int RegisterIOFunction(int key, int (*func)(cGH *, cGF *))
{  
  int return_code;

  /* Allocate memory for the array of functions. */
  if(!functions) functions = CreateKeyedFunctionArray(CACTUS_IO_MAX - CACTUS_IO_MIN+1);

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
					CACTUS_IO_MIN, 
					CACTUS_IO_MAX, 
					key, 
					(void (*)())func);
  };

  return return_code;
}


 /*@@
   @routine    SetupIOFunctions
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
int SetupIOFunctions(void)
{
  
  if(functions&&functions[0])
  {
    Output1D = (int (*)(cGH *, cGF *))functions[0];
  }
  else
  {
    Output1D = CactusDefaultOutput1D;
  }

  if(functions&&functions[0])
  {
    Output2D = (int (*)(cGH *, cGF *))functions[1];
  }
  else
  {
    Output2D = CactusDefaultOutput1D;
  }

  if(functions&&functions[0])
  {
    Output3D = (int (*)(cGH *, cGF *))functions[2];
  }
  else
  {
    Output3D = CactusDefaultOutput1D;
  }


  return 0;
}
