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
#include <string.h>

#include "cctk_Flesh.h"
#include "cctk_Coord.h"
#include "cctk_FortranString.h"
#include "cctk_Groups.h"
#include "cctk_WarnLevel.h"

#include "util_Hash.h"
#include "ErrorCodes.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_Coord_c)


/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/

struct Coordsystem
{
  int dimension;
  struct Coordprops *coords;
};

struct Coordprops
{ 
  char *    name;
  int       index;
  struct Coordpropslist *list;
};

struct Coordpropslist
{

  cGH *GH;

  CCTK_REAL lower;                /* Lower range */
  CCTK_REAL upper;                /* Upper range */

  struct Coordpropslist *next;     /* List */
};


/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/

#define INITIAL_HASH_SIZE    64

static uHash *CoordSystemHash = NULL;


/********************************************************************
 *********************     External Routines   **********************
 ********************************************************************/

 /*@@
   @routine    CoordRegisterSystem
   @date       18th June 2000
   @author     Gabrielle Allen
   @desc 
               Register a coordinate system name and its dimension
   @enddesc 
   @calls     

   @returntype int
   @returndesc 
   Returns 0 for success and negative integer for failure
   0  = success
   -1 = this system name already registered with different dimension    
   -2 = dimension not valid
   @endreturndesc

   @@*/

int CCTK_CoordRegisterSystem(int dim, 
                             const char *systemname)
{
  int i;
  int retval=-1;
  struct Coordsystem *data;

  /* Create hash table if not already done */
  if (!CoordSystemHash)
  {
    CoordSystemHash = Util_HashCreate(INITIAL_HASH_SIZE);
  }

  /* Check if system already exists */
  data = (struct Coordsystem *)Util_HashData(CoordSystemHash, 
                                             strlen(systemname),
                                             systemname,
                                             0);
  if (!data) 
  {
    /* Allocate the memory */
    data = (struct Coordsystem *)malloc(sizeof(struct Coordsystem));
    
    /* Set the data and store */
    if (data && CoordSystemHash)
    {
      if (dim > 0)
      {
        data->dimension = dim;
        data->coords = (struct Coordprops *)malloc
          ((data->dimension)*sizeof(struct Coordprops));
        for (i=0;i<dim;i++)
        {
          data->coords[i].name = NULL;
          data->coords[i].list = NULL;
        }
        retval = Util_HashStore(CoordSystemHash,strlen(systemname),
                                systemname,0,data);
      }
      else
      {
        retval = -2;
        CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
                   "CCTK_CoordRegisterSystem: Dimension %d not valid",
                   dim);
      }
    }
  }
  else
  {
    if (data->dimension == dim)
    {
      retval = 0;
      CCTK_VWarn(4,__LINE__,__FILE__,"Cactus",
                 "CCTK_CoordRegisterSystem: System %s already registered",
                 systemname);      
    }
    else
    {
      retval = -1;
      CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
                 "CCTK_CoordRegisterSystem: System %s already registered with different dimension",
                 systemname);
    }
  }

  return retval;
}

void CCTK_FCALL CCTK_FNAME(CCTK_CoordRegisterSystem)
     (int *ierr,int *dim,ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(systemname)
  *ierr=CCTK_CoordRegisterSystem (*dim,systemname);
  free(systemname);
}



 /*@@
   @routine    CoordRegisterData
   @date       11-12th April 1999
   @author     Gabrielle Allen
   @desc 
               Register a GF as a coordinate with a name and
               a direction.
   @enddesc 
   @calls      

   @returntype int
   @returndesc 
   Returns 0 for success and negative integer for failure
   0  = success
   -1 = no coordinate systems registered
   -2 = this coordinate system not registered
   -3 = direction outside system dimension
   -4 = coordinate name already registered
   -5 = coordinate direction already registered
   @endreturndesc

   @@*/

