 /*@@
   @header    cctki_schedule.h
   @date      Wed Sep 15 22:49:24 1999
   @author    Tom Goodale
   @desc 
   Schedule stuff which can be seen by the rest of the flesh.
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTKI_SCHEDULE_H_
#define _CCTKI_SCHEDULE_H_

/* Types needed by other routines. */

typedef enum {sched_mod_none, sched_before, sched_after, sched_while} t_sched_modifier_type;

typedef struct T_SCHED_MODIFIER
{
  struct T_SCHED_MODIFIER *next;

  t_sched_modifier_type type;
  
  char *argument;

} t_sched_modifier;


#ifdef __cplusplus
extern "C" {
#endif

t_sched_modifier *CCTKi_ScheduleAddModifer(t_sched_modifier *orig, 
					   const char *modifier, 
					   const char *argument);

int CCTKi_ScheduleFunction(const char *gname, 
			   const char *fname, 
			   void *func, 
			   t_sched_modifier *modifiers, 
			   void *attributes);

int CCTKi_ScheduleGroup(const char *gname, 
			const char *thisname, t_sched_modifier *modifiers, 
			void *attributes);

int CCTKi_ScheduleSortAllGroups(void);

#ifdef __cplusplus
}
#endif

#endif /* _CCTKI_SCHEDULE_H_ */
