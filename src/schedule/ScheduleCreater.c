 /*@@
   @file      ScheduleCreater.c
   @date      Tue Aug 31 12:46:08 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "StoreHandledData.h"
#include "Schedule.h"

/* Internal routine prototypes */

static int ScheduleCreateGroup(const char *name);

static t_sched_item *ScheduleCreateItem(const char *name, 
					t_sched_modifier *modifiers, 
					void *attributes);

static int ScheduleAddItem(int ghandle, t_sched_item *item);

static int ScheduleSortGroup(t_sched_group *group);

static t_sched_modifier_type ScheduleTranslateModifierType(const char *modifier);

static int ScheduleItemNumber(t_sched_group *group, 
			      const char *name);


static int ScheduleSetupWhiles(t_sched_item *item);

/* Local variables. */

static char *rcsid="$Header$";

static int n_schedule_groups = 0;
static cHandledData *schedule_groups = NULL;

/********************************************************************
 ********************    External Routines   ************************
 ********************************************************************/

 /*@@
   @routine    CCTKi_ScheduleAddModifer
   @date       Thu Sep  9 21:45:25 1999
   @author     Tom Goodale
   @desc 
   Adds a schedule modifier to a modifier list.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
t_sched_modifier *CCTKi_ScheduleAddModifier(t_sched_modifier *orig, 
                                            const char *modifier, 
                                            const char *argument)
{
  t_sched_modifier *this;

  this = (t_sched_modifier *)malloc(sizeof(t_sched_modifier));

  if(this)
  {
    this->argument = (char *)malloc((strlen(argument)+1)*sizeof(char));
    if(this->argument)
    {
      strcpy(this->argument, argument);

      this->type = ScheduleTranslateModifierType(modifier);

      this->next = orig;
    }
    else
    {
      free(this);
      this = NULL;
    }
  }

  return this;
}

 /*@@
   @routine    CCTKi_ScheduleFunction
   @date       Thu Sep  9 21:42:58 1999
   @author     Tom Goodale
   @desc 
   Adds a function to a schedule group.  Creates the group if necessary.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTKi_ScheduleFunction(const char *gname, 
			   const char *fname, 
			   void *func, 
			   t_sched_modifier *modifiers, 
			   void *attributes)
{
  int retcode;
  int handle;
  t_sched_group *this_group;
  t_sched_item *newitem;

  handle = Util_GetHandle(schedule_groups, gname, (void **)&this_group);

  if(handle < 0)
  {
    handle = ScheduleCreateGroup(gname);
  }

  if(handle < 0)
  {
    retcode = -1;
  }
  else
  {
    newitem = ScheduleCreateItem(fname, modifiers, attributes);

    if(newitem)
    {
      newitem->type = sched_function;
      newitem->function = func;
      retcode = ScheduleAddItem(handle, newitem);
    }
    else
    {
      retcode = -1;
    }
  }

  return retcode;
}
    
 /*@@
   @routine    CCTKi_ScheduleGroup
   @date       Thu Sep  9 21:43:44 1999
   @author     Tom Goodale
   @desc 
   Adds a group to a schedule group.  Creates the group if necessary.   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTKi_ScheduleGroup(const char *gname, 
			const char *thisname, 
			t_sched_modifier *modifiers, 
			void *attributes)
{
  int retcode;
  int handle;
  int thishandle;
  t_sched_group *this_group;
  t_sched_item *newitem;

  /* Find the group within which to schedule this group */
  handle = Util_GetHandle(schedule_groups, gname, (void **)&this_group);

  if(handle < 0)
  {
    handle = ScheduleCreateGroup(gname);
  }

  /* Find this group */
  thishandle = Util_GetHandle(schedule_groups, thisname, (void **)&this_group);

  if(thishandle < 0)
  {
    thishandle = ScheduleCreateGroup(thisname);
  }

  if(handle < 0 || thishandle < 0)
  {
    retcode = -1;
  }
  else
  {
    newitem = ScheduleCreateItem(thisname, modifiers, attributes);

    if(newitem)
    {
      newitem->type = sched_group;
      newitem->group = thishandle;
      retcode = ScheduleAddItem(handle, newitem);
    }
    else
    {
      retcode = -1;
    }
  }

  return retcode;
}

 /*@@
   @routine    CCTKi_ScheduleSortAllGroups
   @date       Wed Sep 15 22:37:49 1999
   @author     Tom Goodale
   @desc 
   Sorts all the schedule groups.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTKi_ScheduleSortAllGroups(void)
{
  int group;
  t_sched_group *gdata;
  int errcode;
  int n_errors;
  
  n_errors  = 0;

  for(group = 0; group < n_schedule_groups; group++)
  {
    if((gdata = (t_sched_group *)Util_GetHandledData(schedule_groups, group)))
    {
      errcode = ScheduleSortGroup(gdata);

      if(errcode)
      {
	fprintf(stderr, 
		"Error while dorting group '%s' - %d remaining unsorted routines.\n", 
		gdata->name,
		-errcode);

	n_errors += -errcode;
      }
    }
  }
   
  return -n_errors;
}

cHandledData *CCTKi_ScheduleGetGroups(void)
{
  return schedule_groups;
}

/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/

 /*@@
   @routine    ScheduleCreateGroup
   @date       Wed Sep  8 11:15:32 1999
   @author     Tom Goodale
   @desc 
   Creates a schedule group.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int ScheduleCreateGroup(const char *name)
{
  int retcode;
  int handle;

  t_sched_group *this_group;

  handle = Util_GetHandle(schedule_groups, name, (void **)&this_group);

  if(handle > -1)
  {
    /* Group already exists */
    retcode = -1;
  }
  else
  {
    this_group = (t_sched_group *)malloc(sizeof(t_sched_group));

    if(this_group)
    {
      this_group->name = (char *)malloc((strlen(name)+1)*sizeof(char));

      if(this_group->name)
      {
	strcpy(this_group->name, name);

	this_group->order = NULL;
	this_group->n_scheditems = 0;
	this_group->scheditems = NULL;
	retcode = Util_NewHandle(&schedule_groups, name, (void *)this_group);
	n_schedule_groups++;
      }
      else
      {
	free(this_group);

	retcode = -2;
      }
    }
    else
    {
      retcode = -2;
    }
  }

  return retcode;
}

 /*@@
   @routine    ScheduleCreateItem
   @date       Thu Sep  9 21:44:17 1999
   @author     Tom Goodale
   @desc 
   Creates a schedule item to be scheduled.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static t_sched_item *ScheduleCreateItem(const char *name, t_sched_modifier *modifiers, void *attributes)
{
  t_sched_item *this;

  this = (t_sched_item *)malloc(sizeof(t_sched_item));

  if(this)
  {
    this->name = (char *)malloc((strlen(name)+1)*sizeof(char));

    if(this->name)
    {
      strcpy(this->name, name);

      this->type     = sched_item_none;
      this->function = NULL;
      this->group    = -1;
      this->modifiers = modifiers;

      this->n_whiles = 0;
      this->whiles = NULL;

      ScheduleSetupWhiles(this);

      this->attributes = attributes;

#ifdef DEBUG_SCHEDULAR
      printf("Created Schedule item %s\n", this->name);
#endif

    }
    else
    {
      free(this);
      this = NULL;
    }
  }

  return this;
}

 /*@@
   @routine    ScheduleAddItem
   @date       Thu Sep  9 21:45:03 1999
   @author     Tom Goodale
   @desc 
   Adds a schedule item to a group.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int ScheduleAddItem(int ghandle, t_sched_item *item)
{
  int retcode;
  t_sched_group *this_group;
  t_sched_item *temp;

  this_group = (t_sched_group *)Util_GetHandledData(schedule_groups, ghandle);

  this_group->n_scheditems++;

  temp = (t_sched_item *)realloc(this_group->scheditems, this_group->n_scheditems*sizeof(t_sched_item));

  if(temp)
  {
    this_group->scheditems = temp;
    this_group->scheditems[this_group->n_scheditems-1] = *item;

#ifdef DEBUG_SCHEDULAR
    printf("Added item '%s' to group '%s'\n", item->name, this_group->name);
#endif

    free(item);

    retcode = 0;
  }
  else
  {
    this_group->n_scheditems--;
    retcode = -1;
  }

  return retcode;
}
  


 /*@@
   @routine    ScheduleTranslateModifierType
   @date       Thu Sep  9 21:45:56 1999
   @author     Tom Goodale
   @desc 
   Translates a modifier type from a string to an enum.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static t_sched_modifier_type ScheduleTranslateModifierType(const char *modifier)
{
  /* FIXME */
  
  t_sched_modifier_type retval;

  retval = sched_mod_none;

  if(!strcmp(modifier, "before"))
  {
    retval = sched_before;
  }
  else if(!strcmp(modifier, "after"))
  {
    retval = sched_after;
  }
  else if(!strcmp(modifier, "while"))
  {
    retval = sched_while;
  }

