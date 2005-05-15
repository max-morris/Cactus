 /*@@
   @file      Complex.c
   @date      Tue Dec 14 12:09:43 1999
   @author    Tom Goodale
   @desc
              Complex variable stuff
   @enddesc
   @version   $Id$
 @@*/

#include <math.h>

#include "cctk_Flesh.h"
#include "cctk_Complex.h"

static const char *rcsid = "$Header$";

CCTK_FILEVERSION(main_Complex_c);


/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/

/********************************************************************
 ********************* Local Routine Prototypes *********************
 ********************************************************************/

/********************************************************************
 ********************* Other Routine Prototypes *********************
 ********************************************************************/

/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/

/********************************************************************
 *********************     External Routines   **********************
 ********************************************************************/

 /*@@
   @routine    CCTK_Cmplx
   @date       Tue Dec 14 12:16:01 1999
   @author     Tom Goodale
   @desc
               Turns two reals into a complex number
   @enddesc
 
   @var        Re
   @vdesc      Real part
   @vtype      CCTK_REAL
   @vio        in
   @endvar
   @var        Im
   @vdesc      Imaginary part
   @vtype      CCTK_REAL
   @vio        in
   @endvar

   @returntype CCTK_COMPLEX
   @returndesc
               The complex number
   @endreturndesc
@@*/
#define DEFINE_CCTK_CMPLX(CCTK_Cmplx, cctk_real, cctk_complex)                \
cctk_complex CCTK_Cmplx (cctk_real Re, cctk_real Im)                          \
{                                                                             \
  cctk_complex result;                                                        \
                                                                              \
                                                                              \
  result.Re = Re;                                                             \
  result.Im = Im;                                                             \
                                                                              \
  return (result);                                                            \
}


 /*@@
   @routine    CCTK_CmplxReal
   @date       Tue Dec 14 12:22:41 1999
   @author     Tom Goodale
   @desc
               Returns the real part of a complex number.
   @enddesc

   @var        complex_number
   @vdesc      The complex number
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar

   @returntype CCTK_REAL
   @returndesc
               The real part
   @endreturndesc
@@*/
#define DEFINE_CCTK_CMPLX_REAL(CCTK_Cmplx, cctk_real, cctk_complex)           \
cctk_real CCTK_Cmplx##Real (cctk_complex complex_number)                      \
{                                                                             \
  return (complex_number.Re);                                                 \
}


 /*@@
   @routine    CCTK_CmplxImag
   @date       Tue Dec 14 12:22:41 1999
   @author     Tom Goodale
   @desc
               Returns the imaginary part of a complex number.
   @enddesc

   @var        complex_number
   @vdesc      The complex number
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar

   @returntype CCTK_REAL
   @returndesc
               The imaginary part
   @endreturndesc
@@*/
#define DEFINE_CCTK_CMPLX_IMAG(CCTK_Cmplx, cctk_real, cctk_complex)           \
cctk_real CCTK_Cmplx##Imag (cctk_complex complex_number)                      \
{                                                                             \
  return (complex_number.Im);                                                 \
}


 /*@@
   @routine    CCTK_CmplxConjg
   @date       Tue Dec 14 12:22:41 1999
   @author     Tom Goodale
   @desc
               Returns the complex conjugate of a complex number.
   @enddesc
 
   @var       in
   @vdesc     The complex number
   @vtype     CCTK_COMPLEX
   @vio       in
   @endvar

   @returntype CCTK_COMPLEX
   @returndesc
               The complex conjugate
   @endreturndesc
@@*/
#define DEFINE_CCTK_CMPLX_CONJG(CCTK_Cmplx, cctk_real, cctk_complex)          \
cctk_complex CCTK_Cmplx##Conjg (cctk_complex complex_number)                  \
{                                                                             \
  cctk_complex result;                                                        \
                                                                              \
                                                                              \
  result.Re =  complex_number.Re;                                             \
  result.Im = -complex_number.Im;                                             \
  return (result);                                                            \
}


 /*@@
   @routine    CCTK_CmplxAbs
   @date       Tue Dec 14 12:26:33 1999
   @author     Tom Goodale
   @desc
               Return the absolute value of a complex number.
   @enddesc
 
   @var        in
   @vdesc      The complex number
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar

   @returntype CCTK_REAL
   @returndesc
               The absolute value of the complex number
   @endreturndesc
@@*/
#define DEFINE_CCTK_CMPLX_ABS(CCTK_Cmplx, cctk_real, cctk_complex)            \
cctk_real CCTK_Cmplx##Abs (cctk_complex complex_number)                       \
{                                                                             \
  return (hypot (complex_number.Re, complex_number.Im));                      \
}


 /*@@
   @routine    CCTK_CmplxAdd
   @date       Sat Dec 4 12:11:04 1999
   @author     Gabrielle Allen
   @desc
               Adds two complex numbers
   @enddesc
 
   @var        a
   @vdesc      First summand
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar
   @var        b
   @vdesc      Second summand
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar

   @returntype CCTK_COMPLEX
   @returndesc
               The sum of a and b
   @endreturndesc
@@*/
#define DEFINE_CCTK_CMPLX_ADD(CCTK_Cmplx, cctk_real, cctk_complex)            \
cctk_complex CCTK_Cmplx##Add (cctk_complex a, cctk_complex b)                 \
{                                                                             \
  cctk_complex result;                                                        \
                                                                              \
                                                                              \
  result.Re = a.Re + b.Re;                                                    \
  result.Im = a.Im + b.Im;                                                    \
  return (result);                                                            \
}


 /*@@
   @routine    CCTK_CmplxSub
   @date       Sat Dec 4 12:11:04 1999
   @author     Gabrielle Allen
   @desc
               Subtracts two complex numbers
   @enddesc
 
   @var        a
   @vdesc      First operand
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar
   @var        b
   @vdesc      Second operand
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar

   @returntype CCTK_COMPLEX
   @returndesc
               The difference
   @endreturndesc
@@*/
#define DEFINE_CCTK_CMPLX_SUB(CCTK_Cmplx, cctk_real, cctk_complex)            \
cctk_complex CCTK_Cmplx##Sub (cctk_complex a, cctk_complex b)                 \
{                                                                             \
  cctk_complex result;                                                        \
                                                                              \
                                                                              \
  result.Re = a.Re - b.Re;                                                    \
  result.Im = a.Im - b.Im;                                                    \
  return (result);                                                            \
}


 /*@@
   @routine    CCTK_CmplxMul
   @date       Sat Dec 4 12:11:04 1999
   @author     Gabrielle Allen
   @desc
               Multiplies two complex numbers
   @enddesc
 
   @var        a
   @vdesc      First operand
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar
   @var        b
   @vdesc      Second operand
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar

   @returntype CCTK_COMPLEX
   @returndesc
               The product
   @endreturndesc
@@*/
#define DEFINE_CCTK_CMPLX_MUL(CCTK_Cmplx, cctk_real, cctk_complex)            \
cctk_complex CCTK_Cmplx##Mul (cctk_complex a, cctk_complex b)                 \
{                                                                             \
  cctk_complex result;                                                        \
                                                                              \
                                                                              \
  result.Re = a.Re*b.Re - a.Im*b.Im;                                          \
  result.Im = a.Im*b.Re + a.Re*b.Im;                                          \
  return (result);                                                            \
}


 /*@@
   @routine    CCTK_CmplxDiv
   @date       Sat Dec 4 12:11:04 1999
   @author     Gabrielle Allen
   @desc
               Divide two complex numbers.
               Make sure that the intermediate values do not overflow.
               See e.g. the C99 compliant implementation in libgcc
               (which ships with gcc).
               This implementation here does not handle nan and inf.
   @enddesc
 
   @var        a
   @vdesc      First operand
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar
   @var        b
   @vdesc      Second operand
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar

   @returntype CCTK_COMPLEX
   @returndesc
               The quotient
   @endreturndesc
@@*/
#define DEFINE_CCTK_CMPLX_DIV(CCTK_Cmplx, cctk_real, cctk_complex)            \
cctk_complex CCTK_Cmplx##Div (cctk_complex a, cctk_complex b)                 \
{                                                                             \
  cctk_real afact, bfact, factor;                                             \
  cctk_complex result;                                                        \
                                                                              \
                                                                              \
  afact = fabs(a.Re) + fabs(a.Im);                                            \
  a.Re /= afact;                                                              \
  a.Im /= afact;                                                              \
  bfact = fabs(b.Re) + fabs(b.Im);                                            \
  b.Re /= bfact;                                                              \
  b.Im /= bfact;                                                              \
  factor = b.Re*b.Re + b.Im*b.Im;                                             \
  result.Re = afact / bfact * (a.Re*b.Re + a.Im*b.Im) / factor;               \
  result.Im = afact / bfact * (a.Im*b.Re - a.Re*b.Im) / factor;               \
                                                                              \
  return (result);                                                            \
}


 /*@@
   @routine    CCTK_CmplxSin
   @date       Wed 12 Dec 2001
   @author     Thomas Radke
   @desc
               Returns the sine of a complex number.
   @enddesc
 
   @var        complex_number
   @vdesc      The complex number
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar

   @returntype CCTK_COMPLEX
   @returndesc
               The sine
   @endreturndesc
@@*/
#define DEFINE_CCTK_CMPLX_SIN(CCTK_Cmplx, cctk_real, cctk_complex)            \
cctk_complex CCTK_Cmplx##Sin (cctk_complex complex_number)                    \
{                                                                             \
  cctk_complex result;                                                        \
                                                                              \
                                                                              \
  if (complex_number.Im == 0.0)                                               \
  {                                                                           \
    result.Re = sin (complex_number.Re);                                      \
    result.Im = 0.0;                                                          \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    result.Re = sin (complex_number.Re) * cosh (complex_number.Im);           \
    result.Im = cos (complex_number.Re) * sinh (complex_number.Im);           \
  }                                                                           \
                                                                              \
  return (result);                                                            \
}


 /*@@
   @routine    CCTK_CmplxCos
   @date       Wed 12 Dec 2001
   @author     Thomas Radke
   @desc
               Returns the cosine of a complex number.
   @enddesc
 
   @var        complex_number
   @vdesc      The complex number
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar

   @returntype CCTK_COMPLEX
   @returndesc
               The cosine
   @endreturndesc
@@*/
#define DEFINE_CCTK_CMPLX_COS(CCTK_Cmplx, cctk_real, cctk_complex)            \
cctk_complex CCTK_Cmplx##Cos (cctk_complex complex_number)                    \
{                                                                             \
  cctk_complex result;                                                        \
                                                                              \
                                                                              \
  if (complex_number.Im == 0.0)                                               \
  {                                                                           \
    result.Re = cos (complex_number.Re);                                      \
    result.Im = 0.0;                                                          \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    result.Re = cos (complex_number.Re) * cosh (complex_number.Im);           \
    result.Im = sin (complex_number.Re) * sinh (complex_number.Im);           \
  }                                                                           \
                                                                              \
  return (result);                                                            \
}


 /*@@
   @routine    CCTK_CmplxExp
   @date       Wed 12 Dec 2001
   @author     Thomas Radke
   @desc
               Returns the exponential of a complex number.
   @enddesc
 
   @var        complex_number
   @vdesc      The complex number
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar

   @returntype CCTK_COMPLEX
   @returndesc
               The exponential
   @endreturndesc
@@*/
#define DEFINE_CCTK_CMPLX_EXP(CCTK_Cmplx, cctk_real, cctk_complex)            \
cctk_complex CCTK_Cmplx##Exp (cctk_complex complex_number)                    \
{                                                                             \
  cctk_real rho, theta;                                                       \
  cctk_complex result;                                                        \
                                                                              \
                                                                              \
  rho = exp (complex_number.Re);                                              \
  theta = complex_number.Im;                                                  \
  result.Re = rho * cos (theta);                                              \
  result.Im = rho * sin (theta);                                              \
                                                                              \
  return (result);                                                            \
}


 /*@@
   @routine    CCTK_CmplxSqrt
   @date       Wed 12 Dec 2001
   @author     Thomas Radke
   @desc
               Returns the square root of a complex number.
   @enddesc
 
   @var        complex_number
   @vdesc      The complex number
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar

   @returntype CCTK_COMPLEX
   @returndesc
               The square root
   @endreturndesc
@@*/
#define DEFINE_CCTK_CMPLX_SQRT(CCTK_Cmplx, cctk_real, cctk_complex)           \
cctk_complex CCTK_Cmplx##Sqrt (cctk_complex complex_number)                   \
{                                                                             \
  cctk_real x, y, w, t;                                                       \
  cctk_complex result;                                                        \
                                                                              \
                                                                              \
  if (complex_number.Re == 0.0 && complex_number.Im == 0.0)                   \
  {                                                                           \
    result.Re = result.Im = 0.0;                                              \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    x = fabs (complex_number.Re);                                             \
    y = fabs (complex_number.Im);                                             \
    if (x >= y)                                                               \
	{                                                                         \
      t = y / x;                                                              \
      w = sqrt (x) * sqrt (0.5 * (1.0 + sqrt (1.0 * t * t)));                 \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      t = x / y;                                                              \
      w = sqrt (y) * sqrt (0.5 * (t + sqrt (1.0 * t * t)));                   \
    }                                                                         \
	                                                                          \
    if (complex_number.Re >= 0.0)                                             \
	{                                                                         \
      result.Re = w;                                                          \
      result.Im = complex_number.Im / (2.0 * w);                              \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      x = complex_number.Im >= 0 ? w : -w;                                    \
      result.Re = complex_number.Im / (2.0 * x);                              \
      result.Im = x;                                                          \
    }                                                                         \
  }                                                                           \
                                                                              \
  return (result);                                                            \
}

 /*@@
   @routine    CCTK_CmplxPow
   @date       
   @author     Yaakoub Y El Khamra
   @desc
               Raises a complex number to a given power
   @enddesc
 
   @var        complex_number
   @vdesc      The complex number
   @vtype      CCTK_COMPLEX
   @vio        in
   @endvar

   @returntype CCTK_COMPLEX
   @returndesc
               The square root
   @endreturndesc
@@*/
#define DEFINE_CCTK_CMPLX_POW(CCTK_Cmplx, cctk_real, cctk_complex)            \
cctk_complex CCTK_Cmplx##Pow (cctk_complex complex_number, cctk_real w)       \
{                                                                             \
  cctk_real R, theta, phi;                                                    \
  cctk_complex result;                                                        \
                                                                              \
  if ( complex_number.Re > 0)                                                 \
  {                                                                           \
    theta = 0.0;                                                              \
    phi   = atan(complex_number.Im/complex_number.Re) + theta;                \
    R     = sqrt(complex_number.Re*complex_number.Re + complex_number.Im*complex_number.Im);\
    R     = pow(R,w);                                                       \
    result.Re = R * cos (w * phi);                                            \
    result.Im = R * sin (w * phi);                                            \
  }                                                                           \
  else if ( complex_number.Re < 0 && complex_number.Im >= 0 )                 \
  {                                                                           \
    theta = 4.0 * atan (1.0);                                                 \
    phi   = atan(complex_number.Im/complex_number.Re) + theta;                \
    R     = sqrt(complex_number.Re*complex_number.Re + complex_number.Im*complex_number.Im);\
    R     = pow(R,w);                                                       \
    result.Re = R * cos (w * phi);                                            \
    result.Im = R * sin (w * phi);                                            \
  }                                                                           \
  else if ( complex_number.Re < 0 && complex_number.Im < 0 )                  \
  {                                                                           \
    theta = 4.0 * atan (1.0);                                                 \
    phi   = atan(complex_number.Im/complex_number.Re) + theta;                \
    R     = sqrt(complex_number.Re*complex_number.Re + complex_number.Im*complex_number.Im);\
    R     = pow(R,w);                                                       \
    result.Re = R * cos (w * phi);                                            \
    result.Im = R * sin (w * phi);                                            \
  }                                                                           \
  else if ( fabs(complex_number.Re) <= 1e-20 && fabs(complex_number.Im) < 1e-20 )\
  {                                                                           \
    result.Re = 0.0;                                                          \
    result.Im = 0.0;                                                          \
  }                                                                           \
  else if ( fabs(complex_number.Re) <= 1e-20 && complex_number.Im < 0 )       \
  {                                                                           \
    phi = 2.0 * atan (1.0);                                                   \
    R     = sqrt(complex_number.Re*complex_number.Re + complex_number.Im*complex_number.Im);\
    R     = pow(R,w);                                                       \
    result.Re = R * cos (w * phi);                                            \
    result.Im = R * sin (w * phi);                                            \
  }                                                                           \
  if ( fabs(complex_number.Re) <= 1e-20 && complex_number.Im < 0 )            \
  {                                                                           \
    phi = -2.0 * atan (1.0);                                                  \
    R     = sqrt(complex_number.Re*complex_number.Re + complex_number.Im*complex_number.Im);\
    R     = pow(R,w);                                                       \
    result.Re = R * cos (w * phi);                                            \
    result.Im = R * sin (w * phi);                                            \
  }                                                                           \
                                                                              \
  return (result);                                                            \
}

