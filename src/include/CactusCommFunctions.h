 /*@@
   @header    CactusCommFunctions.h
   @date      Thu Jan 14 18:37:58 1999
   @author    Tom Goodale
   @desc 
   Overloadable communication functions
   @enddesc 
 @@*/
 
/* $Id$ */

#ifndef _CACTUSCOMMFUNCTIONS_H_
#define _CACTUSCOMMFUNCTIONS_H_

#ifdef _cplusplus
extern "C" {
#endif

cGH *SetupGH(tFleshConfig *, int);
int SetupGF(cGH *, cGF *);

int SyncAllFuncs(cGH *);
int SyncGroupFuncs(cGH *, const char *group);
int SyncOneFunc(cGH *, cGF *);

int ParallelInit(tFleshConfig *);
int ParallelFinalise(tFleshConfig *);

int Reduce(cGH *, cGF *, int operation, void *result);

#ifdef _cplusplus
	   }
#endif

#endif
