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
#include <stdarg.h>

#include "cctk_Types.h"
#include "cctk_Constants.h"
#include "cctk_Flesh.h"
#include "cctk_FortranString.h"
#include "cctk_Groups.h"
#include "cctk_GroupsOnGH.h"
#include "StoreHandledData.h"
#include "cctk_Interp.h"
#include "cctk_WarnLevel.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(comm_Interp_c)

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
  handle = Util_GetHandle(InterpOperators, name, NULL);

  if(handle < 0)
  {
    /* Get a handle for it. */
    handle = Util_NewHandle(&InterpOperators, name, (void *)function);
    
    /* Remember how many interpolation operators there are */
    num_interps++;
   }
  else
  {
    /* Interpolation operator with this name already exists. */
    CCTK_Warn(1,__LINE__,__FILE__,"Cactus",
	      "Interpolation operator with this name already exists");
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

  handle = Util_GetHandle(InterpOperators, interp, data);

#ifdef DEBUG_INTERP
  CCTK_PRINTSEPARATOR
  printf("In CCTK_GetInterpHandle\n");
  printf("--------------------------\n");
  printf("  Got handle %d for %s\n",handle,interp);
  CCTK_PRINTSEPARATOR
#endif

  if (handle < 0)
    CCTK_Warn(1,__LINE__,__FILE__,"Cactus",
	      "No handle found for this interpolation operator");

  return handle;

}  

void CCTK_FCALL CCTK_FNAME(CCTK_GetInterpHandle)
     (int *handle, ONE_FORTSTRING_ARG)
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
   @var        nDims
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
   @vtype      void * [nDims]
   @vio        in
   @endvar 
   @var        coordTypes
   @vdesc      types of passed coordinate fields
   @vtype      int [nDims]
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

int CCTK_InterpGF (cGH *GH,
                   int operation_handle,
                   int nPoints,
                   int nDims,
                   int nInFields,
                   int nOutFields,
                   ...)
{
  va_list indices;
  int i, retcode;
  int index, timelevel;
  void **coords;
  int *coordTypes;
  int *inFieldTypes, *outFieldTypes;
  void **inFields, **outFields;
  CCTK_REAL *origin;
  int (*function)(REGISTER_INTERP_ARGLIST)=NULL; 

  retcode = -1;

  /* Get the pointer to the interpolation operator */
  if (operation_handle < 0)
  {
    CCTK_Warn(3,__LINE__,__FILE__,
	      "Cactus","Invalid handle passed to CCTK_Interp");
  }
  else
  {
    function = (int (*)(REGISTER_INTERP_ARGLIST))
      Util_GetHandledData(InterpOperators,operation_handle);
    
    if (function)
    {

      coords = (void **) malloc (nDims * sizeof (void *));
      coordTypes = (int *) malloc (nDims * sizeof (int));
      inFields = (void **) malloc (nInFields * sizeof (void *));
      inFieldTypes = (int *) malloc (nInFields * sizeof (int));
      outFields = (void **) malloc (nOutFields * sizeof (void *));
      outFieldTypes = (int *) malloc (nOutFields * sizeof (int));
      origin = (CCTK_REAL *) malloc (nDims * sizeof (CCTK_REAL));

      /* Fill in the arrays from the variable argument list */
      va_start (indices, nOutFields);
      for (i = 0; i < nDims; i++)
      {
        coords [i] = va_arg (indices, void *);
      }
      for (i = 0; i < nDims; i++)
      {
        coordTypes [i] = va_arg (indices, int);
      }
      for (i = 0; i < nInFields; i++) 
      {
        index = va_arg (indices, int);
        timelevel = CCTK_NumTimeLevelsFromVarI (index);
        retcode = 0;
        if (timelevel < 0) 
	{
          CCTK_Warn (1, __LINE__,__FILE__,
		     "Cactus","Invalid variable index in CCTK_InterpGF()");
          retcode = -1;
        } 
	else 
	{
          if (--timelevel > 0)
	  {
             --timelevel;
	  }
          inFields [i] = CCTK_VarDataPtrI (GH, timelevel, index);
          inFieldTypes [i] = CCTK_VarTypeI (index);
        }
      }
      for (i = 0; i < nOutFields; i++)
      {
        outFields [i] = va_arg (indices, void *);
      }
      for (i = 0; i < nOutFields; i++)
      {
        outFieldTypes [i] = va_arg (indices, int);
      }
      va_end (indices);

      /* compute processor-local origin */
      for (i = 0; i < nDims; i++)
      {
        origin [i] = GH->cctk_origin_space [i] +
                     GH->cctk_lbnd [i] * GH->cctk_delta_space [i];
      }

      if (! retcode)
      {
        retcode = function(GH, nPoints, nDims, nInFields, nOutFields,
                           GH->cctk_lsh, coords, coordTypes,
                           origin, GH->cctk_delta_space,
                           inFields, inFieldTypes, outFields, outFieldTypes);
      }

      free (origin);
      free (outFieldTypes);
      free (outFields);
      free (inFieldTypes);
      free (inFields);
      free (coordTypes);
      free (coords);
      
    }
    else
      CCTK_Warn(3,__LINE__,__FILE__,"Cactus",
		"Interpolation operation not registered and cannot be called");
  }

  return retcode;

}

