 /*@@
   @header    WarnLevel.h
   @date      Wed Feb 17 00:53:55 1999
   @author    Tom Goodale
   @desc 
   Header for the warning functions.
   @enddesc 
 @@*/

#ifndef _WARNLEVEL_H_
#define _WARNLEVEL_H_

#ifdef __cplusplus 
extern "C" {
#endif

int CCTKi_SetWarnLevel(int level);
void CCTK_Warn(int level, int line, const char *file, const char *thorn, const char *message);
void CCTK_ParamWarn(const char *thorn, const char *message);
void CCTKi_FinaliseParamWarn(void);
int CCTKi_SetErrorLevel(int level);
void CCTK_Info(const char *thorn, const char *message);
 
#ifdef __cplusplus 
}
#endif

#endif
