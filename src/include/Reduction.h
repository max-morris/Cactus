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

int CCTK_Reduce(cGH *GH,
		int retvaltype,
		int retvalnum,
		void *retval,
		int operation_handle,
		int index, ...);

#ifdef __cplusplus
	   }
#endif

#endif
