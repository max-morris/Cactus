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
   @version   $Id$
 @@*/

/* #define DEBUG_INTERP 1 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "cctk_Constants.h"
#include "cctk_Flesh.h"
#include "cctk_FortranString.h"
#include "cctk_Groups.h"
#include "cctk_GroupsOnGH.h"
#include "StoreHandledData.h"
#include "cctk_Interp.h"
#include "cctk_WarnLevel.h"
#include "cctk_Coord.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(comm_Interp_c)


/* macro to read a list of items from a variable argument list into an array */
#define VARARGS_TO_ARRAY(array, type, modifier, count, varargs_list)          \
          {                                                                   \
            int i;                                                            \
                                                                              \
                                                                              \
            for (i = 0; i < (count); i++)                                     \
            {                                                                 \
              (array)[i] = modifier va_arg (varargs_list, type);              \
            }                                                                 \
          }

/* empty define to pass into the VARARGS_TO_ARRAY macro
   (some preprocessors need that) */
#define NOTHING


/* structure holding the routines for a registered interpolation operator */
typedef struct
{
  cInterpOperatorGV interp_operator_GV;
  cInterpOperatorLocal interp_operator_local;
} t_interp_operator;


/* prototypes of Fortran wrappers for interpolation routines */
void CCTK_FCALL CCTK_FNAME (CCTK_InterpHandle)
                           (int *handle,
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (CCTK_InterpGV)
                           (int *fortranreturn,
                            cGH *GH,
                            int *operator_handle,
                            int *coord_system_handle,
                            int *num_points,
                            int *num_in_array_indices,
                            int *num_out_arrays,
                            ...);
void CCTK_FCALL CCTK_FNAME (CCTK_InterpLocal)
                           (int *fortranreturn,
                            cGH *GH,
                            int *operator_handle,
                            int *num_points,
                            int *num_dims,
                            int *num_in_arrays,
                            int *num_out_arrays,
                            ...);

/* static data: interpolation operator database and counter for registered
                operators */
static cHandledData *interp_operators = NULL;
static int num_interp_operators = 0;


 /*@@
   @routine    CCTK_InterpRegisterOperatorGV
   @date       Mon 12 Feb 2001
   @author     Thomas Radke
   @desc
               Registers a routine as an interpolation operator for
               CCTK grid variables
   @enddesc

   @var        operator_GV
   @vdesc      interpolation operator
   @vtype      cctk_interp_gv_operator
   @vio        in
   @endvar
   @var        name
   @vdesc      name identifying the interpolation operator
   @vtype      const char *
   @vio        in
   @endvar

   @returntype int
   @returndesc
               the handle for the newly registered operator
               or negative otherwise
   @endreturndesc
@@*/
int CCTK_InterpRegisterOperatorGV (cInterpOperatorGV operator_GV,
                                   const char *name)
{
  int handle;
  t_interp_operator *operator;


  /* Check arguments */
  if (operator_GV == NULL)
  {
    CCTK_Warn (1, __LINE__, __FILE__, "Cactus",
               "NULL pointer passed as interpolation operator routine");
    handle = -1;
  }
  else
  {
    /* Check that the method hasn't already been registered */
    handle = Util_GetHandle (interp_operators, name, (void **) &operator);

    if (handle < 0)
    {
      /* Get a handle for it. */
      operator = (t_interp_operator *) malloc (sizeof (t_interp_operator));
      if (operator)
      {
        operator->interp_operator_GV = operator_GV;
        operator->interp_operator_local = NULL;
        handle = Util_NewHandle (&interp_operators, name, operator);

        /* Remember how many interpolation operators there are */
        num_interp_operators++;
      }
      else
      {
        CCTK_Warn (1, __LINE__, __FILE__, "Cactus",
                   "Couldn't allocate interpolation operator handle");
        handle = -1;
      }
    }
    else if (operator->interp_operator_GV == NULL)
    {
      operator->interp_operator_GV = operator_GV;
    }
    else
    {
      /* Interpolation operator with this name already exists. */
      CCTK_VWarn (1, __LINE__, __FILE__, "Cactus",
                  "Interpolation operator '%s' already exists",
                  name);
      handle = -1;
    }
  }

  return (handle);
}


 /*@@
   @routine    CCTK_InterpRegisterOperatorLocal
   @date       Mon 12 Feb 2001
   @author     Thomas Radke
   @desc
               Registers a routine as an interpolation operator for
               processor-local arrays
   @enddesc

   @var        operator_local
   @vdesc      interpolation operator
   @vtype      cctk_interp_local_operator
   @vio        in
   @endvar
   @var        name
   @vdesc      name identifying the interpolation operator
   @vtype      const char *
   @vio        in
   @endvar

   @returntype int
   @returndesc
               the handle for the newly registered operator
               or negative otherwise
   @endreturndesc
@@*/
int CCTK_InterpRegisterOperatorLocal (cInterpOperatorLocal operator_local,
                                      const char *name)
{
  int handle;
  t_interp_operator *operator;


  /* Check arguments */
  if (operator_local == NULL)
  {
    CCTK_Warn (1, __LINE__, __FILE__, "Cactus",
               "NULL pointer passed as interpolation operator routine");
    handle = -1;
  }
  else
  {
    /* Check that the method hasn't already been registered */
    handle = Util_GetHandle (interp_operators, name, (void **) &operator);

    if (handle < 0)
    {
      /* Get a handle for it. */
      operator = (t_interp_operator *) malloc (sizeof (t_interp_operator));
      if (operator)
      {
        operator->interp_operator_local = operator_local;
        operator->interp_operator_GV = NULL;
        handle = Util_NewHandle (&interp_operators, name, operator);

        /* Remember how many interpolation operators there are */
        num_interp_operators++;
      }
      else
      {
        CCTK_Warn (1, __LINE__, __FILE__, "Cactus",
                   "Couldn't allocate interpolation operator handle");
        handle = -1;
      }
    }
    else if (operator->interp_operator_local == NULL)
    {
      operator->interp_operator_local = operator_local;
    }
    else
    {
      /* Interpolation operator with this name already exists. */
      CCTK_VWarn (1, __LINE__, __FILE__, "Cactus",
                  "Interpolation operator '%s' already exists",
                  name);
      handle = -1;
    }
  }

  return (handle);
}


 /*@@
   @routine    CCTK_InterpHandle
   @date       July 07 1999
   @author     Thomas Radke
   @desc 
               Returns the handle of a given interpolation operator
   @enddesc 
   @var        interp
   @vdesc      String containing name of interpolation operator
   @vtype      const char *
   @vio        in
   @vcomment
   @endvar 

   @returntype int
   @returndesc
               the handle for the newly registered operator
               or negative otherwise
   @endreturndesc
@@*/

int CCTK_InterpHandle (const char *interp)
{
  int handle;


  handle = Util_GetHandle (interp_operators, interp, NULL);

#ifdef DEBUG_INTERP
  printf("In CCTK_InterpHandle\n");
  printf("--------------------------\n");
  printf("  Got handle %d for %s\n",handle,interp);
#endif

  if (handle < 0)
  {
    CCTK_VWarn (1, __LINE__, __FILE__, "Cactus",
                "No handle found for interpolation operator '%s'", interp);
  }

  return (handle);
}


void CCTK_FCALL CCTK_FNAME (CCTK_InterpHandle)
                           (int *handle, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (interp)
  *handle = CCTK_InterpHandle (interp);
  free (interp);
}


 /*@@
   @routine    CCTK_InterpGV
   @date       Sun 28 Jan 2001
   @author     Thomas Radke
   @desc
               The general CCTK interpolation routine for grid variables
               Just puts the arguments from the variable argument list
               into arrays and calls the appropriate interpolation operator.
   @enddesc
   @var        GH
   @vdesc      pointer to CCTK grid hierarchy
   @vtype      cGH *
   @vio        in
   @endvar
   @var        operator_handle
   @vdesc      handle for the interpolation operator
   @vtype      int
   @vio        in
   @endvar
   @var        coord_system_handle
   @vdesc      handle for the coordinate system
   @vtype      int
   @vio        in
   @endvar
   @var        num_points
   @vdesc      number of points to interpolate at
   @vtype      int
   @vio        in
   @endvar
   @var        num_in_array_indices
   @vdesc      number of passed input array indices
   @vtype      int
   @vio        in
   @endvar
   @var        num_out_arrays
   @vdesc      number of passed output arrays
   @vtype      int
   @vio        in
   @vcomment   end of fixed argument list
   @endvar

   @var        interp_coord_arrays
   @vdesc      list of coordinate arrays for points to interpolate at
   @vtype      void [dimensions of coordinate system][num_points]
   @vio        in
   @endvar
   @var        interp_coord_array_types
   @vdesc      types of passed coordinate arrays
   @vtype      int [dimensions of coordinate system]
   @vio        in
   @endvar
   @var        in_array_indices
   @vdesc      list of grid variables (given by their indices) to interpolate
   @vtype      int [num_in_array_indices]
   @vio        in
   @endvar
   @var        out_arrays
   @vdesc      list of output arrays which receive the interpolation results
   @vtype      void * [num_out_arrays]
   @vio        out
   @endvar
   @var        out_array_types
   @vdesc      types of output arrays which receive the interpolation results
   @vtype      int [num_out_arrays]
   @vio        in
   @endvar

   @returntype int
   @returndesc
               return value of the interpolation operator,
               -1 if invalid interpolation operator handle was passed in
               -2 if invalid coordinate system handle was passed in
   @endreturndesc
@@*/
int CCTK_InterpGV (cGH *GH,
                   int operator_handle,
                   int coord_system_handle,
                   int num_points,
                   int num_in_array_indices,
                   int num_out_arrays,
                   ...)
{
  va_list indices;
  int num_dims, retcode;
  const char *coord_system;
  int *in_array_indices;
  void **interp_coord_arrays, **out_arrays;
  int *interp_coord_array_types, *out_array_types;
  t_interp_operator *operator;


  /* Get the interpolation operator routine and the coordinate system name */
  operator = (t_interp_operator *) Util_GetHandledData (interp_operators,
                                                        operator_handle);
  coord_system = CCTK_CoordSystemName (coord_system_handle);

  if (operator == NULL)
  {
    CCTK_Warn (3, __LINE__, __FILE__, "Cactus",
               "Invalid interpolation operator handle passed to CCTK_InterpGV");
    retcode = -1;
  }
  else if (coord_system == NULL)
  {
    CCTK_Warn (3, __LINE__, __FILE__, "Cactus",
               "Invalid coordinate system handle passed to CCTK_InterpGV");
    retcode = -2;
  }
  else
  {
    num_dims = CCTK_CoordSystemDim (coord_system);

    interp_coord_arrays = (void **) malloc (num_dims * sizeof (void *));
    interp_coord_array_types = (int *) malloc (num_dims * sizeof (int));
    in_array_indices    = (int *) malloc (num_in_array_indices * sizeof (int));
    out_arrays          = (void **) malloc (num_out_arrays * sizeof (void *));
    out_array_types     = (int *) malloc (num_out_arrays * sizeof (int));

    /* Fill in the arrays from the variable argument list */
    va_start (indices, num_out_arrays);
    VARARGS_TO_ARRAY (interp_coord_arrays, void *, NOTHING, num_dims, indices);
    VARARGS_TO_ARRAY (interp_coord_array_types, int, NOTHING, num_dims,indices);
    VARARGS_TO_ARRAY (in_array_indices, int, NOTHING, num_in_array_indices,
                      indices);
    VARARGS_TO_ARRAY (out_arrays, void *, NOTHING, num_out_arrays, indices);
    VARARGS_TO_ARRAY (out_array_types, int, NOTHING, num_out_arrays, indices);
    va_end (indices);

    retcode = operator->interp_operator_GV (GH, coord_system, num_points,
                                            num_in_array_indices,num_out_arrays,
                                            interp_coord_arrays,
                                            interp_coord_array_types,
                                            in_array_indices,
                                            out_arrays, out_array_types);

    free (out_array_types);
    free (out_arrays);
    free (in_array_indices);
    free (interp_coord_array_types);
    free (interp_coord_arrays);
  }

  return (retcode);
}

void CCTK_FCALL CCTK_FNAME (CCTK_InterpGV)
                           (int *fortranreturn,
                            cGH *GH,
                            int *operator_handle,
                            int *coord_system_handle,
                            int *num_points,
                            int *num_in_array_indices,
                            int *num_out_arrays,
                            ...)
{
  va_list indices;
  int num_dims, retcode;
  int *in_array_indices;
  const char *coord_system;
  void **interp_coord_arrays, **out_arrays;
  int *interp_coord_array_types, *out_array_types;
  t_interp_operator *operator;


  /* Get the interpolation operator and the coordinate system name */
  operator = (t_interp_operator *) Util_GetHandledData (interp_operators,
                                                        *operator_handle);
  coord_system = CCTK_CoordSystemName (*coord_system_handle);

  if (operator == NULL)
  {
    CCTK_Warn (3, __LINE__, __FILE__, "Cactus",
               "Invalid interpolation operator handle passed to CCTK_InterpGV");
    retcode = -1;
  }
  else if (coord_system == NULL)
  {
    CCTK_Warn (3, __LINE__, __FILE__, "Cactus",
               "Invalid coordinate system handle passed to CCTK_InterpGV");
    retcode = -1;
  }
  else
  {
    num_dims = CCTK_CoordSystemDim (coord_system);
    interp_coord_arrays = (void **) malloc (num_dims * sizeof (void *));
    interp_coord_array_types = (int *) malloc (num_dims * sizeof (int));
    in_array_indices    = (int *) malloc (*num_in_array_indices * sizeof (int));
    out_arrays          = (void **) malloc (*num_out_arrays * sizeof (void *));
    out_array_types     = (int *) malloc (*num_out_arrays * sizeof (int));

    /* Fill in the arrays from the variable argument list */
    va_start (indices, num_out_arrays);
    VARARGS_TO_ARRAY (interp_coord_arrays, void *, NOTHING, num_dims, indices);
    VARARGS_TO_ARRAY (interp_coord_array_types, int *, *, num_dims, indices);
    VARARGS_TO_ARRAY (in_array_indices, int *, *,*num_in_array_indices,indices);
    VARARGS_TO_ARRAY (out_arrays, void *, NOTHING, *num_out_arrays, indices);
    VARARGS_TO_ARRAY (out_array_types, int *, *, *num_out_arrays, indices);
    va_end (indices);

    retcode = operator->interp_operator_GV (GH, coord_system, *num_points,
                                            *num_in_array_indices,
                                            *num_out_arrays,
                                            interp_coord_arrays,
                                            interp_coord_array_types,
                                            in_array_indices,
                                            out_arrays, out_array_types);

    free (out_array_types);
    free (out_arrays);
    free (in_array_indices);
    free (interp_coord_array_types);
    free (interp_coord_arrays);
  }

  *fortranreturn = retcode;
}


 /*@@
   @routine    CCTK_InterpLocal
   @date       Thu 01 Feb 2001
   @author     Thomas Radke
   @desc
               The general CCTK interpolation routine for a list of local
               arrays.
               Just puts the arguments from the variable argument list
               into arrays and calls the appropriate interpolation operator.
   @enddesc
   @var        GH
   @vdesc      pointer to CCTK grid hierarchy
   @vtype      cGH *
   @vio        in
   @endvar
   @var        operator_handle
   @vdesc      handle for the interpolation operator
   @vtype      int
   @vio        in
   @endvar
   @var        num_points
   @vdesc      number of points to interpolate at
   @vtype      int
   @vio        in
   @endvar
   @var        num_dims
   @vdesc      number of dimensions of the underlying grid
   @vtype      int
   @vio        in
   @endvar
   @var        num_in_arrays
   @vdesc      number of passed input arrays
   @vtype      int
   @vio        in
   @endvar
   @var        num_out_arrays
   @vdesc      number of passed output arrays
   @vtype      int
   @vio        in
   @vcomment   end of fixed argument list
   @endvar

   @var        dims
   @vdesc      coordinate dimensions of the underlying grid
   @vtype      int [num_dims]
   @vio        in
   @endvar
   @var        coord_arrays
   @vdesc      coordinates of the underlying grid
   @vtype      void * [num_dims]
   @vio        in
   @endvar
   @var        coord_array_types
   @vdesc      CCTK data types of passed coordinate arrays
   @vtype      int [num_dims]
   @vio        in
   @endvar
   @var        interp_coord_arrays
   @vdesc      coordinates of points to interpolate at
   @vtype      void * [num_dims]
   @vio        in
   @endvar
   @var        interp_coord_array_types
   @vdesc      CCTK data types of passed interpolation coordinate arrays
   @vtype      int [num_dims]
   @vio        in
   @endvar
   @var        in_arrays
   @vdesc      list of input arrays to interpolate from
   @vtype      void * [num_in_arrays]
   @vio        in
   @endvar
   @var        in_array_types
   @vdesc      CCTK data types of input arrays to interpolate from
   @vtype      int [num_in_arrays]
   @vio        in
   @endvar
   @var        out_arrays
   @vdesc      list of output arrays to hold the interpolation results
   @vtype      void * [num_out_arrays]
   @vio        out
   @endvar
   @var        out_array_types
   @vdesc      CCTK data types of output arrays
   @vtype      int [num_out_arrays]
   @vio        in
   @endvar

   @returntype int
   @returndesc
               return value of the interpolation operator,
               -1 if invalid interpolation operator handle was passed in
   @endreturndesc
@@*/
int CCTK_InterpLocal (cGH *GH,
                      int operator_handle,
                      int num_points,
                      int num_dims,
                      int num_in_arrays,
                      int num_out_arrays,
                      ...)
{
  va_list indices;
  int retcode;
  int *coord_dims;
  int *coord_array_types, *interp_coord_array_types;
  int *in_array_types, *out_array_types;
  void **coord_arrays, **interp_coord_arrays;
  void **in_arrays, **out_arrays;
  t_interp_operator *operator;


  /* Get the interpolation operator */
  operator = (t_interp_operator *) Util_GetHandledData (interp_operators,
                                                        operator_handle);
  if (operator == NULL)
  {
    CCTK_Warn (3, __LINE__, __FILE__, "Cactus",
               "Invalid interpolation operator handle passed to "
               "CCTK_InterpLocal");
    retcode = -1;
  }
  else
  {
    coord_dims = (int *) malloc (num_dims * sizeof (int));
    coord_arrays = (void **) malloc (num_dims * sizeof (void *));
    coord_array_types = (int *) malloc (num_dims * sizeof (int));
    interp_coord_arrays = (void **) malloc (num_dims * sizeof (void *));
    interp_coord_array_types = (int *) malloc (num_dims * sizeof (int));
    in_arrays = (void **) malloc (num_in_arrays * sizeof (void *));
    in_array_types = (int *) malloc (num_in_arrays * sizeof (int));
    out_arrays = (void **) malloc (num_out_arrays * sizeof (void *));
    out_array_types = (int *) malloc (num_out_arrays * sizeof (int));

    /* Fill in the arrays from the variable argument list */
    va_start (indices, num_out_arrays);
    VARARGS_TO_ARRAY (coord_dims, int, NOTHING, num_dims, indices);
    VARARGS_TO_ARRAY (coord_arrays, void *, NOTHING, num_dims, indices);
    VARARGS_TO_ARRAY (coord_array_types, int, NOTHING, num_dims, indices);
    VARARGS_TO_ARRAY (interp_coord_arrays, void *, NOTHING, num_dims, indices);
    VARARGS_TO_ARRAY (interp_coord_array_types, int, NOTHING, num_dims,indices);
    VARARGS_TO_ARRAY (in_arrays, void *, NOTHING, num_in_arrays, indices);
    VARARGS_TO_ARRAY (in_array_types, int, NOTHING, num_in_arrays, indices);
    VARARGS_TO_ARRAY (out_arrays, void *, NOTHING, num_out_arrays, indices);
    VARARGS_TO_ARRAY (out_array_types, int, NOTHING, num_out_arrays, indices);
    va_end (indices);

    retcode = operator->interp_operator_local (GH, num_points, num_dims,
                                               num_in_arrays, num_out_arrays,
                                               coord_dims,
                                               coord_arrays, coord_array_types,
                                               interp_coord_arrays,
                                               interp_coord_array_types,
                                               in_arrays, in_array_types,
                                               out_arrays, out_array_types);

    free (out_array_types);
    free (out_arrays);
    free (in_array_types);
    free (in_arrays);
    free (interp_coord_array_types);
    free (interp_coord_arrays);
    free (coord_array_types);
    free (coord_arrays);
    free (coord_dims);
  }

  return (retcode);
}


void CCTK_FCALL CCTK_FNAME (CCTK_InterpLocal)
                           (int *fortranreturn,
                            cGH *GH,
                            int *operator_handle,
                            int *num_points,
                            int *num_dims,
                            int *num_in_arrays,
                            int *num_out_arrays,
                            ...)
{
  va_list indices;
  int retcode;
  int *coord_dims;
  int *coord_array_types, *interp_coord_array_types;
  int *in_array_types, *out_array_types;
  void **coord_arrays, **interp_coord_arrays;
  void **in_arrays, **out_arrays;
  t_interp_operator *operator;


  /* Get the interpolation operator */
  operator = (t_interp_operator *) Util_GetHandledData (interp_operators,
                                                        *operator_handle);
  if (operator == NULL)
  {
    CCTK_Warn (3, __LINE__, __FILE__, "Cactus",
               "Invalid interpolation operator handle passed to "
               "CCTK_InterpLocal");
    retcode = -1;
  }
  else
  {
    coord_dims = (int *) malloc (*num_dims * sizeof (int));
    coord_arrays = (void **) malloc (*num_dims * sizeof (void *));
    coord_array_types = (int *) malloc (*num_dims * sizeof (int));
    interp_coord_arrays = (void **) malloc (*num_dims * sizeof (void *));
    interp_coord_array_types = (int *) malloc (*num_dims * sizeof (int));
    in_arrays = (void **) malloc (*num_in_arrays * sizeof (void *));
    in_array_types = (int *) malloc (*num_in_arrays * sizeof (int));
    out_arrays = (void **) malloc (*num_out_arrays * sizeof (void *));
    out_array_types = (int *) malloc (*num_out_arrays * sizeof (int));

    /* Fill in the arrays from the variable argument list */
    va_start (indices, num_out_arrays);
    VARARGS_TO_ARRAY (coord_dims, int *, *, *num_dims, indices);
    VARARGS_TO_ARRAY (coord_arrays, void *, NOTHING, *num_dims, indices);
    VARARGS_TO_ARRAY (coord_array_types, int *, *, *num_dims, indices);
    VARARGS_TO_ARRAY (interp_coord_arrays, void *, NOTHING, *num_dims, indices);
    VARARGS_TO_ARRAY (interp_coord_array_types, int *, *, *num_dims, indices);
    VARARGS_TO_ARRAY (in_arrays, void *, NOTHING, *num_in_arrays, indices);
    VARARGS_TO_ARRAY (in_array_types, int *, *, *num_in_arrays, indices);
    VARARGS_TO_ARRAY (out_arrays, void *, NOTHING, *num_out_arrays, indices);
    VARARGS_TO_ARRAY (out_array_types, int *, *, *num_out_arrays, indices);
    va_end (indices);

    retcode = operator->interp_operator_local (GH, *num_points, *num_dims,
                                               *num_in_arrays, *num_out_arrays,
                                               coord_dims,
                                               coord_arrays, coord_array_types,
                                               interp_coord_arrays,
                                               interp_coord_array_types,
                                               in_arrays, in_array_types,
                                               out_arrays, out_array_types);

    free (out_array_types);
    free (out_arrays);
    free (in_array_types);
    free (in_arrays);
    free (interp_coord_array_types);
    free (interp_coord_arrays);
    free (coord_array_types);
    free (coord_arrays);
    free (coord_dims);
  }

  *fortranreturn = retcode;
}


/****************************************************************************/
/********************* Depricated interpolation routines ********************/
/****************************************************************************/

 /*@@
   @routine    CCTK_InterpRegisterOperator
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
 
int CCTK_InterpRegisterOperator (int (*function)(INTERP_REGISTER_ARGLIST),
                                 const char *name)
{
  int handle;


  CCTK_Warn (1, __LINE__, __FILE__, "Cactus",
             "CCTK_InterpRegisterOperator: This routine is depricated and will "
             "be removed in Cactus version 4.0 b11. "
             "Please use the new interpolation interpface !");

  /* Check that the method hasn't already been registered */
  handle = Util_GetHandle(interp_operators, name, NULL);

  if(handle < 0)
  {
    /* Get a handle for it. */
    handle = Util_NewHandle(&interp_operators, name, (void *)function);
    
    /* Remember how many interpolation operators there are */
    num_interp_operators++;
   }
  else
  {
    /* Interpolation operator with this name already exists. */
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
              "Interpolation operator %s already exists",
               name);
    handle = -1;
  }

