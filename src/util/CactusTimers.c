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
#include "cctk_FortranString.h"

#include "cctk_Flesh.h"
#include "cctk_Timers.h"
#include "cctk_WarnLevel.h"
#include "StoreHandledData.h"


static const char *rcsid = "$Header$";

CCTK_FILEVERSION(util_CactusTimers_c)

/********************************************************************
 ***************     Fortran Wrapper Prototypes   *******************
 ********************************************************************/
void CCTK_FCALL CCTK_FNAME (CCTK_NumTimers)
                           (int *ierr);
void CCTK_FCALL CCTK_FNAME (CCTK_NumClocks)
                           (int *ierr);
void CCTK_FCALL CCTK_FNAME (CCTK_TimerCreate)
                           (int *timer_index, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (CCTK_TimerCreateI)
                           (int *ierr);
void CCTK_FCALL CCTK_FNAME (CCTK_TimerDestroy)
                           (int *ierr, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (CCTK_TimerDestroyI)
                           (int *ierr, int *this_timer);
void CCTK_FCALL CCTK_FNAME (CCTK_TimerStart)
                           (int *ierr, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (CCTK_TimerStartI)
                           (int *ierr, int *this_timer);
void CCTK_FCALL CCTK_FNAME (CCTK_TimerStop)
                           (int *ierr, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (CCTK_TimerStopI)
                           (int *ierr, int *this_timer);
void CCTK_FCALL CCTK_FNAME (CCTK_TimerReset)
                           (int *ierr, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (CCTK_TimerResetI)
                           (int *ierr, int *this_timer);
void CCTK_FCALL CCTK_FNAME (CCTK_TimerPrintDataI)
                           (int *ierr, int *this_timer, int *this_clock);
void CCTK_FCALL CCTK_FNAME (CCTK_TimerPrintData)
                           (int *ierr, TWO_FORTSTRING_ARG);


/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/
typedef struct
{
  void **data;
} t_Timer;


/********************************************************************
 ********************* Local Routine Prototypes *********************
 ********************************************************************/
static int  CCTKi_TimerCreate(const char *timername);
static void CCTKi_TimerDestroy(int this_timer, t_Timer *timer);
static void CCTKi_TimerStart(int this_timer, t_Timer *timer);
static void CCTKi_TimerStop(int this_timer, t_Timer *timer);
static void CCTKi_TimerReset(int this_timer, t_Timer *timer);
static void CCTKi_Timer(int this_timer, t_Timer *timer, cTimerData *info);


/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/
static int n_clocks = 0;
static cHandledData *clocks = NULL;

/* The total number of clock values. */
static int n_clock_vals = 0;

static int n_timers = 0;
static cHandledData *timers = NULL;


 /*@@
   @routine    CCTK_ClockRegister
   @date       Wed Sep  1 10:09:27 1999
   @author     Tom Goodale
   @desc 
   Registers a new timer function (clock).
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_ClockRegister(const char *name, const cClockFuncs *functions)
{
  void *tmp;
  int handle;
  t_Timer *timer;
  cClockFuncs *newfuncs;

  newfuncs = (cClockFuncs *)malloc(sizeof(cClockFuncs));

  if(newfuncs)
  {
    newfuncs->name = name;
    newfuncs->n_vals = functions->n_vals;
    newfuncs->create = functions->create;
    newfuncs->destroy = functions->destroy;
    newfuncs->start = functions->start;
    newfuncs->stop = functions->stop;
    newfuncs->reset = functions->reset;
    newfuncs->get = functions->get;
    newfuncs->set = functions->set;
  }

  /* Add this clock to all existing timers */
  for (handle = 0; handle < n_timers; handle++)
  {
    timer = (t_Timer *) Util_GetHandledData (timers, handle);

    tmp = realloc (timer->data, (n_clocks + 1) * sizeof (void *));
    if (tmp)
    {
      timer->data = (void **) tmp;
      timer->data[n_clocks] = functions->create (handle);
    }
  }

  /* Add this clock to the clock database */
  handle = Util_NewHandle(&clocks, name, newfuncs);
  n_clocks++;
  n_clock_vals += functions->n_vals;

  return handle;
}


 /*@@
   @routine    CCTK_ClockName
   @date       Sat 29 Dec 2001
   @author     Gabrielle Allen
   @desc
               Return the name of a clock from a handle
   @enddesc

   @returntype const char *
   @returndesc
   Name of clock
   @endreturndesc
@@*/
const char *CCTK_ClockName (int handle)
{
  const cClockFuncs *funcs;
  const char *clock=NULL;

  funcs = (const cClockFuncs *)Util_GetHandledData(clocks, handle);
  if (funcs)
  {
    clock = funcs->name;
  }

  return (clock);
}

 /*@@
   @routine    CCTK_ClockHandle
   @date       Sat 29 Dec 2001
   @author     Gabrielle Allen
   @desc
               Return the handle of a clock from its name
   @enddesc

   @returntype int
   @returndesc
   Handle of clock, or -1 if the clock wasn't found
   @endreturndesc
@@*/
int CCTK_ClockHandle (const char *clock)
{
  int i,handle;
  const cClockFuncs *funcs;

  handle = -1;

  for (i = 0; i < CCTK_NumClocks(); i++)
  {
    funcs = (const cClockFuncs *)Util_GetHandledData(clocks, i);
    if (funcs)
    {
      if (strcmp(funcs->name,clock)==0)
      {
	handle = i;
      }
    }
  }

  return (handle);
}

 
 /*@@
   @routine    CCTK_NumClocks
   @date       Sat 29 Dec 2001
   @author     Gabrielle Allen
   @desc
               Return the total number of clocks
   @enddesc

   @returntype int
   @returndesc
               the total number of Cactus clocks
   @endreturndesc
@@*/
int CCTK_NumClocks (void)
{
  return (n_clocks);
}

void CCTK_FCALL CCTK_FNAME (CCTK_NumClocks)
                           (int *nclocks)
{
  *nclocks = CCTK_NumClocks();
}


 /*@@
   @routine    CCTK_NumTimers
   @date       Tue 3 Jul 2001
   @author     Thomas Radke
   @desc
               Return the total number of timers.
   @enddesc

   @returntype int
   @returndesc
               the total number of Cactus timers
   @endreturndesc
@@*/
int CCTK_NumTimers (void)
{
  return (n_timers);
}

void CCTK_FCALL CCTK_FNAME (CCTK_NumTimers)
                           (int *ntimers)
{
  *ntimers = CCTK_NumTimers();
}


 /*@@
   @routine    CCTK_TimerName
   @date       Tue 3 Jul 2001
   @author     Thomas Radke
   @desc
               Return the name of a Cactus timer given by its handle.
   @enddesc

   @var        timer_handle
   @vdesc      timer handle
   @vtype      int
   @vio        in
   @endvar

   @returntype int
   @returndesc
               the name of the Cactus timer, or NULL if no timer with that
               handle exists
   @endreturndesc
@@*/
const char *CCTK_TimerName (int timer_handle)
{
  return (Util_GetHandleName (timers, timer_handle));
}



 /*@@
   @routine    CCTK_TimerCreate
   @date       Wed Sep  1 10:09:57 1999
   @author     Tom Goodale
   @desc
               Creates a new timer with a given name
   @enddesc
   @calls      CCTKi_TimerCreate
@@*/
int CCTK_TimerCreate (const char *name)
{
  int retval;

  retval = CCTKi_TimerCreate (name);

  return retval;
}

void CCTK_FCALL CCTK_FNAME (CCTK_TimerCreate)
                           (int *timer_index, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (name)
  *timer_index = CCTK_TimerCreate (name);
  free (name);
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
  char timername[40];

  sprintf (timername, "UNNAMED TIMER %5d", n_timers);
  retval = CCTKi_TimerCreate(timername);

  return retval;
}

void CCTK_FCALL CCTK_FNAME (CCTK_TimerCreateI)
                           (int *ierr)
{
  *ierr = CCTK_TimerCreateI ();
}


 /*@@
   @routine    CCTKi_TimerCreate
   @date       Wed Sep  1 10:09:57 1999
   @author     Tom Goodale
   @desc 
   Creates a new timer with the name given
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int CCTKi_TimerCreate(const char *timername)
{
  int retval;
  t_Timer *timer;
  const cClockFuncs *funcs;
  int this_timer;
  int handle;


  this_timer = -3;

  if(Util_GetHandle(timers, timername, (void **)&timer) > -1)
  {
    /* Handle already exists */
    retval = -3;
  }
  else
  {
    timer = (t_Timer *)malloc(sizeof(t_Timer));

    if(timer)
    {
      timer->data = (void **)malloc(n_clocks*sizeof(void *));

      if(timer->data)
      {
        /* Store the data structure for this timer */
        this_timer = Util_NewHandle(&timers, timername, timer);
        n_timers++;

        /* Create the timer info for this timer */
        for(handle = 0; handle < n_clocks; handle++)
        {
          funcs = (const cClockFuncs *)Util_GetHandledData(clocks, handle);

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
  int retval = 0;

  if((this_timer = Util_GetHandle(timers, name, (void **)&timer)) > -1)
  {
    CCTKi_TimerDestroy(this_timer, timer);
  }
  else
  {
    CCTK_VWarn(8,__LINE__,__FILE__,"Cactus",
                   "CCTK_TimerDestroy: Timer %s not found",name);
    retval = -1;
  }
  return retval;
}

void CCTK_FCALL CCTK_FNAME (CCTK_TimerDestroy)
                           (int *ierr, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (name)
  *ierr = CCTK_TimerDestroy (name);
  free (name);
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
  int retval = 0;

  if((timer = Util_GetHandledData(timers, this_timer)))
  {
    CCTKi_TimerDestroy(this_timer, timer);
  }
  else
  {
    CCTK_VWarn(8,__LINE__,__FILE__,"Cactus",
               "CCTK_TimerDestroyI: Timer %d not found",this_timer);
    retval = -1;
  }
  return retval;
}
      
void CCTK_FCALL CCTK_FNAME (CCTK_TimerDestroyI)
                           (int *ierr, int *this_timer)
{
  *ierr = CCTK_TimerDestroyI (*this_timer);
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
  const cClockFuncs *funcs;
  int handle;

  if(timer)
  {
    if(timer->data)
    {
      /* Destroy the timer info for this timer */
      for(handle = 0; handle < n_clocks; handle++)
      {
        funcs = (const cClockFuncs *)Util_GetHandledData(clocks, handle);
        funcs->destroy(this_timer, timer->data[handle]);
      }
      free(timer->data);
      free(timer);
      Util_DeleteHandle(timers, this_timer);
      n_timers--;
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
  int retval = 0;

  if((this_timer = Util_GetHandle(timers, name, (void **)&timer)) > -1)
  {
    CCTKi_TimerStart(this_timer, timer);
  }
  else
  {
    CCTK_VWarn(8,__LINE__,__FILE__,"Cactus",
               "CCTK_TimerStart: Timer %s not found",name);
    retval = -1;
  }
  return retval;
}

void CCTK_FCALL CCTK_FNAME (CCTK_TimerStart)
                           (int *ierr, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (name)
  *ierr = CCTK_TimerStart (name);
  free (name);
}


int CCTK_TimerStartI(int this_timer)
{
  t_Timer *timer;
  int retval = 0;

  if((timer = Util_GetHandledData(timers, this_timer)))
  {
    CCTKi_TimerStart(this_timer, timer);
  }
  else
  {
    CCTK_VWarn(8,__LINE__,__FILE__,"Cactus",
               "CCTK_TimerStartI: Timer %d not found",this_timer);
    retval = -1;
  }
  return retval;
}

void CCTK_FCALL CCTK_FNAME (CCTK_TimerStartI)
                           (int *ierr, int *this_timer)
{
  *ierr = CCTK_TimerStartI (*this_timer);
}


static void CCTKi_TimerStart(int this_timer, t_Timer *timer)
{
  const cClockFuncs *funcs;
  int handle;

  if(timer)
  {
    if(timer->data)
    {
      /* Start the timer info for this timer */
      for(handle = 0; handle < n_clocks; handle++)
      {
        funcs = (const cClockFuncs *)Util_GetHandledData(clocks, handle);
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
  int retval = 0;

  if((this_timer = Util_GetHandle(timers, name, (void **)&timer)) > -1)
  {
    CCTKi_TimerStop(this_timer, timer);
  }
  else
  {
    CCTK_VWarn(8,__LINE__,__FILE__,"Cactus",
               "CCTK_TimerStop: Timer %s not found",name);
    retval = -1;
  }
  return retval;
}

void CCTK_FCALL CCTK_FNAME (CCTK_TimerStop)
                           (int *ierr, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (name)
  *ierr = CCTK_TimerStop (name);
  free (name);
}


int CCTK_TimerStopI(int this_timer)
{
  t_Timer *timer;
  int retval = 0;

  if((timer = Util_GetHandledData(timers, this_timer)))
  {
    CCTKi_TimerStop(this_timer, timer);
  }
  else
  {
    CCTK_VWarn(8,__LINE__,__FILE__,"Cactus",
               "CCTK_TimerStopI: Timer %d not found",this_timer);
    retval = -1;
  }
  return retval;
}

void CCTK_FCALL CCTK_FNAME (CCTK_TimerStopI)
                           (int *ierr, int *this_timer)
{
  *ierr = CCTK_TimerStopI (*this_timer);
}


static void CCTKi_TimerStop(int this_timer, t_Timer *timer)
{
  const cClockFuncs *funcs;
  int handle;

  if(timer)
  {
    if(timer->data)
    {
      /* Start the timer info for this timer */
      for(handle = 0; handle < n_clocks; handle++)
      {
        funcs = (const cClockFuncs *)Util_GetHandledData(clocks, handle);
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
  int retval = 0;

  if((this_timer = Util_GetHandle(timers, name, (void **)&timer)) > -1)
  {
    CCTKi_TimerReset(this_timer, timer);
  }
  else
  {
    CCTK_VWarn(8,__LINE__,__FILE__,"Cactus",
               "CCTK_TimerReset: Timer %s not found",name);
    retval = -1;
  }
  return retval;
}

void CCTK_FCALL CCTK_FNAME (CCTK_TimerReset)
                           (int *ierr, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (name)
  *ierr = CCTK_TimerReset (name);
  free (name);
}


int CCTK_TimerResetI(int this_timer)
{
  t_Timer *timer;
  int retval = 0;

  if((timer = Util_GetHandledData(timers, this_timer)))
  {
    CCTKi_TimerReset(this_timer, timer);
  }
  else
  {
    CCTK_VWarn(8,__LINE__,__FILE__,"Cactus",
               "CCTK_TimerResetI: Timer %d not found",this_timer);
    retval = -1;
  }
  return retval;
}

void CCTK_FCALL CCTK_FNAME (CCTK_TimerResetI)
                           (int *ierr, int *this_timer)
{
  *ierr = CCTK_TimerResetI (*this_timer);
}


static void CCTKi_TimerReset(int this_timer, t_Timer *timer)
{
  const cClockFuncs *funcs;
  int handle;

  if(timer)
  {
    if(timer->data)
    {
      /* Start the timer info for this timer */
      for(handle = 0; handle < n_clocks; handle++)
      {
        funcs = (const cClockFuncs *)Util_GetHandledData(clocks, handle);
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
  const cClockFuncs *funcs;
  int handle;
  int total_vars;
  
  if(timer)
  {
    total_vars = 0;
    if(timer->data)
    {
      /* Start the timer info for this timer */
      for(handle = 0; handle < n_clocks; handle++)
      {
        funcs = (const cClockFuncs *)Util_GetHandledData(clocks, handle);
        funcs->get(this_timer, timer->data[handle], &(info->vals[total_vars]));
        
        total_vars += funcs->n_vals;
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
    retval->n_vals = n_clock_vals;

    retval->vals = (cTimerVal *)malloc(n_clock_vals*sizeof(cTimerVal));

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
  

 /*@@
   @routine    CCTK_TimerPrintDataI
   @date       
   @author     David Rideout
   @desc 
   Print the values of a timer for a given clock
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_TimerPrintDataI(int this_timer, int this_clock)
{
  int retval;
  cTimerData *info;
  int i,timer;
  int firstclock;
  int lastclock;
  int firsttimer;
  int lasttimer;

  retval = 0;

  if (this_timer == -1)
  {
    firsttimer = 0;
    lasttimer = CCTK_NumTimers();
  }
  else
  {
    firsttimer = this_timer;
    lasttimer = firsttimer + 1;
    if (firsttimer < 0 || firsttimer > CCTK_NumTimers())
    {
      CCTK_VWarn(8,__LINE__,__FILE__,"Cactus",
		 "CCTK_TimerPrintDataI: Timer %d not found",i);
      retval = -1;
      lasttimer = firsttimer;
    }
  }

  for (timer = firsttimer; timer < lasttimer; timer++)
  {
    info = CCTK_TimerCreateData();
    CCTK_TimerI(timer,info); /* return values are always 0 */

    printf("Results from timer \"%s\":\n",CCTK_TimerName(timer));

    if (this_clock == -1)
    {
      firstclock = 0;
      lastclock = info->n_vals;
    }
    else
    {
      firstclock = this_clock;
      lastclock = this_clock+1;
    }

    for (i = firstclock; i < lastclock; i++) 
    {
      switch (info->vals[i].type) 
      {
      case val_int:
        printf("\t%s: %d %s\n", info->vals[i].heading,info->vals[i].val.i, 
               info->vals[i].units);
        break;
        
      case val_long:
        printf("\t%s: %d %s\n", info->vals[i].heading,(int) info->vals[i].val.l, 
               info->vals[i].units);
        break;
        
      case val_double:
        printf("\t%s: %.3f %s\n", info->vals[i].heading,info->vals[i].val.d, 
               info->vals[i].units);
        break;
        
      default:
        CCTK_VWarn (1, __LINE__, __FILE__, "Cactus",
               "CCTK_TimerPrintDataI: Unknown data type for timer info");
        break;
      }
    }
    CCTK_TimerDestroyData(info);
  } 

  return retval;
}

void CCTK_FCALL CCTK_FNAME (CCTK_TimerPrintDataI)
                           (int *ierr, int *this_timer, int *this_clock)
{
  *ierr = CCTK_TimerPrintDataI (*this_timer,*this_clock);
}


int CCTK_TimerPrintData (const char *name, const char *clock)
{
  int this_timer;
  int this_clock;
  int retval;

  retval = 0;

  if (!clock)
  {
    this_clock = -1;
  }
  else
  {
    this_clock = CCTK_ClockHandle(clock);
    if (this_clock == -1)
    {
      CCTK_VWarn(8,__LINE__,__FILE__,"Cactus",
		 "CCTK_TimerPrintData: Clock %s not found",clock);
      retval = -1;
    }
  }

  if (!name)
  {
    this_timer = -1;
  }
  else
  {
    this_timer = Util_GetHandle (timers, name, NULL);
    if (this_timer == -1)
    {
      CCTK_VWarn(8,__LINE__,__FILE__,"Cactus",
		 "CCTK_TimerPrintData: Timer %s not found",name);
      retval = -1;
    }
  }

  if (retval == 0)
  {
    retval = CCTK_TimerPrintDataI(this_timer,this_clock);
  }

  return (retval);
}

void CCTK_FCALL CCTK_FNAME (CCTK_TimerPrintData)
                           (int *ierr, TWO_FORTSTRING_ARG)
{
  TWO_FORTSTRING_CREATE (timer,clock)
  const char *newclock;
  const char *newtimer;

  if (strcmp(clock,"")==0)
  {
    newclock = NULL;
  }
  else
  {
    newclock = clock;
  }

  if (strcmp(timer,"")==0)
  {
    newtimer = NULL;
  }
  else
  {
    newtimer = timer;
  }

  *ierr = CCTK_TimerPrintData (newtimer,newclock);
  free (timer);
  free (clock);
}
