 /*@@
   @header    cctk_Complex.h
   @date      Tue Dec 14 12:28:05 1999
   @author    Tom Goodale
   @desc
              Prototypes for complex numbers.
   @enddesc
   @version   $Header$
 @@*/

#ifndef _CCTK_COMPLEX_H_
#define _CCTK_COMPLEX_H_

#ifdef __cplusplus
extern "C"
{
#endif


/* macro to declare a set of complex functions for a given precision */
#define DECLARE_CMPLX_FUNCTIONS(CCTK_Cmplx, cctk_real, cctk_complex)          \
cctk_complex CCTK_Cmplx        (cctk_real Re, cctk_real Im);                  \
cctk_real    CCTK_Cmplx##Real  (cctk_complex complex_number);                 \
cctk_real    CCTK_Cmplx##Imag  (cctk_complex complex_number);                 \
cctk_complex CCTK_Cmplx##Conjg (cctk_complex complex_number);                 \
cctk_real    CCTK_Cmplx##Abs   (cctk_complex complex_number);                 \
cctk_complex CCTK_Cmplx##Add   (cctk_complex a, cctk_complex b);              \
cctk_complex CCTK_Cmplx##Sub   (cctk_complex a, cctk_complex b);              \
cctk_complex CCTK_Cmplx##Mul   (cctk_complex a, cctk_complex b);              \
cctk_complex CCTK_Cmplx##Div   (cctk_complex a, cctk_complex b);              \
cctk_complex CCTK_Cmplx##Sin   (cctk_complex complex_number);                 \
cctk_complex CCTK_Cmplx##Cos   (cctk_complex complex_number);                 \
cctk_complex CCTK_Cmplx##Exp   (cctk_complex complex_number);                 \
cctk_complex CCTK_Cmplx##Sqrt  (cctk_complex complex_number);


/* declare complex functions for all available precisions */
#if CCTK_HAVE_REAL4
DECLARE_CMPLX_FUNCTIONS (CCTK_Cmplx8, CCTK_REAL4, CCTK_COMPLEX8)
#endif

#if CCTK_HAVE_REAL8
DECLARE_CMPLX_FUNCTIONS (CCTK_Cmplx16, CCTK_REAL8, CCTK_COMPLEX16)
#endif

#if CCTK_HAVE_REAL16
DECLARE_CMPLX_FUNCTIONS (CCTK_Cmplx32, CCTK_REAL16, CCTK_COMPLEX32)
#endif


/* declare the default precision complex functions as #define'd macros */
#ifdef CCTK_REAL_PRECISION_4
#define CCTK_Cmplx      CCTK_Cmplx8
#define CCTK_CmplxReal  CCTK_Cmplx8Real
#define CCTK_CmplxImag  CCTK_Cmplx8Imag
#define CCTK_CmplxConjg CCTK_Cmplx8Conjg
#define CCTK_CmplxAbs   CCTK_Cmplx8Abs
#define CCTK_CmplxAdd   CCTK_Cmplx8Add
#define CCTK_CmplxSub   CCTK_Cmplx8Sub
#define CCTK_CmplxMul   CCTK_Cmplx8Mul
#define CCTK_CmplxDiv   CCTK_Cmplx8Div
#define CCTK_CmplxSin   CCTK_Cmplx8Sin
#define CCTK_CmplxCos   CCTK_Cmplx8Cos
#define CCTK_CmplxExp   CCTK_Cmplx8Exp
#define CCTK_CmplxSqrt  CCTK_Cmplx8Sqrt
#elif CCTK_REAL_PRECISION_8
#define CCTK_Cmplx      CCTK_Cmplx16
#define CCTK_CmplxReal  CCTK_Cmplx16Real
#define CCTK_CmplxImag  CCTK_Cmplx16Imag
#define CCTK_CmplxConjg CCTK_Cmplx16Conjg
#define CCTK_CmplxAbs   CCTK_Cmplx16Abs
#define CCTK_CmplxAdd   CCTK_Cmplx16Add
#define CCTK_CmplxSub   CCTK_Cmplx16Sub
#define CCTK_CmplxMul   CCTK_Cmplx16Mul
#define CCTK_CmplxDiv   CCTK_Cmplx16Div
#define CCTK_CmplxSin   CCTK_Cmplx16Sin
#define CCTK_CmplxCos   CCTK_Cmplx16Cos
#define CCTK_CmplxExp   CCTK_Cmplx16Exp
#define CCTK_CmplxSqrt  CCTK_Cmplx16Sqrt
#elif CCTK_REAL_PRECISION_16
#define CCTK_Cmplx      CCTK_Cmplx32
#define CCTK_CmplxReal  CCTK_Cmplx32Real
#define CCTK_CmplxImag  CCTK_Cmplx32Imag
#define CCTK_CmplxConjg CCTK_Cmplx32Conjg
#define CCTK_CmplxAbs   CCTK_Cmplx32Abs
#define CCTK_CmplxAdd   CCTK_Cmplx32Add
#define CCTK_CmplxSub   CCTK_Cmplx32Sub
#define CCTK_CmplxMul   CCTK_Cmplx32Mul
#define CCTK_CmplxDiv   CCTK_Cmplx32Div
#define CCTK_CmplxSin   CCTK_Cmplx32Sin
#define CCTK_CmplxCos   CCTK_Cmplx32Cos
#define CCTK_CmplxExp   CCTK_Cmplx32Exp
#define CCTK_CmplxSqrt  CCTK_Cmplx32Sqrt
#endif

#ifdef __cplusplus
}
#endif

#endif /* _CCTK_COMPLEX_H_ */
