 /*@@
   @header    Schedule.h
   @date      Mon Sep 13 12:24:48 1999
   @author    Tom Goodale
   @desc 
   Header file for Schedule routines, etc.
   @enddesc 
   @version $Header$
 @@*/

#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

typedef enum {sched_item_none, sched_group, sched_function} t_sched_item_type;

typedef enum {sched_mod_none, sched_before, sched_after, sched_while} t_sched_modifier_type;

typedef struct T_SCHED_MODIFIER
{
  struct T_SCHED_MODIFIER *next;

  t_sched_modifier_type type;
  
  char *argument;

} t_sched_modifier;


#endif
