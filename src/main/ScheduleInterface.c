/*@@
   @file      ScheduleInterface.c
   @date      Thu Sep 16 14:06:21 1999
   @author    Tom Goodale
   @desc 
   Routines to interface the main part of Cactus to the schedular.
   @enddesc 
   @version $Header$
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "cctk_Flesh.h"
#include "cctk_WarnLevel.h"
#include "cctk_Misc.h"

#include "cctk_Schedule.h"
#include "cctki_ScheduleBindings.h"
#include "cctki_Schedule.h"

#include "cctk_Comm.h"
#include "cctk_Sync.h"

#include "cctk_Groups.h"
#include "cctk_GroupsOnGH.h"

#include "cctki_FortranWrappers.h"

#include "CactusTimers.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_ScheduleImplementation_c)


/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/

typedef enum {sched_none, sched_group, sched_function} iSchedType;
typedef enum {schedpoint_misc, schedpoint_analysis} iSchedPoint;

typedef struct 
{
  /* Static data */
  char *description;

  char *thorn;
  char *implementation;

  iSchedType type;

  cFunctionData FunctionData;

  int n_mem_groups;
  int *mem_groups;

  int n_comm_groups;
  int *comm_groups;

  int n_trigger_groups;
  int *trigger_groups;

  /* Timer data */

  int timer_handle;

  /* Dynamic data */
  int *CommOnEntry;
  int *StorageOnEntry;

  int done_entry;

} t_attribute;

typedef struct
{
  cGH *GH;
  iSchedPoint schedpoint;

  cTimerData *info;
  int print_headers;

  /* Stuff passed in in user calls */

  int (*CallFunction)(void *, cFunctionData *, void *);

} t_sched_data;


/********************************************************************
 ********************* Local Routine Prototypes *********************
 ********************************************************************/

static int ScheduleTraverse(const char *where, 
                            void *GH,   
                            int (*CallFunction)(void *, cFunctionData *, void *));

static t_attribute *CreateAttribute(const char *description, 
                                    const char *language, 
                                    const char *name,
                                    const char *thorn,
                                    int n_mem_groups, 
                                    int n_comm_groups, 
                                    int n_trigger_groups, 
                                    int n_sync_groups,
                                    int n_options,
                                    va_list *ap);

static int ParseOptionList(int n_items, 
                           t_attribute *attribute, 
                           va_list *ap);

static int InitialiseOptionList(t_attribute *attribute);

static int ParseOption(t_attribute *attribute, 
                       const char *option);

static t_sched_modifier *CreateModifiers(int n_before, 
                                         int n_after, 
                                         int n_while, 
                                         va_list *ap);
static int CreateGroupIndexList(int n_items, int *array, va_list *ap);
static t_sched_modifier *CreateTypedModifier(t_sched_modifier *modifier,
                                             const char *type,
                                             int n_items,
                                             va_list *ap);
static cFunctionType TranslateFunctionType(const char *where);

static int SchedulePrint(const char *where);

static int CCTKi_SchedulePrintEntry(t_attribute *attribute, t_sched_data *data);
static int CCTKi_SchedulePrintExit(t_attribute *attribute, t_sched_data *data);
static int CCTKi_SchedulePrintWhile(int n_whiles, 
                                    char **whiles, 
                                    t_attribute *attribute, 
                                    t_sched_data *data, 
                                    int first);
static int CCTKi_SchedulePrintFunction(void *function, t_attribute *attribute, t_sched_data *data);

static int CCTKi_ScheduleCallEntry(t_attribute *attribute, t_sched_data *data);
static int CCTKi_ScheduleCallExit(t_attribute *attribute, t_sched_data *data);
static int CCTKi_ScheduleCallWhile(int n_whiles, 
                                   char **whiles, 
                                   t_attribute *attribute, 
                                   t_sched_data *data, 
                                   int first);
static int CCTKi_ScheduleCallFunction(void *function, t_attribute *attribute, t_sched_data *data);

static int SchedulePrintTimes(const char *where, t_sched_data *data);

static int CCTKi_SchedulePrintTimesEntry(t_attribute *attribute, t_sched_data *data);
static int CCTKi_SchedulePrintTimesExit(t_attribute *attribute, t_sched_data *data);
static int CCTKi_SchedulePrintTimesWhile(int n_whiles, 
                                         char **whiles, 
                                         t_attribute *attribute, 
                                         t_sched_data *data, 
                                         int first);
static int CCTKi_SchedulePrintTimesFunction(void *function, t_attribute *attribute, t_sched_data *data);
static void CCTKi_SchedulePrintTimerInfo(cTimerData *info);
static void CCTKi_SchedulePrintTimerHeaders(cTimerData *info);


/********************************************************************
 ********************* Other Routine Prototypes *********************
 ********************************************************************/

/* FIXME: these should be put in a header somewhere */

int CCTKi_TriggerSaysGo(cGH *GH, int variable);
int CCTKi_TriggerAction(void *GH, int variable);


/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/


static int indent_level = 0;

static int n_scheduled_comm_groups = 0;
static int *scheduled_comm_groups = NULL;

static int n_scheduled_storage_groups = 0;
static int *scheduled_storage_groups = NULL;

static cTimerData *timerinfo = NULL;

/********************************************************************
 *********************     External Routines   **********************
 ********************************************************************/

 /*@@
   @routine    CCTK_CallFunction
   @date       Thu Jan 27 11:29:47 2000
   @author     Tom Goodale
   @desc 
   Calls a function depending upon the data passed in the the
   fdata structure.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     function
   @vdesc   pointer to function
   @vtype   void *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     fdata
   @vdesc   data about the function
   @vtype   cFunctionData *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     data
   @vdesc   Data to be passed to the function
   @vtype   void *
   @vio     inout
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - didn't synchronise
   @endreturndesc
@@*/
int CCTK_CallFunction(void *function, 
                      cFunctionData *fdata, 
                      void *data)
{
  void (*standardfunc)(void *);

  int (*noargsfunc)(void);

  int (*oneargfunc)(void *);

  switch(fdata->type)
  {
    case FunctionNoArgs:
      noargsfunc = (int (*)(void))function;
      noargsfunc();
      break;
    case FunctionOneArg:
      oneargfunc = (int (*)(void *))function;
      oneargfunc(data);
      break;
    case FunctionStandard:
      switch(fdata->language)
      {
        case LangC:
          standardfunc = (void (*)(void *))function;
          standardfunc(data);
          break;
        case LangFortran:
          fdata->FortranCaller(data, function);
          break;
        default :
          CCTK_Warn(1,__LINE__,__FILE__,"Cactus", 
		    "CCTK_CallFunction: Unknown language.");
      }
      break;
    default :
      CCTK_Warn(1,__LINE__,__FILE__,"Cactus",
		"CCTK_CallFunction: Unknown function type.");
  }

  /* Return 0, meaning didn't synchronise */
  return 0;
}

