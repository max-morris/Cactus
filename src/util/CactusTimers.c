 /*@@
   @file      CactusTimers.c
   @date      Thu Oct  8 18:30:28 1998
   @author    Tom Goodale
   @desc 
   Timer stuff.
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cctk_Flesh.h"
#include "CactusTimers.h"
#include "StoreHandledData.h"


static const char *rcsid = "$Header$";

CCTK_FILEVERSION(util_CactusTimers_c)

typedef struct
{
  void **data;
} t_Timer;

static void CCTKi_TimerDestroy(int this_timer, t_Timer *timer);
static void CCTKi_TimerStart(int this_timer, t_Timer *timer);
static void CCTKi_TimerStop(int this_timer, t_Timer *timer);
static void CCTKi_TimerReset(int this_timer, t_Timer *timer);
static void CCTKi_Timer(int this_timer, t_Timer *timer, cTimerData *info);

static int n_timertypes = 0;
static cHandledData *handles = NULL;

/* The total number of timer values. */
static int n_timer_vals = 0;


static int n_timers = 0;
static cHandledData *timers = NULL;

 /*@@
   @routine    CCTK_TimerRegister
   @date       Wed Sep  1 10:09:27 1999
   @author     Tom Goodale
   @desc 
   Registers a new timer function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_TimerRegister(const char *name, cTimerFuncs *functions)
{
  int handle;
  cTimerFuncs *newfuncs;

  newfuncs = (cTimerFuncs *)malloc(sizeof(cTimerFuncs));

  if(newfuncs)
  {
    newfuncs->info.n_vals = functions->info.n_vals;
    newfuncs->create = functions->create;
    newfuncs->destroy = functions->destroy;
    newfuncs->start = functions->start;
    newfuncs->stop = functions->stop;
    newfuncs->reset = functions->reset;
    newfuncs->get = functions->get;
    newfuncs->set = functions->set;
  }

  handle = Util_NewHandle(&handles, name, newfuncs);
  n_timertypes++;
  n_timer_vals += functions->info.n_vals;

  return handle;
}


 /*@@
   @routine    CCTK_TimerCreate
   @date       Wed Sep  1 10:09:57 1999
   @author     Tom Goodale
   @desc 
   Creates a new timer
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_TimerCreate(const char *name)
{
  int retval;
  t_Timer *timer;
  cTimerFuncs *funcs;
  int this_timer;
  int handle;

  this_timer = -3;

  if(Util_GetHandle(timers, name, (void **)&timer) > -1)
  {
    /* Handle already exists */
    retval = -3;
  }
  else
  {
    timer = (t_Timer *)malloc(sizeof(t_Timer));

    if(timer)
    {
      timer->data = (void **)malloc(n_timertypes*sizeof(void *));

      if(timer->data)
      {
        /* Store the data structure for this timer */
        this_timer = Util_NewHandle(&timers, name, timer);

        /* Create the timer info for this timer */
        for(handle = 0; handle < n_timertypes; handle++)
        {
          funcs = (cTimerFuncs *)Util_GetHandledData(handles, handle);

          timer->data[handle] = funcs->create(this_timer);
        }
        retval = this_timer;
      }
      else
      {
        free(timer);
        retval = -2;
      }
    }
    else
    {
      retval = -2;
    }
  }

  return retval;
}

 /*@@
   @routine    CCTK_TimerCreateI
   @date       Fri Oct 22 10:21:14 1999
   @author     Tom Goodale
   @desc 
   Creates a timer with a unique name.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_TimerCreateI(void)
{
  int retval;
  char name[20];

  sprintf(name, "timer_%d", n_timers++);

  retval = CCTK_TimerCreate(name);

  return retval;
}

 /*@@
   @routine    CCTK_TimerDestroy
   @date       Wed Sep  1 10:10:20 1999
   @author     Tom Goodale
   @desc 
   Destroys an old timer
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_TimerDestroy(const char *name)
{
  t_Timer *timer;
  int this_timer;

  if((this_timer = Util_GetHandle(timers, name, (void **)&timer)) > -1)
  {
    CCTKi_TimerDestroy(this_timer, timer);
  }

  return 0;
}

 /*@@
   @routine    CCTK_TimerDestroyI
   @date       Thu Oct 21 14:12:51 1999
   @author     Tom Goodale
   @desc 
   Destroys a timer by its handle index
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_TimerDestroyI(int this_timer)
{
  t_Timer *timer;

  if((timer = Util_GetHandledData(timers, this_timer)))
  {
    CCTKi_TimerDestroy(this_timer, timer);
  }
  return 0;
}
      
 /*@@
   @routine    CCTKi_TimerDestroy
   @date       Thu Oct 21 14:14:58 1999
   @author     Tom Goodale
   @desc 
   Internal function which destroys a timer.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static void CCTKi_TimerDestroy(int this_timer, t_Timer *timer)
{
  cTimerFuncs *funcs;
  int handle;

  if(timer)
  {
    if(timer->data)
    {
      /* Destroy the timer info for this timer */
      for(handle = 0; handle < n_timertypes; handle++)
      {
        funcs = (cTimerFuncs *)Util_GetHandledData(handles, handle);
        funcs->destroy(this_timer, timer->data[handle]);
      }
      free(timer->data);
      free(timer);
      Util_DeleteHandle(timers, this_timer);
    }
  }
}

 /*@@
   @routine    CCTK_TimerStart
   @date       Wed Sep  1 10:10:38 1999
   @author     Tom Goodale
   @desc 
   Starts a timer counting.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_TimerStart(const char *name)
{
  t_Timer *timer;
  int this_timer;

  if((this_timer = Util_GetHandle(timers, name, (void **)&timer)) > -1)
  {
    CCTKi_TimerStart(this_timer, timer);
  }

  return 0;
}

int CCTK_TimerStartI(int this_timer)
{
  t_Timer *timer;

  if((timer = Util_GetHandledData(timers, this_timer)))
  {
    CCTKi_TimerStart(this_timer, timer);
  }
  return 0;
}

static void CCTKi_TimerStart(int this_timer, t_Timer *timer)
{
  cTimerFuncs *funcs;
  int handle;

  if(timer)
  {
    if(timer->data)
    {
      /* Start the timer info for this timer */
      for(handle = 0; handle < n_timertypes; handle++)
      {
        funcs = (cTimerFuncs *)Util_GetHandledData(handles, handle);
        funcs->start(this_timer, timer->data[handle]);
      }
    }
  }
}

 /*@@
   @routine    CCTK_TimerStop
   @date       Wed Sep  1 10:10:38 1999
   @author     Tom Goodale
   @desc 
   Stops a timer counting.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_TimerStop(const char *name)
{
  t_Timer *timer;
  int this_timer;

  if((this_timer = Util_GetHandle(timers, name, (void **)&timer)) > -1)
  {
    CCTKi_TimerStop(this_timer, timer);
  }
  return 0;
}

int CCTK_TimerStopI(int this_timer)
{
  t_Timer *timer;

  if((timer = Util_GetHandledData(timers, this_timer)))
  {
    CCTKi_TimerStop(this_timer, timer);
  }
  return 0;
}

static void CCTKi_TimerStop(int this_timer, t_Timer *timer)
{
  cTimerFuncs *funcs;
  int handle;

  if(timer)
  {
    if(timer->data)
    {
      /* Start the timer info for this timer */
      for(handle = 0; handle < n_timertypes; handle++)
      {
        funcs = (cTimerFuncs *)Util_GetHandledData(handles, handle);
        funcs->stop(this_timer, timer->data[handle]);
      }
    }
  }
}

 /*@@
   @routine    CCTK_TimerReset
   @date       Wed Sep  1 10:10:38 1999
   @author     Tom Goodale
   @desc 
   Resets a timer.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_TimerReset(const char *name)
{
  t_Timer *timer;
  int this_timer;

  if((this_timer = Util_GetHandle(timers, name, (void **)&timer)) > -1)
  {
    CCTKi_TimerReset(this_timer, timer);
  }
  return 0;
}

int CCTK_TimerResetI(int this_timer)
{
  t_Timer *timer;

  if((timer = Util_GetHandledData(timers, this_timer)))
  {
    CCTKi_TimerReset(this_timer, timer);
  }
  return 0;
}

static void CCTKi_TimerReset(int this_timer, t_Timer *timer)
{
  cTimerFuncs *funcs;
  int handle;

  if(timer)
  {
    if(timer->data)
    {
      /* Start the timer info for this timer */
      for(handle = 0; handle < n_timertypes; handle++)
      {
        funcs = (cTimerFuncs *)Util_GetHandledData(handles, handle);
        funcs->reset(this_timer, timer->data[handle]);
      }
    }
  }
}

 /*@@
   @routine    CCTK_Timer
   @date       Wed Sep  1 10:10:38 1999
   @author     Tom Goodale
   @desc 
   Gets the values of a timer.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_Timer(const char *name, cTimerData *info)
{
  t_Timer *timer;
  int this_timer;

  if((this_timer = Util_GetHandle(timers, name, (void **)&timer)) > -1)
  {
    CCTKi_Timer(this_timer, timer, info);
  }

  return 0;

}

int CCTK_TimerI(int this_timer, cTimerData *info)
{
  t_Timer *timer;

  if((timer = Util_GetHandledData(timers, this_timer)))
  {
    CCTKi_Timer(this_timer, timer, info);
  }

  return 0;
}




static void CCTKi_Timer(int this_timer, t_Timer *timer, cTimerData *info)
{
  cTimerFuncs *funcs;
  int handle;
  int total_vars;
  
  if(timer)
  {
    total_vars = 0;
    if(timer->data)
    {
      /* Start the timer info for this timer */
      for(handle = 0; handle < n_timertypes; handle++)
      {
        funcs = (cTimerFuncs *)Util_GetHandledData(handles, handle);
        funcs->get(this_timer, timer->data[handle], &(info->vals[total_vars]));
        
        total_vars += funcs->info.n_vals;
      }
    }
    info->n_vals = total_vars;
  }
  else
  {
    info->n_vals = 0;
  }

}

cTimerData *CCTK_TimerCreateData(void)
{
  cTimerData *retval;

  retval = (cTimerData *)malloc(sizeof(cTimerData));

  if(retval)
  {
    retval->n_vals = n_timer_vals;

    retval->vals = (cTimerVal *)malloc(n_timer_vals*sizeof(cTimerVal));

    if(! retval->vals)
    {
      free(retval);
      retval = NULL;
    }
  }

  return retval;
}

int CCTK_TimerDestroyData(cTimerData *info)
{
  if(info)
  {
    if(info->vals)
    {
      free(info->vals);
      info->vals = NULL;
    }
    free(info);
  }
  return 0;
}
  
