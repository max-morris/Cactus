 /*@@
   @file      Coord.c
   @date      11-12th April 1999
   @author    Gabrielle Allen
   @desc 
   Routines to deal with cooordinates and coordinate registration
   @enddesc 
 @@*/

/*#define DEBUG_COORD*/

#include <stdlib.h>
#include <stdio.h>

#include "cctk_Flesh.h"
#include "cctk_Coord.h"
#include "cctk_WarnLevel.h"
#include "cctk_Groups.h"
#include "StoreHandledData.h"
#include "ErrorCodes.h"
#include "cctk_FortranString.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_Coord_c)

struct Coordprops
{ 
  char *    name;
  int       index;
  CCTK_REAL origin;
  int       direction;
};

typedef struct COORD_RANGE 
{

  cGH *GH;

  struct Coordprops *props;       /* Coordinate data */

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
   @routine    CoordRegisterI
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

int CCTK_CoordRegisterI(int dir, int index, const char*name)
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
      printf(" In CCTK_CoordRegisterI\n");
      printf(" ----------------------\n");
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
    CCTK_Warn(1,__LINE__,__FILE__,"Cactus",msg);
    if (msg) free(msg);
    handle = -1;
  }
    
  return handle;

}

void FMODIFIER FORTRAN_NAME(CCTK_CoordRegisterI)(int *handle, int *dir, int *index, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name)
  *handle = CCTK_CoordRegisterI(*dir,*index, name);
  free(name);
}





 /*@@
   @routine    CoordRegister
   @date       11-12th April 1999
   @author     Gabrielle Allen
   @desc 
               Register a GF as a coordinate with a name and
               a direction.
   @enddesc 
   @calls      CCTK_CoordRegisterI, CCTK_VarIndex

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

int CCTK_CoordRegister(int dir, 
                       const char *gfname, 
                       const char *coordname)
{
  
  int retval;
  int index;

  index = CCTK_VarIndex(gfname);

  if (index >= 0)
  { 
     retval = CCTK_CoordRegisterI(dir,index,coordname);
  }
  else
  {
    /* FIXME Should this register this error or a failed to register coord error */
    retval = ERROR_INDEXNOTINRANGE;
  }

  return retval;

}

void FMODIFIER FORTRAN_NAME(CCTK_CoordRegister)(int *handle, 
                                                int *dir, 
                                                TWO_FORTSTRINGS_ARGS)
{
  TWO_FORTSTRINGS_CREATE(gf,name)
  *handle = CCTK_CoordRegister(*dir, gf, name);
  free(gf);
  free(name);
}




int CCTK_CoordRegisterRange(cGH *GH, 
                            CCTK_REAL min, 
                            CCTK_REAL max, 
                            const char *coordname)
{
  coord_range *newguy;
  int retval = 0;

  /* New coord_range */
  newguy = (coord_range *)malloc(sizeof(coord_range));

  if (newguy)
  {
    newguy->GH   = GH;

    newguy->props = CCTKi_CoordData(coordname);

#ifdef DEBUG_COORD
    printf("Registering range (%f,%f) for %s (on %x)\n",min,max,coordname,newguy);
#endif

    newguy->lower = min;
    newguy->upper = max;

    newguy->next = first;
    first = newguy;
  }
  else
  {
    char *msg = (char *) malloc (200*sizeof(char));
    sprintf (msg, "Cannot allocate data for coordinate range\n");
    CCTK_Warn (0,__LINE__,__FILE__,"Cactus", msg);
    free (msg);
  }

  return retval;

}

void FMODIFIER FORTRAN_NAME(CCTK_CoordRegisterRange)(cGH *GH, 
                                                     CCTK_REAL *lower, 
                                                     CCTK_REAL *upper, 
                                                     ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name)
  CCTK_CoordRegisterRange (GH,*lower,*upper,name);
  free(name);
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
      CCTK_Warn(2,__LINE__,__FILE__,"Cactus",msg);
      if (msg) free(msg);
      return ERROR_COORDNOTFOUND;
    }
    
  }
}

void FMODIFIER FORTRAN_NAME(CCTK_CoordIndex)(int *index, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name)
  *index = CCTK_CoordIndex (name);
  free(name);
}





int CCTK_CoordDir(const char *name)
{
  int handle;
  struct Coordprops *coord;

  for (handle = 0;;handle++)
  {
    coord = (struct Coordprops *)Util_GetHandledData(coordinates, handle);
    if (coord)
    {
      if (CCTK_Equals(name,(const char *)coord->name))
        return coord->direction;
    }
    else
    {
      char *msg;
      msg = (char *)malloc( 100*sizeof(char)+strlen(name) );
      sprintf(msg,"Could not find registered coordinate %s",name);
      CCTK_Warn(2,__LINE__,__FILE__,"Cactus",msg);
      if (msg) free(msg);
      return ERROR_COORDNOTFOUND;
    }
    
  }
}