int CCTK_CoordRegisterData(int dir, 
                           const char *gfname, 
                           const char *coordname,
                           const char *systemname)
{
  int i;  
  int retval=0;
  int index;
  int dup=0;
  struct Coordsystem *data=NULL;

  if (CoordSystemHash)
  {
    /* Get structure for this system name */
    data = (struct Coordsystem *)Util_HashData(CoordSystemHash, 
                                               strlen(systemname),
                                               systemname,
                                               0);
    
    if (data)
    {
      
      /* Check direction correct */
      if (dir<1 || dir>data->dimension)
      {
        retval = -3;
        CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
                   "CCTK_CoordRegisterData: Direction %d outside system dimension %d",
                   dir,data->dimension);
      }
      else
      {

        /* Check name not already registered */
        for (i=0;i<data->dimension;i++)
        {
          if (data->coords[i].name)
          {
            if (CCTK_Equals(data->coords[i].name,coordname))
            {
              dup = 1;
              retval = -4;
              CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
                         "CCTK_CoordRegisterData: Coordinate name %s already registered",coordname);
            }
          }
        }
        /* Check direction not already registered */
        if (data->coords[dir-1].name)
        {
          dup = 1;
          retval = -5;
          CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
                     "CCTK_CoordRegisterData: Coordinate direction %d already registered",dir);
        }

        if (dup == 0)
        {
          /* Register name */
          data->coords[dir-1].name = strdup(coordname);
          /* Register index if grid variable */
          index = CCTK_VarIndex(gfname);
          if (index >= 0)
          {  
            data->coords[dir-1].index=index;
          }
          else
          {
            data->coords[dir-1].index=-1;
            CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
                       "CCTK_CoordRegisterData: No grid variable registered");
          }
        }
      }
    }
    else
    {
      retval = -2;
      CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
                 "CCTK_CoordRegisterData: System %s not registered",systemname);
    }
  }
  else
  {
    retval = -1;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
               "CCTK_CoordRegisterData: No coordinate systems registered",systemname);
  }

  return retval;

}

void CCTK_FCALL CCTK_FNAME(CCTK_CoordRegisterData)
     (int *handle,int *dir,THREE_FORTSTRINGS_ARGS)
{
  THREE_FORTSTRINGS_CREATE(gf,name,systemname)
  *handle = CCTK_CoordRegisterData(*dir, gf, name,systemname);
  free(gf);
  free(name);
  free(systemname);
}



int CCTK_CoordRegisterRange(cGH *GH, 
                            CCTK_REAL min, 
                            CCTK_REAL max, 
                            int dir,
                            const char *coordname,
                            const char *systemname)
{
  int i;
  int retval = 0;
  int index = -1;
  struct Coordpropslist *newguy;
  struct Coordsystem *data;

  if (CoordSystemHash)
  {
    data = (struct Coordsystem *)Util_HashData(CoordSystemHash, 
                                               strlen(systemname),
                                               systemname,
                                               0);
    if (data)
    {
      if (dir>-1)
      {
        if (dir == 0 || dir > data->dimension)
        {
          retval = -3;
          CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
                     "CCTK_CoordRegisterRange: Direction %d outside system dimension %d",
                     dir,data->dimension);
        }          
        if (data->coords[dir-1].name)
        {
          index = dir-1;
        }
        else
        {
          retval = -5;
          CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
                     "CCTK_CoordRegisterRange: Coordinate direction %d not registered",dir);
        }
      }
      else
      {
        for (i=0;i<data->dimension;i++)
        {
          if (data->coords[i].name && 
              CCTK_Equals(data->coords[i].name,coordname))
          {
            index = i;
          }
        }
        if (index == -1)
        {
          retval = -4;
          CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
                     "CCTK_CoordRegisterRange: Coordinate name %s not registered",coordname);
        }          
      }

      if (index != -1)
      {
        /* New coord_range */
        newguy = (struct Coordpropslist *)
          malloc(sizeof(struct Coordpropslist));
      
        if (newguy)
        {
          newguy->GH    = GH;
          newguy->lower = min;
          newguy->upper = max;
          newguy->next  = data->coords[index].list;
          data->coords[index].list = newguy;
        }
        else
        {
          retval = -6;
          CCTK_Warn (1,__LINE__,__FILE__,"Cactus",
                     "CCTK_CoordRegisterRange: Cannot allocate data for coordinate range");
        }
      }
    }
    else
    {
      retval = -2;
      CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
                 "CCTK_CoordRegisterRange: System %s not registered",systemname);
    }
  }
  else
  {
    retval = -1;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
               "CCTK_CoordRegisterRange: No coordinate systems registered",systemname);
  }

  return retval;

}

void CCTK_FCALL CCTK_FNAME(CCTK_CoordRegisterRange)
     (int *ierr,
      cGH *GH,
      CCTK_REAL *lower,
      CCTK_REAL *upper,
      int *dir,
      TWO_FORTSTRINGS_ARGS)
{
  TWO_FORTSTRINGS_CREATE(name,systemname)
  *ierr=CCTK_CoordRegisterRange (GH,*lower,*upper,*dir,name,systemname);
  free(name);
  free(systemname);
}





