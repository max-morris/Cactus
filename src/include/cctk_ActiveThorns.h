 /*@@
   @header    ActiveThorns.h
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

int CCTKi_RegisterThorn(const char *name, const char *imp);
int CCTKi_ActivateThorn(const char *name);
int CCTK_IsThornActive(const char *name);
int CCTK_IsThornCompiled(const char *name);
int CCTK_IsImplementationActive(const char *name);
int CCTKi_ListThorns(FILE *file, const char *format, int active);
int CCTKi_ListImplementations(FILE *file, const char *format, int active);
const char *CCTK_ActivatingThorn(const char *imp);

/* FIXME - should return a list or something */
t_sktree *CCTK_ImpThornList (const char *imp);

/* public routines to get information about thorns and imps */
int CCTK_ImplementationList   (int active,      char ***list, int *n_implementations);
int CCTK_ImplementationThornList (const char *imp, char ***list, int *n_thorns);
int CCTK_ThornList(int active, char ***list, int *n_items);

const char *CCTK_ThornImplementation(const char *name);
const char *CCTK_ImplementationThorn(const char *name);


#ifdef __cplusplus 
}
#endif

#endif /* _ACTIVETHORNS_H_ */
