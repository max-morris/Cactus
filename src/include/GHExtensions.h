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

int CCTK_RegisterGHExtensionSetupGH(int handle, 
				    void *(*func)(tFleshConfig *, int, cGH *));

int CCTK_RegisterGHExtensionInitGH(int handle, int (*func)(cGH *));

int CCTK_RegisterGHExtensionrfrTraverseGH(int handle, int (*func)(cGH *, int));

int CCTK_SetupGHExtensions(tFleshConfig *config, 
			   int convergence_level, 
			   cGH *GH);

int CCTK_InitGHExtensions(cGH *GH);

int CCTK_rfrTraverseGHExtensions(cGH *GH, int rfrpoint);

int CCTK_GetGHExtensionHandle(const char *name);


#ifdef _cplusplus
}
#endif

#endif