/*@@
   @routine    CCTKi_ScheduleFunction
   @date       Thu Sep 16 18:19:01 1999
   @author     Tom Goodale
   @desc 
   Schedules a function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     function
   @vdesc   function to be scheduled
   @vtype   void *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     name
   @vdesc   name of function to be scheduled
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     thorn
   @vdesc   name of thorn providing function to be scheduled
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     implementation
   @vdesc   name of implementation thorn belongs to
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     description
   @vdesc   desciption of function to be scheduled
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     where
   @vdesc   where to schedule the function
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     language
   @vdesc   language of function to be scheduled
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_mem_groups
   @vdesc   Number of groups needing memory switched on during this function
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_comm_groups
   @vdesc   Number of groups needing communication switched on during this function
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_trigger_groups
   @vdesc   Number of groups to trigger this function on
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_sync_groups
   @vdesc   Number of groups needing synchronisation after this function
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_options
   @vdesc   Number of options for this schedule block
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_before
   @vdesc   Number of functions/groups to schedule before
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_after
   @vdesc   Number of functions/groups to schedule after
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_while
   @vdesc   Number of vars to schedule while
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar
   @var     ...
   @vdesc   remaining options
   @vtype   multiple const char *
   @vio     in
   @vcomment 
   This should have as many items as the sum of the above n_* options
   @endvar 

   @returntype int
   @returndesc 
   Return val of DoScheduleFunction or
   -1 - memory failure
   @endreturndesc
@@*/
int CCTKi_ScheduleFunction(void *function,
                           const char *name,
                           const char *thorn,
                           const char *implementation,
                           const char *description,
                           const char *where,
                           const char *language,
                           int n_mem_groups,
                           int n_comm_groups,
                           int n_trigger_groups,
                           int n_sync_groups,
                           int n_options,
                           int n_before,
                           int n_after,
                           int n_while,
                           ...
                           )
{
  int retcode;
  t_attribute *attribute;
  t_sched_modifier *modifier;
  va_list ap;

  va_start(ap, n_while);
  
  attribute = CreateAttribute(description, language, thorn, implementation, 
                              n_mem_groups, n_comm_groups, n_trigger_groups, 
                              n_sync_groups, n_options, &ap);
  modifier  = CreateModifiers(n_before, n_after, n_while, &ap);

  va_end(ap);

  if(attribute && (modifier || (n_before == 0 && n_after == 0 && n_while == 0)))
  {
    attribute->FunctionData.type = TranslateFunctionType(where);

    retcode = CCTKi_DoScheduleFunction(where, name, function, modifier, (void *)attribute);

#ifdef DEBUG
    fprintf(stderr, "Scheduled %s at %s\n", name, where);
#endif
  }
  else
  {
    fprintf(stderr, "Internal error: Failed to schedule %s at %s!!!\n", name, where);
    exit(2);
    retcode = -1;
  }

  return retcode;
}

/*@@
   @routine    CCTKi_ScheduleGroup
   @date       Thu Sep 16 18:19:18 1999
   @author     Tom Goodale
   @desc 
   Schedules a group.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     name
   @vdesc   name of group to be scheduled
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     thorn
   @vdesc   name of thorn providing group to be scheduled
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     implementation
   @vdesc   name of implementation group belongs to
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     description
   @vdesc   desciption of group to be scheduled
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     where
   @vdesc   where to schedule the group
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_mem_groups
   @vdesc   Number of groups needing memory switched on during this function
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_comm_groups
   @vdesc   Number of groups needing communication switched on during this function
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_trigger_groups
   @vdesc   Number of groups to trigger this function on
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_sync_groups
   @vdesc   Number of groups needing synchronisation after this function
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_options
   @vdesc   Number of options for this schedule block
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_before
   @vdesc   Number of functions/groups to schedule before
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_after
   @vdesc   Number of functions/groups to schedule after
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_while
   @vdesc   Number of vars to schedule while
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar
   @var     ...
   @vdesc   remaining options
   @vtype   multiple const char *
   @vio     in
   @vcomment 
   This should have as many items as the sum of the above n_* options
   @endvar 

   @returntype int
   @returndesc 
   Return val of DoScheduleGroup or
   -1 - memory failure
   @endreturndesc
@@*/
int CCTKi_ScheduleGroup(const char *name,
                        const char *thorn,
                        const char *implementation,
                        const char *description,
                        const char *where,
                        int n_mem_groups,
                        int n_comm_groups,
                        int n_trigger_groups,
                        int n_sync_groups,
                        int n_options,
                        int n_before,
                        int n_after,
                        int n_while,
                        ...
                        )
{
  int retcode;
  t_attribute *attribute;
  t_sched_modifier *modifier;
  va_list ap;

  va_start(ap, n_while);
  
  attribute = CreateAttribute(description, NULL, thorn, implementation,
                              n_mem_groups, n_comm_groups, n_trigger_groups, 
                              n_sync_groups, n_options, &ap);
  modifier  = CreateModifiers(n_before, n_after, n_while, &ap);

  va_end(ap);

  if(attribute && (modifier || (n_before == 0 && n_after == 0 && n_while == 0)))
  {
    retcode = CCTKi_DoScheduleGroup(where, name, modifier, (void *)attribute);
#ifdef DEBUG
    fprintf(stderr, "Scheduled %s at %s\n", name, where);
#endif
  }
  else
  {
#ifdef DEBUG
    fprintf(stderr, "Failed to schedule %s at %s!!!\n", name, where);
#endif
    retcode = -1;
  }

  return retcode;

}


/*@@
   @routine    CCTKi_ScheduleGroupStorage
   @date       Fri Sep 17 18:55:59 1999
   @author     Tom Goodale
   @desc 
   Schedules a group for storage when a GH is created.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     group
   @vdesc   group name
   @vtype   const char * 
   @vio     in
   @vcomment 
 
   @endvar 
   @returntype int
   @returndesc 
   Group index or
   -1 - memory failure
   @endreturndesc
@@*/
int CCTKi_ScheduleGroupStorage(const char *group)
{
  int retcode;
  int *temp;

  n_scheduled_storage_groups++;
  temp = (int*)realloc(scheduled_storage_groups, n_scheduled_storage_groups*sizeof(int));

  if(temp)
  {
    scheduled_storage_groups = temp;

    scheduled_storage_groups[n_scheduled_storage_groups-1] = CCTK_GroupIndex(group);

    retcode = scheduled_storage_groups[n_scheduled_storage_groups-1];
  }
  else
  {
    retcode = -1;
    n_scheduled_storage_groups--;
  }

  return retcode;

}


