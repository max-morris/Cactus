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

int CCTK_SetWarnLevel(int level);
int CCTK_Warn(int level, const char *thorn, const char *message);
void CCTK_ParamWarn(const char *thorn, const char *message);
void CCTKi_FinaliseParamWarn(void);
int CCTK_SetErrorLevel(int level);
void CCTK_Info(const char *thorn, const char *message);
 
#ifdef __cplusplus 
}
#endif

#endif
