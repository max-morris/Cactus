 /*@@
   @header    GHExtensions.h
   @date      Fri Jan 15 14:15:20 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc
   @version $Id$
 @@*/

#ifndef _GHEXTENSIONS_H_
#define _GHEXTENSIONS_H_

#ifdef _cplusplus
extern "C" {
#endif

int CCTK_RegisterGHExtension(const char *name);

int CCTK_RegisterGHExtensionInitialiser(int handle, void *(*func)(cGH *));


#ifdef _cplusplus
}
#endif

#endif