/*@@
   @routine    CCTKi_ScheduleGroupComm
   @date       Fri Sep 17 18:55:59 1999
   @author     Tom Goodale
   @desc 
   Schedules a group for communication when a GH is created.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     group
   @vdesc   group name
   @vtype   const char * 
   @vio     in
   @vcomment 
 
   @endvar 
   @returntype int
   @returndesc 
   Group index or
   -1 - memory failure
   @endreturndesc
@@*/
int CCTKi_ScheduleGroupComm(const char *group)
{
  int retcode;
  int *temp;

  n_scheduled_comm_groups++;
  temp = (int*)realloc(scheduled_comm_groups, n_scheduled_comm_groups*sizeof(int));

  if(temp)
  {
    scheduled_comm_groups = temp;

    scheduled_comm_groups[n_scheduled_comm_groups-1] = CCTK_GroupIndex(group);

    retcode = scheduled_comm_groups[n_scheduled_comm_groups-1];
  }
  else
  {
    retcode = -1;
    n_scheduled_comm_groups--;
  }

  return retcode;

}


 /*@@
   @routine    CCTK_ScheduleTraverse
   @date       Tue Apr  4 08:05:27 2000
   @author     Tom Goodale
   @desc 
   Traverses a schedule point, and its entry and exit points if necessary.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     where
   @vdesc   Schedule point
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     GH
   @vdesc   GH data
   @vtype   void *
   @vio     inout
   @vcomment 
 
   @endvar 
   @var     CallFunction
   @vdesc   Function called to call a function
   @vtype   int (*)(void *, cFubctionData, void *)
   @vio     in
   @vcomment 
   Set to NULL to use the default
   @endvar 

   @returntype int
   @returndesc 
   0 - success
   1 - memory failure
   @endreturndesc
@@*/
int CCTK_ScheduleTraverse(const char *where, 
                          void *GH,   
                          int (*CallFunction)(void *, cFunctionData *, void *))
{
  int retcode;

  int special;
  const char *current;

  static char *current_point = NULL;
  static int current_length = 0;
  char *temp;

  special=0;

  /* Special entry points have $ in them */
  for(current=where; *current; current++)
  {
    if(*current == '$')
    {
      special = 1;
      break;
    }
  }

  retcode = 0;

  if(special)
  {
    ScheduleTraverse(where, GH, CallFunction);
  }
  else
  {
    if(current_length < strlen(where) + 7)
    {
      current_length = strlen(where)+7;

      temp = realloc(current_point, current_length);
  
      if(temp)
      {
        current_point = temp;
      }
      else
      {
        retcode = 1;
      }
    }
    if(retcode == 0)
    {
      sprintf(current_point, "%s$%s", where, "ENTRY");
      ScheduleTraverse(current_point, GH, CallFunction);

      ScheduleTraverse(where, GH, CallFunction);

      sprintf(current_point, "%s$%s", where, "EXIT");
      ScheduleTraverse(current_point, GH, CallFunction);
    }
  }

  return retcode;
}


/*@@
   @routine    CCTKi_ScheduleGHInit
   @date       Fri Sep 17 21:25:13 1999
   @author     Tom Goodale
   @desc 
   Does any scheduling stuff setup which requires a GH.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     GH
   @vdesc   GH data
   @vtype   void *
   @vio     inout
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - success
   @endreturndesc
@@*/
int CCTKi_ScheduleGHInit(void *GH)
{
  int i;

  for(i = 0; i < n_scheduled_storage_groups; i++)
  {
    CCTK_EnableGroupStorageI(GH,scheduled_storage_groups[i]);
  }

  for(i = 0; i < n_scheduled_comm_groups; i++)
  {
    CCTK_EnableGroupCommI(GH,scheduled_comm_groups[i]);
  }

  return 0;
}

/*@@
   @routine    CCTK_SchedulePrint
   @date       Fri Sep 17 21:52:44 1999
   @author     Tom Goodale
   @desc 
   Prints out the schedule info.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     where
   @vdesc   Schedule point
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - success
   @endreturndesc
@@*/
int CCTK_SchedulePrint(const char *where)
{
  if(!where)
  {
    printf ("  Startup routines\n");
    SchedulePrint("CCTK_STARTUP");
    printf("\n");
    printf ("  Parameter checking routines\n");
    SchedulePrint("CCTK_PARAMCHECK");
    printf("\n");
    printf("  Initialisation\n");
    SchedulePrint("CCTK_BASEGRID$ENTRY");
    SchedulePrint("CCTK_BASEGRID");
    SchedulePrint("CCTK_BASEGRID$EXIT");
    SchedulePrint("CCTK_INITIAL$ENTRY");
    SchedulePrint("CCTK_INITIAL");
    SchedulePrint("CCTK_INITIAL$EXIT");
    SchedulePrint("CCTK_POSTINITIAL$ENTRY");
    SchedulePrint("CCTK_POSTINITIAL");
    SchedulePrint("CCTK_POSTINITIAL$EXIT");
    SchedulePrint("CCTK_POSTSTEP$ENTRY");
    SchedulePrint("CCTK_POSTSTEP");
    SchedulePrint("CCTK_POSTSTEP$EXIT");
    printf("\n");
    printf ("  do loop over timesteps\n");
    SchedulePrint("CCTK_PRESTEP$ENTRY");
    SchedulePrint("CCTK_PRESTEP");
    SchedulePrint("CCTK_PRESTEP$EXIT");
    SchedulePrint("CCTK_EVOL$ENTRY");
    SchedulePrint("CCTK_EVOL");
    SchedulePrint("CCTK_EVOL$EXIT");
    printf ("    t = t+dt\n");
    SchedulePrint("CCTK_POSTSTEP$ENTRY");
    SchedulePrint("CCTK_POSTSTEP");
    SchedulePrint("CCTK_POSTSTEP$EXIT");
    printf ("    if (analysis)\n");
    indent_level +=2;
    SchedulePrint("CCTK_ANALYSIS$ENTRY");
    SchedulePrint("CCTK_ANALYSIS");
    SchedulePrint("CCTK_ANALYSIS$EXIT");
    indent_level -=2;
    printf ("    endif\n");
    printf ("  enddo\n");
    printf ("  Termination routines\n");
    SchedulePrint("CCTK_TERMINATE");
    printf ("  Shutdown routines\n");
    SchedulePrint("CCTK_SHUTDOWN");
  }
  else
  {
    SchedulePrint(where);
  }

  return 0;
}