int CCTK_CoordIndex(int dir, const char *name, const char *systemname)
{
  int i;
  int index=-1;
  int foundit = 0;
  struct Coordsystem *data;

  if (CoordSystemHash)
  {
    data = (struct Coordsystem *)Util_HashData(CoordSystemHash, 
                                               strlen(systemname),
                                               systemname,
                                               0);
    if (data)
    {
      if (dir>-1)
      {
        if (dir>data->dimension)
        {
          index = -4;
          CCTK_VWarn(2,__LINE__,__FILE__,"Cactus",
                     "CCTK_CoordIndex: Direction %d outside dimension %d",
                     dir,data->dimension);
        }
        else
        {
          index = data->coords[dir-1].index;
        }
      }
      else
      {
        for (i=0;i<data->dimension;i++)
        {
          if (data->coords[i].name && 
              CCTK_Equals(data->coords[i].name,name))
          {
            foundit = 1;
            index = data->coords[i].index;
          }
        }
        if (foundit == 0)
        {
          index = -3;
          CCTK_VWarn(4,__LINE__,__FILE__,"Cactus",
                     "CCTK_CoordIndex: Coordinate name %s not found",
                     name);
        }
      }
    }
    else
    {
      index = -2;
      CCTK_VWarn(4,__LINE__,__FILE__,"Cactus",
                 "CCTK_CoordIndex: System %s not registered",systemname);
    }
  }
  else
  {
    CCTK_Warn(4,__LINE__,__FILE__,"Cactus",
              "CCTK_CoordIndex: No coordinate systems registered");
    index = -1;
  }

  return index;
}

void CCTK_FCALL CCTK_FNAME(CCTK_CoordIndex)
     (int *index, int *dir, TWO_FORTSTRINGS_ARGS)
{
  TWO_FORTSTRINGS_CREATE(name,systemname)
  *index = CCTK_CoordIndex (*dir,name,systemname);
  free(name);
  free(systemname);
}






int CCTK_CoordSystemDim(const char *systemname)
{
  int dim;
  struct Coordsystem *data;

  if (CoordSystemHash)
  {
    data = (struct Coordsystem *)Util_HashData(CoordSystemHash, 
                                               strlen(systemname),
                                               systemname,
                                               0);
    
    if (data)
    {
      dim = data->dimension;
    }
    else
    {
      CCTK_VWarn(2,__LINE__,__FILE__,"Cactus",
                 "CCTK_CoordSystemDim: System %s not registered",systemname);
      dim = -2;
    }
  }
  else
  {
    CCTK_Warn(2,__LINE__,__FILE__,"Cactus",
              "CCTK_CoordSystemDim: No coordinate systems registered");
    dim = -1;
  }
  
  return dim;
}

void CCTK_FCALL CCTK_FNAME(CCTK_CoordSystemDim)
     (int *dim, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(systemname)
  *dim = CCTK_CoordSystemDim(systemname);
  free(systemname);
}




 /*@@
   @routine    CoordDir
   @date       18th June 2000
   @author     Gabrielle Allen
   @desc 
               Supplies the direction of a coordinate
   @enddesc 
   @calls     

   @returntype int
   @returndesc 
   Returns the direction, or a negative integer for an error
   -1 = coordinate system not registered
   -2 = coordinate not found in this system
   @endreturndesc

   @@*/

int CCTK_CoordDir(const char *name,const char *systemname)
{
  int i;
  int dir = -1;
  struct Coordsystem *data;

  data = (struct Coordsystem *)Util_HashData(CoordSystemHash, 
                                             strlen(systemname),
                                             systemname,
                                             0);

  if (data)
  {
    for (i=0;i<data->dimension;i++)
    {
      if (CCTK_Equals(data->coords[i].name,name))
      {
        dir = i+1;
      }
    }
    if (dir < 1)
    {
      dir = -2;
      CCTK_VWarn(2,__LINE__,__FILE__,"Cactus",
                "CCTK_CoordDir: Could not find coordinate %s in %s",
                 name,systemname);
    }    
  }
  else
  {
    dir = -1;
    CCTK_VWarn(2,__LINE__,__FILE__,"Cactus",
               "CCTK_CoordSystemDim: System %s not registered",systemname);
  }
  return dir;
}

