 /*@@
   @header    CactusrfrInterface.h
   @date      Thu Feb 11 18:42:03 1999
   @author    Tom Goodale
   @desc 
   A file to contain prototypes for the interface between Cactus and the rfr.
   @enddesc 
 @@*/

#ifndef _CACTUSRFRINTERFACE_H_
#define _CACTUSRFRINTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

int CCTK_rfrStorageOn(void *GH, int group);

int CCTK_rfrStorageOff(void *GH, int group);

int CCTK_rfrCommunicationOn(void *GH, int group);

int CCTK_rfrCommunicationOff(void *GH, int group);

int CCTK_rfrTriggerable(int variable);

int CCTK_rfrTriggerSaysGo(void *GH, int variable);

int CCTK_rfrTriggerAction(void *GH, int group);

int CCTK_rfrCallFunc(void *GH, int language, void *function);


#ifdef __cplusplus
}
#endif


#endif



