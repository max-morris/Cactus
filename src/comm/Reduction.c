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
#include "cctk_Flesh.h"
#include "cctk_FortranString.h"
#include "cctk_Groups.h"
#include "StoreHandledData.h"
#include "cctk_Reduction.h"
#include "cctk_WarnLevel.h"

static char *rcsid = "$Header$";

static cHandledData *ReductionOperators = NULL;
static int num_reductions = 0;
static cHandledData *ReductionArrayOperators = NULL;
static int num_reductions_array = 0;


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
 

int CCTK_RegisterReductionOperator(
         void (*function)(REDUCTION_OPERATOR_REGISTER_ARGLIST),
         const char *name)
{
  int handle;

  /* Check that the method hasn't already been registered */
  handle = Util_GetHandle(ReductionOperators, name, NULL);

  if(handle < 0)
  {
    /* Get a handle for it. */
    handle = Util_NewHandle(&ReductionOperators, name, (void *)function);
    
    /* Remember how many reduction operators there are */
    num_reductions++;
   }
  else
  {
    /* Reduction operator with this name already exists. */
    CCTK_WARN(1,"Reduction operator with this name already exists");
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


 /*@@
   @routine    CCTK_ReductionHandle
   @date       April 28 1999
   @author     Gabrielle Allen
   @desc 
   Returns the handle of a given reduction operator
   @enddesc 
   @var     reduction
   @vdesc   String containing name of reduction operator
   @vtype   const char *
   @vio     in
   @vcomment 
   @endvar 
@@*/

int CCTK_ReductionHandle(const char *reduction)
{

  int handle;
  void **data=NULL; /* isn't used here */

  handle = Util_GetHandle(ReductionOperators, reduction, data);

#ifdef DEBUG_REDUCTION
  CCTK_PRINTSEPARATOR
  printf("In CCTK_ReductionHandle\n");
  printf("-----------------------\n");
  printf("  Got handle %d for %s\n",handle,reduction);
  CCTK_PRINTSEPARATOR
#endif

  if (handle < 0)
    CCTK_WARN(1,"No handle found for this reduction operator");

  return handle;

}  

void FMODIFIER FORTRAN_NAME(CCTK_ReductionHandle)(int *handle, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(reduction)
  *handle = CCTK_ReductionHandle(reduction);
  free(reduction);
}


 /*@@
   @routine    CCTK_Reduce
   @date       April 28 1999
   @author     Gabrielle Allen
   @desc 
               Generic routine for doing a reduction operation on a set of
               Cactus variables.
   @enddesc 
   @var     GH
   @vdesc   pointer to the grid hierarchy
   @vtype   cGH *
   @vio     in
   @endvar 
   @var     proc
   @vdesc   processor that receives the result of the reduction operation
            (a negative value means that all processors get the result)
   @vtype   int
   @vio     in
   @endvar 
   @var     operation_handle
   @vdesc   the handle specifying the reduction operator
   @vtype   int
   @vio     in
   @endvar 
   @var     num_out_vals
   @vdesc   number of elements in the reduction output
   @vtype   int
   @vio     in
   @endvar 
   @var     type_out_vals
   @vdesc   datatype of the output values
   @vtype   int
   @vio     in
   @endvar 
   @var     out_vals
   @vdesc   pointer to buffer holding the output values
   @vtype   void *
   @vio     in
   @endvar 
   @var     num_in_fields
   @vdesc   number of input fields passed in the variable argument list
   @vtype   int
   @vio     in
   @endvar 
   @var     <...>
   @vdesc   list of variables indices of input fields
   @vtype   int
   @vio     in
   @endvar 
@@*/

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
  void (*function)(REDUCTION_OPERATOR_REGISTER_ARGLIST)=NULL; 

  /* Get the pointer to the reduction operator */

  if (operation_handle < 0)

    CCTK_WARN(3,"Invalid handle passed to CCTK_Reduce");

  else
  {
    function = (void (*)(REDUCTION_OPERATOR_REGISTER_ARGLIST))
      Util_GetHandledData(ReductionOperators,operation_handle);
    
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
      CCTK_WARN(3,"Reduction operation is not registered and cannot be called");
  }

  return 1;

}

void FMODIFIER FORTRAN_NAME(CCTK_Reduce)(cGH *GH, 
                  int *fortranreturn,
                  int *proc,
                  int *operation_handle,
                  int *num_out_vals,
                  int *type_out_vals,
                  void *out_vals,
                  int *num_in_fields,
                  ... )
{ 
  va_list indices;
  int i;
  int *in_fields = malloc(*num_in_fields*sizeof(int));
  void (*function)(REDUCTION_OPERATOR_REGISTER_ARGLIST)=NULL; 

  /* Get the pointer to the reduction operator */


  if (*operation_handle < 0)

    CCTK_WARN(3,"Invalid handle passed to CCTK_Reduce");

  else
  {
    function = (void (*)(REDUCTION_OPERATOR_REGISTER_ARGLIST))
      Util_GetHandledData(ReductionOperators,*operation_handle);
    
    if (function)
      {
        
        /* Fill in the array of variable indices from the variable argument list */
        va_start(indices, num_in_fields);
        for (i=0; i<*num_in_fields; i++)
          in_fields[i] = *va_arg(indices,int *);
        va_end(indices);
        
        function(GH,*proc,*num_out_vals,*type_out_vals,out_vals,*num_in_fields,in_fields);
        
        if (in_fields) free(in_fields);
        
      }
    else
      CCTK_WARN(3,"Reduction operation is not registered and cannot be called");
  }

  *fortranreturn= 1;

}


 /*@@
   @routine CCTK_RegisterReductionArrayOperator
   @date    Aug 19 1999
   @author  Thomas Radke
   @desc 
            Registers "function" as a array reduction operator called "name"
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
 
int CCTK_RegisterReductionArrayOperator
         (void (*function)(REDUCTION_ARRAY_OPERATOR_REGISTER_ARGLIST),
         const char *name)
{
  int handle;

  /* Check that the method hasn't already been registered */
  handle = Util_GetHandle(ReductionArrayOperators, name, NULL);

  if(handle < 0)
  {
    /* Get a handle for it. */
    handle = Util_NewHandle(&ReductionArrayOperators, name, (void *)function);
    
    /* Remember how many reduction operators there are */
    num_reductions_array++;
   }
  else
  {
    /* Reduction operator with this name already exists. */
    CCTK_WARN(1,"Array reduction operator with this name already exists");
    handle = -1;
  }

#ifdef DEBUG_REDUCTION
  CCTK_PRINTSEPARATOR
  printf("In CCTK_RegisterReductionArrayOperator\n");
  printf("---------------------------------\n");
  printf("  Registering %s with handle %d\n",name,handle);
  CCTK_PRINTSEPARATOR
#endif
    
  return handle;

}


 /*@@
   @routine CCTK_ReductionArrayHandle
   @date    Aug 19 1999
   @author  Thomas Radke
   @desc 
            Returns the handle of a given array reduction operator
   @enddesc 
   @var     reduction
   @vdesc   String containing name of array reduction operator
   @vtype   const char *
   @vio     in
   @vcomment 
   @endvar 
@@*/

