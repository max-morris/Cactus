 /*@@
   @header    cctk_Interp.h
   @date      July 07 1999
   @author    Thomas Radke
   @desc 
              Header file for using interpolation operators
   @enddesc 
   @history
   @date      July 07 1999
   @author    Thomas Radke
   @hdesc     Just copied from cctk_Reduction.h
   @endhistory
 @@*/


#ifndef _CCTK_INTERP_H_
#define _CCTK_INTERP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* prototype for interpolation operator routine
   working on a list of grid variables */
typedef int (*cInterpOperatorGV) (cGH *GH,
                                  const char *coord_system,
                                  int num_points,
                                  int num_in_array_indices,
                                  int num_out_arrays,
                                  void *interp_coord_arrays[],
                                  int interp_coord_array_types[],
                                  int in_array_indices[],
                                  void *out_arrays[],
                                  int out_array_types[]);

/* prototype for interpolation operator routine
   working on local arrays */
typedef int (*cInterpOperatorLocal) (cGH *GH,
                                     int num_points,
                                     int num_dims,
                                     int num_in_arrays,
                                     int num_out_arrays,
                                     int coord_dims[],
                                     void *coord_arrays[],
                                     int coord_array_types[],
                                     void *interp_coord_arrays[],
                                     int interp_coord_array_types[],
                                     void *in_arrays[],
                                     int in_array_types[],
                                     void *out_arrays[],
                                     int out_array_types[]);

int CCTK_InterpHandle (const char *interp);

int CCTK_InterpRegisterOperatorGV (cInterpOperatorGV operator_GV,
                                   const char *name);

int CCTK_InterpRegisterOperatorLocal (cInterpOperatorLocal operator_local,
                                      const char *name);

int CCTK_InterpGV (cGH *GH,
                   int operator_handle,
                   int coord_system_handle,
                   int num_points,
                   int num_in_array_indices,
                   int num_out_arrays,
                   ...);

int CCTK_InterpLocal (cGH *GH,
                      int operator_handle,
                      int num_points,
                      int num_dims,
                      int num_in_arrays,
                      int num_out_arrays,
                      ...);


/* depricated functions */
#define INTERP_REGISTER_ARGLIST  \
                         cGH *,  \
                         int,    \
                         int,    \
                         int,    \
                         int,    \
                         int *,  \
                         void **,\
                         int *,  \
                         void *, \
                         void *, \
                         void **,\
                         int *,  \
                         void **,\
                         int *    

int CCTK_InterpArray(cGH *GH,
                int operation_handle,
                int nPoints,
                int nCoords,
                int nInFields,
                int nOutFields,
                ...);

int CCTK_InterpRegisterOperator (int (*function)(INTERP_REGISTER_ARGLIST),
                                 const char *name);

int CCTK_Interp(cGH *GH,
                 int operation_handle,
                 int nPoints,
                 int nDims,
                 int nInFields,
                 int nOutFields,
                 ...);

#ifdef __cplusplus
           }
#endif

#endif  /* _INTERP_H_ */
