 /*@@
   @header    Schedule.h
   @date      Mon Sep 13 12:24:48 1999
   @author    Tom Goodale
   @desc 
   Header file for Schedule routines, etc.
   @enddesc 
   @version $Header$
 @@*/

#include "cctki_schedule.h"

#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#ifdef __cplusplus
extern "C" {
#endif


int *CCTKi_ScheduleCreateIVec(int size);
void CCTKi_ScheduleDestroyIVec(int size, int *vector);
signed char **CCTKi_ScheduleCreateArray(int size);
void CCTKi_ScheduleDestroyArray(int size, signed char **array);

int CCTKi_ScheduleAddRow(int size, 
			 signed char **array, 
			 int *order, 
			 int item, 
			 int *thisorders);

int CCTKi_ScheduleSort(int size, signed char **array, int *order);

#ifdef __cplusplus
}
#endif

#endif
