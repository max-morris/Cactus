 /*@@
   @file      CactusTimers.c
   @date      Thu Oct  8 18:30:28 1998
   @author    Tom Goodale
   @desc 
   Timer stuff.
   @enddesc 
 @@*/


#include <stdlib.h>
#include <string.h>

#include "CactusTimers.h"
#include "StoreHandledData.h"


static char *rcsid = "$Header$";



cTimer *CactusNewTimer(void)
{
  cTimer *timer;

  timer = (cTimer*) malloc(sizeof(cTimer));

  return timer;
}

  

void CactusStartTimer(cTimer *timer)
{
}

void CactusStopTimer(cTimer *timer)
{
}

void CactusResetTimer(cTimer *timer)
{
}


typedef struct
{
  void *(*create)(int);
  void (*destroy)(int, void *);
  void (*start)(int, void *);
  void (*stop)(int, void *);
  void (*reset)(int, void *);
  char *(*get)(int, void *);
  void (*set)(int, void *, const char *);
} t_TimerFuncs;

typedef struct
{
  void **data;
} t_Timer;


static int n_timertypes = 0;
static cHandledData *handles = NULL;

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
int CCTK_TimerRegister(const char *name, t_TimerFuncs *functions)
{
  int handle;
  t_TimerFuncs *newfuncs;

  newfuncs = (t_TimerFuncs *)malloc(sizeof(t_TimerFuncs));

  if(newfuncs)
  {
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
  t_TimerFuncs *funcs;
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
	  funcs = (t_TimerFuncs *)Util_GetHandledData(handles, handle);

	  timer->data[handle] = funcs->create(this_timer);
	}
	retval = this_timer;
      }
    }
  }

  return this_timer;
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
void CCTK_TimerDestroy(const char *name)
{
  t_Timer *timer;
  t_TimerFuncs *funcs;
  int this_timer;
  int handle;

  if(this_timer = Util_GetHandle(timers, name, (void **)&timer))
  {
    if(timer)
    {
      if(timer->data)
      {
	/* Destroy the timer info for this timer */
	for(handle = 0; handle < n_timertypes; handle++)
	{
	  funcs = (t_TimerFuncs *)Util_GetHandledData(handles, handle);
	  funcs->destroy(this_timer, timer->data[handle]);
	}
	free(timer->data);
	free(timer);
	Util_DeleteHandle(timers, this_timer);
      }
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
void CCTK_TimerStart(const char *name)
{
  t_Timer *timer;
  t_TimerFuncs *funcs;
  int this_timer;
  int handle;

  if(this_timer = Util_GetHandle(timers, name, (void **)&timer))
  {
    if(timer)
    {
      if(timer->data)
      {
	/* Start the timer info for this timer */
	for(handle = 0; handle < n_timertypes; handle++)
	{
	  funcs = (t_TimerFuncs *)Util_GetHandledData(handles, handle);
	  funcs->start(this_timer, timer->data[handle]);
	}
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
void CCTK_TimerStop(const char *name)
{
  t_Timer *timer;
  t_TimerFuncs *funcs;
  int this_timer;
  int handle;

  if(this_timer = Util_GetHandle(timers, name, (void **)&timer))
  {
    if(timer)
    {
      if(timer->data)
      {
	/* Stop the timer info for this timer */
	for(handle = 0; handle < n_timertypes; handle++)
	{
	  funcs = (t_TimerFuncs *)Util_GetHandledData(handles, handle);
	  funcs->stop(this_timer, timer->data[handle]);
	}
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
void CCTK_TimerReset(const char *name)
{
  t_Timer *timer;
  t_TimerFuncs *funcs;
  int this_timer;
  int handle;

  if(this_timer = Util_GetHandle(timers, name, (void **)&timer))
  {
    if(timer)
    {
      if(timer->data)
      {
	/* Start the timer info for this timer */
	for(handle = 0; handle < n_timertypes; handle++)
	{
	  funcs = (t_TimerFuncs *)Util_GetHandledData(handles, handle);
	  funcs->reset(this_timer, timer->data[handle]);
	}
      }
    }
  }
}

 /*@@
   @routine    CCTK_TimerGet
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
char *CCTK_TimerGet(const char *name)
{
  t_Timer *timer;
  t_TimerFuncs *funcs;
  int this_timer;
  int handle;
  char *this_val;
  char *retval;
  char *temp;
  int retlength;

  retval = NULL;
  
  if(this_timer = Util_GetHandle(timers, name, (void **)&timer))
  {
    if(timer)
    {
      if(timer->data)
      {
	/* Start the timer info for this timer */
	for(handle = 0; handle < n_timertypes; handle++)
	{
	  funcs = (t_TimerFuncs *)Util_GetHandledData(handles, handle);
	  this_val = funcs->get(this_timer, timer->data[handle]);
	  retlength += 2+strlen(this_val);
	  temp = realloc(retval, retlength);
	  if(temp)
	  { 
	    retval = temp;
	    
	    /* If this isn't the first one, add a couple of spaces. */
	    if(retlength > 2+strlen(this_val)) strcat(retval, "  ");
	    
	    strcat(retval, this_val);
	  }
	}
      }
    }
  }

  return retval;

}


