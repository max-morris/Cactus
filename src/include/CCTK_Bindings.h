 /*@@
   @header    CCTK_Bindings.h
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

int CCTK_BindingsParametersInitialise(void);
int CCTK_BindingsVariablesInitialise(void);
int CCTK_BindingsScheduleInitialise(void);

int CCTK_BindingsScheduleRegister(const char *type, void *data);

int CCTK_BindingsParameterHelp(const char *identifier, const char *format, FILE *file);

#ifdef __cplusplus
}
#endif


#endif
