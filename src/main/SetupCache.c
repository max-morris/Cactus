 /*@@
   @file      SetupCache.c
   @date      Tue Nov 30 10:30:09 1999
   @author    Tom Goodale
   @desc 
   Sets up cache stuff for the CCTK
   @enddesc 
   @version $Header$
 @@*/

#include <stdlib.h>

#include "cctk_Config.h"
#include "cctk_Flesh.h"
#include "cctk_Parameter.h"

#include "cctki_Cache.h"

static const char *rcsid = "$Header$";

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
   0 - success
   @endreturndesc

@@*/
int CCTKi_SetupCache(void)
{
  int param_type;
  int manual_cache_setup;

  unsigned long cache_size;
  unsigned long cacheline_bytes;

  manual_cache_setup = (*(CCTK_INT *)CCTK_ParameterGet("manual_cache_setup",
						  "Cactus",&param_type));

  if(manual_cache_setup)
  {
    cache_size = (*(CCTK_INT *)CCTK_ParameterGet("manual_cache_size",
					    "Cactus",&param_type));
    cacheline_bytes = (*(CCTK_INT *)CCTK_ParameterGet("manual_cacheline_bytes",
						 "Cactus",&param_type));
  } 
  else
  {
    cache_size      = CCTK_L2_CACHE_SIZE;
    cacheline_bytes = CCTK_L2_CACHELINE_BYTES;
  }

  Utili_CacheDataSet(cacheline_bytes, cache_size);

  return 0;

}
