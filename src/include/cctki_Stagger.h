 /*@@
   @header    cctki_Stagger.h
   @date      Thu Jan 20 2000
   @author    Gerd Lanfermann
   @desc 
   Prototypes and constants for stagger functions.
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTKI_STAGGER_H_
#define _CCTKI_STAGGER_H_

#ifdef __cplusplus
extern "C" 
{
#endif

int CCTKi_ParseStaggerString(int dim,  
                             const char *imp, 
                             const char *gname,  
                             const char *stype); 

#ifdef __cplusplus
}
#endif

#endif /* _CCTKI_STAGGER_H_ */
