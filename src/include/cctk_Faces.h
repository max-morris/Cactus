 /*@@
   @header    cctk_Faces.h
   @date      11 Feb 2003
   @author    David Rideout
   @desc 
              Macros for generic specification of sets of 'faces' of an 
	      'n-cube'
   @enddesc 
   @version   $Header$
 @@*/

#ifndef _CCTK_FACES_H_
#define _CCTK_FACES_H_ 1

/* The set of all faces */
#define CCTK_ALL_FACES 1073741823
/* (This is 2^30-1, so it will set the 'last' 30 bits to 1.)
 * (The 'top' two bits are useless because there is no unsigned int in
 * Fortran, and face bits come in pairs.)
 */

/* Here will be placed macros which provide a user friendly interface
 * to a general specification for expressing sets of faces of an
 * n-dimensional rectangle.  Each face will be assigned one bit in a
 * word.  Then one can bitwise-or a number of these together to
 * specify a set of faces.  The macros will make this user-friendly
 * somehow (and accessible to Fortran).
 * 
 * Example:
 *
 * #define FACE0 1>>0
 * #define FACE1 1>>1
 * etc.
 *
 * where FACE0 may be the negative x-face, FACE1 the positive x-face, etc.  */

#endif /* _CCTK_FACES_H_ */
