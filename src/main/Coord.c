 /*@@
   @file      Coord.c
   @date      Indeterminate 11-12th April 1999
   @author    Gabrielle Allen
   @desc 
   Routines to deal with cooordinates and coordinate registration
   @enddesc 
 @@*/

/*#define DEBUG_COORD*/

#include <stdlib.h>
#include <stdio.h>

#include "cctk.h"
#include "StoreHandledData.h"
#include "WarnLevel.h"
#include "Coord.h"
#include "ErrorCodes.h"
#include "Groups.h"
#include "Misc.h"

static cHandledData *coordinates = NULL;
static int num_coords = 0;

 /*@@
   @routine    RegisterCoord_ByIndex
   @date       11-12th April 1999
   @author     Gabrielle Allen
   @desc 
               Register a GF as a coordinate with a name, and index
	       and a direction
   @enddesc 
   @calls      CCTK_GetHandle, CCTK_NewHandle, CCTK_Warn

   @var        name        
   @vdesc      Name coordinate is registered as
   @vtype      const char *
   @vio        in
   @vcomment   The name must not be the same as the variable name
   @endvar 

   @var        index
   @vdesc      The index of the variable providing the coordinate
   @vtype      int
   @vio        in
   @vcomment
   @endvar

   @var        dir
   @vdesc      An index giving the coordinate direction
   @vtype      int
   @vio        in
   @vcomment   This should be an optional argument
   @endvar
 
   @returntype int
   @returndesc
      -1           = Duplicate coordinate name, all duplicates
                     will be ignored
      ERROR_MEMORY = Failure with malloc
      >= 0           Handle returned by coordinate registration
   @endreturndesc

   @@*/

int CCTK_RegisterCoord_ByIndex(const char *name, int index, int dir)
{

  int handle;
  struct Coordprops *new_coord;

  /* Check that the method hasn't already been registered */
  handle = CCTK_GetHandle(coordinates, name, NULL);

  if(handle < 0)
  {
    /* New extension. */
    new_coord = (struct Coordprops *)malloc(sizeof(struct Coordprops));

    if(new_coord)
    {
      /* Get a handle for it. */
      handle = CCTK_NewHandle(&coordinates, name, new_coord);

      /* Initialise the coordinate properties structure */
      new_coord->name      = (char *)name;
      new_coord->index     = index;
      new_coord->direction = dir;

      /* Remember how many methods there are */
      num_coords++;

#ifdef DEBUG_COORD
      printf(" In CCTK_RegisterCoord_ByIndex\n");
      printf(" -----------------------------\n");
      printf("   handle %d, name %s,\n",handle,name);
      printf("   index %d, direction %d\n",index,dir);
#endif

    }
    else
    {
      /* Memory failure. */
      handle = ERROR_MEMORY;
    }
  }
  else
  {
    /* Method already exists. */
    char *msg;
    msg = (char *)malloc(200*sizeof(char)+sizeof(name));
    sprintf(msg,"Coordinate with name -%s- already registered",name);
    CCTK_WARN(1,msg);
    if (msg) free(msg);
    handle = -1;
  }
    
  return handle;

}

 /*@@
   @routine    RegisterCoord
   @date       11-12th April 1999
   @author     Gabrielle Allen
   @desc 
               Register a GF as a coordinate with a name and
               a direction.
   @enddesc 
   @calls      CCTK_RegisterCoord_ByIndex, CCTK_GetVarIndex

   @var        name        
   @vdesc      Name coordinate is registered as
   @vtype      const char *
   @vio        in
   @vcomment   The name must not be the same as the variable name
   @endvar 

   @var        gfname
   @vdesc      The name of the variable providing the coordinate
   @vtype      const char *
   @vio        in
   @vcomment
   @endvar

   @var        dir
   @vdesc      An index giving the coordinate direction
   @vtype      int
   @vio        in
   @vcomment   This should be an optional argument
   @endvar
 
   @returntype int
   @returndesc
      -1           = Duplicate coordinate name, all duplicates
                     will be ignored
      ERROR_MEMORY = Failure with malloc
      >= 0           Handle returned by coordinate registration
   @endreturndesc

   @@*/

int CCTK_RegisterCoord(const char *coordname, 
		       const char *gfname, 
		       int dir)
{
  
  int retval;
  int index;

  index = CCTK_GetVarIndex(gfname);

  if (index >= 0)
  { 
     retval = CCTK_RegisterCoord_ByIndex(coordname,index,dir);
  }
  else
  {
    /* FIXME Should this register this error or a failed to register coord error */
    retval = ERROR_INDEXNOTINRANGE;
  }

  return retval;

}


int CCTK_GetCoordIndex(const char *name)
{
  int handle;
  struct Coordprops *coord;

  for (handle = 0;;handle++)
  {
    coord = (struct Coordprops *)CCTK_GetHandledData(coordinates, handle);
    if (coord)
    {
      if (CCTK_Equals(name,(const char *)coord->name))
	return coord->index;
    }
    else
    {
      char *msg;
      msg = (char *)malloc( 100*sizeof(char)+sizeof(name) );
      sprintf(msg,"Could not find registered coordinate %s",name);
      CCTK_WARN(2,msg);
      if (msg) free(msg);
      return ERROR_COORDNOTFOUND;
    }
    
  }
}

