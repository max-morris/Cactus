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

int CCTK_TimerRegister(const char *name, t_TimerFuncs *functions);
int CCTK_TimerCreate(const char *name);
void CCTK_TimerDestroy(const char *name);
void CCTK_TimerDestroyI(int this_timer);
void CCTK_TimerStart(const char *name);
void CCTK_TimerStartI(int this_timer);
void CCTK_TimerStop(const char *name);
void CCTK_TimerStopI(int this_timer);
void CCTK_TimerReset(const char *name);
void CCTK_TimerResetI(int this_timer);
char *CCTK_TimerGet(const char *name);
char *CCTK_TimerGetI(int this_timer);

#ifdef __cplusplus
	   }
#endif

#define INITIALISATION 0
#define EVOLUTION      1
#define ELLIPTIC       2

#endif
