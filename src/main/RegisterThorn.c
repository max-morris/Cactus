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
  

 /*@@
   @routine    GetThornData
   @date       Wed Jan 13 23:25:47 1999
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int GetThornData(const char *thorn, t_thorndata **thorndata)
{
  int retval;

  if((*thorndata = (t_thorndata *)GetNamedData(thorn_data, thorn)))
  {
    retval = 1;
  }
  else
  {
    *thorndata = NULL;
    retval = 0;
  }

  return retval;
}
