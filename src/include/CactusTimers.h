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

typedef enum {val_none, val_int, val_long, val_double} t_TimerValType;

typedef struct
{
  t_TimerValType type;
  const char *heading;
  const char *units;
  union
  {
    int        i;
    long int   l;
    double     d;
  } val;
} t_TimerVal;

typedef struct
{
  int n_vals;
  t_TimerVal *vals;
} t_TimerInfo;


typedef struct
{
  t_TimerInfo info;
  void *(*create)(int);
  void (*destroy)(int, void *);
  void (*start)(int, void *);
  void (*stop)(int, void *);
  void (*reset)(int, void *);
  void (*get)(int, void *, t_TimerVal *);
  void (*set)(int, void *, t_TimerVal *);
} t_TimerFuncs;



/* Function prototypes */

#ifdef __cplusplus
extern "C" {
#endif

int CCTK_TimerRegister(const char *name, t_TimerFuncs *functions);
int CCTK_TimerCreate(const char *name);
int CCTK_TimerCreateI(void);
void CCTK_TimerDestroy(const char *name);
void CCTK_TimerDestroyI(int this_timer);
void CCTK_TimerStart(const char *name);
void CCTK_TimerStartI(int this_timer);
void CCTK_TimerStop(const char *name);
void CCTK_TimerStopI(int this_timer);
void CCTK_TimerReset(const char *name);
void CCTK_TimerResetI(int this_timer);
void CCTK_TimerGet(const char *name, t_TimerInfo *info);
void CCTK_TimerGetI(int this_timer, t_TimerInfo *info);

t_TimerInfo *CCTK_TimerCreateInfo(void);
void CCTK_TimerDestroyInfo(t_TimerInfo *info);

#ifdef __cplusplus
}
#endif

#define INITIALISATION 0
#define EVOLUTION      1
#define ELLIPTIC       2

#endif
