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

/* The functions. */

extern cGH * (*SetupGH)(tFleshConfig *, int);
extern int (*SetupGF)(cGH *, cGF *);

extern int (*SyncAllFuncs)(cGH *);
extern int (*SyncGroupFuncs)(cGH *, const char *group);
extern int (*SyncOneFunc)(cGH*, int );

extern int (*ParallelInit)(tFleshConfig *);
extern int (*ParallelFinalise)(tFleshConfig *);

extern int (*Reduce)(cGH *, int , int operation, void *result);

#ifdef _cplusplus
	   }
#endif

#endif
