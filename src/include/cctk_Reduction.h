 /*@@
   @header    cctk_Reduction.h
   @date      
   @author    Gabrielle Allen
   @desc 
   Header file for using reduction operators
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTK_REDUCTION_H_
#define _CCTK_REDUCTION_H_

#ifdef __cplusplus
extern "C" 
{
#endif

#define REDUCTION_OPERATOR_REGISTER_ARGLIST  \
          cGH *,  \
          int,    \
          int,    \
          int,    \
          void *, \
          int,    \
          int *    

#define REDUCTION_ARRAY_OPERATOR_REGISTER_ARGLIST \
  cGH *GH, \
  int proc, \
  int nDims, \
  int dims [], \
  int nArrays, \
  void *inArrays [], \
  int inType, \
  int nOutVals, \
  void *outVals, \
  int outType

int CCTK_Reduce(cGH *GH,
                int proc,
                int operation_handle,
                int num_out_vals,
                int type_out_vals,
                void *out_vals,
                int num_in_fields, ...);

int CCTK_ReductionHandle(const char *reduction);

int CCTK_RegisterReductionOperator(
         int (*function)(REDUCTION_OPERATOR_REGISTER_ARGLIST),
         const char *name);

int CCTK_ReductionArrayHandle(const char *reduction);

int CCTK_RegisterReductionArrayOperator(
         int (*function)(REDUCTION_ARRAY_OPERATOR_REGISTER_ARGLIST),
         const char *name);

/* FIXME: old interface - should go */
int CCTK_ReduceLocalScalar (cGH *GH, int proc, int operation_handle,
                            void *inScalar, void *outScalar, int dataType);


/* FIXME: old interface - should go */
int CCTK_ReduceLocalArray1D (cGH *GH, int proc, int operation_handle,
                            void *in_array1d, void *out_array1d, 
                            int num_in_array1d, int data_type);

int CCTK_ReduceLocScalar(cGH *GH, int proc, int operation_handle,
                         void *in_scalar, void *out_scalar, int data_type);

int CCTK_ReduceLocArrayToArray1D(cGH *GH, int proc, int operation_handle,
                                 void *in_array1d, void *out_array1d, 
                                 int num_in_array1d,
                                 int data_type);

int CCTK_ReduceLocArrayToArray2D(cGH *GH, int proc, int operation_handle,
                                 void *in_array2d, void *out_array2d, 
                                 int xsize, int ysize,
                                 int data_type);

int CCTK_ReduceLocArrayToArray3D(cGH *GH, int proc, int operation_handle,
                                 void *in_array3d, void *out_array3d, 
                                 int xsize, int  ysize, int zsize,
                                 int data_type);


int CCTK_ReduceArray(cGH *GH,
                     int proc,
                     int operation_handle,
                     int num_out_vals,
                     int type_out_vals,
                     void *out_vals,
                     int num_dims,
                     int num_in_arrays,
                     int type_in_arrays,
                     ... );

#ifdef __cplusplus
}
#endif

#endif /* _CCTK_REDUCTION_H_ */
