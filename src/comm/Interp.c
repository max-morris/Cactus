 /*@@
   @file      Interp.c
   @date      July 07 1999
   @author    Thomas Radke
   @desc
              Registration and invocation routines for interpolation operators.
   @enddesc

   @history
   @date      July 07 1999
   @author    Thomas Radke
   @hdesc     Just copied from Reduction.c

   @date      Wed Feb 20 17:52:55 CET 2002
   @author    Jonathan Thornburg <jthorn@aei.mpg.de>
   @hdesc     * revise registration routines to pull out common code
                into macro and new static function
              * permute arguments to the CCTKi_* registration functions
              * add CCTK_InterpRegisterOpLocalUniform() to register
                new-API interpolators
   @endhistory

   @date      Tue May 11 13:01:51 CEST 2004
   @author    Jonathan Thornburg <jthorn@aei.mpg.de>
   @hdesc     change  CCTK_InterpLocal()  and  CCTK_InterpGV()
              to give level 1 warnings that these APIs are obsolescent
              and will be phased out soon (and to point to their
              replacement APIs ).
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
#include "cctk_Comm.h"
#include "cctk_WarnLevel.h"
#include "cctk_Coord.h"
#include "cctk_ActiveThorns.h"
#include "util_ErrorCodes.h"

static const char *rcsid = "$Header$";

CCTK_FILEVERSION(comm_Interp_c);


/******************************************************************************
 *************************    External Routines   *****************************
 ******************************************************************************/
/* prototypes for external C routines are declared in header cctk_Interp.h
   here only follow the fortran wrapper prototypes */
