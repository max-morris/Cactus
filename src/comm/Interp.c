 /*@@
   @file      Interp.c
   @date      July 07 1999
   @author    Thomas Radke
   @desc 
              This file contains routines to deal with registering and 
              using functions providing interpolation operations.
   @enddesc 
   @history
   @date      July 07 1999
   @author    Thomas Radke
   @hdesc     Just copied from Reduction.c
   @endhistory
 @@*/

/*#define DEBUG_INTERP*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>

#include "cctk.h"
#include "flesh.h"
#include "FortranString.h"
#include "Groups.h"
#include "StoreHandledData.h"
#include "Interp.h"
#include "WarnLevel.h"


static cHandledData *InterpOperators = NULL;
static int num_interps = 0;


 /*@@
   @routine    CCTK_RegisterInterpOperator
   @date       July 07 1999
   @author     Thomas Radke
   @desc 
   Registers "function" as an interpolation operator called "name"
   @enddesc 
   @var     function
   @vdesc   Routine containing interpolation operator
   @vtype   (int (*))
   @vio     
   @vcomment 
   @endvar 
   @var     name
   @vdesc   String containing name of interpolation operator
   @vtype   const char *
   @vio     in
   @vcomment 
   @endvar 
@@*/
 
int CCTK_RegisterInterpOperator(int (*function)(REGISTER_INTERP_ARGLIST),
                                const char *name)
{
  int handle;

  /* Check that the method hasn't already been registered */
  handle = CCTK_GetHandle(InterpOperators, name, NULL);

  if(handle < 0)
  {
    /* Get a handle for it. */
    handle = CCTK_NewHandle(&InterpOperators, name, (void *)function);
    
    /* Remember how many interpolation operators there are */
    num_interps++;
   }
  else
  {
    /* Interpolation operator with this name already exists. */
    CCTK_WARN(1,"Interpolation operator with this name already exists");
    handle = -1;
  }

#ifdef DEBUG_INTERP
  CCTK_PRINTSEPARATOR
  printf("In CCTK_RegisterInterpOperator\n");
  printf("---------------------------------\n");
  printf("  Registering %s with handle %d\n",name,handle);
  CCTK_PRINTSEPARATOR
#endif
    
  return handle;

}


 /*@@
   @routine    CCTK_GetInterpHandle
   @date       July 07 1999
   @author     Thomas Radke
   @desc 
   Returns the handle of a given interpolation operator
   @enddesc 
   @var     interp
   @vdesc   String containing name of interpolation operator
   @vtype   const char *
   @vio     in
   @vcomment 
   @endvar 
@@*/

int CCTK_GetInterpHandle(const char *interp)
{

  int handle;
  void **data=NULL; /* isn't used here */

  handle = CCTK_GetHandle(InterpOperators, interp, data);

#ifdef DEBUG_INTERP
  CCTK_PRINTSEPARATOR
  printf("In CCTK_GetInterpHandle\n");
  printf("--------------------------\n");
  printf("  Got handle %d for %s\n",handle,interp);
  CCTK_PRINTSEPARATOR
#endif

  if (handle < 0)
    CCTK_WARN(1,"No handle found for this interpolation operator");

  return handle;

}  

void FMODIFIER FORTRAN_NAME(CCTK_GetInterpHandle)(int *handle, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(interp)
  *handle = CCTK_GetInterpHandle(interp);
  free(interp);
}



 /*@@
   @routine    CCTK_Interp
   @date       July 07 1999
   @author     Thomas Radke
   @desc 
               The CCTK interpolation routine
               Just puts the arguments from the variable argument list
               into arrays and calls the appropriate interpolation operator.
   @enddesc 
   @var        GH
   @vdesc      pointer to CCTK grid hierarchy
   @vtype      cGH *
   @vio        in
   @endvar 
   @var        operation_handle
   @vdesc      handle for the interpolation operator
   @vtype      int
   @vio        in
   @endvar 
   @var        nPoints
   @vdesc      number of points to interpolate at
   @vtype      int
   @vio        in
   @endvar 
   @var        nCoords
   @vdesc      number of passed coordinate fields
   @vtype      int
   @vio        in
   @endvar 
   @var        nInFields
   @vdesc      number of passed input fields
   @vtype      int
   @vio        in
   @endvar 
   @var        nOutFields
   @vdesc      number of passed output fields
   @vtype      int
   @vio        in
   @vcomment   end of fixed argument list
   @endvar 

   @var        coords
   @vdesc      coordinates of points to interpolate at
   @vtype      void * [nCoords]
   @vio        in
   @endvar 
   @var        coordTypes
   @vdesc      types of passed coordinate fields
   @vtype      int [nCoords]
   @vio        in
   @endvar 
   @var        inFieldIndices
   @vdesc      indices of GF to interpolate from
   @vtype      int [nInfields]
   @vio        in
   @endvar 
   @var        outFields
   @vdesc      pointer to arrays to hold the interpolation results
   @vtype      void * [nOutfields]
   @vio        in
   @endvar 
   @var        outFieldTypes
   @vdesc      types of arrays to hold the interpolation results
   @vtype      int [nOutfields]
   @vio        in
   @endvar 
@@*/

