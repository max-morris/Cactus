 /*@@
   @header    cctki_Parameter.h
   @date      Wed Sep 15 22:49:24 1999
   @author    Tom Goodale
   @desc 
   Internal parameter prototypes
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTKI_PARAMETER_H_
#define _CCTKI_PARAMETER_H_

#ifdef __cplusplus
extern "C" 
{
#endif

int CCTKi_SetParameter(const char *parameter, const char *value);
int CCTKi_NumParameterFileErrors(int level);

#ifdef __cplusplus
}
#endif

#endif /* _CCTKI_PARAMETER_H_ */