/*@@
   @routine    CCTK_SchedulePrintTimes
   @date       Fri Sep 17 21:52:44 1999
   @author     Tom Goodale
   @desc 
   Prints out the schedule timings.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     where
   @vdesc   Schedule point
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - success
   @endreturndesc
@@*/
int CCTK_SchedulePrintTimes(const char *where)
{
  t_sched_data data;

  data.GH = NULL;
  data.schedpoint = schedpoint_misc;
  data.print_headers = 1;

  if(!timerinfo)
  {
    timerinfo = CCTK_TimerCreateData();
  }
  
  data.info = timerinfo;

  if(!where)
  {
    SchedulePrintTimes("CCTK_STARTUP", &data);
    printf("\n");
    SchedulePrintTimes("CCTK_PARAMCHECK", &data);
    printf("\n");
    SchedulePrintTimes("CCTK_BASEGRID$ENTRY", &data);
    SchedulePrintTimes("CCTK_BASEGRID", &data);
    SchedulePrintTimes("CCTK_BASEGRID$EXIT", &data);
    SchedulePrintTimes("CCTK_INITIAL$ENTRY", &data);
    SchedulePrintTimes("CCTK_INITIAL", &data);
    SchedulePrintTimes("CCTK_INITIAL$EXIT", &data);
    SchedulePrintTimes("CCTK_POSTINITIAL$ENTRY", &data);
    SchedulePrintTimes("CCTK_POSTINITIAL", &data);
    SchedulePrintTimes("CCTK_POSTINITIAL$EXIT", &data);
    SchedulePrintTimes("CCTK_POSTSTEP$ENTRY", &data);
    SchedulePrintTimes("CCTK_POSTSTEP", &data);
    SchedulePrintTimes("CCTK_POSTSTEP$EXIT", &data);
    printf("\n");
    SchedulePrintTimes("CCTK_PRESTEP$ENTRY", &data);
    SchedulePrintTimes("CCTK_PRESTEP", &data);
    SchedulePrintTimes("CCTK_PRESTEP$EXIT", &data);
    SchedulePrintTimes("CCTK_EVOL$ENTRY", &data);
    SchedulePrintTimes("CCTK_EVOL", &data);
    SchedulePrintTimes("CCTK_EVOL$EXIT", &data);
    printf("\n");
    SchedulePrintTimes("CCTK_ANALYSIS$ENTRY", &data);    
    SchedulePrintTimes("CCTK_ANALYSIS", &data);    
    SchedulePrintTimes("CCTK_ANALYSIS$EXIT", &data);    
    printf("\n");
    SchedulePrintTimes("CCTK_TERMINATE", &data);
    SchedulePrintTimes("CCTK_SHUTDOWN", &data);
  }
  else
  {
    SchedulePrintTimes(where, &data);
  }

  return 0;
}

/*@@
   @routine    CCTK_TranslateLanguage
   @date       Thu Sep 16 18:18:31 1999
   @author     Tom Goodale
   @desc 
   Translates a language string into an internal enum.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     sval
   @vdesc   Language
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype cLanguage
   @returndesc 
   The language
   @endreturndesc
@@*/
cLanguage CCTK_TranslateLanguage(const char *sval)
{
  cLanguage retcode;

  if(CCTK_Equals(sval, "C"))
  {
    retcode = LangC;
  }
  else if(CCTK_Equals(sval, "Fortran"))
  {
    retcode = LangFortran;
  }
  else
  {
    fprintf(stderr, "Unknown language %s\n", sval);
    retcode = LangNone;
  }

  return retcode;
}

/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/

/*@@
   @routine    ScheduleTraverse
   @date       Fri Sep 17 21:52:44 1999
   @author     Tom Goodale
   @desc 
   Traverses the given schedule point.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     where
   @vdesc   Schedule point
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     GH
   @vdesc   GH data
   @vtype   void *
   @vio     inout
   @vcomment 
 
   @endvar 
   @var     CallFunction
   @vdesc   Function called to call a function
   @vtype   int (*)(void *, cFubctionData, void *)
   @vio     in
   @vcomment 
   Set to NULL to use the default
   @endvar 

   @returntype int
   @returndesc 
   0 - success
   @endreturndesc
@@*/

