 /*@@
   @header    cctk_schedule.h
   @date      Thu Sep 16 19:05:27 1999
   @author    Tom Goodale
   @desc 
   Routines for creating schedule stuff.
   @enddesc 
 @@*/

#ifndef _CCTK_SCHEDULE_H_
#define _CCTK_SCHEDULE_H_

#ifdef __cplusplus
extern "C" {
#endif

int CCTK_ScheduleFunction(void *function,
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
                          ...);

int CCTK_ScheduleGroup(const char *name,
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
                       ...);

#ifdef __cplusplus
}
#endif

#endif /*_CCTK_SCHEDULE_H_*/
