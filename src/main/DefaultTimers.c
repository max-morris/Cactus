 /*@@
   @file      DefaultTimers.c
   @date      Wed Oct 20 16:17:42 1999
   @author    Tom Goodale
   @desc 
   Default Cactus timers
   @enddesc 
 @@*/

static char *rcsid = "$Header$";

#include <stdio.h>
#include <stdlib.h>

#include "config.h"

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include "CactusTimers.h"

/* Prototypes for registration functions */

#ifdef HAVE_TIME_GETTIMEOFDAY
void CCTKi_RegisterTimersGetTimeOfDay(void);
#endif

#ifdef HAVE_TIME_GETRUSAGE
void CCTKi_RegisterTimersGetrUsage(void);
#endif

 /*@@
   @routine    CCTKi_RegisterDefaultTimerFunctions
   @date       Wed Oct 20 18:27:20 1999
   @author     Tom Goodale
   @desc 

   Master flesh timer registration function./
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTKi_RegisterDefaultTimerFunctions(void)
{

#ifdef HAVE_TIME_GETTIMEOFDAY
  CCTKi_RegisterTimersGetTimeOfDay();
#endif

#ifdef HAVE_TIME_GETRUSAGE
  CCTKi_RegisterTimersGetrUsage();
#endif

}



/*********************************************************************
 ****************             Actual timers      *********************
 *********************************************************************/



/*********************************************************************
 ****************   gettimeofday based timer     *********************
 *********************************************************************/

#ifdef HAVE_TIME_GETTIMEOFDAY

#include <unistd.h>

/* A structure to hold the relevent data */
typedef struct 
{
  struct timeval total;
  struct timeval last;
} t_GetTimeOfDayTimer;

