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

/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/

typedef enum {sched_none, sched_group, sched_function} t_sched_type;
typedef enum {lang_none, lang_c, lang_fortran} t_lang_type;


typedef struct 
{
  char *description;

  char *thorn;
  char *implementation;

  t_sched_type type;
  t_lang_type language;
  
  int n_mem_groups;
  int *mem_groups;

  int n_comm_groups;
  int *comm_groups;

  int n_trigger_groups;
  int *trigger_groups;

  int n_trigger_vars;
  int *trigger_vars;

} t_attribute;

typedef struct
{
  cGH *GH;
  int analysis;
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

static int CCTKi_SchedulePrintEntry(t_attribute *attribute, t_sched_data *data);
static int CCTKi_SchedulePrintExit(t_attribute *attribute, t_sched_data *data);
static int CCTKi_SchedulePrintWhile(int n_whiles, char **whiles, t_attribute *attribute, t_sched_data *data);
static int CCTKi_SchedulePrintFunction(void *function, t_attribute *attribute, t_sched_data *data);

static int CCTKi_ScheduleCallEntry(t_attribute *attribute, t_sched_data *data);
static int CCTKi_ScheduleCallExit(t_attribute *attribute, t_sched_data *data);
static int CCTKi_ScheduleCallWhile(int n_whiles, char **whiles, t_attribute *attribute, t_sched_data *data);
static int CCTKi_ScheduleCallFunction(void *function, t_attribute *attribute, t_sched_data *data);

/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/


static int indent_level = 0;


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

  if(attribute && modifier)
  {
    retcode = CCTKi_ScheduleFunction(where, name, function, modifier, (void *)attribute);
  }
  else
  {
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
  }
  else
  {
    retcode = -1;
  }

  return retcode;

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
      }
      else
      {
        this->type = sched_group;
        this->language = lang_none;
      }
      
      CreateGroupIndexList(n_mem_groups,     this->mem_groups, ap);
      CreateGroupIndexList(n_comm_groups,    this->comm_groups, ap);
      CreateGroupIndexList(n_trigger_groups, this->trigger_groups, ap);

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
  modifier = CreateTypedModifier(modifier, "after", n_before, ap);
  modifier = CreateTypedModifier(modifier, "while", n_before, ap);

  return modifier;
}

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

  return 0;  
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
  return 1;
}

static int CCTKi_ScheduleCallExit(t_attribute *attribute, 
                                  t_sched_data *data)
{
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

  calledfunc = (void (*)(void *))function;

  /* Call the function. */
  
  calledfunc(data->GH);

  return 1;
}
