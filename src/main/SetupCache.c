 /*@@
   @file      SetupCache.c
   @date      Tue Nov 30 10:30:09 1999
   @author    Tom Goodale
   @desc 
   Sets up cache stuff for the CCTK
   @enddesc 
 @@*/

#include <stdlib.h>

#include "cctk_Config.h"
#include "cctk_Flesh.h"
#include "cctk_Parameter.h"

#include "cctki_Cache.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_SetupCache_c)

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
  int param_type;
  int manual_cache_setup;

  unsigned long cache_size;
  unsigned long cacheline_bytes;

  manual_cache_setup = (*(int *)CCTK_ParameterGet("manual_cache_setup",
						  "Cactus",&param_type));

  if(manual_cache_setup)
  {
    cache_size = (*(int *)CCTK_ParameterGet("manual_cache_size",
					    "Cactus",&param_type));
    cacheline_bytes = (*(int *)CCTK_ParameterGet("manual_cacheline_bytes",
						 "Cactus",&param_type));
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

}
