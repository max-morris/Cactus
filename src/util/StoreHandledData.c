 /*@@
   @file      StoreHandledData.c
   @date      Mon Feb  1 17:54:26 1999
   @author    Tom Goodale
   @desc 
   Stores data referenced by a handle
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include "hll.h"
#include "hll_internal.h"

static char *rcsid="$Id$";

/* Static variables defining the array. */

typedef struct 
{
  unsigned int in_use;
  char *name;
  void *data;
} cHandleStorage;

typedef struct
{
  cHandleStorage *array;
  unsigned array_size;
  unsigned first_unused;
} cHandledData;


 /*@@
   @routine    HLLi_AddHierarchy
   @date       Fri May  8 12:56:43 1998
   @author     Tom Goodale
   @desc 
   Adds an HLL Hierarchy object to the array.
   Resizes the array if necessary.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_NewHandle(cHandledData **storage, char *name, void *data)
{
  int return_code;

  void *temp;

  /* Check if the array needs to be resized. */
  if(hll_first_unused <= hll_array_size)
  {
    if(!(temp = (void *)realloc(hll_array, (hll_array_size+1)*sizeof(hll_storage))))
    {
      /* Failed to allocate memory for new array element. */
      *handle = HLL_HANDLE_NULL;

      return_code = HLL_MEMORY_FAILURE;
    }
    else
    {
      hll_array = temp;

      /* Fill in data in array. */
      hll_array[hll_array_size].in_use = TRUE;
      hll_array[hll_array_size].hierarchy = hierarchy;

      *handle = hll_array_size;

      /* Increase array size counter. */
      hll_array_size++;

      /* Record position of first unused array element. */
      hll_first_unused = hll_array_size;

      return_code = HLL_SUCCESS;
    };
  }
  else
  {
    /* There's an available element in the array. */

    if(hll_array[hll_first_unused].in_use == TRUE)
    {
      /* The pointers have become corrupted in some fashion.
       *
       * Could write a repair function, but probably safer to just
       * produce an error.
       */
      *handle = HLL_HANDLE_NULL;

      return_code = HLL_FATAL_ERROR;
    }
    else
    {      
      /* Fill in data in array. */
      hll_array[hll_first_unused].in_use = TRUE;
      hll_array[hll_first_unused].hierarchy = hierarchy;

      *handle = hll_first_unused;
      
      /* Change pointer to first unused array element. */
      hll_first_unused = HLLi_FindNextUnused(hll_first_unused);

      return_code = HLL_SUCCESS;
    };
  };

  return return_code;

}

 /*@@
   @routine    HLLi_RemoveHierarchy
   @date       Fri May  8 14:13:16 1998
   @author     Tom Goodale
   @desc 
   Removes an HLL Hierarchy from the array.
   @enddesc 
   @calls     
   @calledby   HLL_FreeHierarchy
   @history 
 
   @endhistory 

@@*/
int HLLi_RemoveHierarchy(HLL handle)
{
  int return_code;

  if((handle != HLL_HANDLE_NULL)&&(handle < hll_first_unused)&&(handle >= 0))
  {
    /* It's a valid handle. */
    hll_array[handle].in_use = FALSE;
    hll_array[handle].hierarchy = NULL;

    if(handle < hll_first_unused)
    {
      hll_first_unused = handle;
    };

    return_code = HLL_SUCCESS;
  }
  else
  {
    /* The handle does not exist. */
    return_code = HLL_HANDLE_INVALID;
  };

  return return_code;

}


 /*@@
   @routine    HLLi_FindNextUnused
   @date       Fri May  8 14:09:13 1998
   @author     Tom Goodale
   @desc 
   Finds the next unused element in the array holding all HLL handles.
   Returns the size of the array if all are in use.
   Assumes there are no unused ones before the value of the `first' parameter.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int HLLi_FindNextUnused(int first)
{
  int current;

  current = first+1;

  /* Loop until the end of the array. */

  while(current < hll_array_size)
  {
    if(hll_array[current].in_use==FALSE) break;

    current++;
  }
    
  return current;

}
    
 /*@@
   @routine    HLLi_GetHierarchy
   @date       Fri May  8 17:36:33 1998
   @author     Tom Goodale
   @desc 
   Gets a pointer to the HLL Hierarchy corresponding to the give handle.
   @enddesc 
   @calls     
   @calledby   HLL_InitialiseHierarchy HLL_SetFlags HLL_EvolveHierarchy HLL_FreeHierarchy
   @history 
 
   @endhistory 

@@*/
HLL_Hierarchy *HLLi_GetHierarchy(int handle)
{
  HLL_Hierarchy *hierarchy;

  if((handle != HLL_HANDLE_NULL)&&
     (handle < hll_array_size)&&
     (handle >= 0)&&
     (hll_array[handle].in_use == TRUE))
  {
    /* The hierarchy exists */
    hierarchy = hll_array[handle].hierarchy;
  }
  else
  {
    /* The Hierarchy is non-existant. */
    hierarchy = NULL;
  };

  return hierarchy;
}