static int ScheduleTraverse(const char *where, 
                            void *GH,   
                            int (*CallFunction)(void *, cFunctionData *, void *))
{
  t_sched_data data;

  int (*calling_function)(void *, t_attribute *, t_sched_data *);

  data.GH = (cGH *)GH;
  
  if(CallFunction)
  {
    data.CallFunction = CallFunction;
  }
  else
  {
    data.CallFunction = CCTK_CallFunction;
  }

  if(CCTK_Equals(where, "CCTK_ANALYSIS"))
  {
    data.schedpoint = schedpoint_analysis;
  }
  else
  {
    data.schedpoint = schedpoint_misc;
  }

  calling_function = CCTKi_ScheduleCallFunction;
  
  CCTKi_DoScheduleTraverse(where,
     (int (*)(void *, void *))                    CCTKi_ScheduleCallEntry, 
     (int (*)(void *, void *))                    CCTKi_ScheduleCallExit, 
     (int  (*)(int, char **, void *, void *, int))CCTKi_ScheduleCallWhile, 
     (int (*)(void *, void *, void *))            calling_function, 
     (void *)&data);

  return 0;
}

 /*@@
   @routine    CreateAttribute
   @date       Thu Sep 16 18:22:48 1999
   @author     Tom Goodale
   @desc 
   Creates an attribute structure for a schedule item.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     description
   @vdesc   desciption of function to be scheduled
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     language
   @vdesc   language of function to be scheduled
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     thorn
   @vdesc   name of thorn providing function to be scheduled
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     implementation
   @vdesc   name of implementation thorn belongs to
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_mem_groups
   @vdesc   Number of groups needing memory switched on during this function
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_comm_groups
   @vdesc   Number of groups needing communication switched on during this function
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_trigger_groups
   @vdesc   Number of groups to trigger this function on
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_sync_groups
   @vdesc   Number of groups needing synchronisation after this function
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_options
   @vdesc   Number of options for this schedule block
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     ap
   @vdesc   options
   @vtype   va_list of multiple const char *
   @vio     inout
   @vcomment 
   This should have as many items as the sum of the above n_* options
   @endvar 

   @returntype t_attribute
   @returndesc 
   The attribute
   @endreturndesc
@@*/
static t_attribute *CreateAttribute(const char *description, 
                                    const char *language, 
                                    const char *thorn,
                                    const char *implementation,
                                    int n_mem_groups, 
                                    int n_comm_groups, 
                                    int n_trigger_groups, 
                                    int n_sync_groups,
                                    int n_options,
                                    va_list *ap)
{
  t_attribute *this;

  this = (t_attribute *)malloc(sizeof(t_attribute));

  if(this)
  {
    this->description    = (char *)malloc((strlen(description)+1)*sizeof(char));
    this->thorn          = (char *)malloc((strlen(thorn)+1)*sizeof(char));
    this->implementation = (char *)malloc((strlen(implementation)+1)*sizeof(char));
    this->mem_groups     = (int *)malloc(n_mem_groups*sizeof(int));
    this->comm_groups    = (int *)malloc(n_comm_groups*sizeof(int));
    this->trigger_groups = (int *)malloc(n_trigger_groups*sizeof(int));
    this->FunctionData.SyncGroups = (int *)malloc(n_sync_groups*sizeof(int));
    this->StorageOnEntry = (int *)malloc(n_mem_groups*sizeof(int));
    this->CommOnEntry    = (int *)malloc(n_comm_groups*sizeof(int));

    if(this->description     && 
       this->thorn           &&
       this->implementation  &&
       (this->mem_groups || n_mem_groups==0)         &&
       (this->comm_groups || n_comm_groups==0)       &&
       (this->trigger_groups || n_trigger_groups==0) &&
       (this->FunctionData.SyncGroups || n_sync_groups==0))
    {
      strcpy(this->description,    description);
      strcpy(this->thorn,          thorn);
      strcpy(this->implementation, implementation);

      if(language)
      {
        this->type = sched_function;
        this->FunctionData.language = CCTK_TranslateLanguage(language);
        this->FunctionData.FortranCaller = (int (*)(cGH *,void *))CCTKi_FortranWrapper(thorn);
      }
      else
      {
        this->type = sched_group;
      }
      
      /* Create the lists of indices of groups we're interested in. */
      CreateGroupIndexList(n_mem_groups,     this->mem_groups, ap);
      CreateGroupIndexList(n_comm_groups,    this->comm_groups, ap);
      CreateGroupIndexList(n_trigger_groups, this->trigger_groups, ap);
      CreateGroupIndexList(n_sync_groups,    this->FunctionData.SyncGroups, ap);

      /* Check the miscellaneous options */

      InitialiseOptionList(this);
      ParseOptionList(n_options, this, ap);

      this->n_mem_groups     = n_mem_groups;
      this->n_comm_groups    = n_comm_groups;
      this->n_trigger_groups = n_trigger_groups;
      this->FunctionData.n_SyncGroups = n_sync_groups;

      /* Add a timer to the item */
      
      this->timer_handle = CCTK_TimerCreateI();
    }
    else
    {
      free(this->description);
      free(this->comm_groups);
      free(this->trigger_groups);
      free(this->FunctionData.SyncGroups);
      free(this);
      this = NULL;
    }
  }

  return this;
}

/*@@
   @routine    CreateModifier
   @date       Thu Sep 16 18:23:13 1999
   @author     Tom Goodale
   @desc 
   Creates a schedule modifier list.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     n_before
   @vdesc   Number of functions/groups to schedule before
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_after
   @vdesc   Number of functions/groups to schedule after
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     n_while
   @vdesc   Number of vars to schedule while
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar
   @var     ap
   @vdesc   options
   @vtype   va_list of multiple const char *
   @vio     inout
   @vcomment 
   This should have as many items as the sum of the above n_* options
   @endvar 

   @returntype t_sched_modifier *
   @returndesc 
   the schedule modifier
   @endreturndesc
@@*/
static t_sched_modifier *CreateModifiers(int n_before, 
                                         int n_after, 
                                         int n_while, 
                                         va_list *ap)
{
  t_sched_modifier *modifier;

  modifier = CreateTypedModifier(NULL, "before", n_before, ap);
  modifier = CreateTypedModifier(modifier, "after", n_after, ap);
  modifier = CreateTypedModifier(modifier, "while", n_while, ap);

  return modifier;
}

/*@@
   @routine    CreateGroupIndexList
   @date       Fri Sep 17 21:51:51 1999
   @author     Tom Goodale
   @desc 
   Gets the next n_items group names from the variable argument list
   and converts them to indices.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     n_items
   @vdesc   number of items on the list
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     array
   @vdesc   array of indices
   @vtype   int *
   @vio     out
   @vcomment 
 
   @endvar 
   @var     ap
   @vdesc   argument list
   @vtype   va_list of const char *
   @vio     inout
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - success
   @endreturndesc
@@*/
static int CreateGroupIndexList(int n_items, int *array, va_list *ap)
{
  int i;
  const char *item;

  for(i=0; i < n_items; i++)
  {
    item = va_arg(*ap, const char *);

    array[i] = CCTK_GroupIndex(item);
  }

  return 0;
}


 /*@@
   @routine    ParseOptionList
   @date       Thu Jan 27 20:26:42 2000
   @author     Tom Goodale
   @desc 
   Extracts the list of miscellaneous options in a schedule
   group definition.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     n_items
   @vdesc   number of items on the list
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     attribute
   @vdesc   attribute list
   @vtype   t_attribute *
   @vio     inout
   @vcomment 
 
   @endvar 
   @var     ap
   @vdesc   argument list
   @vtype   va_list of const char *
   @vio     inout
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - success
   @endreturndesc
@@*/
static int ParseOptionList(int n_items, 
                           t_attribute *attribute, 
                           va_list *ap)
{
  int i;
  const char *item;

  for(i=0; i < n_items; i++)
  {
    item = va_arg(*ap, const char *);

    ParseOption(attribute, item);
  }

  return 0;
}

 /*@@
   @routine    InitialiseOptionList
   @date       Thu Jan 27 20:36:54 2000
   @author     Tom Goodale
   @desc 
   Initialises the miscellaneous option list for a schedule group. 
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     attribute
   @vdesc   option attribute
   @vtype   t_attribute *
   @vio     out
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - success
   @endreturndesc
@@*/
int InitialiseOptionList(t_attribute *attribute)
{
  attribute->FunctionData.global = 0;

  return 0;
}

 /*@@
   @routine    ParseOption
   @date       Thu Jan 27 20:29:36 2000
   @author     Tom Goodale
   @desc 
   Parses an individual option to a schedule group.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     attribute
   @vdesc   option attribute
   @vtype   t_attribute *
   @vio     out
   @vcomment 
 
   @endvar 
   @var     option
   @vdesc   Option
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - success
   @endreturndesc
@@*/
static int ParseOption(t_attribute *attribute, 
                       const char *option)
{
  if(CCTK_Equals(option, "GLOBAL"))
  {
    attribute->FunctionData.global = 1;
  }
  else
  {
    CCTK_Warn(1,__LINE__,__FILE__,"Cactus", 
	      "ParseOption: Unknown option for schedule group.\n");
  }

  return 0;
}

