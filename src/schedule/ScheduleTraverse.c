 /*@@
   @file      ScheduleTraverse.c
   @date      Thu Sep 16 08:58:37 1999
   @author    Tom Goodale
   @desc 
   Routins to traverse schedule groups.
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>

#include "cctk_Flesh.h"

#include "cctki_Schedule.h"
#include "StoreHandledData.h"
#include "Schedule.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(schedule_ScheduleTraverse_c)


/* Local routine prototypes */
static int ScheduleTraverseGroup(cHandledData *schedule_groups, 
                                 t_sched_group *group,
                                 void *attributes,
                                 int n_whiles,
                                 char **whiles,
                                 int (*item_entry)(void *, void *),
                                 int (*item_exit)(void *, void *),
                                 int  (*while_check)(int, char **, void *, void *),
                                 int (*function_process)(void *, void *, void *),
                                 void *data);

static int ScheduleTraverseFunction(void *function,
                                    void *attributes,
                                    int n_whiles,
                                    char **whiles,
                                    int (*item_entry)(void *, void *),
                                    int (*item_exit)(void *, void *),
                                    int  (*while_check)(int, char **, void *, void *),
                                    int (*function_process)(void *, void *, void *),
                                    void *data);

/********************************************************************
 ********************    External Routines   ************************
 ********************************************************************/

 /*@@
   @routine    CCTKi_DoScheduleTraverse
   @date       Thu Sep 16 09:05:23 1999
   @author     Tom Goodale
   @desc 
   Traverses the group with the given name.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTKi_DoScheduleTraverse(const char *group_name,
                           int (*item_entry)(void *, void *),
                           int (*item_exit)(void *, void *),
                           int  (*while_check)(int, char **, void *, void *),
                           int (*function_process)(void *, void *, void *),
                           void *data)
{
  cHandledData *schedule_groups;
  t_sched_group *group;
  int handle;
  int retcode;

  schedule_groups = CCTKi_DoScheduleGetGroups();

  handle = Util_GetHandle(schedule_groups, group_name, (void *)&group);

  if(handle >= 0)
  {
    retcode = ScheduleTraverseGroup(schedule_groups, 
                                    group, 
                                    NULL,
                                    0,
                                    NULL,
                                    item_entry, 
                                    item_exit, 
                                    while_check, 
                                    function_process,
                                    data);
  }
  else
  {
    retcode = handle;
  }

  return retcode;
}

/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/

 /*@@
   @routine    ScheduleTraverseGroup
   @date       Thu Sep 16 09:07:44 1999
   @author     Tom Goodale
   @desc 
   Traverses the given schedule group.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int ScheduleTraverseGroup(cHandledData *schedule_groups, 
                                 t_sched_group *group,
                                 void *attributes,
                                 int n_whiles,
                                 char **whiles,
                                 int (*item_entry)(void *, void *),
                                 int (*item_exit)(void *, void *),
                                 int  (*while_check)(int, char **, void *, void *),
                                 int (*function_process)(void *, void *, void *),
                                 void *data)
{
  int item;
  int doit;
  int called_item_entry;
  t_sched_group *newgroup;

  /* If there is a while-list associated with this item, check if the group should be
   * exectuted at all.
   */

  if(n_whiles > 0 && while_check)
  {
    doit = while_check(n_whiles, whiles, attributes, data);
  }
  else
  {
    doit = 1;
  }

  /* Call a item entry function if it is defined. */
  if(doit)
  {
    called_item_entry = 1;

    if(item_entry)
    {
      doit = item_entry(attributes, data);
    }
  }
  else
  {
    called_item_entry = 0;
  }

  /* Now traverse the group. */
  while(doit )
  {
      
    /* Traverse in the sorted order - assumes group has been sorted ! */
    for(item = 0 ; item < group->n_scheditems; item++)
    {
      switch(group->scheditems[group->order[item]].type)
      {
        case sched_function :
          ScheduleTraverseFunction(group->scheditems[group->order[item]].function, 
                                   group->scheditems[group->order[item]].attributes,
                                   group->scheditems[group->order[item]].n_whiles,
                                   group->scheditems[group->order[item]].whiles,
                                   item_entry,
                                   item_exit,                                  
                                   while_check,
                                   function_process,
                                   data);
          break;
        case sched_group :
          newgroup = (t_sched_group *)Util_GetHandledData(schedule_groups, 
                                                          group->scheditems[group->order[item]].group);
          ScheduleTraverseGroup(schedule_groups,
                                newgroup, 
                                group->scheditems[group->order[item]].attributes,
                                group->scheditems[group->order[item]].n_whiles,
                                group->scheditems[group->order[item]].whiles,
                                item_entry, 
                                item_exit, 
                                while_check, 
                                function_process,
                                data);
          break;
        default :
          fprintf(stderr, "Unknown schedule item type %d\n", group->scheditems[group->order[item]].type);
      }
    }

    /* Check the while_list again. */
    if(n_whiles > 0 && while_check)
    {
      doit = while_check(n_whiles, whiles, attributes, data) ;
    }
    else
    {
      doit = 0;
    }
  }

  /* Call the group_exit function if it's defined. */
  if(called_item_entry)
  {
    if(item_exit)
    {
      item_exit(attributes, data);
    }
  }

  return 0;
}

 /*@@
   @routine    ScheduleTraverseFunction
   @date       Thu Sep 16 11:51:58 1999
   @author     Tom Goodale
   @desc 
   Deals with a function in the schedule list
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int ScheduleTraverseFunction(void *function,
                                    void *attributes,
                                    int n_whiles,
                                    char **whiles,
                                    int (*item_entry)(void *, void *),
                                    int (*item_exit)(void *, void *),
                                    int  (*while_check)(int, char **, void *, void *),
                                    int (*function_process)(void *, void *, void *),
                                    void *data)
{
  int doit;
  int called_item_entry;

  /* If there is a while-list associated with this function, check if the function should be
   * executed at all.
   */

  if(n_whiles > 0 && while_check)
  {
    doit = while_check(n_whiles, whiles, attributes, data);
  }
  else
  {
    doit = 1;
  }

  /* Call a item entry function if it is defined. */
  if(doit)
  {
    called_item_entry = 1;

    if(item_entry)
    {
      doit = item_entry(attributes, data);
    }
  }
  else
  {
    called_item_entry = 0;
  }

  /* Now traverse the . */
  while(doit )
  {
    
    /* Now actually do something with the function. */
    function_process(function, attributes, data);

    /* Check the while_list again. */
    if(n_whiles > 0 && while_check)
    {
      doit = while_check(n_whiles, whiles, attributes, data) ;
    }
    else

    {
      doit = 0;
    }
  }

  /* Call the item_exit function if it's defined. */
  if(called_item_entry)
  {
    if(item_exit)
    {
      item_exit(attributes, data);
    }
  }

  return 0;
}