void CCTK_FCALL CCTK_FNAME (CCTK_InterpHandle)
                           (int *handle,
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (CCTK_InterpGV)
                           (int *fortranreturn,
                            cGH **GH,
                            const int *operator_handle,
                            const int *coord_system_handle,
                            const int *num_points,
                            const int *num_in_array_indices,
                            const int *num_out_arrays,
                            ...);
void CCTK_FCALL CCTK_FNAME (CCTK_InterpLocal)
                           (int *fortranreturn,
                            cGH **GH,
                            const int *operator_handle,
                            const int *num_points,
                            const int *num_dims,
                            const int *num_in_arrays,
                            const int *num_out_arrays,
                            ...);
void CCTK_FCALL CCTK_FNAME (CCTK_InterpGridArrays)
                           (int *ierror,
                            const cGH **GH,
                            const int *N_dims,
                            const int *local_interp_handle,
                            const int *param_table_handle,
                            const int *coord_system_handle,
                            const int *N_interp_points,
                              const int *interp_coords_type,
                              const void *const interp_coords[],
                            const int *N_input_arrays,
                              const CCTK_INT input_array_indices[],
                            const int *N_output_arrays,
                              const CCTK_INT output_array_types[],
                              void *const output_arrays[]);

/******************************************************************************
 *************************    Internal Data Structures ************************
 ******************************************************************************/

/* structure holding the routines for a registered interpolation operator */
struct  interp_info
        {
        const char *thorn_name;
        const char *implementation_name;
        const char *operator_name;
        cInterpOperatorGV     interp_operator_GV;
        cInterpOperatorLocal  interp_operator_local;
        cInterpOpLocalUniform interp_op_local_uniform;
        };

/******************************************************************************
 *************************    Static Variables   ******************************
 ******************************************************************************/

/* static data: interpolation operator database and counter for registered
                operators */
static cHandledData *interp_operators = NULL;
static int num_interp_operators = 0;

/******************************************************************************
 ****************** Prototypes for Functions Local to this File ***************
 ******************************************************************************/

static
  int GetOrSetupInterpInfo(const char *thorn_name,
                           const char *operator_name,
                           struct interp_info **pp_interp_info);

/******************************************************************************
 *************************    Macro Definitions   *****************************
 ******************************************************************************/
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



/******************************************************************************
 ************************* Registration Functions *****************************
 ******************************************************************************/

 /*@@
   @routine    CCTK_NumInterpOperators
   @date       Mon Oct 22 2001
   @author     Gabrielle Allen
   @desc
               The number of interp operators registered
   @enddesc
   @returntype int
   @returndesc
               number of interpolation operators
   @endreturndesc
@@*/
int CCTK_NumInterpOperators()
{
  return num_interp_operators;
}

/******************************************************************************/

 /*@@
   @routine    CCTK_InterpOperatorImplementation
   @date       Mon Oct 22 2001
   @author     Gabrielle Allen
   @desc
   Provide the implementation which provides an interpolation operator
   @enddesc
   @returntype int
   @returndesc
               Implementation which supplied the interpolation operator
   @endreturndesc
@@*/
const char *CCTK_InterpOperatorImplementation(int handle)
{
  struct interp_info *operator;
  const char *imp=NULL;

  operator = (struct interp_info *)
    Util_GetHandledData (interp_operators, handle);

  if (operator)
  {
    imp = operator->implementation_name;
  }

  return imp;
}

/******************************************************************************/

/*@@
  @routine    CCTK_INTERP_REGISTER_FN_BODY (macro)
  @date       Fri Feb 22 15:45:42 CET 2002
  @author     Jonathan Thornburg <jthorn@aei.mpg.de>
  @desc       This macro expands into the *body* (not including the
              opening/closing { }) of a function which registers a
              user-specified function as an interpolation operator.
  @enddesc

  @var        operator_ptr_fnarg
  @vdesc      the "function pointer pointing to the interpolation operator"
              argument to the registration function
  @vtype      cInterpOperatorGV or cInterpOperatorLocal
              or cInterpOpLocalUniform or ... as appropriate
  @vio        in
  @endvar

  @var        operator_name_fnarg
  @vdesc      the "character-string name identifying the interpolation
              operator" argument to the registration function
  @vtype      const char *
  @vio        in
  @endvar

  @var        thorn_name_fnarg
  @vdesc      the "character-string name identifying which thorn provides
              the operator being registered" argument to the registration
              function
  @vtype      const char *
  @vio        in
  @endvar

  @var        function_name_string
  @vdesc      the name of the registration function, as a literal C
              string (encased in "double quotes")
  @vtype      const char *
  @vio        in
  @endvar

  @var        struct_field_name
  @vdesc      the name of the field in  struct interp_info  which holds
              the function pointer pointing to the interpolation operator
  @vtype      const char *
  @vio        in
  @endvar

  @returntype int
  @returndesc the registration function returns...
              the handle for the newly registered operator, or<p>
              -1 NULL pointer was passed as interpolation operator routine<p>
              -2 failed to allocate memory<p>
              -3 interpolation operator by given name already exists
  @endreturndesc

  @history
  @date       Mon 12 Feb 2001
  @author     Thomas Radke
  @hdesc      Original version

  @date       Thu Feb 21 16:03:25 CET 2002
  @author     Jonathan Thornburg <jthorn@aei.mpg.de>
  @hdesc      * move common logic in all interpolator-registration
                functions into new  GetOrSetupInterpInfo()  function
              * convert remaining boilerplate which differs from one
                registration function to another, into this macro
  @endhistory
  @@*/
#define CCTK_INTERP_REGISTER_FN_BODY(operator_ptr_fnarg,                \
                                     operator_name_fnarg,               \
                                     thorn_name_fnarg,                  \
                                     function_name_string,              \
                                     struct_field_name)                 \
  /* begin function body */                                             \
  int handle;                                                           \
  struct interp_info *p_interp_info;                                    \
                                                                        \
  if (operator_ptr_fnarg == NULL)                                       \
  {                                                                     \
    CCTK_VWarn(0, __LINE__, __FILE__, "Cactus",                         \
               function_name_string ":\n"                               \
               "   (called from thorn %s)\n"                            \
               "   NULL function pointer passed\n"                      \
               "   for interpolation operator \"%s\"!"                  \
               ,                                                        \
               thorn_name_fnarg,                                        \
               operator_name_fnarg);                                    \
    return -1;                                  /*** ERROR RETURN ***/  \
  }                                                                     \
                                                                        \
  handle = GetOrSetupInterpInfo(thorn_name_fnarg,                       \
                                operator_name_fnarg,                    \
                                &p_interp_info);                        \
  if (handle < 0)                                                       \
  {                                                                     \
    return handle;                              /*** ERROR RETURN ***/  \
  }                                                                     \
                                                                        \
  /* check that the operator isn't already registered */                \
  if (p_interp_info->struct_field_name != NULL)                         \
  {                                                                     \
    CCTK_VWarn(0, __LINE__, __FILE__, "Cactus",                         \
               function_name_string ":\n"                               \
               "   (called from thorn %s)\n"                            \
               "   Ignoring attempt to register operator \"%s\"\n"      \
               "   because it has already been registered by thorn %s"  \
               ,                                                        \
               thorn_name_fnarg,                                        \
               operator_name_fnarg,                                     \
               p_interp_info->thorn_name);                              \
    return -3;                                  /*** ERROR RETURN ***/  \
  }                                                                     \
                                                                        \
  /* now, (finally!) the actual registration */                         \
  p_interp_info->struct_field_name = operator_ptr_fnarg;                \
                                                                        \
  return handle;                                /*** NORMAL RETURN ***/ \
  /* end function body */                                       /* end macro */

