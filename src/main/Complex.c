 /*@@
   @file      Complex.c
   @date      Tue Dec 14 12:09:43 1999
   @author    Tom Goodale
   @desc 
   Complex variable stuff
   @enddesc 
 @@*/

#include <math.h>

#include "cctk_Flesh.h"
#include "cctk_Complex.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_Complex_c)

 /*@@
   @routine    CCTK_Cmplx
   @date       Tue Dec 14 12:16:01 1999
   @author     Tom Goodale
   @desc 
   Turns two reals into a complex number
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
CCTK_COMPLEX CCTK_Cmplx(CCTK_REAL Re, CCTK_REAL Im)
{
  CCTK_COMPLEX complex_number;

  complex_number.Re = Re;
  complex_number.Im = Im;
  
  return complex_number;
}

 /*@@
   @routine    CCTK_CmplxReal
   @date       Tue Dec 14 12:22:41 1999
   @author     Tom Goodale
   @desc 
   Returns the real part of a complex number.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
CCTK_REAL CCTK_CmplxReal(CCTK_COMPLEX complex_number)
{
  return complex_number.Im;
}

 /*@@
   @routine    CCTK_CmplxImag
   @date       Tue Dec 14 12:22:41 1999
   @author     Tom Goodale
   @desc 
   Returns the imaginary part of a complex number.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
CCTK_REAL CCTK_CmplxImag(CCTK_COMPLEX complex_number)
{
  return complex_number.Im;
}

 /*@@
   @routine    CCTK_CmplxConjg
   @date       Tue Dec 14 12:22:41 1999
   @author     Tom Goodale
   @desc 
   Returns the complex conjugate of a complex number.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
CCTK_COMPLEX CCTK_CmplxConjg(CCTK_COMPLEX in)
{
  CCTK_COMPLEX conjg;

  conjg.Re = in.Re;
  conjg.Im = -in.Im;

  return conjg;
}

 /*@@
   @routine    CCTK_CmplxAbs
   @date       Tue Dec 14 12:26:33 1999
   @author     Tom Goodale
   @desc 
   Return the absolute value of a complex number.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
CCTK_REAL CCTK_CmplxAbs(CCTK_COMPLEX in)
{
  CCTK_REAL cabs;

  cabs = sqrt(in.Re*in.Re + in.Im*in.Im);

  return cabs;
}

 /*@@
   @routine    CCTK_CmplxAdd
   @date       Sat Dec 4 12:11:04 1999
   @author     Gabrielle Allen
   @desc 
   Adds two complex numbers
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
CCTK_COMPLEX CCTK_CmplxAdd(CCTK_COMPLEX a,CCTK_COMPLEX b)
{
  CCTK_COMPLEX add_ab;
  add_ab.Re = a.Re + b.Re;
  add_ab.Im = a.Im + b.Im;
  return add_ab;
}

 /*@@
   @routine    CCTK_CmplxSub
   @date       Sat Dec 4 12:11:04 1999
   @author     Gabrielle Allen
   @desc 
   Subtracts two complex numbers
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
CCTK_COMPLEX CCTK_CmplxSub(CCTK_COMPLEX a,CCTK_COMPLEX b)
{
  CCTK_COMPLEX sub_ab;
  sub_ab.Re = a.Re - b.Re;
  sub_ab.Im = a.Im - b.Im;
  return sub_ab;
}

 /*@@
   @routine    CCTK_CmplxMul
   @date       Sat Dec 4 12:11:04 1999
   @author     Gabrielle Allen
   @desc 
   Multiplies two complex numbers
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
CCTK_COMPLEX CCTK_CmplxMul(CCTK_COMPLEX a,CCTK_COMPLEX b)
{
  CCTK_COMPLEX mult_ab;
  mult_ab.Re = a.Re*b.Re-a.Im*b.Im;
  mult_ab.Im = a.Im*b.Re+a.Re*b.Im;
  return mult_ab;
}

 /*@@
   @routine    CCTK_CmplxDiv
   @date       Sat Dec 4 12:11:04 1999
   @author     Gabrielle Allen
   @desc 
   Divides two complex numbers
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
CCTK_COMPLEX CCTK_CmplxDiv(CCTK_COMPLEX a,CCTK_COMPLEX b)
{
  CCTK_COMPLEX div_ab;
  CCTK_REAL fac;

  fac = b.Re*b.Re+b.Im*b.Im;
  
  if (fac != 0)
  {
    div_ab.Re = (a.Re*b.Re+a.Im*b.Im)/fac;
    div_ab.Im = (a.Im*b.Re-a.Re*b.Im)/fac;
  }
  else
  {
    CCTK_Warn(0,__LINE__,__FILE__,"Cactus","Divide by zero in CCTK_CmplxDiv");
    div_ab.Re = 0;
    div_ab.Im = 0;
  }
  return div_ab;
}


/* Routines to be added */

/*
CCTK_CmplxSqrt
CCTK_CmplxSin
CCTK_CmplxCos
CCTK_CmplxLog
CCTK_CmplxExp

*/
