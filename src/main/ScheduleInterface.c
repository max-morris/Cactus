 /*@@
   @file      ScheduleInterface.c
   @date      Thu Sep 16 14:06:21 1999
   @author    Tom Goodale
   @desc 
   Routines to interface the main part of Cactus to the schedular.
   @enddesc 
 @@*/

static char *rcsid = "$Header$";

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "cctk.h"
#include "cctk_Schedule.h"
#include "cctki_Schedule.h"

#include "cctk_Flesh.h"
#include "cctk_Comm.h"

#include "cctk_Groups.h"
#include "cctk_GroupsOnGH.h"

#include "rfrInterface.h"

#include "cctk_FortranWrappers.h"

#include "CactusTimers.h"

/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/

typedef enum {sched_none, sched_group, sched_function} t_sched_type;
typedef enum {lang_none, lang_c, lang_fortran} t_lang_type;
typedef enum {schedpoint_misc, schedpoint_analysis} t_schedpoint;

typedef struct 
{
  /* Static data */
  char *description;

  char *thorn;
  char *implementation;

  t_sched_type type;
  t_lang_type language;

  int (*fortran_caller)(cGH *, void *);

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
  t_schedpoint schedpoint;

  int whiling;

  t_TimerInfo *info;
  int print_headers;
} t_sched_data;


/********************************************************************
 ********************* Local Routine Prototypes *********************
 ********************************************************************/

static t_attribute *CreateAttribute(const char *description, 
                                    const char *language, 
                                    const char *name,
                                    const char *thorn,
                                    int n_mem_groups, 
                                    int n_comm_groups, 
                                    int n_trigger_groups, 
                                    va_list *ap);
static t_sched_modifier *CreateModifiers(int n_before, 
                                         int n_after, 
                                         int n_while, 
                                         va_list *ap);
static int CreateGroupIndexList(int n_items, int *array, va_list *ap);
static t_sched_modifier *CreateTypedModifier(t_sched_modifier *modifier,
                                             const char *type,
                                             int n_items,
                                             va_list *ap);
static t_lang_type TranslateLanguage(const char *sval);

static int SchedulePrint(const char *where);

static int CCTKi_SchedulePrintEntry(t_attribute *attribute, t_sched_data *data);
static int CCTKi_SchedulePrintExit(t_attribute *attribute, t_sched_data *data);
static int CCTKi_SchedulePrintWhile(int n_whiles, char **whiles, t_attribute *attribute, t_sched_data *data);
static int CCTKi_SchedulePrintFunction(void *function, t_attribute *attribute, t_sched_data *data);

static int CCTKi_ScheduleCallEntry(t_attribute *attribute, t_sched_data *data);
static int CCTKi_ScheduleCallExit(t_attribute *attribute, t_sched_data *data);
static int CCTKi_ScheduleCallWhile(int n_whiles, char **whiles, t_attribute *attribute, t_sched_data *data);
static int CCTKi_ScheduleCallFunction(void *function, t_attribute *attribute, t_sched_data *data);

static int CCTKi_ScheduleStartupFunction(void *function, t_attribute *attribute, t_sched_data *data);

static int SchedulePrintTimes(const char *where, t_sched_data *data);

static int CCTKi_SchedulePrintTimesEntry(t_attribute *attribute, t_sched_data *data);
static int CCTKi_SchedulePrintTimesExit(t_attribute *attribute, t_sched_data *data);
static int CCTKi_SchedulePrintTimesWhile(int n_whiles, char **whiles, t_attribute *attribute, t_sched_data *data);
static int CCTKi_SchedulePrintTimesFunction(void *function, t_attribute *attribute, t_sched_data *data);
static void CCTKi_SchedulePrintTimerInfo(t_TimerInfo *info);
static void CCTKi_SchedulePrintTimerHeaders(t_TimerInfo *info);


/********************************************************************
 ********************* Other Routine Prototypes *********************
 ********************************************************************/

/* FIXME: these should be renamed and put in a header somewhere */

int CCTKi_rfrTriggerSaysGo(cGH *GH, int variable);
int CCTKi_rfrTriggerAction(void *GH, int variable);


/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/


static int indent_level = 0;

static int n_scheduled_comm_groups = 0;
static int *scheduled_comm_groups = NULL;

static int n_scheduled_storage_groups = 0;
static int *scheduled_storage_groups = NULL;

static t_TimerInfo *timerinfo = NULL;

