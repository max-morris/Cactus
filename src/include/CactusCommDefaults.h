 /*@@
   @header    CactusCommDefaults.h
   @date      Tue Sep 29 12:40:39 1998
   @author    Tom Goodale
   @desc 
   Prototypes for default communication functions.
   @enddesc 
 @@*/


/* $Id$ */

#ifndef _CACTUSCOMMDEFAULTS_H_
#define _CACTUSCOMMDEFAULTS_H_

#ifdef _cplusplus
extern "C" {
#endif

int CactusDefaultSetupGH(cGH *);
int CactusDefaultSetupGF(cGH *, cGF *);

int CactusDefaultSyncAllFuncs(cGH *);
int CactusDefaultSyncGroupFuncs(cGH *, const char *group);
int CactusDefaultSyncOneFunc(cGH *, cGF *);

int CactusDefaultParallelInit(tFleshConfig *);
int CactusDefaultParallelFinalise(tFleshConfig *);

int CactusDefaultReduce(cGH *, cGF *, int operation, void *result);

#ifdef _cplusplus
	   }
#endif

#endif
