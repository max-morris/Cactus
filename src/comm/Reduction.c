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
  CCTK_PRINTSEPARATOR
  printf("In CCTK_RegisterReductionOperator\n");
  printf("---------------------------------\n");
  printf("  Registering %s with handle %d\n",name,handle);
  CCTK_PRINTSEPARATOR
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


int CCTK_Reduce(  cGH *GH, 
		  int proc,
		  int operation_handle,
                  int num_out_vals,
		  int type_out_vals,
		  void *out_vals,
		  int num_in_fields,
		  ... )
{
 
  va_list indices;
  int i;
  int *in_fields = malloc(num_in_fields*sizeof(int));
  void (*function)(REGISTER_ARGLIST); 

  /* Get the pointer to the reduction operator */
  function = (void (*)(REGISTER_ARGLIST))
    CCTK_GetHandledData(ReductionOperators,operation_handle);

  if (function)
  {

    /* Fill in the array of variable indices from the variable argument list */
    va_start(indices, num_in_fields);
    for (i=0; i<num_in_fields; i++)
      in_fields[i] = va_arg(indices,int);
    va_end(indices);
    
    function(GH,proc,num_out_vals,type_out_vals,out_vals,num_in_fields,in_fields);

    if (in_fields) free(in_fields);

  }
  else
    CCTK_Warn(3,"CCTK","Reduction operation is not registered and cannot be called");
  
}








