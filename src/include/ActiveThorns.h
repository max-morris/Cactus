 /*@@
   @header    ActiveThorns.h
   @date      Sun Jul  4 17:39:50 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc
   @version $Header$
 @@*/

#ifndef __ACTIVETHORNS_H_
#define __ACTIVETHORNS_H_

#ifdef __cplusplus 
extern "C" {
#endif

int CCTK_RegisterThorn(const char *name, const char *imp);
int CCTK_ActivateThorn(const char *name);
int CCTK_IsThornActive(const char *name);
int CCTK_IsImplementationActive(const char *name);
int CCTK_ListThorns(FILE *file, const char *format, int active);
int CCTK_ListImplementations(FILE *file, const char *format, int active);

#ifdef __cplusplus 
}
#endif

#endif /* _ACTIVETHORNS_H_ */
