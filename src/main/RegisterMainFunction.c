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


static char *rcsid = "$Id$";


#define CACTUS_MAIN_MIN 0
#define CACTUS_MAIN_MAX 2

int (*Initialise)(tFleshConfig *);

int (*Evolve)(tFleshConfig *);

int (*Shutdown)(tFleshConfig *);

static void (**functions)() = NULL;


int RegisterMainFunction(int key, int (*func)(tFleshConfig *))
{  
  int return_code;

  if(!functions) functions = CreateKeyedFunctionArray(CACTUS_MAIN_MAX - CACTUS_MAIN_MIN+1);

  if(!functions) 
  {
    fprintf(stderr, "Memory allocation error at line %d in file %s\n", __LINE__, __FILE__);
    exit(1);
  }
  else
  {
    return_code = RegisterKeyedFunction(functions, 
					CACTUS_MAIN_MIN, 
					CACTUS_MAIN_MAX, 
					key, 
					(void (*)())func);
  };

  return return_code;
}


int SetupMainFunctions(void)
{
  
  Initialise = (int (*)(tFleshConfig *))functions[0];
  
  Evolve     = (int (*)(tFleshConfig *))functions[1];

  Shutdown   = (int (*)(tFleshConfig *))functions[2];

  return 0;
}