/*@@
   @routine    CreateTypedModifier
   @date       Fri Sep 17 21:50:59 1999
   @author     Tom Goodale
   @desc 
   Adds the next n_items items from the variable argument list
   onto the modifer.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     modifier
   @vdesc   base schedule modifier
   @vtype   t_sched_modifier
   @vio     inout
   @vcomment 
   This is a list which gets expanded by this function
   @endvar 
   @var     type
   @vdesc   modifier type
   @vtype   const char *
   @vio     in
   @vcomment 
   before, after, while 
   @endvar 
   @var     n_items
   @vdesc   Number of items on list
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     ap
   @vdesc   argument list
   @vtype   va_list of const char *
   @vio     inout
   @vcomment 
 
   @endvar 

   @returntype t_sched_modifier *
   @returndesc 
   modifier list
   @endreturndesc

@@*/
static t_sched_modifier *CreateTypedModifier(t_sched_modifier *modifier,
                                             const char *type,
                                             int n_items,
                                             va_list *ap)
{
  int i;
  const char *item;

  for(i=0; i < n_items; i++)
  {
    item = va_arg(*ap, const char *);

    modifier = CCTKi_ScheduleAddModifier(modifier, type, item);
  }

  return modifier;  
}

/*@@
   @routine    TranslateFunctionType
   @date       Mon Jan 24 16:52:06 2000
   @author     Tom Goodale
   @desc 
   Translates a string saying what schedule point 
   a function is registered at into the appropriate
   function type.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     where
   @vdesc   schedule point
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype cFunctionType
   @returndesc 
   The function type
   @endreturndesc
@@*/
static cFunctionType TranslateFunctionType(const char *where)
{
  cFunctionType retcode;

  int special;
  const char *current;

  special = 0;

  /* Special entry points have $ in them */
  for(current=where; *current; current++)
  {
    if(*current == '$')
    {
      special = 1;
      break;
    }
  }

  if(special)
  {
    retcode = FunctionOneArg;
  }    
  else if(CCTK_Equals(where, "CCTK_STARTUP"))
  {
    retcode = FunctionNoArgs;
  }
  else if(CCTK_Equals(where, "CCTK_SHUTDOWN"))
  {
    retcode = FunctionNoArgs;
  }
  else
  {
    retcode = FunctionStandard;
  }

  return retcode;
}

/*@@
   @routine    SchedulePrint
   @date       Sun Sep 19 13:31:23 1999
   @author     Tom Goodale
   @desc 
   Traverses the schedule data for a particular entry point and
   prints out the data.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     where
   @vdesc   Schedule point
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   return of DoScheduleTravers or
   0 - where is NULL
   @endreturndesc
@@*/
static int SchedulePrint(const char *where)
{
  int retcode;
  t_sched_data data;

  data.GH = NULL;
  data.schedpoint = schedpoint_misc;

  if(where)
  {
    retcode = CCTKi_DoScheduleTraverse(where,
       (int (*)(void *, void *))                    CCTKi_SchedulePrintEntry, 
       (int (*)(void *, void *))                    CCTKi_SchedulePrintExit, 
       (int  (*)(int, char **, void *, void *, int))CCTKi_SchedulePrintWhile, 
       (int (*)(void *, void *, void *))            CCTKi_SchedulePrintFunction, 
       (void *)&data);
  }
  else
  {
    retcode = 0;
  }

  return retcode;
}

/*@@
   @routine    SchedulePrintTimes
   @date       Fri Oct 22 12:35:06 1999
   @author     Tom Goodale
   @desc 
   Prints the times for a particular schedule entry point.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     where
   @vdesc   Schedule point
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     data
   @vdesc   schedule data
   @vtype   t_sched_data
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   return of DoScheduleTravers or
   0 - where is NULL
   @endreturndesc
@@*/
static int SchedulePrintTimes(const char *where, t_sched_data *data)
{
  int retcode;

  if(where)
  {
    retcode = CCTKi_DoScheduleTraverse(where,
       (int (*)(void *, void *))                    CCTKi_SchedulePrintTimesEntry, 
       (int (*)(void *, void *))                    CCTKi_SchedulePrintTimesExit, 
       (int  (*)(int, char **, void *, void *, int))CCTKi_SchedulePrintTimesWhile, 
       (int (*)(void *, void *, void *))            CCTKi_SchedulePrintTimesFunction, 
       (void *)data);
  }
  else
  {
    retcode = 0;
  }

  return retcode;
}

/********************************************************************
 *********************     Printing Routines   **********************
 ********************************************************************/


 /*@@
   @routine    CCTKi_SchedulePrintEntry
   @date       Sun Sep 19 13:31:23 1999
   @author     Tom Goodale
   @desc 
   Routine called on entry to a group when traversing for printing.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     attribute
   @vdesc   schedule item attributes
   @vtype   t_attribute *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     data
   @vdesc   data associated with schedule item
   @vtype   t_sched_data
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - schedule item is inactive
   1 - schedule item is active
   @endreturndesc
@@*/
static int CCTKi_SchedulePrintEntry(t_attribute *attribute, 
                                    t_sched_data *data)
{
  indent_level += 2;

  return 1;
}

/*@@
   @routine    CCTKi_SchedulePrintExit
   @date       Sun Sep 19 13:31:23 1999
   @author     Tom Goodale
   @desc 
   Routine called on exit to a group when traversing for printing.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     attribute
   @vdesc   schedule item attributes
   @vtype   t_attribute *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     data
   @vdesc   data associated with schedule item
   @vtype   t_sched_data
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   1 - this has no meaning
   @endreturndesc
@@*/
static int CCTKi_SchedulePrintExit(t_attribute *attribute, 
                                   t_sched_data *data)
{
  indent_level -=2;

  return 1;
}

