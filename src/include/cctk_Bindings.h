 /*@@
   @header    cctk_Bindings.h
   @date      Tue Jun  1 16:42:28 1999
   @author    Tom Goodale
   @desc 
   Accessable functions from the bindings.
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTK_BINDINGS_H_
#define _CCTK_BINDINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

int CCTKi_BindingsParametersInitialise(void);
int CCTKi_BindingsVariablesInitialise(void);
int CCTKi_BindingsScheduleInitialise(void);

int CCTKi_BindingsScheduleRegister(const char *type, void *data);

int CCTKi_BindingsParameterHelp(const char *identifier, const char *format, FILE *file);

#ifdef __cplusplus
}
#endif


#endif
