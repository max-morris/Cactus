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
#include "cctk_WarnLevel.h"
#include "cctk_Coord.h"
#include "ErrorCodes.h"
#include "cctk_Groups.h"
#include "cctk_Misc.h"
#include "cctk_FortranString.h"

struct Coordprops
{ 
  char * name;
  int    index;
  CCTK_REAL origin;
  int    direction;
};

typedef struct COORD_RANGE {

  cGH *GH;

  struct Coordprops *props;	  /* Coordinate data */

  CCTK_REAL lower;                /* Lower range */
  CCTK_REAL upper;                /* Upper range */

  struct COORD_RANGE *next;       /* List */

} coord_range;

static cHandledData *coordinates = NULL;
static int num_coords = 0;
static struct COORD_RANGE *first = NULL;

struct Coordprops *CCTKi_CoordData(const char *name);
int CCTKi_CoordHande(const char *name);

 /*@@
   @routine    RegisterCoordI
   @date       11-12th April 1999
   @author     Gabrielle Allen
   @desc 
               Register a GF as a coordinate with a name, and index
	       and a direction
   @enddesc 
   @calls      Util_GetHandle, Util_NewHandle, CCTK_Warn

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

int CCTK_RegisterCoordI(int dir, int index, const char*name)
{

  int handle;
  struct Coordprops *new_coord;

  /* Check that the method hasn't already been registered */
  handle = Util_GetHandle(coordinates, name, NULL);

  if(handle < 0)
  {
    /* New extension. */
    new_coord = (struct Coordprops *)malloc(sizeof(struct Coordprops));

    if(new_coord)
    {
      /* Get a handle for it. */
      handle = Util_NewHandle(&coordinates, name, new_coord);

      /* Initialise the coordinate properties structure */
      new_coord->name      = (char *)name;
      new_coord->index     = index;
      new_coord->direction = dir;
      new_coord->origin    = 0;

      /* Remember how many methods there are */
      num_coords++;

#ifdef DEBUG_COORD
      printf(" In CCTK_RegisterCoordI\n");
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
    msg = (char *)malloc(200*sizeof(char)+strlen(name));
    sprintf(msg,"Coordinate with name -%s- already registered",name);
    CCTK_WARN(1,msg);
    if (msg) free(msg);
    handle = -1;
  }
    
  return handle;

}

void FMODIFIER FORTRAN_NAME(CCTK_RegisterCoordI)(int *handle, int *dir, int *index, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name)
  *handle = CCTK_RegisterCoordI(*dir,*index, name);
  free(name);
}


 /*@@
   @routine    RegisterCoord
   @date       11-12th April 1999
   @author     Gabrielle Allen
   @desc 
               Register a GF as a coordinate with a name and
               a direction.
   @enddesc 
   @calls      CCTK_RegisterCoordI, CCTK_VarIndex

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

int CCTK_RegisterCoord(int dir, const char *gfname, 
		       const char *coordname)
{
  
  int retval;
  int index;

  index = CCTK_VarIndex(gfname);

  if (index >= 0)
  { 
     retval = CCTK_RegisterCoordI(dir,index,coordname);
  }
  else
  {
    /* FIXME Should this register this error or a failed to register coord error */
    retval = ERROR_INDEXNOTINRANGE;
  }

  return retval;

}


void FMODIFIER FORTRAN_NAME(CCTK_RegisterCoord)(int *handle, int *dir, TWO_FORTSTRINGS_ARGS)
{
  TWO_FORTSTRINGS_CREATE(gf,name)
  *handle = CCTK_RegisterCoord(*dir, gf, name);
  free(gf);
  free(name);
}


int CCTK_RegisterCoordRange(cGH *GH, CCTK_REAL min, CCTK_REAL max, 
			    const char *coordname)
{
  coord_range *newguy;

  /* New coord_range */
  newguy = (coord_range *)malloc(sizeof(coord_range));

  newguy->GH   = GH;

  newguy->props = CCTKi_CoordData(coordname);

#ifdef DEBUG_COORD
  printf("Registering range (%f,%f) for %s (on %x)\n",min,max,coordname,newguy);
#endif

  newguy->lower = min;
  newguy->upper = max;

  newguy->next = first;
  first = newguy;

  return 1;

}