/*@@
   @routine    CCTKi_SchedulePrintWhile
   @date       Sun Sep 19 13:31:23 1999
   @author     Tom Goodale
   @desc 
   Routine called for while of a group when traversing for printing.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     n_whiles
   @vdesc   number of while statements
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     whiles
   @vdesc   while statements
   @vtype   char **
   @vio     in
   @vcomment 
 
   @endvar 
   @var     attribute
   @vdesc   schedule item attributes
   @vtype   t_attribute *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     data
   @vdesc   data associated with schedule item
   @vtype   t_sched_data
   @vio     in
   @vcomment 
 
   @endvar 
   @var     first
   @vdesc   flag - is this the first time we are checking while on this schedule item
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - schedule item is inactive
   1 - schedule item is active
   @endreturndesc
@@*/
static int CCTKi_SchedulePrintWhile(int n_whiles, 
                                    char **whiles, 
                                    t_attribute *attribute, 
                                    t_sched_data *data,
                                    int first)
{
  int i;

  if(first)
  {
    for(i=0; i < indent_level+2; i++) printf(" ");

    printf("while (");
  
    for(i = 0; i < n_whiles; i++)
    {
      if(i > 0)
      {
        printf(" && ");
      }

      printf("%s", whiles[i]);
    }
    
    printf(")\n");
  }
  else
  {
    for(i=0; i < indent_level; i++) printf(" ");

    printf("end while\n");
  }

  return first;
}

/*@@
   @routine    CCTKi_SchedulePrintFunction
   @date       Sun Sep 19 13:36:25 1999
   @author     Tom Goodale
   @desc 
   Function which actually prints out data about a group or a function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     function
   @vdesc   the function to be called
   @vtype   void *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     attribute
   @vdesc   schedule item attributes
   @vtype   t_attribute *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     data
   @vdesc   data associated with schedule item
   @vtype   t_sched_data
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   1 - this has no meaning
   @endreturndesc
@@*/
static int CCTKi_SchedulePrintFunction(void *function, 
                                       t_attribute *attribute, 
                                       t_sched_data *data)
{
  int i;
  for(i=0; i < indent_level; i++) printf(" ");

  printf("%s: %s\n", attribute->thorn, attribute->description);

  return 1;  
}


/********************************************************************
 *********************     Calling Routines   ***********************
 ********************************************************************/


 /*@@
   @routine    CCTKi_ScheduleCallEntry
   @date       Sun Sep 19 13:24:06 1999
   @author     Tom Goodale
   @desc 
   Routine called when a schedule group is entered.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     attribute
   @vdesc   schedule item attributes
   @vtype   t_attribute *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     data
   @vdesc   data associated with schedule item
   @vtype   t_sched_data
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - schedule item is inactive
   1 - schedule item is active
   @endreturndesc
@@*/
static int CCTKi_ScheduleCallEntry(t_attribute *attribute, 
                                   t_sched_data *data)
{
  int i;
  int index; 
  int last;
  int go;

  if(attribute)
  {
    go = 0;

    if(data->schedpoint == schedpoint_analysis)
    {
      /* In analysis, so check triggers */
      for (i = 0; i < attribute->n_trigger_groups ; i++) 
      { 
        index = CCTK_FirstVarIndexI(attribute->trigger_groups[i]);
        last  = index + CCTK_NumVarsInGroupI(attribute->trigger_groups[i]) -1;
        for(; index <= last ; index++)
        {
          go = go || CCTKi_TriggerSaysGo(data->GH, index);
        }
      }
    }
    else
    {
      go = 1;
    }

    if(go)
    {
      /* Switch on storage for groups */
      for(i = 0; i < attribute->n_mem_groups; i++)
      {
        attribute->StorageOnEntry[i] = CCTK_EnableGroupStorageI(data->GH,attribute->mem_groups[i]);
      }

      /* Switch on communication for groups. */
      for(i = 0; i < attribute->n_comm_groups; i++)
      {
        attribute->CommOnEntry[i] = CCTK_EnableGroupCommI(data->GH,attribute->comm_groups[i]);
      }
    }

    /* Remember if we have switched on storage and comm or not. */
    attribute->done_entry = go;
  }
  else
  {
    go = 1;
  }

  return go;
}

/*@@
   @routine    CCTKi_ScheduleCallExit
   @date       Sun Sep 19 13:25:24 1999
   @author     Tom Goodale
   @desc 
   Routine called on exit from a schedule group.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     attribute
   @vdesc   schedule item attributes
   @vtype   t_attribute *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     data
   @vdesc   data associated with schedule item
   @vtype   t_sched_data
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   1 - this has no meaning
   @endreturndesc
@@*/
static int CCTKi_ScheduleCallExit(t_attribute *attribute, 
                                  t_sched_data *data)
{
  int i;
  int vindex;
  int last;

  /* Only do this if the entry routine did stuff. */
  if(attribute && attribute->done_entry)
  {

    if(data->schedpoint == schedpoint_analysis)
    {
      /* In analysis, so do any trigger actions. */
      for (i = 0; i < attribute->n_trigger_groups ; i++) 
      { 
        vindex = CCTK_FirstVarIndexI(attribute->trigger_groups[i]);
        last  = vindex + CCTK_NumVarsInGroupI(attribute->trigger_groups[i]) - 1;
        for(; vindex <= last ; vindex++)
        {
          CCTKi_TriggerAction(data->GH, vindex);
        }
      }
    }

    /* Switch off storage if it was switched on in entry. */
    for(i = 0; i < attribute->n_mem_groups; i++)
    {
      if(!attribute->StorageOnEntry[i]) CCTK_DisableGroupStorageI(data->GH,attribute->mem_groups[i]);
    }

    /* Switch off communication if it was done in entry. */
    for(i = 0; i < attribute->n_comm_groups; i++)
    {
      if(!attribute->CommOnEntry[i]) CCTK_DisableGroupCommI(data->GH,attribute->comm_groups[i]);
    }

    /* Synchronise variable groups associated with this schedule group. */
    CCTK_SyncGroupsI(data->GH, 
                     attribute->FunctionData.n_SyncGroups,  
                     attribute->FunctionData.SyncGroups);
  }

  return 1;
}