/********************************************************************
 ********************************************************************
 ********************************************************************/

#ifdef TEST_SCHEDULETRAVERSE

#define func_x(x) \
int func_ ## x (void) { return printf("I'm func " #x "\n"); }

func_x(a)
func_x(b)
func_x(c)

int fprocess(void *function, void *attributes, void *data)
{
  int (*func)(void);

  func = (int (*)(void)) function;

  func();

  return 1;
}

int main(int argc, char *argv[])
{
  t_sched_modifier *modifier;

  modifier = CCTKi_DoScheduleAddModifer(NULL, "before", "c");
  modifier = CCTKi_DoScheduleAddModifer(modifier, "after",  "a");

  CCTKi_DoScheduleFunction("group_a", "c", func_c, NULL, NULL);
  CCTKi_DoScheduleFunction("group_a", "b", func_b, modifier, NULL);
  CCTKi_DoScheduleFunction("group_a", "a", func_a, NULL, NULL);
  CCTKi_DoScheduleFunction("group_b", "a", func_a, NULL, NULL);
  CCTKi_DoScheduleFunction("group_b", "b", func_b, NULL, NULL);
  CCTKi_DoScheduleGroup("group_a", "group_b", modifier, NULL);

  CCTKi_DoScheduleSortAllGroups();

  CCTKi_DoScheduleTraverse("group_a", NULL, NULL, NULL, fprocess, NULL);

  return 0;
}
#endif