#ifdef DEBUG_INTERP
  printf("In CCTK_InterpRegisterOperator\n");
  printf("---------------------------------\n");
  printf("  Registering %s with handle %d\n",name,handle);
#endif
    
  return (handle);
}


 /*@@
   @routine    CCTK_InterpGF
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
  int vindex, timelevel;
  void **coords;
  int *coordTypes;
  int *inFieldTypes, *outFieldTypes;
  void **inFields, **outFields;
  CCTK_REAL *origin;
  int (*function)(INTERP_REGISTER_ARGLIST)=NULL; 

  CCTK_Warn (1, __LINE__, __FILE__, "Cactus",
             "CCTK_InterpGF: This routine is depricated and will "
             "be removed in Cactus version 4.0 b11. "
             "Please use the new interpolation interpface !");

  retcode = -1;

  /* Get the pointer to the interpolation operator */
  if (operation_handle < 0)
  {
    CCTK_Warn(3,__LINE__,__FILE__,
	      "Cactus","Invalid handle passed to CCTK_Interp");
  }
  else
  {
    function = (int (*)(INTERP_REGISTER_ARGLIST))
      Util_GetHandledData(interp_operators,operation_handle);
    
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
        vindex = va_arg (indices, int);
        timelevel = CCTK_NumTimeLevelsFromVarI (vindex);
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
          inFields [i] = CCTK_VarDataPtrI (GH, timelevel, vindex);
          inFieldTypes [i] = CCTK_VarTypeI (vindex);
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
  int vindex, timelevel;
  void **coords;
  int *coordTypes;
  void **inFields, **outFields;
  int *inFieldTypes, *outFieldTypes;
  CCTK_REAL *origin;
  int (*function)(INTERP_REGISTER_ARGLIST)=NULL; 

  CCTK_Warn (1, __LINE__, __FILE__, "Cactus",
             "CCTK_InterpGF: This routine is depricated and will "
             "be removed in Cactus version 4.0 b11. "
             "Please use the new interpolation interpface !");

  retcode = -1;

  /* Get the pointer to the interpolation operator */

  if (*operation_handle < 0)

    CCTK_Warn(3,__LINE__,__FILE__,"Cactus",
	      "Invalid handle passed to CCTK_Interp");

  else
  {
    function = (int (*)(INTERP_REGISTER_ARGLIST))
      Util_GetHandledData(interp_operators,*operation_handle);
    
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
        vindex = *va_arg (indices, int *);
        timelevel = CCTK_NumTimeLevelsFromVarI (vindex);
        retcode = 0;
        if (timelevel < 0) {
          CCTK_Warn (1,__LINE__,__FILE__,"Cactus", 
		     "Invalid variable index in CCTK_InterpGF()");
          retcode = -1;
        } else {
          if (--timelevel > 0)
             --timelevel;
          inFields [i] = CCTK_VarDataPtrI (GH, timelevel, vindex);
          inFieldTypes [i] = CCTK_VarTypeI (vindex);
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
   @var        operator_handle
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
                 int operator_handle,
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
  int (*function)(INTERP_REGISTER_ARGLIST)=NULL; 

  CCTK_Warn (1, __LINE__, __FILE__, "Cactus",
             "CCTK_Interp: This routine is depricated and will "
             "be removed in Cactus version 4.0 b11. "
             "Please use the new interpolation interpface !");

  retcode = -1;

  /* Get the pointer to the interpolation operator */
  if (operator_handle < 0)
  {
    CCTK_Warn(3,__LINE__,__FILE__,"Cactus",
              "Invalid handle passed to CCTK_Interp");
  }
  else
  {

    function = (int (*)(INTERP_REGISTER_ARGLIST))
      Util_GetHandledData(interp_operators,operator_handle);
    
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
            /* CCTK_CHAR will be promoted to int */
            if (i == 0)
              origin = malloc (nDims * sizeof (int));
            ((int *) origin) [i] = va_arg (indices, int);
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

      printf("retcode is %d\n",retcode);
      for (i = 0; i < nDims; i++)
      {
        switch (coordTypes [i]) {
          case CCTK_VARIABLE_CHAR:
            /* CCTK_CHAR will be promoted to int */
            if (i == 0)
              delta = malloc (nDims * sizeof (int));
            ((int *) delta) [i] = va_arg (indices, int);
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
      int *operator_handle,
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
  int (*function)(INTERP_REGISTER_ARGLIST)=NULL; 

  CCTK_Warn (1, __LINE__, __FILE__, "Cactus",
             "CCTK_Interp: This routine is depricated and will "
             "be removed in Cactus version 4.0 b11. "
             "Please use the new interpolation interpface !");

  retcode = -1;

  /* Get the pointer to the interpolation operator */

  if (*operator_handle < 0)

    CCTK_Warn(3,__LINE__,__FILE__,"Cactus",
              "Invalid handle passed to CCTK_Interp");

  else
  {
    function = (int (*)(INTERP_REGISTER_ARGLIST))
      Util_GetHandledData(interp_operators,*operator_handle);
    
    if (function)
    {
      
      dims = (int *) malloc (*nDims * sizeof (CCTK_INT));
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
            /* CCTK_CHAR will be promoted to int */
            if (i == 0)
              origin = malloc (*nDims * sizeof (int));
            ((int *) origin) [i] = *va_arg (indices, int *);
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
          /* CCTK_CHAR will be promoted to int */
          case CCTK_VARIABLE_CHAR:
            if (i == 0)
              delta = malloc (*nDims * sizeof (int));
            ((int *) delta) [i] = *va_arg (indices, int *);
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