void CCTK_FCALL CCTK_FNAME(CCTK_InterpGF)
     (int *fortranreturn,
      cGH *GH,
      int *operation_handle,
      int *nPoints,
      int *nDims,
      int *nInFields,
      int *nOutFields,
      ...)
{ 
  va_list indices;
  int i, retcode;
  int index, timelevel;
  void **coords;
  int *coordTypes;
  void **inFields, **outFields;
  int *inFieldTypes, *outFieldTypes;
  CCTK_REAL *origin;
  int (*function)(REGISTER_INTERP_ARGLIST)=NULL; 

  retcode = -1;

  /* Get the pointer to the interpolation operator */

  if (*operation_handle < 0)

    CCTK_Warn(3,__LINE__,__FILE__,"Cactus",
	      "Invalid handle passed to CCTK_Interp");

  else
  {
    function = (int (*)(REGISTER_INTERP_ARGLIST))
      Util_GetHandledData(InterpOperators,*operation_handle);
    
    if (function)
    {
      
      coords = (void **) malloc (*nDims * sizeof (void *));
      coordTypes = (int *) malloc (*nDims * sizeof (int));
      inFields = (void **) malloc (*nInFields * sizeof (void *));
      inFieldTypes = (int *) malloc (*nInFields * sizeof (int));
      outFields = (void **) malloc (*nOutFields * sizeof (void *));
      outFieldTypes = (int *) malloc (*nOutFields * sizeof (int));
      origin = (CCTK_REAL *) malloc (*nDims * sizeof (CCTK_REAL));

      /* Fill in the arrays from the variable argument list */
      va_start (indices, nOutFields);
      for (i = 0; i < *nDims; i++)
        coords [i] = va_arg (indices, void *);
      for (i = 0; i < *nDims; i++)
        coordTypes [i] = *va_arg (indices, int *);
      for (i = 0; i < *nInFields; i++) {
        index = *va_arg (indices, int *);
        timelevel = CCTK_NumTimeLevelsFromVarI (index);
        retcode = 0;
        if (timelevel < 0) {
          CCTK_Warn (1,__LINE__,__FILE__,"Cactus", 
		     "Invalid variable index in CCTK_InterpGF()");
          retcode = -1;
        } else {
          if (--timelevel > 0)
             --timelevel;
          inFields [i] = CCTK_VarDataPtrI (GH, timelevel, index);
          inFieldTypes [i] = CCTK_VarTypeI (index);
        }
      }
      for (i = 0; i < *nOutFields; i++)
        outFields [i] = va_arg (indices, void *);
      for (i = 0; i < *nOutFields; i++)
        outFieldTypes [i] = *va_arg (indices, int *);
      va_end (indices);

      /* compute processor-local origin */
      for (i = 0; i < *nDims; i++)
        origin [i] = GH->cctk_origin_space [i] +
                     GH->cctk_lbnd [i] * GH->cctk_delta_space [i];

      if (! retcode)
        retcode = function(GH, *nPoints, *nDims, *nInFields, *nOutFields,
                           GH->cctk_lsh, coords, coordTypes,
                           origin, GH->cctk_delta_space,
                           inFields, inFieldTypes, outFields, outFieldTypes);
      
      free (origin);
      free (outFieldTypes);
      free (outFields);
      free (inFieldTypes);
      free (inFields);
      free (coordTypes);
      free (coords);

    }
    else
      CCTK_Warn(3,__LINE__,__FILE__,"Cactus",
		"Interpolation operation not registered and cannot be called");
  }

  *fortranreturn = retcode;
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
   @var        nDims
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

   @var        dims
   @vdesc      coordinate dimensions of the underlying grid
   @vtype      int [nDims]
   @vio        in
   @endvar 
   @var        coords
   @vdesc      coordinates of points to interpolate at
   @vtype      void * [nDims]
   @vio        in
   @endvar 
   @var        coordTypes
   @vdesc      types of passed coordinate fields
   @vtype      int [nDims]
   @vio        in
   @endvar 
   @var        origin
   @vdesc      origin of the underlying grid
   @vtype      <type of coordinate fields> [nDims]
   @vio        in
   @endvar 
   @var        delta
   @vdesc      deltas between 2 neighboring points of the underlying grid
   @vtype      <type of coordinate fields> [nDims]
   @vio        in
   @endvar 
   @var        inFields
   @vdesc      pointer to input fields to interpolate from
   @vtype      void * [nInfields]
   @vio        in
   @endvar 
   @var        inFieldTypes
   @vdesc      variable types of input fields to interpolate from
   @vtype      int [nInfields]
   @vio        in
   @endvar 
   @var        outFields
   @vdesc      pointer to fields to hold the interpolation results
   @vtype      void * [nOutfields]
   @vio        out
   @endvar 
   @var        outFieldTypes
   @vdesc      types of fields to hold the interpolation results
   @vtype      int [nOutfields]
   @vio        in
   @endvar 
@@*/

int CCTK_Interp (cGH *GH,
                 int operation_handle,
                 int nPoints,
                 int nDims,
                 int nInFields,
                 int nOutFields,
                 ...)
{
  va_list indices;
  int i, retcode;
  int *dims;
  void **coords;
  int *coordTypes;
  void *origin=NULL, *delta=NULL;
  void **inFields, **outFields;
  int *inFieldTypes, *outFieldTypes;
  int (*function)(REGISTER_INTERP_ARGLIST)=NULL; 

  retcode = -1;

  /* Get the pointer to the interpolation operator */
  if (operation_handle < 0)
  {
    CCTK_Warn(3,__LINE__,__FILE__,"Cactus",
	      "Invalid handle passed to CCTK_Interp");
  }
  else
  {
    function = (int (*)(REGISTER_INTERP_ARGLIST))
      Util_GetHandledData(InterpOperators,operation_handle);
    
    if (function)
    {

      dims = (int *) malloc (nDims * sizeof (int));
      coords = (void **) malloc (nDims * sizeof (void *));
      coordTypes = (int *) malloc (nDims * sizeof (int));
      inFields = (void **) malloc (nInFields * sizeof (void *));
      inFieldTypes = (int *) malloc (nInFields * sizeof (int));
      outFields = (void **) malloc (nOutFields * sizeof (void *));
      outFieldTypes = (int *) malloc (nOutFields * sizeof (int));
      retcode = 0;

      /* Fill in the arrays from the variable argument list */
      va_start (indices, nOutFields);
      for (i = 0; i < nDims; i++)
      {
        dims [i] = va_arg (indices, int);
      }
      for (i = 0; i < nDims; i++)
      {
        coords [i] = va_arg (indices, void *);
      }
      for (i = 0; i < nDims; i++)
      {
        coordTypes [i] = va_arg (indices, int);
      }

      for (i = 0; i < nDims; i++)
      {
        switch (coordTypes [i]) {
          case CCTK_VARIABLE_CHAR:
            if (i == 0)
              origin = malloc (nDims * sizeof (CCTK_CHAR));
            ((CCTK_CHAR *) origin) [i] = va_arg (indices, CCTK_CHAR);
            break;

          case CCTK_VARIABLE_INT:
            if (i == 0)
              origin = malloc (nDims * sizeof (CCTK_INT));
            ((CCTK_INT *) origin) [i] = va_arg (indices, CCTK_INT);
            break;

          case CCTK_VARIABLE_REAL:
            if (i == 0)
              origin = malloc (nDims * sizeof (CCTK_REAL));
            ((CCTK_REAL *) origin) [i] = va_arg (indices, CCTK_REAL);
            break;

          case CCTK_VARIABLE_COMPLEX:
            if (i == 0)
              origin = malloc (nDims * sizeof (CCTK_COMPLEX));
            ((CCTK_COMPLEX *) origin) [i] = va_arg (indices, CCTK_COMPLEX);
            break;

          default:
            CCTK_Warn(1,__LINE__,__FILE__,"Cactus", 
		      "Invalid variable type for coordinates");
            retcode = -1;
            break;
        }
      }

      for (i = 0; i < nDims; i++)
      {
        switch (coordTypes [i]) {
          case CCTK_VARIABLE_CHAR:
            if (i == 0)
              delta = malloc (nDims * sizeof (CCTK_CHAR));
            ((CCTK_CHAR *) delta) [i] = va_arg (indices, CCTK_CHAR);
            break;

          case CCTK_VARIABLE_INT:
            if (i == 0)
              delta = malloc (nDims * sizeof (CCTK_INT));
            ((CCTK_INT *) delta) [i] = va_arg (indices, CCTK_INT);
            break;

          case CCTK_VARIABLE_REAL:
            if (i == 0)
              delta = malloc (nDims * sizeof (CCTK_REAL));
            ((CCTK_REAL *) delta) [i] = va_arg (indices, CCTK_REAL);
            break;

          case CCTK_VARIABLE_COMPLEX:
            if (i == 0)
              delta = malloc (nDims * sizeof (CCTK_COMPLEX));
            ((CCTK_COMPLEX *) delta) [i] = va_arg (indices, CCTK_COMPLEX);
            break;

          default:
            CCTK_Warn (1,__LINE__,__FILE__,"Cactus", 
		       "Invalid variable type for coordinates");
            retcode = -1;
            break;
        }
      }

      for (i = 0; i < nInFields; i++)
      {
        inFields [i] = va_arg (indices, void *);
      }
      for (i = 0; i < nInFields; i++)
      {
        inFieldTypes [i] = va_arg (indices, int);
      }
      for (i = 0; i < nOutFields; i++)
      {
        outFields [i] = va_arg (indices, void *);
      }
      for (i = 0; i < nOutFields; i++)
      {
        outFieldTypes [i] = va_arg (indices, int);
      }
      va_end (indices);

      if (! retcode)
      {
        retcode = function(GH, nPoints, nDims, nInFields, nOutFields,
                           dims, coords, coordTypes, origin, delta,
                           inFields, inFieldTypes, outFields, outFieldTypes);
      }

      free (outFieldTypes);
      free (outFields);
      free (inFieldTypes);
      free (inFields);
      free (delta);
      free (origin);
      free (coordTypes);
      free (coords);
      free (dims);
      
    }
    else
    {
      CCTK_Warn(3,__LINE__,__FILE__,"Cactus",
		"Interpolation operation not registered and cannot be called");
    }
  }

  return retcode;

}

void CCTK_FCALL CCTK_FNAME(CCTK_Interp)
     (int *fortranreturn,
      cGH *GH,
      int *operation_handle,
      int *nPoints,
      int *nDims,
      int *nInFields,
      int *nOutFields,
      ...)
{ 
  va_list indices;
  int i, retcode;
  int *dims;
  void **coords;
  int *coordTypes;
  void *origin=NULL, *delta=NULL;
  void **inFields, **outFields;
  int *inFieldTypes, *outFieldTypes;
  int (*function)(REGISTER_INTERP_ARGLIST)=NULL; 

  retcode = -1;

  /* Get the pointer to the interpolation operator */

  if (*operation_handle < 0)

    CCTK_Warn(3,__LINE__,__FILE__,"Cactus",
	      "Invalid handle passed to CCTK_Interp");

  else
  {
    function = (int (*)(REGISTER_INTERP_ARGLIST))
      Util_GetHandledData(InterpOperators,*operation_handle);
    
    if (function)
    {
      
      dims = (int *) malloc (*nDims * sizeof (int));
      coords = (void **) malloc (*nDims * sizeof (void *));
      coordTypes = (int *) malloc (*nDims * sizeof (int));
      inFields = (void **) malloc (*nInFields * sizeof (void *));
      inFieldTypes = (int *) malloc (*nInFields * sizeof (int));
      outFields = (void **) malloc (*nOutFields * sizeof (void *));
      outFieldTypes = (int *) malloc (*nOutFields * sizeof (int));
      retcode = 0;

      /* Fill in the arrays from the variable argument list */
      va_start (indices, nOutFields);
      for (i = 0; i < *nDims; i++)
        dims [i] = *va_arg (indices, int *);
      for (i = 0; i < *nDims; i++)
        coords [i] = va_arg (indices, void *);
      for (i = 0; i < *nDims; i++)
        coordTypes [i] = *va_arg (indices, int *);

      for (i = 0; i < *nDims; i++)
        switch (coordTypes [i]) {
          case CCTK_VARIABLE_CHAR:
            if (i == 0)
              origin = malloc (*nDims * sizeof (CCTK_CHAR));
            ((CCTK_CHAR *) origin) [i] = *va_arg (indices, CCTK_CHAR *);
            break;

          case CCTK_VARIABLE_INT:
            if (i == 0)
              origin = malloc (*nDims * sizeof (CCTK_INT));
            ((CCTK_INT *) origin) [i] = *va_arg (indices, CCTK_INT *);
            break;

          case CCTK_VARIABLE_REAL:
            if (i == 0)
              origin = malloc (*nDims * sizeof (CCTK_REAL));
            ((CCTK_REAL *) origin) [i] = *va_arg (indices, CCTK_REAL *);
            break;

          case CCTK_VARIABLE_COMPLEX:
            if (i == 0)
              origin = malloc (*nDims * sizeof (CCTK_COMPLEX));
            ((CCTK_COMPLEX *) origin) [i] = *va_arg (indices, CCTK_COMPLEX *);
            break;

          default:
            CCTK_Warn (1,__LINE__,__FILE__,"Cactus",
		       "Invalid variable type for coordinates");
            retcode = -1;
            break;
        }

      for (i = 0; i < *nDims; i++)
        switch (coordTypes [i]) {
          case CCTK_VARIABLE_CHAR:
            if (i == 0)
              delta = malloc (*nDims * sizeof (CCTK_CHAR));
            ((CCTK_CHAR *) delta) [i] = *va_arg (indices, CCTK_CHAR *);
            break;

          case CCTK_VARIABLE_INT:
            if (i == 0)
              delta = malloc (*nDims * sizeof (CCTK_INT));
            ((CCTK_INT *) delta) [i] = *va_arg (indices, CCTK_INT *);
            break;

          case CCTK_VARIABLE_REAL:
            if (i == 0)
              delta = malloc (*nDims * sizeof (CCTK_REAL));
            ((CCTK_REAL *) delta) [i] = *va_arg (indices, CCTK_REAL *);
            break;

          case CCTK_VARIABLE_COMPLEX:
            if (i == 0)
              delta = malloc (*nDims * sizeof (CCTK_COMPLEX));
            ((CCTK_COMPLEX *) delta) [i] = *va_arg (indices, CCTK_COMPLEX *);
            break;

          default:
            CCTK_Warn (1,__LINE__,__FILE__,"Cactus", 
		       "Invalid variable type for coordinates");
            retcode = -1;
            break;
        }

      for (i = 0; i < *nInFields; i++)
        inFields [i] = va_arg (indices, void *);
      for (i = 0; i < *nInFields; i++)
        inFieldTypes [i] = *va_arg (indices, int *);
      for (i = 0; i < *nOutFields; i++)
        outFields [i] = va_arg (indices, void *);
      for (i = 0; i < *nOutFields; i++)
        outFieldTypes [i] = *va_arg (indices, int *);
      va_end (indices);

      if (! retcode)
        retcode = function(GH, *nPoints, *nDims, *nInFields, *nOutFields,
                           dims, coords, coordTypes, origin, delta,
                           inFields, inFieldTypes, outFields, outFieldTypes);
      
      free (outFieldTypes);
      free (outFields);
      free (inFieldTypes);
      free (inFields);
      free (delta);
      free (origin);
      free (coordTypes);
      free (coords);
      free (dims);
    }
    else
      CCTK_Warn(3,__LINE__,__FILE__,"Cactus",
		"Interpolation operation not registered and cannot be called");
  }

  *fortranreturn = retcode;
}