/******************************************************************************/

/*@@
  @routine    CCTKi_InterpRegisterOperatorGV
  @routine    CCTKi_InterpRegisterOperatorLocal
  @routine    CCTK_InterpRegisterOpLocalUniform
  @date       Thu Feb 21 16:02:05 CET 2002
  @author     Jonathan Thornburg <jthorn@aei.mpg.de>
  @desc       Each of these functions registers a user-specified
              function as the corresponding type of interpolation
              operator.  The function body is produced by the
              CCTK_INTERP_REGISTER_FN_BODY() macro defined above.
  @enddesc

  @var        operator_ptr
  @vdesc      function pointer pointing to the interpolation operator
  @vtype      cInterpOperatorLocal or cInterpOpLocalUniform as appropriate
  @vio        in
  @endvar

  @var        operator_name
  @vdesc      character-string name identifying the interpolation operator
  @vtype      const char *
  @vio        in
  @endvar

  @var        thorn_name
  @vdesc      character-string name identifying which thorn provides
              the operator being registered
  @vtype      const char *
  @vio        in
  @endvar

  @returntype int
  @returndesc
              the handle for the newly registered operator, or<p>
              -1 NULL pointer was passed as interpolation operator routine<p>
              -2 failed to allocate memory<p>
              -3 interpolation operator by given name already exists
  @endreturndesc

  @history
  @date       Mon 12 Feb 2001
  @author     Thomas Radke
  @hdesc      Original version

  @date       Thu Feb 21 16:03:25 CET 2002
  @author     Jonathan Thornburg <jthorn@aei.mpg.de>
  @hdesc      * move common logic in all interpolator-registration
                functions into new  GetOrSetupInterpInfo()  function
              * convert remaining boilerplate which differs from one
                registration function to another, into this macro
  @endhistory
  @@*/

/**************************************/

int CCTKi_InterpRegisterOperatorGV(cInterpOperatorGV operator_ptr,
                                   const char *operator_name,
                                   const char *thorn_name)
{
/* One line for cpp.pl (Beta 13) */
CCTK_INTERP_REGISTER_FN_BODY(operator_ptr,operator_name,thorn_name,"CCTKi_InterpRegisterOperatorGV",interp_operator_GV) /* no semicolon here! */
}

/**************************************/

int CCTKi_InterpRegisterOperatorLocal(cInterpOperatorLocal operator_ptr,
                                      const char *operator_name,
                                      const char *thorn_name)
{
/* One line for cpp.pl (beta 13) */
CCTK_INTERP_REGISTER_FN_BODY(operator_ptr,operator_name,thorn_name,"CCTKi_InterRegisterOperatorLocal",interp_operator_local) /* no semicolon here */
}

/**************************************/

int CCTK_InterpRegisterOpLocalUniform(cInterpOpLocalUniform operator_ptr,
                                      const char *operator_name,
                                      const char *thorn_name)
{
CCTK_INTERP_REGISTER_FN_BODY(operator_ptr,operator_name,thorn_name,"CCTK_InterpRegisterOpLocalUniform",interp_op_local_uniform) /* no semicolon here! */
}

/******************************************************************************/

/*@@
  @routine      GetOrSetupInterpInfo
  @date         Thu Feb 21 14:41:35 CET 2002
  @author       Jonathan Thornburg <jthorn@aei.mpg.de>
  @desc         This is an internal worker routine used as part of the
                process of registering an interpolation operator.  It
                gets the  interpolator handle and the  struct interp_info
                in which the operator information will be stored.

                If some interpolation operator is already registered
                under the specified operator name, we use the existing
                 struct interp_info .  Otherwise, we allocate a new
                 struct interp_info  and set it up (thorn, implementation,
                and operator names assigned from this function's arguments,
                all operator pointers set to NULL), then use it.
  @enddesc

  @var          thorn_name
  @vdesc        the character-string name of the thorn doing the registration
  @vtype        const char *thorn_name
  @vio          in
  @endvar

  @var          operator_name
  @vdesc        name identifying the interpolation operator
  @vtype        const char *operator_name
  @vio          in
  @endvar

  @var          pp_interp_info
  @vdesc        pointer to a  struct interp_info *  pointer which this
                function will set to point to the appropriate
                 struct interp_info
  @vtype        struct interp_info **pp_interp_info
  @vio          out
  @endvar

  @returntype   int
  @returndesc   the handle for the interpolation operator, or<p>
                -2 failed to allocate memory (in this case
                   *pp_interp_info will be set to NULL)<p>
  @endreturndesc
  @@*/