/********************************************************************
 *********************     External Routines   **********************
 ********************************************************************/

 /*@@
   @routine    CCTK_ScheduleFunction
   @date       Thu Sep 16 18:19:01 1999
   @author     Tom Goodale
   @desc 
   Schedules a function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
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
                          ...)
{
  int retcode;
  t_attribute *attribute;
  t_sched_modifier *modifier;
  va_list ap;

  va_start(ap, n_while);
  
  attribute = CreateAttribute(description, language, thorn, implementation, 
                              n_mem_groups, n_comm_groups, n_trigger_groups, &ap);
  modifier  = CreateModifiers(n_before, n_after, n_while, &ap);

  va_end(ap);

  if(attribute && (modifier || (n_before == 0 && n_after == 0 && n_while == 0)))
  {
    retcode = CCTKi_ScheduleFunction(where, name, function, modifier, (void *)attribute);
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
   @routine    CCTK_ScheduleGroup
   @date       Thu Sep 16 18:19:18 1999
   @author     Tom Goodale
   @desc 
   Schedules a group.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
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
                       ...)
{
  int retcode;
  t_attribute *attribute;
  t_sched_modifier *modifier;
  va_list ap;

  va_start(ap, n_while);
  
  attribute = CreateAttribute(description, NULL, thorn, implementation,
                              n_mem_groups, n_comm_groups, n_trigger_groups, &ap);
  modifier  = CreateModifiers(n_before, n_after, n_while, &ap);

  va_end(ap);

  if(attribute && (modifier || (n_before == 0 && n_after == 0 && n_while == 0)))
  {
    retcode = CCTKi_ScheduleGroup(where, name, modifier, (void *)attribute);
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
   @routine    CCTK_ScheduleGroupStorage
   @date       Fri Sep 17 18:55:59 1999
   @author     Tom Goodale
   @desc 
   Schedules a group for storage when a GH is created.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_ScheduleGroupStorage(const char *group)
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
   @routine    CCTK_ScheduleGroupComm
   @date       Fri Sep 17 18:55:59 1999
   @author     Tom Goodale
   @desc 
   Schedules a group for communication when a GH is created.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_ScheduleGroupComm(const char *group)
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
   @date       Fri Sep 17 21:52:44 1999
   @author     Tom Goodale
   @desc 
   Traverses the given schedule point.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_ScheduleTraverse(const char *where, void *GH)
{
  t_sched_data data;

  int (*calling_function)(void *, t_attribute *, t_sched_data *);
  
  data.GH = (cGH *)GH;
  
  if(CCTK_Equals(where, "CCTK_STARTUP"))
  {
    calling_function = CCTKi_ScheduleStartupFunction;
  }
  else
  {
    calling_function = CCTKi_ScheduleCallFunction;
  }

  if(CCTK_Equals(where, "CCTK_ANALYSIS"))
  {
    data.schedpoint = schedpoint_analysis;
  }
  else
  {
    data.schedpoint = schedpoint_misc;
  }
  
  CCTKi_ScheduleTraverse(where,
                         (int (*)(void *, void *))               CCTKi_ScheduleCallEntry, 
                         (int (*)(void *, void *))               CCTKi_ScheduleCallExit, 
                         (int  (*)(int, char **, void *, void *))CCTKi_ScheduleCallWhile, 
                         (int (*)(void *, void *, void *))       calling_function, 
                         (void *)&data);

  return 0;
}

 /*@@
   @routine    CCTK_ScheduleGHInit
   @date       Fri Sep 17 21:25:13 1999
   @author     Tom Goodale
   @desc 
   Does any scheduling stuff setup which requires a GH.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_ScheduleGHInit(void *GH)
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
    SchedulePrint("CCTK_BASEGRID");
    SchedulePrint("CCTK_INITIAL");
    SchedulePrint("CCTK_POSTINITIAL");
    SchedulePrint("CCTK_POSTSTEP");
    printf("\n");
    printf ("  do loop over timesteps\n");
    SchedulePrint("CCTK_PRESTEP");
    SchedulePrint("CCTK_EVOL");
    printf ("    t = t+dt\n");
    SchedulePrint("CCTK_POSTSTEP");
    printf ("    if (analysis)\n");
    indent_level +=2;
    SchedulePrint("CCTK_ANALYSIS");
    indent_level -=2;
    printf ("    endif\n");
    printf ("  enddo\n");
  }
  else
  {
    SchedulePrint(where);
  }

  return 0;
}

int CCTK_SchedulePrintTimes(const char *where)
{
  t_sched_data data;

  data.GH = NULL;
  data.schedpoint = schedpoint_misc;
  data.whiling = 0;
  data.print_headers = 1;

  if(!timerinfo)
  {
    timerinfo = CCTK_TimerCreateInfo();
  }
  
  data.info = timerinfo;

  if(!where)
  {
    SchedulePrintTimes("CCTK_STARTUP", &data);
    printf("\n");
    SchedulePrintTimes("CCTK_PARAMCHECK", &data);
    printf("\n");
    SchedulePrintTimes("CCTK_BASEGRID", &data);
    SchedulePrintTimes("CCTK_INITIAL", &data);
    SchedulePrintTimes("CCTK_POSTINITIAL", &data);
    SchedulePrintTimes("CCTK_POSTSTEP", &data);
    printf("\n");
    SchedulePrintTimes("CCTK_PRESTEP", &data);
    SchedulePrintTimes("CCTK_EVOL", &data);
    printf("\n");
    SchedulePrintTimes("CCTK_ANALYSIS", &data);
  }
  else
  {
    SchedulePrintTimes(where, &data);
  }

  return 0;
}

/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/

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

@@*/
static t_attribute *CreateAttribute(const char *description, 
                                    const char *language, 
                                    const char *thorn,
                                    const char *implementation,
                                    int n_mem_groups, 
                                    int n_comm_groups, 
                                    int n_trigger_groups, 
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
    this->StorageOnEntry = (int *)malloc(n_mem_groups*sizeof(int));
    this->CommOnEntry    = (int *)malloc(n_comm_groups*sizeof(int));

    if(this->description     && 
       this->thorn           &&
       this->implementation  &&
       (this->mem_groups || n_mem_groups==0)      &&
       (this->comm_groups || n_comm_groups==0)    &&
       (this->trigger_groups || n_trigger_groups==0))
    {
      strcpy(this->description,    description);
      strcpy(this->thorn,          thorn);
      strcpy(this->implementation, implementation);

      if(language)
      {
        this->type = sched_function;
        this->language = TranslateLanguage(language);
        this->fortran_caller = (int (*)(cGH *,void *))CCTK_FortranWrapper(thorn);
      }
      else
      {
        this->type = sched_group;
        this->language = lang_none;
      }
      
      /* Create the lists of indices of groups we're interested in. */
      CreateGroupIndexList(n_mem_groups,     this->mem_groups, ap);
      CreateGroupIndexList(n_comm_groups,    this->comm_groups, ap);
      CreateGroupIndexList(n_trigger_groups, this->trigger_groups, ap);

      this->n_mem_groups     = n_mem_groups;
      this->n_comm_groups    = n_comm_groups;
      this->n_trigger_groups = n_trigger_groups;

      /* Add a timer to the item */
      
      this->timer_handle = CCTK_TimerCreateI();
    }
    else
    {
      free(this->description);
      free(this->comm_groups);
      free(this->trigger_groups);
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
   @routine    TranslateLanguage
   @date       Thu Sep 16 18:18:31 1999
   @author     Tom Goodale
   @desc 
   Translates a language string into an internal enum.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static t_lang_type TranslateLanguage(const char *sval)
{
  t_lang_type retcode;

  if(CCTK_Equals(sval, "C"))
  {
    retcode = lang_c;
  }
  else if(CCTK_Equals(sval, "Fortran"))
  {
    retcode = lang_fortran;
  }
  else
  {
    fprintf(stderr, "Unknown language %s\n", sval);
    retcode = lang_none;
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

@@*/
static int SchedulePrint(const char *where)
{
  int retcode;
  t_sched_data data;

  data.GH = NULL;
  data.schedpoint = schedpoint_misc;
  data.whiling = 0;

  if(where)
  {
    retcode = CCTKi_ScheduleTraverse(where,
                                     (int (*)(void *, void *))               CCTKi_SchedulePrintEntry, 
                                     (int (*)(void *, void *))               CCTKi_SchedulePrintExit, 
                                     (int  (*)(int, char **, void *, void *))CCTKi_SchedulePrintWhile, 
                                     (int (*)(void *, void *, void *))       CCTKi_SchedulePrintFunction, 
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

@@*/
static int SchedulePrintTimes(const char *where, t_sched_data *data)
{
  int retcode;

  if(where)
  {
    retcode = CCTKi_ScheduleTraverse(where,
                                     (int (*)(void *, void *))               CCTKi_SchedulePrintTimesEntry, 
                                     (int (*)(void *, void *))               CCTKi_SchedulePrintTimesExit, 
                                     (int  (*)(int, char **, void *, void *))CCTKi_SchedulePrintTimesWhile, 
                                     (int (*)(void *, void *, void *))       CCTKi_SchedulePrintTimesFunction, 
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
   Routine called for while ofo a group when traversing for printing.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int CCTKi_SchedulePrintWhile(int n_whiles, 
                                    char **whiles, 
                                    t_attribute *attribute, 
                                    t_sched_data *data)
{
  int i;

  if(!data->whiling)
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

  data->whiling = !data->whiling;

  return data->whiling;
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
          go = go || CCTKi_rfrTriggerSaysGo(data->GH, index);
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

@@*/
static int CCTKi_ScheduleCallExit(t_attribute *attribute, 
                                  t_sched_data *data)
{
  int i;
  int index;
  int last;

  /* Only do this if the entry routine did stuff. */
  if(attribute && attribute->done_entry)
  {

    if(data->schedpoint == schedpoint_analysis)
    {
      /* In analysis, so do any trigger actions. */
      for (i = 0; i < attribute->n_trigger_groups ; i++) 
      { 
        index = CCTK_FirstVarIndexI(attribute->trigger_groups[i]);
        last  = index + CCTK_NumVarsInGroupI(attribute->trigger_groups[i]) - 1;
        for(; index <= last ; index++)
        {
          CCTKi_rfrTriggerAction(data->GH, index);
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

@@*/
static int CCTKi_ScheduleCallWhile(int n_whiles, 
                                   char **whiles, 
                                   t_attribute *attribute, 
                                   t_sched_data *data)
{
  int i;
  int retcode;

  retcode = 1;

  /* FIXME - should do a lot of validation either here or on registration */
  for(i = 0; i < n_whiles; i++)
  {
    retcode = retcode && *((int *)CCTK_VarDataPtr(data->GH, 0, whiles[i]));
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

@@*/
static int CCTKi_ScheduleCallFunction(void *function, 
                                      t_attribute *attribute, 
                                      t_sched_data *data)
{

  void (*calledfunc)(void *);

  CCTK_TimerStartI(attribute->timer_handle);
  if(attribute->language == lang_fortran)
  {
    /* Call the fortran wrapper. */
    attribute->fortran_caller(data->GH, function);
  }
  else
  {
    calledfunc = (void (*)(void *))function;

    /* Call the function. */
  
    calledfunc(data->GH);
  }

  CCTK_TimerStopI(attribute->timer_handle);

  return 1;
}

/********************************************************************
 ***************  Specialised Startup Routines   ********************
 ********************************************************************/

 /*@@
   @routine    CCTKi_ScheduleStartupFunction
   @date       Sun Sep 19 13:30:00 1999
   @author     Tom Goodale
   @desc 
   Startup routines take no arguments, so use this calling function instead
   of the one generally used.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int CCTKi_ScheduleStartupFunction(void *function, 
                                         t_attribute *attribute, 
                                         t_sched_data *data)
{

  int (*calledfunc)(void);

  calledfunc = (int (*)(void))function;

  /* Call the function. */
  
  calledfunc();

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

@@*/
static int CCTKi_SchedulePrintTimesEntry(t_attribute *attribute, 
                                         t_sched_data *data)
{
  /*  indent_level += 2;*/
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

@@*/
static int CCTKi_SchedulePrintTimesExit(t_attribute *attribute, 
                                        t_sched_data *data)
{
  /*  indent_level -=2; */
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

@@*/
static int CCTKi_SchedulePrintTimesWhile(int n_whiles, 
                                         char **whiles, 
                                         t_attribute *attribute, 
                                         t_sched_data *data)
{

#if 0
  int i;
  if(!data->whiling)
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
#endif

  data->whiling = !data->whiling;

  return data->whiling;
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

@@*/
static int CCTKi_SchedulePrintTimesFunction(void *function, 
                                            t_attribute *attribute, 
                                            t_sched_data *data)
{
  int i;
  for(i=0; i < indent_level; i++) printf(" ");

  CCTK_TimerGetI(attribute->timer_handle, data->info);

  if(data->print_headers)
  {
    CCTKi_SchedulePrintTimerHeaders(data->info);

    data->print_headers = 0;
  }

  printf("%-16s: %-50s\t", attribute->thorn, attribute->description);

  CCTKi_SchedulePrintTimerInfo(data->info);

  return 1;  
}

static void CCTKi_SchedulePrintTimerInfo(t_TimerInfo *info)
{
  int i;

  switch(info->vals[0].type)
  {
    case val_int:
      printf("%d", info->vals[0].val.i); break;
    case val_long:
      printf("%ld", info->vals[0].val.l); break;
    case val_double:
      printf("%g", info->vals[0].val.d); break;
    default:
      printf("Unknown value type at line %d of %s\n", __LINE__, __FILE__);
  }

  for(i = 1; i < info->n_vals; i++)
  {
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

static void CCTKi_SchedulePrintTimerHeaders(t_TimerInfo *info)
{
  int i;

  printf("%40s", info->vals[0].heading);

  for(i = 1; i < info->n_vals; i++)
  {
    printf("\t%s", info->vals[i].heading);
  }

  printf("\n");

}
