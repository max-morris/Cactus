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
#include <cctk_Math.h>



double CCTK_copysign(double x, double y)
{
#ifdef HAVE_COPYSIGN
  return copysign(x, y);
#else
  return y >= 0.0 ? fabs(x) : -fabs(x);
#endif
}

double CCTK_FNAME(CCTK_copysign)(double *x, double *y);
double CCTK_FNAME(CCTK_copysign)(double *x, double *y)
{
  return CCTK_copysign(*x, *y);
}



int CCTK_fpclassify(double x)
{
#ifdef HAVE_FPCLASSIFY
  return fpclassify(x);
#else
  return 0;                     /* don't know what else to return */
#endif
}

int CCTK_FNAME(CCTK_fpclassify)(double *x);
int CCTK_FNAME(CCTK_fpclassify)(double *x)
{
  return CCTK_fpclassify(*x);
}



int CCTK_isfinite(double x)
{
#ifdef HAVE_ISFINITE
  return isfinite(x);
#else
  return 1;                     /* default */
#endif
}

int CCTK_FNAME(CCTK_isfinite)(double *x);
int CCTK_FNAME(CCTK_isfinite)(double *x)
{
  return CCTK_isfinite(*x);
}



int CCTK_isinf(double x)
{
#ifdef HAVE_ISINF
  return isinf(x);
#else
  return 0;                     /* default */
#endif
}

int CCTK_FNAME(CCTK_isinf)(double *x);
int CCTK_FNAME(CCTK_isinf)(double *x)
{
  return CCTK_isinf(*x);
}



int CCTK_isnan(double x)
{
#ifdef HAVE_ISNAN
  return isnan(x);
#else
  return 0;                     /* default */
#endif
}

int CCTK_FNAME(CCTK_isnan)(double *x);
int CCTK_FNAME(CCTK_isnan)(double *x)
{
  return CCTK_isnan(*x);
}



int CCTK_isnormal(double x)
{
#ifdef HAVE_ISNORMAL
  return isnormal(x);
#else
  return 1;                     /* default */
#endif
}

int CCTK_FNAME(CCTK_isnormal)(double *x);
int CCTK_FNAME(CCTK_isnormal)(double *x)
{
  return CCTK_isnormal(*x);
}



int CCTK_signbit(double x)
{
#ifdef HAVE_SIGNBIT
  return signbit(x);
#else
  return x < 0.0;
#endif
}

int CCTK_FNAME(CCTK_signbit)(double *x);
int CCTK_FNAME(CCTK_signbit)(double *x)
{
  return CCTK_signbit(*x);
}