int CCTK_ReductionArrayHandle(const char *reduction)
{

  int handle;
  void **data=NULL; /* isn't used here */

  handle = Util_GetHandle(ReductionArrayOperators, reduction, data);

#ifdef DEBUG_REDUCTION
  CCTK_PRINTSEPARATOR
  printf("In CCTK_ReductionArrayHandle\n");
  printf("-----------------------\n");
  printf("  Got handle %d for %s\n",handle,reduction);
  CCTK_PRINTSEPARATOR
#endif

  if (handle < 0)
    CCTK_WARN(1,"No handle found for this array reduction operator");

  return handle;

}  

void FMODIFIER FORTRAN_NAME(CCTK_ReductionArrayHandle)
                           (int *operation_handle, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(reduction)
  *operation_handle = CCTK_ReductionArrayHandle(reduction);
  free(reduction);
}


 /*@@
   @routine    CCTK_ReduceArray
   @date       Aug 19 1999
   @author     Thomas Radke
   @desc 
               Generic routine for doing a reduction operation on a set of
               arrays.
   @enddesc 
   @var     GH
   @vdesc   pointer to the grid hierarchy
   @vtype   cGH *
   @vio     in
   @endvar 
   @var     proc
   @vdesc   processor that receives the result of the reduction operation
            (a negative value means that all processors get the result)
   @vtype   int
   @vio     in
   @endvar 
   @var     operation_handle
   @vdesc   the handle specifying the reduction operator
   @vtype   int
   @vio     in
   @endvar 
   @var     num_out_vals
   @vdesc   number of elements in the reduction output
   @vtype   int
   @vio     in
   @endvar 
   @var     type_out_vals
   @vdesc   datatype of the reduction output
   @vtype   int
   @vio     in
   @endvar 
   @var     out_vals
   @vdesc   pointer to buffer holding the output values
   @vtype   void *
   @vio     in
   @endvar 
   @var     num_dims
   @vdesc   number of dimensions of input arrays
   @vtype   int
   @vio     in
   @endvar 
   @var     num_in_fields
   @vdesc   number of input fields passed in the variable argument list
   @vtype   int
   @vio     in
   @endvar 
   @var     type_in_arrays
   @vdesc   datatype of the input arrays
   @vtype   int
   @vio     in
   @endvar 
   @var     <...>
   @vdesc   list of dimensions of input arrays
   @vtype   int
   @vio     in
   @endvar 
   @var     <...>
   @vdesc   list of pointers to input arrays
   @vtype   void *
   @vio     in
   @endvar 
@@*/

