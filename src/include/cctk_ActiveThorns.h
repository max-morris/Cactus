 /*@@
   @header    cctk_ActiveThorns.h
   @date      Sun Jul  4 17:39:50 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc
   @version $Header$
 @@*/

#ifndef __CCTK_ACTIVETHORNS_H_
#define __CCTK_ACTIVETHORNS_H_

#include <stdio.h>
#include "SKBinTree.h"   

#ifdef __cplusplus 
extern "C" {
#endif

int         CCTK_IsThornActive(const char *name);
int         CCTK_IsThornCompiled(const char *name);
int         CCTK_IsImplementationActive(const char *name);
const char *CCTK_ActivatingThorn(const char *imp);

/* FIXME - should return a list or something */
t_sktree *CCTK_ImpThornList (const char *imp);

const char *CCTK_ThornImplementation(const char *name);
const char *CCTK_ImplementationThorn(const char *name);

#ifdef __cplusplus 
}
#endif

#endif /* _CCTK_ACTIVETHORNS_H_ */
