 /*@@
   @header    Implementations.h
   @date      Fri Jan 15 13:58:14 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
   @version $Id$
 @@*/

int CCTK_RecordImplementation(const char *implementation,
			      const char *thorn);

int GetImplementationThorns(const char *implementation, char ***thornlist);

int CCTK_RegisterThorn(const char *name, const char *implementation);

