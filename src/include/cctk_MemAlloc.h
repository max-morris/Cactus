 /*@@
   @header    cctk_MemAlloc.h
   @date      Thu Jan 20 2000
   @author    Gerd Lanfermann
   @desc
   Prototypes for Cactus MemAlloc functions.
   @enddesc
   @version $Header$
 @@*/

#ifndef _CCTK_STAGGER_H_
#define _CCTK_STAGGER_H_

#ifdef __cplusplus
extern "C" {
#endif

void *CCTKi_malloc(t_size size, int line, const char *file);
void CCTKi_free(void *pointer);
void CCTK_TotalMemory(void);

#ifdef __cplusplus
}
#endif

#endif


