 /*@@
   @header    CactusTimers.h
   @date      Thu Oct  8 18:31:45 1998
   @author    Tom Goodale
   @desc 
   Timer stuff
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CACTUSTIMERS_H_
#define _CACTUSTIMERS_H_

/*  Typedefs */

typedef enum {val_none, val_int, val_long, val_double} cTimerValType;

typedef struct
{
  cTimerValType type;
  const char *heading;
  const char *units;
  union
  {
    int        i;
    long int   l;
    double     d;
  } val;
} cTimerVal;

typedef struct
{
  int n_vals;
  cTimerVal *vals;
} cTimerData;


typedef struct
{
  cTimerData info;
  void *(*create)(int);
  void (*destroy)(int, void *);
  void (*start)(int, void *);
  void (*stop)(int, void *);
  void (*reset)(int, void *);
  void (*get)(int, void *, cTimerVal *);
  void (*set)(int, void *, cTimerVal *);
} cTimerFuncs;



/* Function prototypes */

#ifdef __cplusplus
extern "C" {
#endif

int CCTK_TimerRegister(const char *name, cTimerFuncs *functions);
int CCTK_TimerCreate(const char *name);
int CCTK_TimerCreateI(void);
int CCTK_TimerDestroy(const char *name);
int CCTK_TimerDestroyI(int this_timer);
int CCTK_TimerStart(const char *name);
int CCTK_TimerStartI(int this_timer);
int CCTK_TimerStop(const char *name);
int CCTK_TimerStopI(int this_timer);
int CCTK_TimerReset(const char *name);
int CCTK_TimerResetI(int this_timer);
int CCTK_Timer(const char *name, cTimerData *info);
int CCTK_TimerI(int this_timer, cTimerData *info);

cTimerData *CCTK_TimerCreateData(void);
int CCTK_TimerDestroyData(cTimerData *info);

#ifdef __cplusplus
}
#endif

#define INITIALISATION 0
#define EVOLUTION      1
#define ELLIPTIC       2

#endif
