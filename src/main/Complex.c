 /*@@
   @file      Complex.c
   @date      Tue Dec 14 12:09:43 1999
   @author    Tom Goodale
   @desc
              Complex variable stuff
   @enddesc
   @version $Header$
 @@*/

#include <math.h>

#include "cctk_Flesh.h"
#include "cctk_Complex.h"
#include "cctk_WarnLevel.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_Complex_c)


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
  cctk_complex complex_number;                                                \
                                                                              \
                                                                              \
  complex_number.Re = Re;                                                     \
  complex_number.Im = Im;                                                     \
                                                                              \
  return (complex_number);                                                    \
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
  cctk_complex conjg;                                                         \
                                                                              \
                                                                              \
  conjg.Re =  complex_number.Re;                                              \
  conjg.Im = -complex_number.Im;                                              \
  return (conjg);                                                             \
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
  return (sqrt (complex_number.Re*complex_number.Re +                         \
                complex_number.Im*complex_number.Im));                        \
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
  cctk_complex sum;                                                           \
                                                                              \
                                                                              \
  sum.Re = a.Re + b.Re;                                                       \
  sum.Im = a.Im + b.Im;                                                       \
  return (sum);                                                               \
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
  cctk_complex diff;                                                          \
                                                                              \
                                                                              \
  diff.Re = a.Re - b.Re;                                                      \
  diff.Im = a.Im - b.Im;                                                      \
  return (diff);                                                              \
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
  cctk_complex mult;                                                          \
                                                                              \
                                                                              \
  mult.Re = a.Re*b.Re - a.Im*b.Im;                                            \
  mult.Im = a.Im*b.Re + a.Re*b.Im;                                            \
  return (mult);                                                              \
}


 /*@@
   @routine    CCTK_CmplxDiv
   @date       Sat Dec 4 12:11:04 1999
   @author     Gabrielle Allen
   @desc
               Divides two complex numbers
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
  cctk_real factor;                                                           \
  cctk_complex quot;                                                          \
                                                                              \
                                                                              \
  factor = b.Re*b.Re + b.Im*b.Im;                                             \
  if (factor != 0)                                                            \
  {                                                                           \
    quot.Re = (a.Re*b.Re + a.Im*b.Im) / factor;                               \
    quot.Im = (a.Im*b.Re - a.Re*b.Im) / factor;                               \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    CCTK_Warn (0, __LINE__, __FILE__, "Cactus",                               \
                         "Divide by zero in CCTK_CmplxDiv");                  \
    quot.Re = quot.Im = 0;                                                    \
  }                                                                           \
  return (quot);                                                              \
}


/* Routines to be added */

/*
CCTK_CmplxSqrt
CCTK_CmplxSin
CCTK_CmplxCos
CCTK_CmplxLog
CCTK_CmplxExp
*/


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
          DEFINE_CCTK_CMPLX_DIV   (CCTK_Cmplx, cctk_real, cctk_complex)

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
