 /*@@
   @file      rfrConstants.h
   @date      
   @author   
   @desc 
   
   @enddesc 
   @version $Header$
 @@*/

#ifndef _RFRCONSTANTS_H
#define _RFRCONSTANTS_H

/* Be careful changing ordering. Look at tree in rfrInitialize before you do! */
#define CCTK_PARAMCHECK  0
#define CCTK_BASEGRID  1

#define CCTK_RECOVER   3
#define CCTK_RECOVER_PARAMETERS   4
#define CCTK_RECOVER_VARIABLES   5

/* Be sure to keep the INITIAL*, POSTSTEP*, etc... contiguous */
#define CCTK_INITIAL   11
#define CCTK_POSTINITIAL   12

#define CCTK_CPINITIAL 25

#define CCTK_PRESTEP   30

#define CCTK_POSTSTEP   40

#define CCTK_EVOL       60

#define CCTK_CHECKPOINT 75 

#define CCTK_ANALYSIS   80

#define CCTK_TERMINATE  90

#define CCTK_CONVERGENCE 100


/* This shouldn't be here, but need it externally visable. */

#ifdef __cplusplus
extern "C"
{
#endif

int rfrInitTree(void **rfr_top,   
                int (*StorageOn)(void *, int),
                int (*StorageOff)(void *, int),
                int (*CommunicationOn)(void *, int),
                int (*CommunicationOff)(void *, int),
                int (*Triggerable)(int),
                int (*TriggerSaysGo)(void *, int),
                int (*TriggerAction)(void *, int),
                int (*CallFunc)(void *, int, void *));

void rfrPrintDescs(void *rfr_top, void *data, int when, char *tag, char *spacing);

#ifdef __cplusplus
}
#endif

#endif /*  _RFRCONSTANTS_H */