void FMODIFIER FORTRAN_NAME(CCTK_RegisterCoordRange)(cGH *GH, CCTK_REAL *lower, CCTK_REAL *upper, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name)
  CCTK_RegisterCoordRange (GH,*lower,*upper,name);
  free(name);
}

int CCTKi_CoordHandle(const char *name)
{
  int handle;
  struct Coordprops *coord;

  for (handle = 0;;handle++)
  {
    coord = (struct Coordprops *)Util_GetHandledData(coordinates, handle);
    if (coord)
    {
      if (CCTK_Equals(name,(const char *)coord->name))
	return handle;
    }
    else
    {
      char *msg;
      msg = (char *)malloc( 100*sizeof(char)+strlen(name) );
      sprintf(msg,"Could not find registered coordinate %s",name);
      CCTK_WARN(2,msg);
      if (msg) free(msg);
      return ERROR_COORDNOTFOUND;
    }
    
  }
}                       

struct Coordprops *CCTKi_CoordData(const char *name)
{
  int handle;
  struct Coordprops *coord;

  for (handle = 0;;handle++)
  {
    coord = (struct Coordprops *)Util_GetHandledData(coordinates, handle);
    if (coord)
    {
      if (CCTK_Equals(name,(const char *)coord->name))
	return coord;
    }
    else
    {
      char *msg;
      msg = (char *)malloc( 100*sizeof(char)+strlen(name) );
      sprintf(msg,"Could not find registered coordinate %s",name);
      CCTK_WARN(2,msg);
      if (msg) free(msg);
      return NULL;
    }
    
  }
}                       

int CCTK_CoordIndex(const char *name)
{
  int handle;
  struct Coordprops *coord;

  for (handle = 0;;handle++)
  {
    coord = (struct Coordprops *)Util_GetHandledData(coordinates, handle);
    if (coord)
    {
      if (CCTK_Equals(name,(const char *)coord->name))
	return coord->index;
    }
    else
    {
      char *msg;
      msg = (char *)malloc( 100*sizeof(char)+strlen(name) );
      sprintf(msg,"Could not find registered coordinate %s",name);
      CCTK_WARN(2,msg);
      if (msg) free(msg);
      return ERROR_COORDNOTFOUND;
    }
    
  }
}

void FMODIFIER FORTRAN_NAME(CCTK_CoordIndex)(int *handle, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name)
  *handle = CCTK_CoordIndex (name);
  free(name);
}


CCTK_REAL CCTK_CoordOrigin(const char *name)
{
  int handle;
  struct Coordprops *coord;

  for (handle = 0;;handle++)
  {
    coord = (struct Coordprops *)Util_GetHandledData(coordinates, handle);
    if (coord)
    {
      if (CCTK_Equals(name,(const char *)coord->name))
	return coord->origin;
    }
    else
    {
      char *msg;
      msg = (char *)malloc( 100*sizeof(char)+strlen(name) );
      sprintf(msg,"Could not find registered coordinate %s",name);
      CCTK_WARN(2,msg);
      if (msg) free(msg);
      return ERROR_COORDNOTFOUND;
    }
    
  }

}


int CCTK_CoordRange(cGH *GH, CCTK_REAL *lower, CCTK_REAL *upper, const char *name)
{
  coord_range *curr;

  for (curr=first;curr;curr=curr->next)
  {

#ifdef DEBUG_COORD
    printf("name  = %s, currname = %s\n",name,curr->props->name);
#endif

    if (curr->GH == GH && CCTK_Equals(name,curr->props->name))
    {
      *lower = curr->lower;
      *upper = curr->upper;

#ifdef DEBUG_COORD
      printf("Returning range (%f,%f) for %s (from %x)\n",*lower,*upper,name,curr);
#endif

      return;
    }
  }
}


void FMODIFIER FORTRAN_NAME(CCTK_CoordRange)(int *ierr, 
					     cGH *GH, 
					     CCTK_REAL *lower, 
					     CCTK_REAL *upper, 
					     ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name)
  *ierr = CCTK_CoordRange (GH,lower,upper,name);
  free(name);
}
