#define DEBUG
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
#include "cctk_schedule.h"
#include "cctki_schedule.h"

#include "cctk_Flesh.h"
#include "cctk_Comm.h"

#include "cctk_Groups.h"
#include "cctk_GroupsOnGH.h"

#include "rfrInterface.h"

#include "cctk_FortranWrappers.h"

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

  /* Dynamic data */
  int *CommOnEntry;
  int *StorageOnEntry;

  int done_entry;

} t_attribute;

typedef struct
{
  cGH *GH;
  t_schedpoint schedpoint;
  
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

/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/


static int indent_level = 0;

static int n_scheduled_comm_groups = 0;
static int *scheduled_comm_groups = NULL;

static int n_scheduled_storage_groups = 0;
static int *scheduled_storage_groups = NULL;


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
#ifdef DEBUG
    fprintf(stderr, "Failed to schedule %s at %s!!!\n", name, where);
#endif
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

  if(attribute && modifier)
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
  t_sched_data data;

  data.GH = NULL;
  data.schedpoint = schedpoint_misc;

  if(!where)
  {
    printf ("startup routines\n");
    SchedulePrint("CCTK_STARTUP");
    printf("\n");
    printf ("Parameter checking routines\n");
    SchedulePrint("CCTK_PARAMCHECK");
    printf("\n");
    printf("Initialisation\n");
    SchedulePrint("CCTK_INITIAL");
    SchedulePrint("CCTK_POSTINITIAL");
    SchedulePrint("CCTK_POSTSTEP");
    printf("\n");
    printf ("do loop over timesteps\n");
    SchedulePrint("CCTK_PRESTEP");
    SchedulePrint("CCTK_EVOL");
    SchedulePrint("CCTK_BOUND");
    printf ("  t = t+dt\n");
    SchedulePrint("CCTK_POSTSTEP");
    printf ("  if (analysis)\n");
    indent_level +=2;
    SchedulePrint("CCTK_ANALYSIS");
    indent_level -=2;
    printf ("  endif\n");
    printf ("enddo\n");
  }
  else
  {
    SchedulePrint(where);
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
       this->mem_groups      &&
       this->comm_groups     &&
       this->trigger_groups)
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

static int SchedulePrint(const char *where)
{
  int retcode;
  t_sched_data data;

  data.GH = NULL;
  data.schedpoint = schedpoint_misc;

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

/********************************************************************
 *********************     Printing Routines   **********************
 ********************************************************************/


static int CCTKi_SchedulePrintEntry(t_attribute *attribute, 
                                    t_sched_data *data)
{
  indent_level++;
  return 1;
}

static int CCTKi_SchedulePrintExit(t_attribute *attribute, 
                                   t_sched_data *data)
{
  indent_level--;
  return 1;
}

static int CCTKi_SchedulePrintWhile(int n_whiles, 
                                    char **whiles, 
                                    t_attribute *attribute, 
                                    t_sched_data *data)
{
  return 0;
}

static int CCTKi_SchedulePrintFunction(void *function, 
                                       t_attribute *attribute, 
                                       t_sched_data *data)
{
  int i;
  for(i=0; i < indent_level; i++) printf(" ");

  printf("%s\n", attribute->description);

  return 1;  
}


/********************************************************************
 *********************     Calling Routines   ***********************
 ********************************************************************/


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
      for (i = 0; i < attribute->n_trigger_groups ; i++) 
      { 
        index = CCTK_FirstVarIndexI(attribute->trigger_groups[i]);
        last  = index + CCTK_NumVarsInGroupI(attribute->trigger_groups[i]) - 1;
        for(; index < last ; index++)
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
      for(i = 0; i < attribute->n_mem_groups; i++)
      {
        attribute->StorageOnEntry[i] = CCTK_EnableGroupStorageI(data->GH,attribute->mem_groups[i]);
      }

      for(i = 0; i < attribute->n_comm_groups; i++)
      {
        attribute->CommOnEntry[i] = CCTK_EnableGroupCommI(data->GH,attribute->comm_groups[i]);
      }
    }

    attribute->done_entry = go;
  }
  else
  {
    go = 1;
  }

  return go;
}

static int CCTKi_ScheduleCallExit(t_attribute *attribute, 
                                  t_sched_data *data)
{
  int i;
  int index;
  int last;

  if(attribute && attribute->done_entry)
  {

    if(data->schedpoint == schedpoint_analysis)
    {
      for (i = 0; i < attribute->n_trigger_groups ; i++) 
      { 
        index = CCTK_FirstVarIndexI(attribute->trigger_groups[i]);
        last  = index + CCTK_NumVarsInGroupI(attribute->trigger_groups[i]) - 1;
        for(; index < last ; index++)
        {
          CCTKi_rfrTriggerAction(data->GH, index);
        }
      }
    }


    for(i = 0; i < attribute->n_mem_groups; i++)
    {
      if(!attribute->StorageOnEntry[i]) CCTK_DisableGroupStorageI(data->GH,attribute->mem_groups[i]);
    }

    for(i = 0; i < attribute->n_comm_groups; i++)
    {
      if(!attribute->CommOnEntry[i]) CCTK_DisableGroupCommI(data->GH,attribute->comm_groups[i]);
    }
  }

  return 1;
}

static int CCTKi_ScheduleCallWhile(int n_whiles, 
                                   char **whiles, 
                                   t_attribute *attribute, 
                                   t_sched_data *data)
{
  return 0;
}

static int CCTKi_ScheduleCallFunction(void *function, 
                                      t_attribute *attribute, 
                                      t_sched_data *data)
{

  void (*calledfunc)(void *);

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

  return 1;
}

/********************************************************************
 ***************  Specialised Startup Routines   ********************
 ********************************************************************/

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
