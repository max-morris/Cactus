 /*@@
   @header    cctki_WarnLevel.h
   @date      Wed Feb 17 00:53:55 1999
   @author    Tom Goodale
   @desc 
   Header for the internal warning functions.
   @enddesc 
 @@*/

#ifndef _CCTKI_WARNLEVEL_H_
#define _CCTKI_WARNLEVEL_H_

#ifdef __cplusplus 
extern "C" {
#endif

int CCTKi_SetWarnLevel(int level);
void CCTKi_FinaliseParamWarn(void);
int CCTKi_SetErrorLevel(int level);
void CCTKi_NotYetImplemented(const char *message);
  
#ifdef __cplusplus 
}
#endif

#endif