void FMODIFIER FORTRAN_NAME(CCTK_CoordDir)(int *dir, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name)
  *dir = CCTK_CoordDir(name);
  free(name);
}





 /*@@
   @routine    CoordRange
   @date       10th January 2000
   @author     Gabrielle Allen
   @desc 
               Supplies the global range of the named coordinate.
   @enddesc 
   @calls     

   @var        
   @vdesc      
   @vtype      
   @vio        
   @vcomment   
   @endvar 

   @returntype int
   @returndesc Returns zero for success and negative for error
   @endreturndesc

   @@*/

int CCTK_CoordRange(cGH *GH, 
                    CCTK_REAL *lower, 
                    CCTK_REAL *upper, 
                    const char *name)
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
      printf("Returning range (%f,%f) for %s (from %x)\n",
             *lower,*upper,name,curr);
#endif

      return 0;
    }
  }

  return -1;

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





 /*@@
   @routine    CoordLocalRange
   @date       10th January 2000
   @author     Gabrielle Allen
   @desc 
               Returns the range of the coordinate on this processor
               For now this is done in a straightforward manner, assuming
               that a regular grid is used and that the coordinate is a
               Grid function.
   @enddesc 
   @calls     

   @var        
   @vdesc      
   @vtype      
   @vio        
   @vcomment   
   @endvar 

   @returntype int
   @returndesc
   @endreturndesc

   @@*/

int CCTK_CoordLocalRange(cGH *GH, 
                         CCTK_REAL *lower, 
                         CCTK_REAL *upper, 
                         const char *name)
{

  CCTK_REAL global_lower;
  CCTK_REAL global_upper;
  int dir = CCTK_CoordDir(name);
  
  CCTK_CoordRange(GH,&global_lower,&global_upper,name);

  *lower = global_lower+GH->cctk_lbnd[dir-1]*GH->cctk_delta_space[dir-1];
  *upper = global_lower+GH->cctk_ubnd[dir-1]*GH->cctk_delta_space[dir-1];

#ifdef DEBUG  
  printf("Upper/Lower are %f,%f\n",*lower,*upper);
#endif

  return 0;

}
  
void FMODIFIER FORTRAN_NAME(CCTK_CoordLocalRange)(int *ierr, 
                                                  cGH *GH, 
                                                  CCTK_REAL *lower, 
                                                  CCTK_REAL *upper, 
                                                  ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name)
  *ierr = CCTK_CoordLocalRange (GH,lower,upper,name);
  free(name);
}





/* ================== */
/* Internal Functions */
/* ================== */

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
      CCTK_Warn(2,__LINE__,__FILE__,"Cactus",msg);
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
      CCTK_Warn(2,__LINE__,__FILE__,"Cactus",msg);
      if (msg) free(msg);
      return NULL;
    }
    
  }
}                       





/* DEPRECATED 4.0b5 */

int CCTK_RegisterCoordI(int dir, int index, const char*name)
{
  return CCTK_CoordRegisterI(dir,index,name);
}

void FMODIFIER FORTRAN_NAME(CCTK_RegisterCoord)(int *handle, int *dir, TWO_FORTSTRINGS_ARGS)
{
  TWO_FORTSTRINGS_CREATE(gf,name)
  *handle = CCTK_RegisterCoord(*dir, gf, name);
  free(gf);
  free(name);
}

int CCTK_RegisterCoordRange(cGH *GH, 
                            CCTK_REAL min, 
                            CCTK_REAL max, 
                            const char *coordname)
{
  return CCTK_CoordRegisterRange(GH,min,max,coordname);
}

void FMODIFIER FORTRAN_NAME(CCTK_RegisterCoordRange)(cGH *GH, 
                                                     CCTK_REAL *lower, 
                                                     CCTK_REAL *upper, 
                                                     ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name)
  CCTK_RegisterCoordRange (GH,*lower,*upper,name);
  free(name);
}

int CCTK_RegisterCoord(int dir, 
                       const char *gfname, 
                       const char *coordname)
{
  return CCTK_CoordRegister(dir,gfname,coordname);
}

/* DEPRECATED */

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
      CCTK_Warn(2,__LINE__,__FILE__,"Cactus",msg);
      if (msg) free(msg);
      return ERROR_COORDNOTFOUND;
    }
    
  }

}


