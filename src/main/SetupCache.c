 /*@@
   @file      SetupCache.c
   @date      Tue Nov 30 10:30:09 1999
   @author    Tom Goodale
   @desc 
   Sets up cache stuff for the CCTK
   @enddesc 
 @@*/

static char *rcsid = "$Header$";

#include <stdlib.h>

#include "cctk_Config.h"
#include "cctki_Cache.h"
#include "cctk_Parameters.h"

/********************************************************************
 *********************   CCTK Local Routines   **********************
 ********************************************************************/

 /*@@
   @routine    CCTKi_SetupCache
   @date       Tue Nov 30 10:50:02 1999
   @author     Tom Goodale
   @desc 
   Sets the cache information.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

   @returntype int
   @returndesc
   0
   @endreturndesc

@@*/
int CCTKi_SetupCache(void)
{
  DECLARE_CCTK_PARAMETERS

  unsigned long cache_size;
  unsigned long cacheline_bytes;

  if(manual_cache_setup)
  {
    cache_size = manual_cache_size;
    cacheline_bytes = manual_cacheline_bytes;
  }
  else
  {
    /* FIXME:  Remove this check for release */
#ifdef CCTK_L2_CACHE_SIZE
    cache_size      = CCTK_L2_CACHE_SIZE;
    cacheline_bytes = CCTK_L2_CACHELINE_BYTES;
#else
    cache_size      = 0;
    cacheline_bytes = 0;
#endif
  }

  Utili_CacheDataSet(cacheline_bytes, cache_size);

  return 0;

  USE_CCTK_PARAMETERS

}
