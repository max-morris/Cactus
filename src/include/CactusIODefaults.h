 /*@@
   @header    CactusIODefaults.h
   @date      Tue Sep 29 12:40:39 1998
   @author    Tom Goodale
   @desc 
   Prototypes for default IO functions.
   @enddesc 
 @@*/


/* $Id$ */

#ifndef _CACTUSIODEFAULTS_H_
#define _CACTUSIODEFAULTS_H_

#ifdef _cplusplus
extern "C" {
#endif

int CactusDefaultOutput1D(cGH *, cGF *);
int CactusDefaultOutput2D(cGH *, cGF *);
int CactusDefaultOutput3D(cGH *, cGF *);

#ifdef _cplusplus
	   }
#endif

#endif