/*@@
   @routine    CCTKi_ScheduleCallWhile
   @date       Sun Sep 19 13:27:53 1999
   @author     Tom Goodale
   @desc 
   Routine called to check variables to see if a group or function should be executed.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     n_whiles
   @vdesc   number of while statements
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     whiles
   @vdesc   while statements
   @vtype   char **
   @vio     in
   @vcomment 
 
   @endvar 
   @var     attribute
   @vdesc   schedule item attributes
   @vtype   t_attribute *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     data
   @vdesc   data associated with schedule item
   @vtype   t_sched_data
   @vio     in
   @vcomment 
 
   @endvar 
   @var     first
   @vdesc   flag - is this the first time we are checking while on this schedule item
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - schedule item is inactive
   1 - schedule item is active
   @endreturndesc
@@*/
static int CCTKi_ScheduleCallWhile(int n_whiles, 
                                   char **whiles, 
                                   t_attribute *attribute, 
                                   t_sched_data *data,
                                   int first)
{
  int i;
  int retcode;

  retcode = 1;

  /* FIXME - should do a lot of validation either here or on registration */
  for(i = 0; i < n_whiles; i++)
  {
    retcode = retcode && *((CCTK_INT *)CCTK_VarDataPtr(data->GH, 0, whiles[i]));
  }

  return retcode;
}

/*@@
   @routine    CCTKi_ScheduleCallFunction
   @date       Sun Sep 19 13:29:14 1999
   @author     Tom Goodale
   @desc 
   The routine which actually calls a function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     function
   @vdesc   the function to be called
   @vtype   void *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     attribute
   @vdesc   schedule item attributes
   @vtype   t_attribute *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     data
   @vdesc   data associated with schedule item
   @vtype   t_sched_data
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   1 - this has no meaning
   @endreturndesc
@@*/
static int CCTKi_ScheduleCallFunction(void *function, 
                                      t_attribute *attribute, 
                                      t_sched_data *data)
{
  int synchronised;

  CCTK_TimerStartI(attribute->timer_handle);

  /* Use whatever has been chosen as the calling function for this 
   * function. 
   */
  synchronised = data->CallFunction(function, &(attribute->FunctionData), data->GH);

  CCTK_TimerStopI(attribute->timer_handle);

  /* Synchronise the groups if necessary */
  if(!synchronised)
  {
    CCTK_SyncGroupsI(data->GH, 
                     attribute->FunctionData.n_SyncGroups,  
                     attribute->FunctionData.SyncGroups);
  }

  return 1;
}

/********************************************************************
 ****************     Timer Printing Routines   *********************
 ********************************************************************/

 /*@@
   @routine    CCTKi_SchedulePrintTimesEntry
   @date       Fri Oct 22 12:26:26 1999
   @author     Tom Goodale
   @desc 
   Routine called on entry to a group when traversing for printing.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     attribute
   @vdesc   schedule item attributes
   @vtype   t_attribute *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     data
   @vdesc   data associated with schedule item
   @vtype   t_sched_data
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - schedule item is inactive
   1 - schedule item is active
   @endreturndesc
@@*/
static int CCTKi_SchedulePrintTimesEntry(t_attribute *attribute, 
                                         t_sched_data *data)
{
  return 1;
}

/*@@
   @routine    CCTKi_SchedulePrintTimesExit
   @date       Fri Oct 22 12:26:26 1999
   @author     Tom Goodale
   @desc 
   Routine called on exit to a group when traversing for printing.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     attribute
   @vdesc   schedule item attributes
   @vtype   t_attribute *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     data
   @vdesc   data associated with schedule item
   @vtype   t_sched_data
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   1 - this has no meaning
   @endreturndesc
@@*/
static int CCTKi_SchedulePrintTimesExit(t_attribute *attribute, 
                                        t_sched_data *data)
{
  return 1;
}

/*@@
   @routine    CCTKi_SchedulePrintTimesWhile
   @date       Fri Oct 22 12:26:26 1999
   @author     Tom Goodale
   @desc 
   Routine called for while ofo a group when traversing for printing.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     n_whiles
   @vdesc   number of while statements
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     whiles
   @vdesc   while statements
   @vtype   char **
   @vio     in
   @vcomment 
 
   @endvar 
   @var     attribute
   @vdesc   schedule item attributes
   @vtype   t_attribute *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     data
   @vdesc   data associated with schedule item
   @vtype   t_sched_data
   @vio     in
   @vcomment 
 
   @endvar 
   @var     first
   @vdesc   flag - is this the first time we are checking while on this schedule item
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - schedule item is inactive
   1 - schedule item is active
   @endreturndesc
@@*/
static int CCTKi_SchedulePrintTimesWhile(int n_whiles, 
                                         char **whiles, 
                                         t_attribute *attribute, 
                                         t_sched_data *data,
                                         int first)
{
  return first;
}

/*@@
   @routine    CCTKi_SchedulePrintTimesFunction
   @date       Fri Oct 22 12:26:26 1999
   @author     Tom Goodale
   @desc 
   Function which actually prints out data about a group or a function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     function
   @vdesc   the function to be called
   @vtype   void *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     attribute
   @vdesc   schedule item attributes
   @vtype   t_attribute *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     data
   @vdesc   data associated with schedule item
   @vtype   t_sched_data
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   1 - this has no meaning
   @endreturndesc
@@*/
static int CCTKi_SchedulePrintTimesFunction(void *function, 
                                            t_attribute *attribute, 
                                            t_sched_data *data)
{
  int i;
  for(i=0; i < indent_level; i++) printf(" ");

  CCTK_TimerI(attribute->timer_handle, data->info);

  if(data->print_headers)
  {
    CCTKi_SchedulePrintTimerHeaders(data->info);

    data->print_headers = 0;
  }

  printf("%-10.10s: %-40.40s", attribute->thorn, attribute->description);

  CCTKi_SchedulePrintTimerInfo(data->info);

  return 1;  
}

static void CCTKi_SchedulePrintTimerInfo(cTimerData *info)
{
  int i;

  /*  switch(info->vals[0].type) */
  for(i=0;i < info->n_vals; i++)
  {
    /*   case val_int:
      printf("%d", info->vals[0].val.i); break;
    case val_long:
      printf("%ld", info->vals[0].val.l); break;
    case val_double:
      printf("%g", info->vals[0].val.d); break;
    default:
      printf("Unknown value type at line %d of %s\n", __LINE__, __FILE__);
  }

  for(i = 1; i < info->n_vals; i++)
  {*/
    switch(info->vals[i].type)
    {
      case val_int:
        printf("\t%d", info->vals[i].val.i); break;
      case val_long:
        printf("\t%ld", info->vals[i].val.l); break;
      case val_double:
        printf("\t%g", info->vals[i].val.d); break;
      default:
        printf("Unknown value type at line %d of %s\n", __LINE__, __FILE__);
    }
  }

  printf("\n");

}

static void CCTKi_SchedulePrintTimerHeaders(cTimerData *info)
{
  int i;

  printf("%40s", info->vals[0].heading);

  for(i = 1; i < info->n_vals; i++)
  {
    printf("\t%s", info->vals[i].heading);
  }

  printf("\n");

}