int CCTK_Interp (cGH *GH,
                 int operation_handle,
                 int nPoints,
                 int nCoords,
                 int nInFields,
                 int nOutFields,
                 ...)
{
  va_list indices;
  int i;
  void **coords;
  int *coordTypes;
  int *inFieldIndices;
  void **outFields;
  int *outFieldTypes;
  int (*function)(REGISTER_INTERP_ARGLIST)=NULL; 

  i = 0;

  /* Get the pointer to the interpolation operator */
  if (operation_handle < 0)

    CCTK_WARN(3,"Invalid handle passed to CCTK_Interp");

  else
  {
    function = (int (*)(REGISTER_INTERP_ARGLIST))
      CCTK_GetHandledData(InterpOperators,operation_handle);
    
    if (function)
    {

      coords = (void **) malloc (nCoords * sizeof (void *));
      coordTypes = (int *) malloc (nCoords * sizeof (int));
      inFieldIndices = (int *) malloc (nInFields * sizeof (int));
      outFields = (void **) malloc (nOutFields * sizeof (void *));
      outFieldTypes = (int *) malloc (nOutFields * sizeof (int));

      /* Fill in the arrays from the variable argument list */
      va_start (indices, nOutFields);
      for (i = 0; i < nCoords; i++)
        coords [i] = va_arg (indices, void *);
      for (i = 0; i < nCoords; i++)
        coordTypes [i] = va_arg (indices, int);
      for (i = 0; i < nInFields; i++)
        inFieldIndices [i] = va_arg (indices, int);
      for (i = 0; i < nOutFields; i++)
        outFields [i] = va_arg (indices, void *);
      for (i = 0; i < nOutFields; i++)
        outFieldTypes [i] = va_arg (indices, int);
      va_end (indices);

      i = function(GH, nPoints, nCoords, nInFields, nOutFields,
                   coords, coordTypes,inFieldIndices, outFields, outFieldTypes);
      
      free (outFieldTypes);
      free (outFields);
      free (inFieldIndices);
      free (coordTypes);
      free (coords);
      
    }
    else
      CCTK_WARN(3,"Interpolation operation is not registered and cannot be called");
  }

  return i;

}

void FMODIFIER FORTRAN_NAME(CCTK_Interp)(cGH *GH,
                                         int *fortranreturn,
                                         int *operation_handle,
                                         int *nPoints,
                                         int *nCoords,
                                         int *nInFields,
                                         int *nOutFields,
                                         ...)
{ 
  va_list indices;
  int i;
  void **coords;
  int *coordTypes;
  int *inFieldIndices;
  void **outFields;
  int *outFieldTypes;
  int (*function)(REGISTER_INTERP_ARGLIST)=NULL; 

  i = 0;

  /* Get the pointer to the interpolation operator */

  if (*operation_handle < 0)

    CCTK_WARN(3,"Invalid handle passed to CCTK_Interp");

  else
  {
    function = (int (*)(REGISTER_INTERP_ARGLIST))
      CCTK_GetHandledData(InterpOperators,*operation_handle);
    
    if (function)
    {
      
      coords = (void **) malloc (*nCoords * sizeof (void *));
      coordTypes = (int *) malloc (*nCoords * sizeof (int));
      inFieldIndices = (int *) malloc (*nInFields * sizeof (int));
      outFields = (void **) malloc (*nOutFields * sizeof (void *));
      outFieldTypes = (int *) malloc (*nOutFields * sizeof (int));

      /* Fill in the arrays from the variable argument list */
      va_start (indices, nOutFields);
      for (i = 0; i < *nCoords; i++)
        coords [i] = va_arg (indices, void *);
      for (i = 0; i < *nCoords; i++)
        coordTypes [i] = *va_arg (indices, int *);
      for (i = 0; i < *nInFields; i++)
        inFieldIndices [i] = *va_arg (indices, int *);
      for (i = 0; i < *nOutFields; i++)
        outFields [i] = va_arg (indices, void *);
      for (i = 0; i < *nOutFields; i++)
        outFieldTypes [i] = *va_arg (indices, int *);
      va_end (indices);

      i = function(GH, *nPoints, *nCoords, *nInFields, *nOutFields,
                   coords, coordTypes,inFieldIndices, outFields, outFieldTypes);
      
      free (outFieldTypes);
      free (outFields);
      free (inFieldIndices);
      free (coordTypes);
      free (coords);

    }
    else
      CCTK_WARN(3,"Interpolation operation is not registered and cannot be called");
  }

  *fortranreturn = i;
}
