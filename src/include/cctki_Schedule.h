 /*@@
   @header    cctki_Schedule.h
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

/* Routines to create items */
t_sched_modifier *CCTKi_ScheduleAddModifier(t_sched_modifier *orig, 
                                           const char *modifier, 
                                           const char *argument);

int CCTKi_ScheduleFunction
  (
   void *function,
   const char *name,
   const char *thorn,
   const char *implementation,
   const char *description,
   const char *where,
   const char *language,
   int n_mem_groups,
   int n_comm_groups,
   int n_trigger_groups,
   int n_before,
   int n_after,
   int n_while,
   ...
  );

int CCTKi_ScheduleGroup
  (
   const char *name,
   const char *thorn,
   const char *implementation,
   const char *description,
   const char *where,
   int n_mem_groups,
   int n_comm_groups,
   int n_trigger_groups,
   int n_before,
   int n_after,
   int n_while,
   ...
  );

int CCTKi_ScheduleGroupStorage(const char *group);

int CCTKi_ScheduleGroupComm(const char *group);

int CCTKi_ScheduleTraverse(const char *where, 
			   void *GH,   
			   int (*calling_function)(void *, void *, void *));

int CCTKi_ScheduleGHInit(void *GH);


int CCTKi_DoScheduleFunction
  (
   const char *gname, 
   const char *fname, 
   void *func, 
   t_sched_modifier *modifiers, 
   void *attributes
  );

int CCTKi_DoScheduleGroup
  (
   const char *gname, 
   const char *thisname, 
   t_sched_modifier *modifiers, 
   void *attributes
  );

/* Routine to sort the groups - must be called before traversal. */
int CCTKi_DoScheduleSortAllGroups(void);

/* Traversal routine */
int CCTKi_DoScheduleTraverse
  (
   const char *group_name,
   int (*item_entry)(void *, void *),
   int (*item_exit)(void *, void *),
   int  (*while_check)(int, char **, void *, void *),
   int (*function_process)(void *, void *, void *),
   void *data
  );

#ifdef __cplusplus
}
#endif

#endif /* _CCTKI_SCHEDULE_H_ */
