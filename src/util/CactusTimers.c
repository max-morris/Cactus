 /*@@
   @file      CactusTimers.c
   @date      Thu Oct  8 18:30:28 1998
   @author    Tom Goodale
   @desc 
   Timer stuff.
   @enddesc 
 @@*/

#include "CactusTimers.h"


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
