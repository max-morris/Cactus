 /*@@
   @header    cctk_Functions.h
   @date      Sat Jan 22 12:21:34 2000
   @author    Tom Goodale
   @desc 
   Externally visable stuff for the function registry.
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTK_FUNCTIONS_H_
#define _CCTK_FUNCTIONS_H_

typedef struct
{
  int n_args;
  int type;
} cFunctionType;

#ifdef __cplusplus
extern "C"
{
#endif

void *CCTK_FunctionRegister(const char *name, 
                            void (*function)(void), 
                            cFunctionType *ftype);

int CCTK_FunctionCall(void *data, ...);

#ifdef __cplusplus
}
#endif

#endif /* _CCTK_FUNCTIONS_H_ */