#ifdef DEBUG_SCHEDULAR
  printf("Translated modifier type %s to %d\n", modifier, retval);
#endif

  return retval;
}

 /*@@
   @routine    ScheduleSortGroup
   @date       Mon Sep 13 11:30:19 1999
   @author     Tom Goodale
   @desc 
   Sorts the routines in a group.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int ScheduleSortGroup(t_sched_group *group)
{
  int item;
  int *order;
  int *thisorders;
  t_sched_modifier *modifier;
  signed char **array;
  int number;
  int mod;
  int i, j;
  int errcode;

  /* Create the data staructures */
  array      = CCTKi_ScheduleCreateArray(group->n_scheditems);
  order      = CCTKi_ScheduleCreateIVec(group->n_scheditems);
  thisorders = CCTKi_ScheduleCreateIVec(group->n_scheditems);
  
  for(item=0; item < group->n_scheditems; item++)
  {
#ifdef DEBUG_SCHEDULAR
    printf("Scheduling item %d '%s'\n", item, group->scheditems[item].name);
#endif
    for(modifier = group->scheditems[item].modifiers; modifier; modifier = modifier->next)
    {
      if(modifier->type == sched_while)
      {
	continue;
      }
      number = ScheduleItemNumber(group, modifier->argument);

#ifdef DEBUG_SCHEDULAR
      printf("Scheduling against item %d '%s' - mod-type %d\n", number, group->scheditems[number].name, modifier->type);
#endif
      if(number >= 0 && number < group->n_scheditems)
      {
	switch(modifier->type)
	{
	  case sched_before : mod = -1;  break;
	  case sched_after  : mod = 1; break;
	  default :
	    mod = 0;
	}
#ifdef DEBUG_SCHEDULAR
	printf("Modifier is %d\n", mod);
#endif

	thisorders[number] = mod;
      }
    }

#ifdef DEBUG_SCHEDULAR
    printf("Orderlist for item %d is...\n", item);
    for(i=0; i < group->n_scheditems; i++)
    {
      printf("  %d", thisorders[i]);
    }
    printf("\n");
#endif

    CCTKi_ScheduleAddRow(group->n_scheditems, array, order, item, thisorders);

    /* Clear the array for the next item. */
    for(i=0; i < group->n_scheditems; i++)
    {
      thisorders[i] = 0;
    }
  }

