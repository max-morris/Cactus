 /*@@
   @file      rfr_constants.h
   @date      
   @author   
   @desc 
   
   @enddesc 
 @@*/

/* $Id$ */

/* Be careful changing ordering. Look at tree in rfrInitialize before you do! */
#define CCTK_PARAMCHECK  0
#define CCTK_BASEGRID  1

#define CCTK_RECOVER   5

/* Be sure to keep the INITIAL*, POSTSTEP*, etc... contiguous */
#define CCTK_INITIAL0  10
#define CCTK_INITIAL   11
#define CCTK_INITIAL2  12
#define CCTK_INITIAL3  13
#define CCTK_INITIAL4  14
#define CCTK_INITIAL5  15
#define CCTK_INITIAL6  16
#define CCTK_INITIAL7  17
#define CCTK_INITIAL8  18
#define CCTK_INITIAL9  19

#define CCTK_CPINITIAL 25

#define CCTK_PRESTEP   30
#define CCTK_PRESTEP2  31
#define CCTK_PRESTEP3  33
#define CCTK_PRESTEP4  34
#define CCTK_PRESTEP5  35


#define CCTK_POSTSTEP   40
#define CCTK_POSTSTEP2  41
#define CCTK_POSTSTEP3  42
#define CCTK_POSTSTEP4  43
#define CCTK_POSTSTEP5  44
#define CCTK_POSTSTEP6  45
#define CCTK_POSTSTEP7  46
#define CCTK_POSTSTEP8  47
#define CCTK_POSTSTEP9  48
#define CCTK_POSTSTEP10 49



#define CCTK_EVOL       60

#define CCTK_BOUND      70

#define CCTK_CHECKPOINT 75 

#define CCTK_ANALYSIS   80

#define CCTK_TERMINATE  90

#define CCTK_CONVERGENCE 100


/* This shouldn't be here, but need it externally visable. */

int rfrInitTree(void **rfr_top,   
		int (*StorageOn)(void *, int),
		int (*StorageOff)(void *, int),
		int (*CommunicationOn)(void *, int),
		int (*CommunicationOff)(void *, int),
		int (*Triggerable)(int),
		int (*TriggerSaysGo)(void *, int),
		int (*TriggerAction)(void *, int),
		int (*CallFunc)(void *, int, void *));

