 /*@@
   @header    cctk_Complex.h
   @date      Tue Dec 14 12:28:05 1999
   @author    Tom Goodale
   @desc 
   Prototypes for complex numbers.
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTK_COMPLEX_H_
#define _CCTK_COMPLEX_H_

#ifdef __cplusplus 
extern "C" 
{
#endif

CCTK_COMPLEX CCTK_Cmplx(CCTK_REAL Re, CCTK_REAL Im);

CCTK_REAL CCTK_CmplxReal(CCTK_COMPLEX complex_number);
CCTK_REAL CCTK_CmplxImag(CCTK_COMPLEX complex_number);

CCTK_COMPLEX CCTK_CmplxConjg(CCTK_COMPLEX in);
CCTK_REAL CCTK_CmplxAbs(CCTK_COMPLEX in);

CCTK_COMPLEX CCTK_CmplxAdd(CCTK_COMPLEX a,CCTK_COMPLEX b);
CCTK_COMPLEX CCTK_CmplxSub(CCTK_COMPLEX a,CCTK_COMPLEX b);
CCTK_COMPLEX CCTK_CmplxMul(CCTK_COMPLEX a,CCTK_COMPLEX b);
CCTK_COMPLEX CCTK_CmplxDiv(CCTK_COMPLEX a,CCTK_COMPLEX b);

#ifdef __cplusplus 
}
#endif

#endif /* __CCTK_COMPLEX_H_ */
