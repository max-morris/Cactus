 /*@@
   @header    CactusTimers.h
   @date      Thu Oct  8 18:31:45 1998
   @author    Tom Goodale
   @desc 
   Timer stuff
   @enddesc 
 @@*/


/* $Id$ */

#ifndef _CACTUSTIMERS_H_
#define _CACTUSTIMERS_H_

/*  Typedefs */

typedef struct
{
  double basic[4];
} cInternalTimer;
  
typedef struct
{
  cInternalTimer total;
  cInternalTimer last;  
} cTimer;

/* Function prototypes */

#ifdef __cplusplus
extern "C" {
#endif

void CactusStartTimer(cTimer *timer);
void CactusStopTimer(cTimer *timer);
void CactusResetTimer(cTimer *timer);

#ifdef __cplusplus
	   }
#endif

#define INITIALISATION 0
#define EVOLUTION      1
#define ELLIPTIC       2

#endif