static
  int GetOrSetupInterpInfo(const char *thorn_name,
                           const char *operator_name,
                           struct interp_info **pp_interp_info)
{
  /* has some operator already been registered under this operator name? */
  int handle = Util_GetHandle(interp_operators,
                              operator_name,
                              (void **) pp_interp_info);
  /* yes ==> use the existing handle */
  if (handle >= 0)
  {
    return handle;                                      /*** NORMAL RETURN ***/
  }

  /* no ==> set up a new  struct interp_info  for the registration */
  *pp_interp_info = (struct interp_info *) malloc(sizeof(struct interp_info));
  if (*pp_interp_info == NULL)
  {
    return -2;                                          /*** ERROR RETURN ***/
  }

  (*pp_interp_info)->thorn_name = thorn_name;
  (*pp_interp_info)->implementation_name = CCTK_ThornImplementation(thorn_name);
  (*pp_interp_info)->operator_name = operator_name;
  (*pp_interp_info)->interp_operator_GV      = NULL;
  (*pp_interp_info)->interp_operator_local   = NULL;
  (*pp_interp_info)->interp_op_local_uniform = NULL;

  handle = Util_NewHandle(&interp_operators,
                          operator_name,
                          (void *) *pp_interp_info);
  num_interp_operators++;

  return handle;                                        /*** NORMAL RETURN ***/
}

/******************************************************************************
 ************* User Functions to Get Interpolator Handle/Name/etc *************
 ******************************************************************************/

 /*@@
   @routine    CCTK_InterpHandle
   @date       July 07 1999
   @author     Thomas Radke
   @desc
               Returns the handle of a given interpolation operator
   @enddesc
   @var        name
   @vdesc      String containing name of interpolation operator
   @vtype      const char *
   @vio        in
   @vcomment
   @endvar

   @returntype int
   @returndesc
               the handle for the operator registered by this name
               or negative otherwise
   @endreturndesc
@@*/
int CCTK_InterpHandle (const char *name)
{
  int handle;


  handle = Util_GetHandle (interp_operators, name, NULL);

#ifdef DEBUG_INTERP
  printf("In CCTK_InterpHandle\n");
  printf("--------------------------\n");
  printf("  Got handle %d for %s\n",handle,name);
#endif

  if (handle < 0)
  {
    CCTK_VWarn (1, __LINE__, __FILE__, "Cactus",
                "No handle found for interpolation operator '%s'", name);
  }

  return (handle);
}

/******************************************************************************/

void CCTK_FCALL CCTK_FNAME (CCTK_InterpHandle)
                           (int *handle, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (name)
  *handle = CCTK_InterpHandle (name);
  free (name);
}

/******************************************************************************/

 /*@@
   @routine    CCTK_InterpOperator
   @date       December 27 2001
   @author     Gabrielle Allen
   @desc
               Returns the name of a interpolation operator
   @enddesc
   @var        handle
   @vdesc      Handle for interpolation operator
   @vtype      int
   @vio        in
   @vcomment
   @endvar

   @returntype const char *
   @returndesc
   The name of the interpolation operator, or NULL if the handle
   is invalid
   @endreturndesc
@@*/
const char *CCTK_InterpOperator (int handle)
{
  const char *name=NULL;
  struct interp_info *operator;

  if (handle < 0)
  {
    CCTK_VWarn (6, __LINE__, __FILE__, "Cactus",
                "CCTK_InterpHandle: Handle %d invalid", handle);
  }
  else
  {
    operator = (struct interp_info *) Util_GetHandledData (interp_operators,
                                                          handle);
    if (operator)
    {
      name = operator->operator_name;
    }
    else
    {
      CCTK_VWarn (6, __LINE__, __FILE__, "Cactus",
                  "CCTK_InterpHandle: Handle %d invalid", handle);
    }
  }

  return name;
}


