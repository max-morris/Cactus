 /*@@
   @header    Coord.h
   @date      Mon April 12 1999
   @author    Gabrielle Allen
   @desc 
   Prototypes and constants for coordinate functions
   @enddesc 
   @version $Header$
 @@*/

#ifndef _COORD_H_
#define _COORD_H_

/* Prototypes */

#ifdef __cplusplus 
extern "C" {
#endif


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

  CCTK_REAL lower;                      /* Lower range */
  CCTK_REAL upper;                      /* Upper range */

  struct COORD_RANGE *next;        /* List */

} coord_range;


int CCTK_CoordRange(cGH *GH, CCTK_REAL *lower, CCTK_REAL *upper, const char *name);

int CCTK_RegisterCoord(const char *coordname, 
		       const char *gfname, 
		       int dir);

int CCTK_RegisterCoordI(const char *name, int index, int dir);

int CCTK_CoordIndex(const char *name);

CCTK_REAL CCTK_CoordOrigin(const char *name);

#ifdef __cplusplus 
}
#endif

#endif