void CCTK_FCALL CCTK_FNAME(CCTK_CoordDir)
     (int *dir, TWO_FORTSTRINGS_ARGS)
{
  TWO_FORTSTRINGS_CREATE(name,systemname)
  *dir = CCTK_CoordDir(name,systemname);
  free(name);
  free(systemname);
}





 /*@@
   @routine    CoordRange
   @date       10th January 2000
   @author     Gabrielle Allen
   @desc 
               Supplies the global range of the named coordinate.
	       
	       You specify the direction (coorddir=-1;1,2,...) 
	       or the name (coordname). The name will be used 
	       if coordir==-1
	       

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
                    int coorddir,
                    const char *coordname,
                    const char *systemname)
{
  int i;
  int retval=0;
  struct Coordpropslist *curr;
  struct Coordsystem    *data;
  struct Coordprops     *coord;


  if (lower == NULL || upper == NULL)
  {
    CCTK_Warn (2, __LINE__, __FILE__, "Cactus",
               "CCTK_CoordRange: NULL pointer(s) passed for lower/upper");
    retval = -1;
  }
  else if (coorddir <= 0 && coordname == NULL)
  {
    CCTK_Warn (2, __LINE__, __FILE__, "Cactus",
               "CCTK_CoordRange: No coordinate name given");
    retval = -2;
  }
  else if (systemname == NULL)
  {
    CCTK_Warn (2, __LINE__, __FILE__, "Cactus",
               "CCTK_CoordRange: No coordinate system name given");
    retval = -3;
  }
  else if (CoordSystemHash)
  {
    data = (struct Coordsystem *)Util_HashData(CoordSystemHash, 
                                               strlen(systemname),
                                               systemname,
                                               0);

    if (data)
    {
      if (coorddir>0)
      {
        coord = &data->coords[coorddir-1];
      }
      else
      {
        coord = NULL;
        for (i=0;i<data->dimension;i++)
        {
          if (CCTK_Equals(data->coords[i].name,coordname))
          {
            coord = &data->coords[i];
            break;
          }
        }
        if (coord == NULL)
        {
          CCTK_VWarn(2, __LINE__, __FILE__, "Cactus",
                     "CCTK_CoordRange: Coordinate name '%s' not registered",
                     coordname);
          retval = -4;
        }
      }
      if (coord)
      {
        for (curr=coord->list;curr;curr=curr->next)
        {

#ifdef DEBUG_COORD
          printf("curr  = %p\n",curr);
          printf("lower = %f\n",curr->lower);
          printf("upper = %f\n",curr->upper);
          printf("next  = %p\n",curr->next);
#endif

          if (curr->GH == GH)
          {
            *lower = curr->lower;
            *upper = curr->upper;

#ifdef DEBUG_COORD
            printf("Returning range (%f,%f) (from %p)\n", *lower,*upper,curr);
#endif
          }
        }
      }
    }
    else
    {
      CCTK_VWarn(2, __LINE__, __FILE__, "Cactus",
                 "CCTK_CoordRange: Coordinate system '%s' not registered",
                 systemname);
      retval = -5;
    }
  }
  else
  {
    CCTK_Warn(2, __LINE__, __FILE__, "Cactus",
              "CCTK_CoordRange: No coordinate systems registered");
    retval = -6;
  }

  return retval;
}

void CCTK_FCALL CCTK_FNAME(CCTK_CoordRange)
     (int *ierr,
      cGH *GH,
      CCTK_REAL *lower,
      CCTK_REAL *upper,
      int *dir,
      TWO_FORTSTRINGS_ARGS)
{
  TWO_FORTSTRINGS_CREATE(name,systemname)
  *ierr = CCTK_CoordRange (GH,lower,upper,*dir,name,systemname);
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
                         int dir,
                         const char *name,
                         const char *systemname)
{

  CCTK_REAL global_lower;
  CCTK_REAL global_upper;
  
  CCTK_CoordRange(GH,&global_lower,&global_upper,dir,name,systemname);

  *lower = global_lower+GH->cctk_lbnd[dir-1]*GH->cctk_delta_space[dir-1];
  *upper = global_lower+GH->cctk_ubnd[dir-1]*GH->cctk_delta_space[dir-1];

#ifdef DEBUG  
  printf("Upper/Lower are %f,%f\n",*lower,*upper);
#endif

  return 0;

}
  
void CCTK_FCALL CCTK_FNAME(CCTK_CoordLocalRange)
     (int *ierr,
      cGH *GH,
      CCTK_REAL *lower,
      CCTK_REAL *upper,
      int *dir,
      TWO_FORTSTRINGS_ARGS)
{
  TWO_FORTSTRINGS_CREATE(name,systemname)
  *ierr = CCTK_CoordLocalRange (GH,lower,upper,*dir,name,systemname);
  free(name);
  free(systemname);
}