static char *GetTimeOfDayHeading = "Data from gettimeofday call";
static char *GetTimeOfDayUnits   = "secs";

 /*@@
   @routine    CCTKi_TimerGetTimeOfDayCreate
   @date       Wed Oct 20 18:28:19 1999
   @author     Tom Goodale
   @desc 
   Create the timer structure for use with the gettimeofday function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void *CCTKi_TimerGetTimeOfDayCreate(int timernum)
{
  t_GetTimeOfDayTimer *this;

  this = malloc(sizeof(t_GetTimeOfDayTimer));

  if(this)
  {
    this->total.tv_sec  = 0;
    this->total.tv_usec = 0;
  }

  return this;
}

 /*@@
   @routine    CCTKi_TimerGetTimeOfDayDestroy
   @date       Wed Oct 20 18:28:19 1999
   @author     Tom Goodale
   @desc 
   Destroy the timer structure for use with the gettimeofday function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_TimerGetTimeOfDayDestroy(int timernum, void *data)
{
  if(data)
  {
    free(data);
  }
}


 /*@@
   @routine    CCTKi_TimerGetTimeOfDayStart
   @date       Wed Oct 20 18:28:19 1999
   @author     Tom Goodale
   @desc 
   Start the timer with the gettimeofday function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_TimerGetTimeOfDayStart(int timernum, void *idata)
{
  t_GetTimeOfDayTimer *data;

  struct timeval tp;
  struct timezone tzp;

  data = (t_GetTimeOfDayTimer *)idata;

  gettimeofday(&tp, &tzp);

  data->last = tp;

#ifdef DEBUG_TIMERS
  printf("Starting gettimeofday timer %d\n", timernum);
#endif
}

 /*@@
   @routine    CCTKi_TimerGetTimeOfDayStart
   @date       Wed Oct 20 18:28:19 1999
   @author     Tom Goodale
   @desc 
   Stop the timer with the gettimeofday function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_TimerGetTimeOfDayStop(int timernum, void *idata)
{
  t_GetTimeOfDayTimer *data;

  struct timeval tp;
  struct timezone tzp;

  data = (t_GetTimeOfDayTimer *)idata;

  gettimeofday(&tp, &tzp);

  data->total.tv_sec  += (tp.tv_sec  - data->last.tv_sec);
  data->total.tv_usec += (tp.tv_usec - data->last.tv_usec);

#ifdef DEBUG_TIMERS
  printf("Stopping gettimeofday timer %d\n", timernum);
#endif
}

 /*@@
   @routine    CCTKi_TimerGetTimeOfDayReset
   @date       Wed Oct 20 18:28:19 1999
   @author     Tom Goodale
   @desc 
   Reset the timer with the gettimeofday function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_TimerGetTimeOfDayReset(int timernum, void *idata)
{
  t_GetTimeOfDayTimer *data;

  data = (t_GetTimeOfDayTimer *)idata;

  data->last.tv_sec  = 0;
  data->last.tv_usec  = 0;
  data->total.tv_sec  = 0;
  data->total.tv_usec  = 0;

}

 /*@@
   @routine    CCTKi_TimerGetTimeOfDayGet
   @date       Wed Oct 20 18:28:19 1999
   @author     Tom Goodale
   @desc 
   Get the time recorded with the gettimeofday function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_TimerGetTimeOfDayGet(int timernum, void *idata, t_TimerVal *vals)
{
  t_GetTimeOfDayTimer *data;

  data = (t_GetTimeOfDayTimer *)idata;

  vals[0].type    = val_double;
  vals[0].heading = GetTimeOfDayHeading;
  vals[0].units   = GetTimeOfDayUnits;
  vals[0].val.d   = data->total.tv_sec + (double)data->total.tv_usec/1000000.0;
}

 /*@@
   @routine    CCTKi_TimerGetTimeOfDaySet
   @date       Wed Oct 20 18:28:19 1999
   @author     Tom Goodale
   @desc 
   Set the time for a gettimeofday function based timer.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_TimerGetTimeOfDaySet(int timernum, void *idata, t_TimerVal *vals)
{
  t_GetTimeOfDayTimer *data;

  data = (t_GetTimeOfDayTimer *)idata;

  data->total.tv_sec  = (long)vals[0].val.d;
  data->total.tv_usec = (long)(1000000*vals[0].val.d-data->total.tv_sec);
}

 /*@@
   @routine    CCTKi_RegisterTimersGetTimeOfDay
   @date       Wed Oct 20 18:32:17 1999
   @author     Tom Goodale
   @desc 
   Register all the timer functions associated with the gettimeofday function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_RegisterTimersGetTimeOfDay(void)
{
  t_TimerFuncs functions;

  functions.info.n_vals = 1;
  functions.create  = CCTKi_TimerGetTimeOfDayCreate;
  functions.destroy = CCTKi_TimerGetTimeOfDayDestroy;
  functions.start   = CCTKi_TimerGetTimeOfDayStart;
  functions.stop    = CCTKi_TimerGetTimeOfDayStop;
  functions.reset   = CCTKi_TimerGetTimeOfDayReset;
  functions.get     = CCTKi_TimerGetTimeOfDayGet;
  functions.set     = CCTKi_TimerGetTimeOfDaySet;

  CCTK_TimerRegister("GetrUsage", &functions);
}
    
#endif /* HAVE_TIME_GETTIMEOFDAY */


/*********************************************************************
 ****************       getrusage based timers     *******************
 *********************************************************************/

#ifdef HAVE_TIME_GETRUSAGE

#include <sys/resource.h>

/* A structure to hold the relevent data */
typedef struct 
{
  struct timeval total;
  struct timeval last;
} t_GetrUsageTimer;

