 /*@@
   @header    cctk_WarnLevel.h
   @date      Wed Feb 17 00:53:55 1999
   @author    Tom Goodale
   @desc 
   Header for the warning functions.
   @enddesc 
 @@*/

#ifndef _CCTK_WARNLEVEL_H_
#define _CCTK_WARNLEVEL_H_

#ifdef __cplusplus 
extern "C" {
#endif

void CCTK_Warn(int level, 
	       int line, 
	       const char *file, 
	       const char *thorn, 
	       const char *message);
void CCTK_VWarn(int level, 
                int line, 
                const char *file, 
                const char *thorn, 
                const char *format, ...);
void CCTK_ParamWarn(const char *thorn, const char *message);
void CCTK_Info(const char *thorn, const char *message);
 
#ifdef __cplusplus 
}
#endif

#endif