/* macro to define a set of complex functions for a given precision */
#define DEFINE_CMPLX_FUNCTIONS(CCTK_Cmplx, cctk_real, cctk_complex)           \
          DEFINE_CCTK_CMPLX       (CCTK_Cmplx, cctk_real, cctk_complex)       \
          DEFINE_CCTK_CMPLX_REAL  (CCTK_Cmplx, cctk_real, cctk_complex)       \
          DEFINE_CCTK_CMPLX_IMAG  (CCTK_Cmplx, cctk_real, cctk_complex)       \
          DEFINE_CCTK_CMPLX_CONJG (CCTK_Cmplx, cctk_real, cctk_complex)       \
          DEFINE_CCTK_CMPLX_ABS   (CCTK_Cmplx, cctk_real, cctk_complex)       \
          DEFINE_CCTK_CMPLX_ADD   (CCTK_Cmplx, cctk_real, cctk_complex)       \
          DEFINE_CCTK_CMPLX_SUB   (CCTK_Cmplx, cctk_real, cctk_complex)       \
          DEFINE_CCTK_CMPLX_MUL   (CCTK_Cmplx, cctk_real, cctk_complex)       \
          DEFINE_CCTK_CMPLX_DIV   (CCTK_Cmplx, cctk_real, cctk_complex)       \
          DEFINE_CCTK_CMPLX_SIN   (CCTK_Cmplx, cctk_real, cctk_complex)       \
          DEFINE_CCTK_CMPLX_COS   (CCTK_Cmplx, cctk_real, cctk_complex)       \
          DEFINE_CCTK_CMPLX_EXP   (CCTK_Cmplx, cctk_real, cctk_complex)       \
          DEFINE_CCTK_CMPLX_SQRT  (CCTK_Cmplx, cctk_real, cctk_complex)       \
          DEFINE_CCTK_CMPLX_POW   (CCTK_Cmplx, cctk_real, cctk_complex)       \

/* define complex functions for all available precisions */
#ifdef CCTK_REAL4
  DEFINE_CMPLX_FUNCTIONS (CCTK_Cmplx8, CCTK_REAL4, CCTK_COMPLEX8)
#endif

#ifdef CCTK_REAL8
  DEFINE_CMPLX_FUNCTIONS (CCTK_Cmplx16, CCTK_REAL8, CCTK_COMPLEX16)
#endif

#ifdef CCTK_REAL16
  DEFINE_CMPLX_FUNCTIONS (CCTK_Cmplx32, CCTK_REAL16, CCTK_COMPLEX32)
#endif