static char *GetrUsageHeading = "Data from getrusage call";
static char *GetrUsageUnits   = "secs";

 /*@@
   @routine    CCTKi_TimerGetrUsageCreate
   @date       Wed Oct 20 18:28:19 1999
   @author     Tom Goodale
   @desc 
   Create the timer structure for use with the getrusage function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void *CCTKi_TimerGetrUsageCreate(int timernum)
{
  t_GetrUsageTimer *this;

  this = malloc(sizeof(t_GetrUsageTimer));

  if(this)
  {
    this->total.tv_sec  = 0;
    this->total.tv_usec = 0;
  }

  return this;
}

 /*@@
   @routine    CCTKi_TimerGetrUsageDestroy
   @date       Wed Oct 20 18:28:19 1999
   @author     Tom Goodale
   @desc 
   Destroy the timer structure for use with the getrusage function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_TimerGetrUsageDestroy(int timernum, void *data)
{
  if(data)
  {
    free(data);
  }
}


 /*@@
   @routine    CCTKi_TimerGetrUsageStart
   @date       Wed Oct 20 18:28:19 1999
   @author     Tom Goodale
   @desc 
   Start the timer with the getrusage function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_TimerGetrUsageStart(int timernum, void *idata)
{
  t_GetrUsageTimer *data;

  struct rusage ru;

  data = (t_GetrUsageTimer *)idata;

  getrusage(RUSAGE_SELF, &ru);

  data->last = ru.ru_utime;

#ifdef DEBUG_TIMERS
  printf("Starting getrusage timer %d\n", timernum);
#endif
}

 /*@@
   @routine    CCTKi_TimerGetrUsageStart
   @date       Wed Oct 20 18:28:19 1999
   @author     Tom Goodale
   @desc 
   Stop the timer with the getrusage function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_TimerGetrUsageStop(int timernum, void *idata)
{
  t_GetrUsageTimer *data;

  struct rusage ru;

  data = (t_GetrUsageTimer *)idata;

  getrusage(RUSAGE_SELF, &ru);

  data->total.tv_sec += ru.ru_utime.tv_sec - data->last.tv_sec;
  data->total.tv_usec += ru.ru_utime.tv_usec - data->last.tv_usec;

#ifdef DEBUG_TIMERS
  printf("Starting getrusage timer %d\n", timernum);
#endif
}

 /*@@
   @routine    CCTKi_TimerGetrUsageReset
   @date       Wed Oct 20 18:28:19 1999
   @author     Tom Goodale
   @desc 
   Reset the timer with the getrusage function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_TimerGetrUsageReset(int timernum, void *idata)
{
  t_GetrUsageTimer *data;

  data = (t_GetrUsageTimer *)idata;

  data->total.tv_sec  = 0;
  data->total.tv_usec = 0;

}

 /*@@
   @routine    CCTKi_TimerGetrUsageGet
   @date       Wed Oct 20 18:28:19 1999
   @author     Tom Goodale
   @desc 
   Get the time recorded with the getrusage function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_TimerGetrUsageGet(int timernum, void *idata, t_TimerVal *vals)
{
  t_GetrUsageTimer *data;

  data = (t_GetrUsageTimer *)idata;

  vals[0].type    = val_double;
  vals[0].heading = GetrUsageHeading;
  vals[0].units   = GetrUsageUnits;
  vals[0].val.d   = data->total.tv_sec + (double)data->total.tv_usec/1000000.0;

}

 /*@@
   @routine    CCTKi_TimerGetrUsageSet
   @date       Wed Oct 20 18:28:19 1999
   @author     Tom Goodale
   @desc 
   Set the time for a getrusage function based timer.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_TimerGetrUsageSet(int timernum, void *idata, t_TimerVal *vals)
{
  t_GetrUsageTimer *data;

  data = (t_GetrUsageTimer *)idata;

  data->total.tv_sec  = (long)vals[0].val.d;
  data->total.tv_usec = (long)(1000000*vals[0].val.d-data->total.tv_sec);
}

 /*@@
   @routine    CCTKi_RegisterTimersGetrUsage
   @date       Wed Oct 20 18:32:17 1999
   @author     Tom Goodale
   @desc 
   Register all the timer functions associated with the getrusage function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_RegisterTimersGetrUsage(void)
{
  t_TimerFuncs functions;

  functions.info.n_vals = 1;
  functions.create  = CCTKi_TimerGetrUsageCreate;
  functions.destroy = CCTKi_TimerGetrUsageDestroy;
  functions.start   = CCTKi_TimerGetrUsageStart;
  functions.stop    = CCTKi_TimerGetrUsageStop;
  functions.reset   = CCTKi_TimerGetrUsageReset;
  functions.get     = CCTKi_TimerGetrUsageGet;
  functions.set     = CCTKi_TimerGetrUsageSet;

  CCTK_TimerRegister("GetrUsage", &functions);

}

#endif /* HAVE_TIME_GETRUSAGE */
