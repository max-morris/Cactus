 /*@@
   @header    Reduction.h
   @date      
   @author    Gabrielle Allen
   @desc 
   Header file for using reduction operators
   @enddesc 
 @@*/

/* $Id$ */


#ifndef _REDUCTION_H_
#define _REDUCTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#define REGISTER_ARGLIST  \
	  cGH *,  \
          int,    \
          int,    \
          int,    \
          void *, \
          int,    \
          int *    

int CCTK_Reduce(cGH *GH,
		int proc,
		int operation_handle,
		int num_out_vals,
		int type_out_vals,
		void *out_vals,
		int num_in_fields, ...);

int CCTK_ReductionHandle(const char *reduction);

int CCTK_RegisterReductionOperator(void (*function)(REGISTER_ARGLIST),
				   const char *name);


#ifdef __cplusplus
	   }
#endif

#endif
