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

typedef struct
{
  void *(*create)(int);
  void (*destroy)(int, void *);
  void (*start)(int, void *);
  void (*stop)(int, void *);
  void (*reset)(int, void *);
  double (*get)(int, void *);
  void (*set)(int, void *, double);
} t_TimerFuncs;


/* Function prototypes */

#ifdef __cplusplus
extern "C" {
#endif

cTimer *CactusNewTimer(void);

void CactusStartTimer(cTimer *timer);
void CactusStopTimer(cTimer *timer);
void CactusResetTimer(cTimer *timer);

int CCTK_TimerRegister(const char *name, t_TimerFuncs *functions);

#ifdef __cplusplus
	   }
#endif

#define INITIALISATION 0
#define EVOLUTION      1
#define ELLIPTIC       2

#endif
