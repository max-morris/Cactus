 /*@@
   @file      RegisterThorn.c
   @date      Wed Jan 13 20:56:18 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>

#include "flesh.h";

#include "StoreNamedData.h";


static pNamedData *thorn_data = NULL;

int CCTK_RegisterThorn(const char *name, const char *implementation,
		       const t_thorndata *data)
{
  int retval;


  if(StoreNamedData(&thorn_data, name, (void *)data))
  {
    fprintf(stderr, "Error storing thorn data for thorn %s", name);

    retval = 1;
  }
  else
  {
    if(CCTK_RecordImplementation(implementation, name))
    {
      fprintf(stderr, "Error storing implementation data for thorn %s", name);
      retval = 2;
    }
    else
    {
      retval = 0;
    }
  }

  return retval;
}
  

  
