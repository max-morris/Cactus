 /*@@
   @header    cctk_Stagger.h
   @date      Thu Jan 20 2000
   @author    Gerd Lanfermann
   @desc 
   Prototypes and constants for stagger functions.
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTK_STAGGER_H_
#define _CCTK_STAGGER_H_

#ifdef __cplusplus
extern "C" {
#endif

int CCTK_StaggerVars(void);
int CCTK_StaggerCodeGrpIdx(int gindex);
int CCTK_StaggerCodeGrp(const char *gname);
int CCTK_StaggerCodeName(const char *stype);
int CCTK_DirStaggerCodeVal(int dir, int sc);
int CCTK_DirStaggerCodeName(int dir, const char *stype);

#ifdef __cplusplus
}
#endif

#endif
