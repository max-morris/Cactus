 /*@@
   @header    cctki_ActiveThorns.h
   @date      Sun Jul  4 17:39:50 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc
   @version $Header$
 @@*/

#ifndef __CCTKI_ACTIVETHORNS_H_
#define __CCTKI_ACTIVETHORNS_H_

#include <stdio.h>
#include "SKBinTree.h"   

#ifdef __cplusplus 
extern "C" {
#endif

int CCTKi_RegisterThorn(const char *name, const char *imp);
int CCTKi_ActivateThorn(const char *name);
int CCTKi_PrintThorns(FILE *file, const char *format, int active);
int CCTKi_PrintImps(FILE *file, const char *format, int active);

int CCTKi_ImplementationList
 (
  int active,
  char ***list,
  int *n_implementations
 );
int CCTKi_ImplementationThornList 
 (
  const char *imp, 
  char ***list, 
  int *n_thorns
 );
int CCTKi_ThornList
 (
  int active, 
  char ***list, 
  int *n_items
 );

#ifdef __cplusplus 
}
#endif

#endif /* _CCTKI_ACTIVETHORNS_H_ */
