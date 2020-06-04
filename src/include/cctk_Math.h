 /*@@
   @header    cctk_Math.h
   @date      2012-10-17
   @author    Erik Schnetter
   @desc
              Miscellaneous math routines, providing fallback C
              implementations for broken C++ compilers, and providing
              dummy implementations for broken C compilers.
   @enddesc
 @@*/

#ifndef _CCTK_MATH_H_
#define _CCTK_MATH_H_

#ifdef __cplusplus
extern "C" {
#endif

double CCTK_copysign(double x, double y);
int CCTK_fpclassify(double x);
int CCTK_isfinite(double x);
int CCTK_isinf(double x);
int CCTK_isnan(double x);
int CCTK_isnormal(double x);
int CCTK_signbit(double x);

  /* int CCTK_IEEE_fpclassify(double x); */
int CCTK_IEEE_isfinite(double x);
int CCTK_IEEE_isinf(double x);
int CCTK_IEEE_isnan(double x);
int CCTK_IEEE_isnormal(double x);
int CCTK_IEEE_signbit(double x);

#ifdef __cplusplus
}   
#endif

#endif  /* #ifndef _CCTK_MATH_H_ */
