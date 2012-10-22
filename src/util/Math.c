 /*@@
   @file      Math.c
   @date      2012-10-17
   @author    Erik Schnetter
   @desc
              Miscellaneous math routines, providing fallback C
              implementations for broken C++ compilers, and providing
              dummy implementations for broken C compilers.
   @enddesc
 @@*/

#include <math.h>
#include <cctk_Config.h>



double CCTK_copysign(double x, double y)
{
#ifdef HAVE_COPYSIGN
  return copysign(x, y);
#else
  return y >= 0.0 ? fabs(x) : -fabs(x);
#endif
}

int CCTK_fpclassify(double x)
{
#ifdef HAVE_FPCLASSIFY
  return fpclassify(x);
#else
  return 0;                     /* don't know what else to return */
#endif
}

int CCTK_isfinite(double x)
{
#ifdef HAVE_ISFINITE
  return isfinite(x);
#else
  return 1;                     /* default */
#endif
}

int CCTK_isinf(double x)
{
#ifdef HAVE_ISINF
  return isinf(x);
#else
  return 0;                     /* default */
#endif
}

int CCTK_isnan(double x)
{
#ifdef HAVE_ISNAN
  return isnan(x);
#else
  return 0;                     /* default */
#endif
}

int CCTK_isnormal(double x)
{
#ifdef HAVE_ISNORMAL
  return isnormal(x);
#else
  return 1;                     /* default */
#endif
}

int CCTK_signbit(double x)
{
#ifdef HAVE_SIGNBIT
  return signbit(x);
#else
  return x < 0.0;
#endif
}
