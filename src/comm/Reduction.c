 /*@@
   @file      Reduction.c
   @date      
   @author    Gabrielle Allen
   @desc 
   This file contains routines to deal with registering and 
   using functions providing reduction operations.
   @enddesc 
 @@*/

/*#define DEBUG_REDUCTION*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>

#include "cctk.h"
#include "flesh.h"
#include "Groups.h"
#include "StoreHandledData.h"
#include "Reduction.h"
#include "WarnLevel.h"

static char *rcsid = "$Header$";

static cHandledData *ReductionOperators = NULL;
static int num_reductions = 0;

#define REGISTER_ARGLIST  \
	  cGH *,  \
          int,    \
          int,    \
          int,    \
          void *, \
          int,    \
          int *    

 /*@@
   @routine    CCTK_RegisterReductionOperator
   @date       April 28 1999
   @author     Gabrielle Allen
   @desc 
   Registers "function" as a reduction operator called "name"
   @enddesc 
   @var     function
   @vdesc   Routine containing reduction operator
   @vtype   (void (*))
   @vio     
   @vcomment 
   @endvar 
   @var     name
   @vdesc   String containing name of reduction operator
   @vtype   const char *
   @vio     in
   @vcomment 
   @endvar 
@@*/
 
int CCTK_RegisterReductionOperator(void (*function)(REGISTER_ARGLIST),
				  const char *name)
{
  int handle;

  /* Check that the method hasn't already been registered */
  handle = CCTK_GetHandle(ReductionOperators, name, NULL);

  if(handle < 0)
  {
    /* Get a handle for it. */
    handle = CCTK_NewHandle(&ReductionOperators, name, (void *)function);
    
    /* Remember how many reduction operators there are */
    num_reductions++;
   }
  else
  {
    /* Reduction operator with this name already exists. */
    CCTK_Warn(1,"CCTK","Reduction operator with this name already exists");
    handle = -1;
  }

#ifdef DEBUG_REDUCTION
  printf("----------------------------------------------------------------\n");
  printf("In CCTK_RegisterReductionOperator\n");
  printf("---------------------------------\n");
  printf("  Registering %s with handle %d\n",name,handle);
  printf("----------------------------------------------------------------\n");
#endif
    
  return handle;

}



int CCTK_GetReductionHandle(const char *reduction)
{

  int handle;
  void **data=NULL; /* isn't used here */

  handle = CCTK_GetHandle(ReductionOperators, reduction, data);

#ifdef DEBUG_REDUCTION
  printf("----------------------------------------------------------------\n");
  printf("In CCTK_GetReductionHandle\n");
  printf("--------------------------\n");
  printf("  Got handle %d for %s\n",handle,reduction);
  printf("----------------------------------------------------------------\n");
#endif

  if (handle < 0)
    CCTK_Warn(1,"CCTK","No handle found for this reduction operator");

  return handle;

}  


int CCTK_Reduce(cGH *GH, int retvartype, int retvarnum, void *retval,
		int operation_handle, int index, ...)
{
 
  va_list indices;
  int invarnum;
  int *index_array;
  void (*function)(REGISTER_ARGLIST); 


  /* Fill in the array of variable indices from the variable argument list */
  invarnum = 0;
  index_array = (int *)malloc(sizeof(int));
  index_array[0] = index;

  va_start(indices, index);
  invarnum++;
  index_array = (int *)realloc(index_array,invarnum*sizeof(int));
  index_array[invarnum-1] = index;
  va_end(indices);


  /* Get the pointer to the reduction operator */
  function = (void (*)(REGISTER_ARGLIST))
    CCTK_GetHandledData(ReductionOperators,operation_handle);


  /* Call the function providing the reduction, if it exists */
  /* FIX ME : Generalise (GAB) */
  if (function)
  {
    function(GH,-1,CCTK_VARIABLE_REAL,retvarnum,retval,invarnum,index_array);
  }
  else
    CCTK_Warn(1,"CCTK","Reduction operation is not registered and cannot be called");
  
  if (index_array) free(index_array);

  return 1;

}