int CCTK_ReduceArray(  cGH *GH, 
                       int proc,
                       int operation_handle,
                       int num_out_vals,
                       int type_out_vals,
                       void *out_vals,
                       int num_dims,
                       int num_in_arrays,
                       int type_in_arrays,
                       ... )
{
 
  va_list indices;
  int i;
  int *dims;
  void **in_arrays;
  void (*function)(REDUCTION_ARRAY_OPERATOR_REGISTER_ARGLIST)=NULL; 


  /* Get the pointer to the reduction operator */
  if (operation_handle < 0)
  {
    CCTK_WARN(3,"Invalid handle passed to CCTK_ReduceArray");
    return (1);
  }

  function = (void (*)(REDUCTION_ARRAY_OPERATOR_REGISTER_ARGLIST))
             Util_GetHandledData(ReductionArrayOperators,operation_handle);
    
  if (! function)
  {
    CCTK_WARN(3, "Array reduction operation is not registered "
                 "and cannot be called");
    return (1);
  }

  /* allocate memory for dims and input array pointers */
  dims = (int *) malloc (num_dims * sizeof (int));
  in_arrays = (void **) malloc (num_in_arrays * sizeof (void *));

  /* Fill in the arrays of dims and input array pointers
     from the variable argument list */

  va_start(indices, type_in_arrays);

  for (i = 0; i < num_dims; i++)
    dims [i] = va_arg (indices, int);
  for (i = 0; i < num_in_arrays; i++)
    in_arrays [i] = va_arg (indices, void *);

  va_end(indices);
        
  function (GH, proc, num_dims, dims, num_in_arrays, in_arrays, type_in_arrays,
            num_out_vals, out_vals, type_out_vals);
        
  free (in_arrays);
  free (dims);
        
  return 0;
}

void FMODIFIER FORTRAN_NAME(CCTK_ReduceArray)(cGH *GH, 
                                              int *fortran_return,
                                              int *proc,
                                              int *operation_handle,
                                              int *num_out_vals,
                                              int *type_out_vals,
                                              void *out_vals,
                                              int *num_dims,
                                              int *num_in_arrays,
                                              int *type_in_arrays,
                                              ... )
{
 
  va_list varargs;
  int i;
  int *dims;
  void **in_arrays;
  void (*function)(REDUCTION_ARRAY_OPERATOR_REGISTER_ARGLIST)=NULL; 


  *fortran_return = 1;

  /* Get the pointer to the reduction operator */
  if (*operation_handle < 0)
  {
    CCTK_WARN (3,"Invalid handle passed to CCTK_ReduceArray");
    return;
  }

  function = (void (*) (REDUCTION_ARRAY_OPERATOR_REGISTER_ARGLIST))
              Util_GetHandledData (ReductionArrayOperators, *operation_handle);
    
  if (! function)
  {
    CCTK_WARN (3, "Array reduction operation is not registered "
                  "and cannot be called");
    return;
  }

  /* allocate memory for dims and input array pointers */
  dims = (int *) malloc (*num_dims * sizeof (int));
  in_arrays = (void **) malloc (*num_in_arrays * sizeof (void *));

  /* Fill in the arrays of dims and input array pointers
     from the variable argument list */

  va_start (varargs, type_in_arrays);

  for (i = 0; i < *num_dims; i++)
    dims [i] = *va_arg (varargs, int *);
  for (i = 0; i < *num_in_arrays; i++)
    in_arrays [i] = va_arg (varargs, void *);

  va_end (varargs);
        
  function (GH, *proc, *num_dims, dims, *num_in_arrays, in_arrays,
            *type_in_arrays, *num_out_vals, out_vals, *type_out_vals);
        
  free (in_arrays);
  free (dims);
        
  *fortran_return = 0;
}


 /*@@
   @routine    CCTK_ReduceLocalScalar
   @date       Aug 19 1999
   @author     Thomas Radke
   @desc    Wrapper function for reduction of a single scalar
   @enddesc 
   @var     GH
   @vdesc   pointer to the grid hierarchy
   @vtype   cGH *
   @vio     in
   @endvar 
   @var     proc
   @vdesc   processor that receives the result of the reduction operation
            (a negative value means that all processors get the result)
   @vtype   int
   @vio     in
   @endvar 
   @var     operation_handle
   @vdesc   the handle specifying the reduction operator
   @vtype   int
   @vio     in
   @endvar 
                            void *in_scalar, void *out_scalar, int data_type)
   @var     in_scalar
   @vdesc   pointer to input scalar
   @vtype   void *
   @vio     in
   @endvar 
   @var     out_scalar
   @vdesc   pointer to output scalar
   @vtype   void *
   @vio     in
   @endvar 
   @var     data_type
   @vdesc   datatype for both input and output scalar
   @vtype   int
   @vio     in
   @endvar 
@@*/

int CCTK_ReduceLocalScalar (cGH *GH, int proc, int operation_handle,
                            void *in_scalar, void *out_scalar, int data_type)
{
  return (CCTK_ReduceArray (GH, proc, operation_handle,
                            1, data_type, out_scalar,
                            1, 1, data_type, 1, in_scalar));
}


void FMODIFIER FORTRAN_NAME(CCTK_ReduceLocalScalar)(cGH *GH, 
                                                    int *fortran_return,
                                                    int *proc,
                                                    int *operation_handle,
                                                    void *in_scalar,
                                                    void *out_scalar,
                                                    int *data_type)
{
  *fortran_return = CCTK_ReduceArray (GH, *proc, *operation_handle,
                                      1, *data_type, out_scalar,
                                      1, 1, *data_type, 1, in_scalar);
}
