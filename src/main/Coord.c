 /*@@
   @file      Coord.c
   @date      Indeterminate 11-12th April 1999
   @author    Gabrielle Allen
   @desc 
   Routines to deal with cooordinates and coordinate registration
   @enddesc 
 @@*/

#define DEBUG_COORDS

#include <stdio.h>

#include "cctk.h"
#include "StoreNamedData.h"
#include "WarnLevel.h"
#include "Coords.h"

static pNamedData *coordinates = NULL;

 /*@@
   @routine    RegisterCoord_ByIndex
   @date       11-12th April 1999
   @author     Gabrielle Allen
   @desc 
               Register a GF as a coordinate with a name and
               an (optional) direction.
   @enddesc 
   @@*/

int CCTK_RegisterCoord_ByIndex(const char *name, int index, int dir)
{
  int retval = 0;
  int *store_data;

  store_data = (int *)malloc(2*sizeof(int));
  store_data[0] = index;
  store_data[1] = dir;

  if (StoreNamedData(&coordinates, 
                     name, 
                     &store_data))
  {
    char *message;
    message = (char *)malloc( (100+sizeof(name))*sizeof(char) );
    sprintf(message,"Memory failure while registering coordinate %s\n",
          name);
    CCTK_Warn(0,message);
    if (message) free(message);
    retval = -1;
  }

#ifdef DEBUG_COORDS
     printf(" In RegisterCoord\n ----------------\n");
     printf("   Registering index %d as %s in dir %d\n",
	    store_data[0],name,store_data[1]); 
#endif

  return retval;

}


 /*@@
   @routine    RegisterCoord
   @date       11-12th April 1999
   @author     Gabrielle Allen
   @desc 
               Register a GF as a coordinate with a name and
               an (optional) direction.
   @enddesc 
   @@*/

int CCTK_RegisterCoord(const char *coordname, 
		       const char *gfname, 
		       int dir)
{
  
  int retval;
  int index;
  index = CCTK_GetVarIndex(gfname);
  if (index < 0)
  {
    char *msg;
    msg = (char *)malloc( (100+strlen(gfname)) );
    sprintf(msg,"Error from CCTK_GetVarNum for -%s- in CCTK_RegisterCoord",
            gfname);
    CCTK_Warn(1,msg);
    if (msg) free(msg);
    
    retval = -2; /* Error from CCTK_GetVarNum */

  }
  else
  {
     retval = CCTK_RegisterCoord_ByIndex(coordname,index,dir);
  }

  return retval;

}



int CCTK_GetCoordIndex(const char *name)
{
  void *data;
  int index;
  int retval;
  
  index = GetNamedData(coordinates,name);
  /*  if (!data)
  {
    printf("No data found\n");
  }
  index = (int *)data;
  if (index)
    retval = index;
  */
  printf("Pointer is %x %d\n",index,index);
  printf("Index is %d\n",retval);
  
  return index;

}






