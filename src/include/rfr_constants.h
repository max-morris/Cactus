 /*@@
   @file      rfr_constants.h
   @date      
   @author   
   @desc 
   
   @enddesc 
 @@*/

/* $Id$ */

/* Be careful changing ordering. Look at tree in rfrInitialize before you do! */
#define CACTUS_BASEGRID  1

#define CACTUS_RECOVER   5

/* Be sure to keep the INITIAL*, POSTSTEP*, etc... contiguous */
#define CACTUS_INITIAL0  10
#define CACTUS_INITIAL   11
#define CACTUS_INITIAL2  12
#define CACTUS_INITIAL3  13
#define CACTUS_INITIAL4  14
#define CACTUS_INITIAL5  15
#define CACTUS_INITIAL6  16
#define CACTUS_INITIAL7  17
#define CACTUS_INITIAL8  18
#define CACTUS_INITIAL9  19

#define CACTUS_CPINITIAL 25

#define CACTUS_PRESTEP   30
#define CACTUS_PRESTEP2  31
#define CACTUS_PRESTEP3  33
#define CACTUS_PRESTEP4  34
#define CACTUS_PRESTEP5  35


#define CACTUS_POSTSTEP   40
#define CACTUS_POSTSTEP2  41
#define CACTUS_POSTSTEP3  42
#define CACTUS_POSTSTEP4  43
#define CACTUS_POSTSTEP5  44
#define CACTUS_POSTSTEP6  45
#define CACTUS_POSTSTEP7  46
#define CACTUS_POSTSTEP8  47
#define CACTUS_POSTSTEP9  48
#define CACTUS_POSTSTEP10 49



#define CACTUS_EVOL       60

#define CACTUS_BOUND      70

#define CACTUS_CHECKPOINT 75 

#define CACTUS_ANALYSIS   80

#define CACTUS_TERMINATE  90

#define CACTUS_CONVERGENCE 100


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

