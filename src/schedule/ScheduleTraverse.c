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

#include "StoreHandledData.h"
#include "Schedule.h"

static char *rcsid = "$Header$";

/* Local routine prototypes */
static int ScheduleTraverseGroup(cHandledData *schedule_groups, 
                                 t_sched_group *group,
                                 void *attributes,
                                 int n_whiles,
                                 char **whiles,
                                 void (*item_entry)(void *),
                                 void (*item_exit)(void *),
                                 int  (*while_check)(int, char **, void *),
                                 void (*function_process)(void *, void *));

static int ScheduleTraverseFunction(void *function,
                                    void *attributes,
                                    int n_whiles,
                                    char **whiles,
                                    void (*item_entry)(void *),
                                    void (*item_exit)(void *),
                                    int  (*while_check)(int, char **, void *),
                                    void (*function_process)(void *, void *));

/********************************************************************
 ********************    External Routines   ************************
 ********************************************************************/

 /*@@
   @routine    CCTKi_ScheduleTraverse
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
int CCTKi_ScheduleTraverse(const char *group_name,
                           void (*item_entry)(void *),
                           void (*item_exit)(void *),
                           int  (*while_check)(int, char **, void *),
                           void (*function_process)(void *, void *))
{
  cHandledData *schedule_groups;
  t_sched_group *group;
  int handle;
  int retcode;

  schedule_groups = CCTKi_ScheduleGetGroups();

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
                                    function_process);
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
                                 void (*item_entry)(void *),
                                 void (*item_exit)(void *),
                                 int  (*while_check)(int, char **, void *),
                                 void (*function_process)(void *, void *))
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
    doit = while_check(n_whiles, whiles, attributes);
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
      item_entry(attributes);
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
                                   function_process);
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
                                function_process);
          break;
        default :
          fprintf(stderr, "Unknown schedule item type %d\n", group->scheditems[group->order[item]].type);
      }
    }

    /* Check the while_list again. */
    if(n_whiles > 0 && while_check)
    {
      doit = while_check(n_whiles, whiles, attributes) ;
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
      item_exit(attributes);
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
                                    void (*item_entry)(void *),
                                    void (*item_exit)(void *),
                                    int  (*while_check)(int, char **, void *),
                                    void (*function_process)(void *, void *))
{
  int doit;
  int called_item_entry;

  /* If there is a while-list associated with this function, check if the function should be
   * executed at all.
   */

  if(n_whiles > 0 && while_check)
  {
    doit = while_check(n_whiles, whiles, attributes);
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
      item_entry(attributes);
    }
  }
  else
  {
    called_item_entry = 0;
  }

  /* Now traverse the . */
  while(doit )
  {
      
    function_process(function, attributes);

    /* Check the while_list again. */
    if(n_whiles > 0 && while_check)
    {
      doit = while_check(n_whiles, whiles, attributes) ;
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
      item_exit(attributes);
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

void fprocess(void *function, void *attributes)
{
  int (*func)(void);

  func = (int (*)(void)) function;

  func();

}

int main(int argc, char *argv[])
{
  t_sched_modifier *modifier;

  modifier = CCTKi_ScheduleAddModifer(NULL, "before", "c");
  modifier = CCTKi_ScheduleAddModifer(modifier, "after",  "a");

  CCTKi_ScheduleFunction("group_a", "c", func_c, NULL, NULL);
  CCTKi_ScheduleFunction("group_a", "b", func_b, modifier, NULL);
  CCTKi_ScheduleFunction("group_a", "a", func_a, NULL, NULL);
  CCTKi_ScheduleFunction("group_b", "a", func_a, NULL, NULL);
  CCTKi_ScheduleFunction("group_b", "b", func_b, NULL, NULL);
  CCTKi_ScheduleGroup("group_a", "group_b", modifier, NULL);

  CCTKi_ScheduleSortAllGroups();

  CCTKi_ScheduleTraverse("group_a", NULL, NULL, NULL, fprocess);

  return 0;
}
#endif
