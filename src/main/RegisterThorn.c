 /*@@
   @file      RegisterThorn.c
   @date      Wed Jan 13 20:56:18 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>

#include "flesh.h"
#include "Implementations.h"

#include "StoreNamedData.h"


static pNamedData *thorn_data = NULL;

 /*@@
   @routine    CCTK_RegisterThorn
   @date       Wed Jan 13 23:26:04 1999
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_RegisterThorn(const char *name, const char *implementation)
{
  int retval;


  if(CCTK_RecordImplementation(implementation, name))
  {
    fprintf(stderr, "Error storing implementation data for thorn %s", name);
    retval = 1;
  }
  else
  {
    retval = 0;
  }

  return retval;
}
  