#ifdef DEBUG_SCHEDULAR
  printf("Initial array is...\n");
  for(i=0; i < group->n_scheditems; i++)
  {
    for(j=0; j < group->n_scheditems; j++)
    {
      printf("  %d", (int)array[i][j]);
    }

    printf("\n");
  }

  printf("Initial order is...\n");
  for(i=0; i < group->n_scheditems; i++)
  {
    printf("  %d", order[i]);
  }
  printf("\n");
  
  printf("Sorting array...\n");
#endif

  errcode = CCTKi_ScheduleSort(group->n_scheditems, array, order);

  if(errcode)
  {
    fprintf(stderr, "Schedule sort failed with error code %d\n", errcode);
  }

#ifdef DEBUG_SCHEDULAR
  printf("Final array is...\n");
  for(i=0; i < group->n_scheditems; i++)
  {
    for(j=0; j < group->n_scheditems; j++)
    {
      printf("  %d", (int)array[i][j]);
    }

    printf("\n");
  }

  printf("Final order is...\n");
  for(i=0; i < group->n_scheditems; i++)
  {
    printf("  %d", order[i]);
  }
  printf("\n");
#endif

  /* Free memory */
  CCTKi_ScheduleDestroyIVec(group->n_scheditems,thisorders);
  CCTKi_ScheduleDestroyArray(group->n_scheditems, array);

  group->order = order;

  return errcode;
}

 /*@@
   @routine    ScheduleItemNumber
   @date       Mon Sep 13 11:30:49 1999
   @author     Tom Goodale
   @desc 
   Returns the number of a specific item in the array of schedule items.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int ScheduleItemNumber(t_sched_group *group, const char *name)
{
  int retval;
  int i;
  
  retval = -1;
  /* This is the quick-to-write way to do this.  Should really put into a
   * hash table or a tree, but this will do for the moment.
   */
  for(i = 0 ; i < group->n_scheditems; i++)
  {
    if(!strcmp(group->scheditems[i].name, name))
    {
      retval = i;
      break;
    }
  }

  return retval;
}


 /*@@
   @routine    ScheduleSetupWhiles
   @date       Wed Sep 15 20:10:28 1999
   @author     Tom Goodale
   @desc 
   Make an array of all the whiles in the modifier list for a schedule item.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int ScheduleSetupWhiles(t_sched_item *item)
{
  int retval;
  t_sched_modifier *modifier;
  char **temp;

  retval = 0;

  for(modifier = item->modifiers; modifier; modifier = modifier->next)
  {
    if(modifier->type == sched_while)
    {
      item->n_whiles++;
      temp = (char **)realloc(item->whiles, item->n_whiles*sizeof(char *));

      if(temp)
      {
	item->whiles = temp;

	temp[item->n_whiles-1] = (char *)malloc((strlen(modifier->argument)+1)*sizeof(char));
	if(temp[item->n_whiles-1])
	{
	  strcpy(temp[item->n_whiles-1], modifier->argument);
	}
	else
	{
	  item->n_whiles--;
	  retval--;
	}
      }
      else
      {
	retval--;
      }
    }
  }

  return retval;
}

/********************************************************************
 ********************************************************************
 ********************************************************************/

#ifdef TEST_SCHEDULECREATOR

#define func_x(x) \
int func_ ## x (void) { return printf("I'm func " #x "\n"); }

func_x(a)
func_x(b)
func_x(c)

#define func_x_proto(x) int func_ ## x (void);

int main(int argc, char *argv[])
{
  t_sched_modifier *modifier;

  modifier = CCTKi_ScheduleAddModifier(NULL, "before", "c");
  modifier = CCTKi_ScheduleAddModifier(modifier, "after",  "a");

  CCTKi_ScheduleFunction("group_a", "c", func_c, NULL, NULL);
  CCTKi_ScheduleFunction("group_a", "b", func_b, modifier, NULL);
  CCTKi_ScheduleFunction("group_a", "a", func_a, NULL, NULL);
  CCTKi_ScheduleFunction("group_b", "a", func_a, NULL, NULL);
  CCTKi_ScheduleFunction("group_b", "b", func_b, NULL, NULL);
  CCTKi_ScheduleGroup("group_a", "group_b", modifier, NULL);

  CCTKi_ScheduleSortAllGroups();

  return 0;
}
#endif