/******************************************************************************
 ****************** User Functions to Do Interpolation ************************
 ******************************************************************************/

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
  int *in_array_indices, *interp_coord_array_types, *out_array_types;
  const void **interp_coord_arrays;
  void **out_arrays;
  struct interp_info *operator;


  /* warn the user that this API is obselete and will disappear soon! */
    CCTK_VWarn(1, __LINE__, __FILE__, "Cactus",
"\n"
"***\n"
"*** CCTK_InterpGV()  is obsolescent and will be phased out soon!\n"
"*** You should convert your code to use CCTK_InterpGridArrays() instead.\n"
"***\n");

  /* Get the interpolation operator routine and the coordinate system name */
  operator = (struct interp_info *) Util_GetHandledData (interp_operators,
                                                        operator_handle);
  coord_system = CCTK_CoordSystemName (coord_system_handle);

  if ((operator == NULL) || (operator->interp_operator_GV == NULL))
  {
    CCTK_VWarn(0, __LINE__, __FILE__, "Cactus",
"\n"
"   CCTK_InterpGV(): no interpolation operator is registered\n"
"                    under the handle %d\n"
"                    (did you activate PUGHInterp or some other thorn\n"
"                     providing this interpolation operator?)"
               ,
               operator_handle);
    retcode = -1;
  }
  else if (coord_system == NULL)
  {
    CCTK_Warn (3, __LINE__, __FILE__, "Cactus",
               "CCTK_InterpGV: Invalid coordinate system handle passed to CCTK_InterpGV");
    retcode = -2;
  }
  else
  {
    num_dims = CCTK_CoordSystemDim (coord_system);

    interp_coord_arrays = (const void **) malloc (num_dims * sizeof (void *));
    interp_coord_array_types = (int *) malloc (num_dims * sizeof (int));
    in_array_indices    = (int *) malloc (num_in_array_indices * sizeof (int));
    out_arrays          = (void **) malloc (num_out_arrays * sizeof (void *));
    out_array_types     = (int *) malloc (num_out_arrays * sizeof (int));

    /* Fill in the arrays from the variable argument list */
    va_start (indices, num_out_arrays);
    VARARGS_TO_ARRAY (interp_coord_arrays, void *, NOTHING, num_dims, indices);
    VARARGS_TO_ARRAY (interp_coord_array_types, int, NOTHING, num_dims,indices);
    VARARGS_TO_ARRAY (in_array_indices, int, NOTHING, num_in_array_indices,indices);
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

/******************************************************************************/

void CCTK_FCALL CCTK_FNAME (CCTK_InterpGV)
                           (int *fortranreturn,
                            cGH **GH,
                            const int *operator_handle,
                            const int *coord_system_handle,
                            const int *num_points,
                            const int *num_in_array_indices,
                            const int *num_out_arrays,
                            ...)
{
  va_list indices;
  int num_dims, retcode;
  const char *coord_system;
  int *in_array_indices, *interp_coord_array_types, *out_array_types;
  const void **interp_coord_arrays;
  void **out_arrays;
  struct interp_info *operator;


  /* warn the user that this API is obselete and will disappear soon! */
    CCTK_VWarn(1, __LINE__, __FILE__, "Cactus",
"\n"
"***\n"
"*** CCTK_InterpGV()  is obsolescent and will be phased out soon!\n"
"*** You should convert your code to use CCTK_InterpGridArrays() instead.\n"
"***\n");

  /* Get the interpolation operator and the coordinate system name */
  operator = (struct interp_info *) Util_GetHandledData (interp_operators,
                                                        *operator_handle);
  coord_system = CCTK_CoordSystemName (*coord_system_handle);

  if ((operator == NULL) || (operator->interp_operator_GV == NULL))
  {
    CCTK_VWarn(0, __LINE__, __FILE__, "Cactus",
"\n"
"   CCTK_InterpGV(): no interpolation operator is registered\n"
"                    under the handle %d\n"
"                    (did you activate PUGHInterp or some other thorn\n"
"                     providing this interpolation operator?)"
               ,
               *operator_handle);
    retcode = -1;
  }
  else if (coord_system == NULL)
  {
    CCTK_Warn (0, __LINE__, __FILE__, "Cactus",
"CCTK_InterpGV: Invalid coordinate system handle passed to CCTK_InterpGV");
    retcode = -1;
  }
  else
  {
    num_dims = CCTK_CoordSystemDim (coord_system);
    interp_coord_arrays = (const void **) malloc (num_dims * sizeof (void *));
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

    retcode = operator->interp_operator_GV (*GH, coord_system, *num_points,
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

/******************************************************************************/

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
  const void **coord_arrays, **interp_coord_arrays, **in_arrays;
  void **out_arrays;
  struct interp_info *operator;

  /* warn the user that this API is obselete and will disappear soon! */
    CCTK_VWarn(1, __LINE__, __FILE__, "Cactus",
"\n"
"***\n"
"*** CCTK_InterpLocal()  is obsolescent and will be phased out soon!\n"
"*** You should convert your code to use CCTK_InterpLocalUniform() instead.\n"
"***\n");

  /* Get the interpolation operator */
  operator = (struct interp_info *) Util_GetHandledData (interp_operators,
                                                        operator_handle);
  if ((operator == NULL) || (operator->interp_operator_local == NULL))
  {
    CCTK_VWarn(0, __LINE__, __FILE__, "Cactus",
"\n"
"   CCTK_InterpLocal(): no interpolation operator is registered\n"
"                       under the handle %d\n"
"                       (did you activate LocalInterp or some other thorn\n"
"                        providing this interpolation operator?)"
               ,
               operator_handle);
    retcode = -1;
  }
  else
  {
    coord_dims = (int *) malloc (num_dims * sizeof (int));
    coord_arrays = (const void **) malloc (num_dims * sizeof (void *));
    coord_array_types = (int *) malloc (num_dims * sizeof (int));
    interp_coord_arrays = (const void **) malloc (num_dims * sizeof (void *));
    interp_coord_array_types = (int *) malloc (num_dims * sizeof (int));
    in_arrays = (const void **) malloc (num_in_arrays * sizeof (void *));
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

/******************************************************************************/

void CCTK_FCALL CCTK_FNAME (CCTK_InterpLocal)
                           (int *fortranreturn,
                            cGH **GH,
                            const int *operator_handle,
                            const int *num_points,
                            const int *num_dims,
                            const int *num_in_arrays,
                            const int *num_out_arrays,
                            ...)
{
  va_list indices;
  int retcode;
  int *coord_dims;
  int *coord_array_types, *interp_coord_array_types;
  int *in_array_types, *out_array_types;
  const void **coord_arrays, **interp_coord_arrays, **in_arrays;
  void **out_arrays;
  struct interp_info *operator;


  /* warn the user that this API is obselete and will disappear soon! */
    CCTK_VWarn(1, __LINE__, __FILE__, "Cactus",
"\n"
"***\n"
"*** CCTK_InterpLocal()  is obsolescent and will be phased out soon!\n"
"*** You should convert your code to use CCTK_InterpLocalUniform() instead.\n"
"***\n");

  /* Get the interpolation operator */
  operator = (struct interp_info *) Util_GetHandledData (interp_operators,
                                                        *operator_handle);
  if ((operator == NULL) || (operator->interp_operator_local == NULL))
  {
    CCTK_VWarn(0, __LINE__, __FILE__, "Cactus",
"\n"
"   CCTK_InterpLocal(): no interpolation operator is registered\n"
"                       under the handle %d\n"
"                       (did you activate LocalInterp or some other thorn\n"
"                        providing this interpolation operator?)"
               ,
               *operator_handle);
    retcode = -1;
  }
  else
  {
    coord_dims = (int *) malloc (*num_dims * sizeof (int));
    coord_arrays = (const void **) malloc (*num_dims * sizeof (void *));
    coord_array_types = (int *) malloc (*num_dims * sizeof (int));
    interp_coord_arrays = (const void **) malloc (*num_dims * sizeof (void *));
    interp_coord_array_types = (int *) malloc (*num_dims * sizeof (int));
    in_arrays = (const void **) malloc (*num_in_arrays * sizeof (void *));
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

    retcode = operator->interp_operator_local (*GH, *num_points, *num_dims,
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


 /*@@
   @routine    CCTK_InterpGridArrays
   @date       Mon 16 Dec 2002
   @author     Thomas Radke
   @desc
               The general CCTK interpolation routine for grid variables

               Here only the fortran wrapper is defined which calls the
               the C routine CCTK_InterpGridArrays(). This is an overloadable
               routine defined in src/comm/OverloadComm.c.
   @enddesc
   @var        GH
   @vdesc      pointer to CCTK grid hierarchy
   @vtype      const cGH *
   @vio        in
   @endvar
   @var        N_dims
   @vdesc      (reference to) number of dimensions for the interpolation
   @vtype      const int *
   @vio        in
   @endvar
   @endvar
   @var        local_interp_handle
   @vdesc      (reference to) the handle which specifies the local interpolator
               to use
   @vtype      const int *
   @vio        in
   @endvar
   @var        param_table_handle
   @vdesc      (reference to) the parameter table handle for passing optional
               parameters to the interpolator routine
   @vtype      const int *
   @vio        in
   @endvar
   @var        coord_system_handle
   @vdesc      (reference to) the handle for the underlying coordinate system
   @vtype      const int *
   @vio        in
   @endvar
   @var        N_interp_points
   @vdesc      (reference to) the number of points to interpolate at
   @vtype      const int *
   @vio        in
   @endvar
   @var        interp_coords_type
   @vdesc      (reference to) the CCTK datatype of the coordinate arrays as
               passed via <interp_coords> (common datatype for all arrays)
   @vtype      const int *
   @vio        in
   @endvar
   @var        interp_coords
   @vdesc      list of <N_dims> arrays with coordinate for <N_interp_points>
               points to interpolate at
   @vtype      const void *const []
   @vio        in
   @endvar
   @var        N_input_arrays
   @vdesc      (reference to) the number of input arrays
   @vtype      const int *
   @vio        in
   @endvar
   @var        input_array_indices
   @vdesc      list of <N_input_arrays> grid variables (given by their indices)
               to interpolate
   @vtype      const CCTK_INT []
   @vio        in
   @endvar
   @var        N_output_arrays
   @vdesc      (reference to) the number of output arrays
   @vtype      const int *
   @vio        in
   @endvar
   @var        out_array_types
   @vdesc      list of <N_output_arrays> requested CCTK datatypes for the
               output arrays
   @vtype      const CCTK_INT []
   @vio        in
   @endvar
   @var        output_arrays
   @vdesc      list of <N_output_arrays> output arrays (given by their pointers)
               which receive the interpolation results
   @vtype      void *const []
   @vio        out
   @endvar

   @returntype int
   @returndesc
               return code from routine which overloades CCTK_InterpGridArrays()
               The return code is passed back in <ierror>.
   @endreturndesc
@@*/
void CCTK_FCALL CCTK_FNAME (CCTK_InterpGridArrays)
                           (int *ierror,
                            const cGH **GH,
                            const int *N_dims,
                            const int *local_interp_handle,
                            const int *param_table_handle,
                            const int *coord_system_handle,
                            const int *N_interp_points,
                              const int *interp_coords_type,
                              const void *const interp_coords[],
                            const int *N_input_arrays,
                              const CCTK_INT input_array_indices[],
                            const int *N_output_arrays,
                              const CCTK_INT output_array_types[],
                              void *const output_arrays[])
{
  *ierror = CCTK_InterpGridArrays (*GH, *N_dims, *local_interp_handle,
                                   *param_table_handle, *coord_system_handle,
                                   *N_interp_points, *interp_coords_type,
                                   interp_coords,
                                   *N_input_arrays,input_array_indices,
                                   *N_output_arrays, output_array_types,
                                   output_arrays);
}


/******************************************************************************/

/*@@
  @routine    CCTK_InterpLocalUniform
  @date       22 Oct 2001
  @author     Jonathan Thornburg <jthorn@aei.mpg.de>
  @desc
        This API interpolates a set of data arrays defined on a uniform
        N-dimensional tensor-product grid, to a set of interpolation points.
        A key-value table is used to pass options to the interpolation
        operator.
  @enddesc

  ***** misc arguments *****

  @var          N_dims
  @vdesc        dimensionality of the interpolation
  @vtype        int N_dims                      (must be >= 1)
  @endvar

  @var          operator_handle
  @vdesc        handle to the interpolation operator
  @vtype        int operator_handle             (must be >= 0)
  @endvar

  @var          param_table_handle
  @vdesc        handle to a key-value table giving additonal parameters
                for the interpolation
  @vtype        int param_table_handle          (must be >= 0)
  @endvar

  ***** arguments specifying the coordinate system *****

  @var          coord_origin
  @vdesc        (pointer to) array[N_dims] of values giving the
                x,y,z,... coordinates which correspond to the
                integer input-array subscripts (0,0,0,...,0)
                (note there is no implication here that such a
                grid point need actually exist; the arrays just
                give the coordinates it would have if it did exist)
  @vtype        const CCTK_REAL coord_origin[N_dims]
  @endvar

  @var          coord_delta
  @vdesc        (pointer to) array[N_dims] of values giving the
                coordinate spacing of the grid
  @vtype        const CCTK_REAL coord_delta[N_dims]
  @endvar

  ***** arguments specifying the interpolation points *****

  @var          N_interp_points
  @vdesc number of interpolation points
  @vtype        int N_interp_points             (must be >= 0)
  @endvar

  @var          interp_coords_type_code
  @vdesc        one of the CCTK_VARIABLE_* codes giving the data
                type of the arrays pointed to by  interp_coords[]
  @vtype        int
  @endvar

  @var          interp_coords
  @vdesc        (pointer to) array[N_dims] of pointers
                to arrays[N_interp_points] giving
                x,y,z,... coordinates of interpolation points
  @vtype        const void *const interp_coords[N_dims]
  @endvar

  ***** arguments specifying the inputs (the data to be interpolated) *****

  @var          N_input_arrays
  @vdesc        number of arrays input to the interpolation
  @vtype        int N_input_arrays              (must be >= 0)
  @endvar

  @var          input_array_dims
  @vdesc        dimensions of the input arrays: unless overridden by
                entries in the parameter table, all input arrays are
                taken to have these dimensions, with [0] the most contiguous
                axis and [N_dims-1] the least contiguous axis, and
                array subscripts in the range 0 <= subscript < dims[axis]
  @vtype        const int input_array_dims[N_dims]
  @endvar

  @var          input_array_type_codes
  @vdesc        (pointer to) array of CCTK_VARIABLE_* codes
                giving the data types of the input arrays
  @vtype        const int input_array_type_codes[N_input_arrays]
  @endvar

  @var          input_arrays
  @vdesc        (pointer to) array[N_input_arrays] of pointers to input arrays
  @vtype        const void *const input_arrays[N_input_arrays]
  @endvar

  ***** arguments specifying the outputs (the interpolation results) *****

  @var          N_output_arrays
  @vdesc        number of arrays output from the interpolation
  @vtype        int N_output_arrays             (must be >= 0)
  @endvar

  @var          output_array_type_codes
  @vdesc        (pointer to) array of CCTK_VARIABLE_* codes
                giving the data types of the output arrays
  @vtype        const int output_array_type_codes[N_output_arrays]
  @endvar

  @var          output_arrays
  @vdesc        (pointer to) array[N_output_arrays] of pointers to output arrays
  @vtype        void *const output_arrays[N_output_arrays]
  @endvar

  ***** return result *****

  @returntype   int
  @returndesc    0 ==> successful, otherwise
                 UTIL_ERROR_BAD_HANDLE ==> bad operator_handle
  @endreturndesc
  @@*/
int CCTK_InterpLocalUniform(int N_dims,
                            int operator_handle,
                            int param_table_handle,
                            /***** coordinate system *****/
                            const CCTK_REAL coord_origin[],
                            const CCTK_REAL coord_delta[],
                            /***** interpolation points *****/
                            int N_interp_points,
                            int interp_coords_type_code,
                            const void *const interp_coords[],
                            /***** input arrays *****/
                            int N_input_arrays,
                            const CCTK_INT input_array_dims[],
                            const CCTK_INT input_array_type_codes[],
                            const void *const input_arrays[],
                            /***** output arrays *****/
                            int N_output_arrays,
                            const CCTK_INT output_array_type_codes[],
                            void *const output_arrays[])
{
  const struct interp_info *p_interp_info
        = (struct interp_info *)Util_GetHandledData(interp_operators,
                                                   operator_handle);
  if ( (p_interp_info == NULL)
       || (p_interp_info->interp_op_local_uniform == NULL) )
    {
    CCTK_VWarn(0, __LINE__, __FILE__, "Cactus",
"\n"
"   CCTK_InterpLocalUniform(): no interpolation operator is registered\n"
"                              under the handle %d\n"
"                              (did you activate LocalInterp or some other thorn\n"
"                               providing this interpolation operator?)"
               ,
               operator_handle);
    return UTIL_ERROR_BAD_HANDLE;
    }

  return p_interp_info->interp_op_local_uniform(N_dims,
                                                param_table_handle,
                                                /*** coordinate system ***/
                                                coord_origin, coord_delta,
                                                /*** interpolation points ***/
                                                N_interp_points,
                                                interp_coords_type_code,
                                                interp_coords,
                                                /***** input arrays *****/
                                                N_input_arrays,
                                                input_array_dims,
                                                input_array_type_codes,
                                                input_arrays,
                                                /***** output arrays *****/
                                                N_output_arrays,
                                                output_array_type_codes,
                                                output_arrays);
}
