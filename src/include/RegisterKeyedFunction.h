 /*@@
   @header    RegisterKeyedFunction.h
   @date      Tue Sep 29 09:41:49 1998
   @author    Tom Goodale
   @desc 
   Header file for keyed function registration.
   @enddesc 
 @@*/

/* $Id$ */


#ifndef _REGISTERKEYEDFUNCTION_H_
#define _REGISTERKEYEDFUNCTION_H_

/*****************************************************/
/* Function prototypes. */

#ifdef __cplusplus
extern "C" {
#endif

int RegisterKeyedFunction(void (*array[])(), 
			  int min, int max, 
			  int key, void (*func)());

void  (**(CreateKeyedFunctionArray(int size)))();

#ifdef __cplusplus
	   }
#endif

/*****************************************************/

/* Possible return codes. */

enum RegisterKeyedFunctionErrors {REG_KEYED_FUNCTION_SUCCESS, 
				  REG_KEYED_FUNCTION_ALREADY_ASSIGNED,
				  REG_KEYED_FUNCTION_RANGE_ERROR};


/*****************************************************/


#endif
