 /*@@
   @header    rfrInterface.h
   @date      Tue Jun  1 16:50:22 1999
   @author    Tom Goodale
   @desc 
   CCTK rfr interface calls.
   @enddesc 
   @version $Header$
 @@*/


#ifndef _RFRINTERFACE_H_
#define _RFRINTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

int CCTK_rfrTraverse(cGH *GH, int rfrpoint);
int CCTKi_rfrPrintTree(cGH *GH,void *rfr_top);
int CCTKi_rfrStorageOn(void *GH, int group);
int CCTKi_rfrStorageOff(void *GH, int group);
int CCTKi_rfrCommunicationOn(void *GH, int group);
int CCTKi_rfrCommunicationOff(void *GH, int group);
int CCTKi_rfrTriggerable(int entrypoint);
int CCTKi_rfrCallFunc(void *GH, int language, void *function);

#ifdef __cplusplus
}
#endif

#endif
