 /*@@
   @header    cctk_Coord.h
   @date      Mon April 12 1999
   @author    Gabrielle Allen
   @desc 
   Prototypes and constants for coordinate functions
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTK_COORD_H_
#define _CCTK_COORD_H_

#ifdef __cplusplus 
extern "C" 
{
#endif

int CCTK_CoordIndex(const char *name);

int CCTK_CoordDir(const char *name);

int CCTK_CoordRange(cGH *GH, 
                    CCTK_REAL *lower, 
                    CCTK_REAL *upper, 
                    const char *name);

int CCTK_CoordLocalRange(cGH *GH, 
                         CCTK_REAL *lower, 
                         CCTK_REAL *upper, 
                         const char *name);

int CCTK_CoordRegister(int dir, 
                       const char *gfname, 
                       const char *coordname);

int CCTK_CoordRegisterI(int dir, 
                        int index, 
                        const char *name);

int CCTK_CoordRegisterRange(cGH *GH, 
                            CCTK_REAL min, 
                            CCTK_REAL max, 
                            const char *coordname);

/* BEGIN DEPRECATED 4.0b5 */

int CCTK_RegisterCoord(int dir, const char *gfname, const char *coordname);

int CCTK_RegisterCoordI(int dir, int index, const char *name);

int CCTK_RegisterCoordRange( cGH *GH, CCTK_REAL min, CCTK_REAL max, const char *coordname);

/* END DEPRECATED 4.0b5 */

/* DEPRECATED */

CCTK_REAL CCTK_CoordOrigin(const char *name);

#ifdef __cplusplus 
}
#endif

#endif /* _CCTK_COORD_H_ */
