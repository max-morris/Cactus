/*@@
   @file      Parameters.c
   @date      Mon Jun 28 21:44:17 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
 @@*/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "SKBinTree.h"

#include "cctk_ActiveThorns.h"

#include "ParameterBindings.h"


const char *rcsid="$Header$";

int STR_cmpi(const char *string1, const char *string2);

#define STR_CMP(a,b) STR_cmpi(a,b)

#define SCOPE_GLOBAL        1
#define SCOPE_RESTRICTED    2
#define SCOPE_PRIVATE       3
#define SCOPE_NOT_GLOBAL    4
#define SCOPE_ANY           5

typedef struct RANGE
{
  struct RANGE *last;
  struct RANGE *next;
  char *range;
  char *origin;
  int active;
  char *description;
} t_range;

typedef struct PARAMETER
{
  char *name;
  char *thorn;
  int scope;

  char *description;
  char *defval;

  int type;
  void *data;

  t_range *range;

  int n_set;
  int steerable;

} t_parameter;

typedef struct PARAMLIST
{
  struct PARAMLIST *last;
  struct PARAMLIST *next;

  t_parameter *param;
} t_paramlist;

typedef struct PARAMTREENODE
{
  t_paramlist *paramlist;
} t_paramtreenode;


static t_parameter *ParameterFind(const char *name, 
                                  const char *thorn, 
                                  int scope);

static t_parameter *ParameterNew(const char *thorn,
				 const char *name,
				 const char *type,
				 const char *scope,
				 int        steerable,
				 const char *description,
				 const char *defval,
				 void       *data);

static int ParameterCheck(const char *thorn,
                          const char *name,
                          const char *type,
                          const char *scope,
                          int        steerable,
                          const char *description,
                          const char *defval,
                          void       *data);

static void *ParameterGetSimple(t_parameter *param, 
				int *type);

static int ParameterSetSimple(t_parameter *param, const char *value);

static t_paramtreenode *ParameterPTreeNodeFind(t_sktree *tree, 
                                               const char *name);

static t_sktree *ParameterPTreeNodeAdd(t_sktree **tree,
				       const char *name,
				       t_paramtreenode *new_node);

static int ParameterGetScope(const char *scope);
static int ParameterGetType(const char *type);

static int ParameterInsert(t_sktree **tree, 
			   t_parameter *newparam);

static int ParameterPrintSimple(t_parameter *param, 
				const char *format,
				FILE *file);

static int ParameterExtend(t_parameter *parameter,  
			   const char *range_origin, 
			   const char *range, 
			   const char *range_description);

static int ParameterListAddParam(t_paramlist **paramlist,
				 t_parameter *newparam);


static int ParameterSetKeyword(t_parameter *param, const char *value);
static int ParameterSetString(t_parameter *param, const char *value);
static int ParameterSetSentence(t_parameter *param, const char *value);
static int ParameterSetInteger(t_parameter *param, const char *value);
static int ParameterSetReal(t_parameter *param, const char *value);
static int ParameterSetBoolean(t_parameter *param, const char *value);



static t_sktree *paramtree=NULL;




/*@@
   @routine    ParameterCreate
   @date       Tue Jun 29 10:08:56 1999
   @author     Tom Goodale
   @desc 
   Creates a parameter originating from a thorn/implementation.    
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     name
   @vdesc   The name of the parameter
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     thorn
   @vdesc   The originating thorn
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @var     scope
   @vdesc   The scope of the parameter
   @vtype   const char *
   @vio     in
   @vcomment 
   Private, Restricted, or Global
   @endvar 
   @var     steerable
   @vdesc   Is the parameter steerable ?
   @vtype   int
   @vio     in
   @vcomment 
   True or false
   @endvar 
   @var     description
   @vdesc   A description of the parameter
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     defval
   @vdesc   The default value
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     data
   @vdesc   A pointer to the memory holding the parameter
   @vtype   void *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   0 on success, 
   -1 on failure to create the parameter
   -2 if parameter inconsistent
   @endreturndesc

@@*/
int ParameterCreate(const char *name,
		    const char *thorn,
                    const char *type,
                    const char *scope,
                    int        steerable,
                    const char *description,
                    const char *defval,
                    void       *data,
		    int n_ranges,
		    ...)
{
  int retval;
  int iscope;
  int i;
  t_parameter *parameter;
  va_list ranges;
  const char *rangeval;
  const char *rangedesc;

  iscope = ParameterGetScope(scope);

  parameter = ParameterFind(name, thorn, iscope);

  if(!parameter)
  {
    parameter = ParameterNew(thorn, name, 
			     type, scope, steerable, description, defval, data);

    if(n_ranges)
    {
      va_start(ranges, n_ranges);
      
      for(i=0; i < n_ranges; i++)
      {
	rangeval = (const char *)va_arg(ranges, const char *);
	rangedesc = (const char *)va_arg(ranges, const char *);
	
	ParameterExtend(parameter, thorn, rangeval, rangedesc);
      }
      va_end(ranges);

    }

    retval = ParameterSetSimple(parameter, defval);

  }
  else
  {
    retval = ParameterCheck(thorn, name, 
                            type, scope, steerable, description, defval, data);
  }

  return retval;
}

/*@@
   @routine    ParameterAddRange
   @date       Tue Jun 29 10:15:53 1999
   @author     Tom Goodale
   @desc 
   Adds a range.  Only allowed to add a range if in appropriate scope.  
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     origin
   @vdesc   The originating implementation or thorn of the parameter
   @vtype   const char *
   @vio     in
   @vcomment 

   @endvar
   @var     name
   @vdesc   The name of the parameter
   @vtype   const char *
   @vio     in
   @vcomment 

   @endvar
   @var     range_origin
   @vdesc   The originating implementation or thorn of the range
   @vtype   const char *
   @vio     in
   @vcomment 

   @endvar
   @var     range
   @vdesc   The new range
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     range_description
   @vdesc   A description of the new range
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
 
   @returntype int
   @returndesc
   0 on success, 
   A -ve number on failure
   @endreturndesc

@@*/
int ParameterAddRange(const char *implementation, 
                      const char *name,
                      const char *range_origin,
                      const char *range,
                      const char *range_description)
{
  int retval;
  t_parameter *parameter;

  /* For the moment do this in the quick and dirty way 8-(  FIXME */
  t_sktree *thornlist;

  t_sktree *node;

  /*printf("Extending parameter %s::%s from thorn %s\n", implementation, name, range_origin);*/

  thornlist = CCTK_ImpThornList(implementation);

  retval = -1;

  if(thornlist)
  {
    for(node= SKTreeFindFirst(thornlist);
	node; 
	node = node->next)
    {
      parameter = ParameterFind(name, node->key, SCOPE_RESTRICTED);
      
      if(parameter)
      {
	retval = ParameterExtend(parameter,  range_origin, range, range_description);
      }
      else
      {
	retval = -1;
      }
    }
  }
  
  return retval;
}

/*@@
   @routine    ParameterSet
   @date       Tue Jun 29 10:22:22 1999
   @author     Tom Goodale
   @desc 
   Sets the value (checks for steerable if not initialisation).
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     name
   @vdesc   The name of the parameter
   @vtype   const char *
   @vio     in
   @vcomment 
   name 
   @endvar
   @var     thorn
   @vdesc   The originating thorn
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     implementation
   @vdesc   The originating implementation
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     value
   @vdesc   The value of the parameter
   @vtype   const char *
   @vio     in
   @vcomment 

   @endvar
   @returntype int
   @returndesc
   The number of parameters set
   @endreturndesc

@@*/
int ParameterSet(const char *name,
		 const char *thorn,
                 const char *value)
{
  int retval;
  int iscope;
  t_parameter *param;

  param = ParameterFind(name, thorn, SCOPE_ANY);

  if(param)
  {
    retval = ParameterSetSimple(param, value);
  }
  else
  {
    retval = -1;
  }

  return retval;
}

/*@@
   @routine    ParameterPrintDescription
   @date       Tue Jun 29 10:24:49 1999
   @author     Tom Goodale
   @desc 
   Prints out a description on the given file descriptor with the given
   format.  Should include all data - i.e. ranges, range descriptions,
   range origins, and default value.
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     name
   @vdesc   The name of the parameter
   @vtype   const char *
   @vio     in
   @vcomment 
   name
   @endvar
   @var     thorn
   @vdesc   The originating thorn
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     implementation
   @vdesc   The originating implementation
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     format
   @vdesc   The printf format string
   @vtype   const char *
   @vio     in
   @vcomment 
   This is for each line of the test, and should have one %s in it.
   @endvar
   @var     file
   @vdesc   File descriptor
   @vtype   FILE *
   @vio     in
   @vcomment 
   The file to pront out on.
   @endvar 

   @returntype int
   @returndesc
   1 on success, 0 on failure.
   @endreturndesc

@@*/
int ParameterPrintDescription(const char *name,
			      const char *thorn,
                              const char *format,
                              FILE *file)
{
  int retval;
  int iscope;
  t_parameter *param;

  param = NULL;

  param = ParameterFind(name, thorn, SCOPE_ANY);

  if(param)
  {
    retval = ParameterPrintSimple(param, format, file);
  }
  else
  {
    retval = -1;
  }

  return retval;
}

/*@@
   @routine    ParameterGet
   @date       Tue Jun 29 10:28:20 1999
   @author     Tom Goodale
   @desc 
   Gets the pointer to the parameter - should be used for checkpointing and
   recovery.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     name
   @vdesc   The name of the parameter
   @vtype   const char *
   @vio     in
   @vcomment 
   name
   @endvar
   @var     thorn
   @vdesc   The originating thorn
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     implementation
   @vdesc   The originating implementation
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     type
   @vdesc   The type of the parameter
   @vtype   int *
   @vio     out
   @vcomment 
   An integer representing the type of the parameter
   @endvar

   @returntype void *
   @returndesc
   The address of the parameter.
   @endreturndesc

@@*/
void *ParameterGet(const char *name,
		   const char *thorn,
		   int *type)
{
  void *retval;
  int iscope;
  t_parameter *param;

  param = NULL;

  param = ParameterFind(name, thorn, SCOPE_ANY);

  if(param)
  {
    retval = ParameterGetSimple(param, type);
  }
  else
  {
    retval = NULL;
  }


  return retval;

}

/*@@
   @routine    ParameterWalk
   @date       Tue Jun 29 10:30:21 1999
   @author     Tom Goodale
   @desc 
   Gets parameters in order, restricted to ones from 'origin', or all if
   'origin' is NULL.  Starts with the first parameter if 'first' is true,
   otherwise gets the next one.
   Can be used for generating full help file, or for walking the list and
   checkpointing.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     first
   @vdesc   Flag to indicate get first parameter or not
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 
   @var     origin
   @vdesc   The origin of this walk
   @vtype   const char *
   @vio     in
   @vcomment 
   The thorn or implementation to walk, or NULL if to walk all params.
   @endvar 

   @returntype const char *
   @returndesc
   The full name of the parameter. (Implementation::name).
   @endreturndesc

@@*/
const char *ParameterWalk(int first,
                          const char *origin)
{
  return NULL;
}


static t_parameter *ParameterFind(const char *name, 
                                  const char *thorn, 
                                  int scope)
{
  t_parameter *retval;
  t_paramtreenode *node;
  t_paramlist *list;

  retval = NULL;
  node = NULL;
  list = NULL;

  node = ParameterPTreeNodeFind(paramtree, name);

  if(node)
  {
    for(list = node->paramlist; list; list = list->next)
    {
      if(! thorn)
      {
	if(list->param->scope == SCOPE_GLOBAL)
	{
	  break;
	}
      }
      else if(scope == SCOPE_ANY)
      {
	if(thorn && ! STR_CMP(thorn, list->param->thorn)) break;
      }
      else if(!STR_CMP(thorn, list->param->thorn) && list->param->scope == scope)
      {
	break;
      }	  
    }
  }

  if(list)
  {
    retval = list->param;
  }
  else
  {
    retval = NULL;
  }
   
  return retval;
}

 /*@@
   @routine    ParameterNew
   @date       Mon Jul 26 10:59:42 1999
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static t_parameter *ParameterNew(const char *thorn,
				 const char *name,
				 const char *type,
				 const char *scope,
				 int        steerable,
				 const char *description,
				 const char *defval,
				 void       *data)
{
  int retval;
  t_parameter *newparam;
  char ** stringdata;
  
  retval = -1;

  newparam = (t_parameter *)malloc(sizeof(t_parameter));

  if(newparam)
  {
    newparam->thorn = (char *)malloc(sizeof(char)*(1+strlen(thorn)));
    newparam->name = (char *)malloc(sizeof(char)*(1+strlen(name)));
    newparam->scope = ParameterGetScope(scope);
    newparam->type = ParameterGetType(type);
    newparam->steerable=steerable;
    newparam->description = (char *)malloc(sizeof(char)*(1+strlen(description)));
    newparam->defval = (char *)malloc(sizeof(char)*(1+strlen(defval)));
    newparam->data = data;
    newparam->range = NULL;
    
    if(newparam->type == PARAMETER_STRING ||
       newparam->type == PARAMETER_SENTENCE ||
       newparam->type == PARAMETER_KEYWORD)
    {
      stringdata = (char **)data;
      *stringdata = NULL;
    }

    if(newparam->thorn &&
       newparam->name &&
       newparam->description &&
       newparam->defval)
    {
      strcpy(newparam->name, name);
      strcpy(newparam->thorn, thorn);
      strcpy(newparam->description, description);
      strcpy(newparam->defval, defval);

      retval = ParameterInsert(&paramtree, newparam);
    }
  }
    
  return newparam;
}

static int ParameterCheck(const char *thorn,
                          const char *name,
                          const char *type,
                          const char *scope,
                          int        steerable,
                          const char *description,
                          const char *defval,
                          void       *data)
{
  return -2;
}


static t_paramtreenode *ParameterPTreeNodeFind(t_sktree *tree, 
                                               const char *name)
{
  t_sktree *node;
  t_paramtreenode *retval;
  
  node = SKTreeFindNode(tree, name);

  if(node)
  {
    retval = (t_paramtreenode *)(node->data);
  }
  else
  {
    retval = NULL;
  }

  return retval;
  
}

static t_sktree *ParameterPTreeNodeAdd(t_sktree **tree,
				       const char *name,
				       t_paramtreenode *new_node)
{
  t_sktree *root;
  
  root = SKTreeStoreData(*tree, *tree, name, new_node);
  
  if(!*tree)
  {
    *tree = root;
  }

  return root;
}

static int ParameterGetScope(const char *scope)
{
  int retval;

  retval = -1;

  if(!STR_CMP(scope, "GLOBAL"))
  {
    retval = SCOPE_GLOBAL;
  }

  if(!STR_CMP(scope, "RESTRICTED"))
  {
    retval = SCOPE_RESTRICTED;
  }

  if(!STR_CMP(scope, "PRIVATE"))
  {
    retval = SCOPE_PRIVATE;
  }

  return retval;
}

static int ParameterGetType(const char *type)
{
  int retval;

  retval = -1;

#define PTYPE(x)                     \
  if(!STR_CMP(type, #x))             \
  {                                  \
    retval = PARAMETER_##x;          \
  }                                  \


  PTYPE(KEYWORD);
  PTYPE(STRING);
  PTYPE(SENTENCE);
  PTYPE(INT);
  PTYPE(REAL);
  PTYPE(BOOLEAN);

#undef PTYPE
  if(retval == -1)
  {
    fprintf(stderr, "What on earth kind of a parameter is %s ? \n",type);
  }

  return retval;
}

 /*@@
   @routine    ParameterInsert
   @date       Fri Jul 16 10:08:25 1999
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int ParameterInsert(t_sktree **tree, 
			   t_parameter *newparam)
{
  int retval;
  t_sktree *treenode;
  t_paramtreenode *node;
  t_paramlist *list;

  treenode = SKTreeFindNode(*tree, newparam->name);

  if(treenode)
  {
    retval = ParameterListAddParam(&(((t_paramtreenode *)(treenode->data))->paramlist),
			 newparam);
  }
  else
  {
    node = (t_paramtreenode *)malloc(sizeof(t_paramtreenode));
    list = (t_paramlist *)malloc(sizeof(t_paramlist));

    if(node && list)
    {
      node->paramlist=list;
      list->param = newparam;
      list->last=NULL;
      list->next=NULL;
      treenode = SKTreeStoreData(*tree, *tree, newparam->name, node);
      if(!*tree) *tree = treenode;
      retval = 0;
    }
    else
    {
      retval = -1;
      free(list);
      free(node);
    }
  }

  return retval;
}

 /*@@
   @routine    ParameterGetSimple
   @date       Fri Jul 16 10:07:46 1999
   @author     Tom Goodale
   @desc 
   Gets the value of a parameter
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static void *ParameterGetSimple(t_parameter *param, 
				int *type)
{
  *type = param->type;
  return param->data;
}

 /*@@
   @routine    ParameterExtend
   @date       Thu Jul 15 12:55:06 1999
   @author     Tom Goodale
   @desc 
   Adds a range to a parameter.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int ParameterExtend(t_parameter *parameter,  
			   const char *range_origin, 
			   const char *range, 
			   const char *range_description)
{
  int retcode;
  int order;
  t_range *newrange;
  t_range *rangenode;
  t_range *lastnode;

  newrange = (t_range *)malloc(sizeof(t_range));

  if(newrange)
  {
    newrange->range = (char *)malloc(sizeof(char)*(1+strlen(range)));
    newrange->origin = (char *)malloc(sizeof(char)*(1+strlen(range_origin)));
    newrange->description = (char *)malloc(sizeof(char)*(1+strlen(range_description)));
  }

  if(newrange && 
     newrange->range &&
     newrange->origin &&
     newrange->description)
  {
    /* Fill out the data */
    newrange->last = NULL;
    newrange->next = NULL;
    newrange->active = 0;
    strcpy(newrange->range, range);
    strcpy(newrange->origin, range_origin);
    strcpy(newrange->description, range_description);

    lastnode = NULL;

    /* Search the list for the right place to insert it. */
    for(rangenode=parameter->range; rangenode; rangenode=rangenode->next)
    {
      lastnode = rangenode;

      order = STR_CMP(range_origin, rangenode->origin);

      if(order <= 0)
      {
	/* Insert before this node */
	newrange->next=rangenode;
	newrange->last=rangenode->last;
	rangenode->last=newrange;
	if(parameter->range == rangenode) parameter->range=newrange;
	if(newrange->last) newrange->last->next = newrange;
	break;
      }
    }

    if(!rangenode)
    {
      /* Insert at the end of the list */
      newrange->next=NULL;
      newrange->last=lastnode;
      if(parameter->range == NULL) parameter->range=newrange;
      if(newrange->last) newrange->last->next = newrange;
    }

    retcode = 0;
  }
  else
  {
    retcode = -1;
  }

  return retcode;
}

static int ParameterSetSimple(t_parameter *param, const char *value)
{
  int retval;

  retval = -2;

  /*  fprintf(stdout, "Setting value of parameter %s::%s\n", param->thorn, param->name);*/
  switch(param->type)
  {
    case PARAMETER_KEYWORD  :
      retval = ParameterSetKeyword(param, value); break;
    case PARAMETER_STRING   :
      retval = ParameterSetString(param, value); break;
    case PARAMETER_SENTENCE :
      retval = ParameterSetSentence(param, value); break;
    case PARAMETER_INT  :
      retval = ParameterSetInteger(param, value); break;
    case PARAMETER_REAL     :
      retval = ParameterSetReal(param, value); break;
    case PARAMETER_BOOLEAN  :
      retval = ParameterSetBoolean(param, value); break;
    default            :
      fprintf(stderr, "Unknown parameter type %d\n", param->type);
  }

  return retval;
}

static int ParameterPrintSimple(t_parameter *param, 
				const char *format,
				FILE *file)
{
  t_range *range;

  fprintf(file, format, "Parameter", param->name);
  fprintf(file, format, "Thorn", param->thorn);
  fprintf(file, format, "Desc", param->description);
  fprintf(file, format, "Def", param->defval);

  for(range=param->range; range; range=range->next)
  {
    fprintf(file, format, "Range:", range->range);
  }

  return 0;
}

static int ParameterListAddParam(t_paramlist **paramlist,
				 t_parameter *newparam)
{
  int retval;
  t_paramlist *node;
  node = (t_paramlist *)malloc(sizeof(t_paramlist));

  if(node)
  {
    node->param = newparam;
    
    /* Place at beginning of list for now. */
    node->next = *paramlist;
    node->last = NULL;
    (*paramlist)->last = node;

    *paramlist = node;

    retval = 0;
  }
  else
  {
    retval = 1;
  }

  return retval;
}


static int ParameterSetKeyword(t_parameter *param, const char *value)
{
  int retval;
  t_range *range;

  retval = -1;
  for(range = param->range; range ; range = range->next)
  {
    if(CCTK_IsThornActive(range->origin)||CCTK_Equals(param->thorn, range->origin))
    {
      if(!STR_CMP(value, range->range))
      {
	retval = CCTK_SetString(param->data, value);
	break;
      }
    }
  }

  if(retval == -1)
  {
    fprintf(stderr, 
	    "Unable to set keyword %s::%s - %s not in any active range\n", 
	    param->thorn,
	    param->name,
	    value);
    if(*((char **)param->data) == NULL)
    {
      fprintf(stderr, "Since this was the default value, setting anyway - please fix!\n");

      CCTK_SetString(param->data, value);
    }
  }

  return retval;
}

static int ParameterSetString(t_parameter *param, const char *value)
{
  int retval;
  t_range *range;

  retval = -1;
  for(range = param->range; range ; range = range->next)
  {
    if(CCTK_IsThornActive(range->origin)||CCTK_Equals(param->thorn, range->origin))
    {
      if(CCTK_RegexMatch(value, range->range, 0, NULL))
      {
	retval = CCTK_SetString(param->data, value);
      }
      break;
    }
  }

  if(retval == -1)
  {
    fprintf(stderr, 
	    "Unable to set string %s::%s - %s not in any active range\n", 
	    param->thorn,
	    param->name,
	    value);

    if(*((char **)param->data) == NULL)
    {
      fprintf(stderr, "Since this was the default value, setting anyway - please fix!\n");

      CCTK_SetString(param->data, value);
    }
      
  }

  return retval;
}

static int ParameterSetSentence(t_parameter *param, const char *value)
{
  int retval;
  t_range *range;

  retval = -1;
  for(range = param->range; range ; range = range->next)
  {
    if(CCTK_IsThornActive(range->origin)||CCTK_Equals(param->thorn, range->origin))
    {
      if(CCTK_RegexMatch(value, range->range, 0, NULL))
      {
	retval = CCTK_SetString(param->data, value);
      }
      break;
    }
  }

  if(retval == -1)
  {
    fprintf(stderr, 
	    "Unable to set sentence %s::%s - %s not in any active range\n", 
	    param->thorn,
	    param->name,
	    value);

    if(*((char **)param->data) == NULL)
    {
      fprintf(stderr, "Since this was the default value, setting anyway - please fix!\n");

      CCTK_SetString(param->data, value);
    }
  }

  return retval;
}

static int ParameterSetInteger(t_parameter *param, const char *value)
{
  int retval;
  t_range *range;
  int inval;
  CCTK_INT *val;

  inval = atoi(value);
  val = (CCTK_INT *)param->data;
  retval = -1;
  for(range = param->range; range ; range = range->next)
  {
    if(CCTK_IsThornActive(range->origin)||CCTK_Equals(param->thorn, range->origin))
    {
      if(Util_IntInRange(inval, range->range))
      {
	*val = inval;
	retval = 0;
      }
      break;
    }
  }

  if(retval == -1)
  {
    fprintf(stderr, 
	    "Unable to set integer %s::%s - %s not in any active range\n", 
	    param->thorn,
	    param->name,
	    value);
  }

  return retval;
}

static int ParameterSetReal(t_parameter *param, const char *value)
{
  int retval;
  t_range *range;
  double inval;
  CCTK_REAL *val;

  inval = atof(value);
  val = (CCTK_REAL *)param->data;
  retval = -1;
  for(range = param->range; range ; range = range->next)
  {
    if(CCTK_IsThornActive(range->origin)||CCTK_Equals(param->thorn, range->origin))
    {
      if(Util_DoubleInRange(inval, range->range)||CCTK_Equals(param->thorn, range->origin))
      {
	*val = inval;
	retval = 0;
      }
      break;
    }
  }

  if(retval == -1)
  {
    fprintf(stderr, 
	    "Unable to set real %s::%s - %s not in any active range\n", 
	    param->thorn,
	    param->name,
	    value);
  }

  return retval;
}

static int ParameterSetBoolean(t_parameter *param, const char *value)
{
  int retval;

  retval = -1;
  retval = CCTK_SetBoolean(param->data, value);

  if(retval == -1)
  {
    fprintf(stderr, 
	    "Unable to set boolean %s::%s - %s not recognised\n", 
	    param->thorn,
	    param->name,
	    value);
  }

  return retval;
}

/*#define TEST_PARAMETERS*/
#ifdef TEST_PARAMETERS

struct 
{
  int a;
  char *foo;
  double beta;
} params;

int main(void)
{
  ParameterCreate("a", "thorn1", "imp1", "int", "global", 0,
		  "The a param", "2", &(params.a));

  ParameterCreate("foo", "thorn2", "imp2", "keyword", "private", 0,
		  "The foo param", "bingo", &(params.foo));

  printf("Testing thorn,null\n");

  ParameterPrintDescription("a",
			    "thorn1", /*const char *thorn,*/
			    NULL, /* const char *implementation,*/
                            "..%s..%s\n",/*  const char *format,*/
                            stdout);

  printf("Testing null,imp\n");

  ParameterPrintDescription("a",
			    NULL, /*const char *thorn,*/
			    "imp1", /* const char *implementation,*/
                            "..%s..%s\n",/*  const char *format,*/
                            stdout);

  printf("Testing thorn,thorn\n");

  ParameterPrintDescription("a",
			    "thorn1", /*const char *thorn,*/
			    "thorn1", /* const char *implementation,*/
                            "..%s..%s\n",/*  const char *format,*/
                            stdout);

  printf("Testing imp,imp\n");

  ParameterPrintDescription("a",
			    "imp1", /*const char *thorn,*/
			    "imp1", /* const char *implementation,*/
                            "..%s..%s\n",/*  const char *format,*/
                            stdout);

  printf("Testing imp,null\n");
  ParameterPrintDescription("a",
			    "imp1", /*const char *thorn,*/
			    NULL, /* const char *implementation,*/
                            "..%s..%s\n",/*  const char *format,*/
                            stdout);

  printf("Adding a range to a\n");
  ParameterAddRange("imp1", 
                    "a",
                    "imp1",
                    "1:7:0",
                    "A nice range for a");

  printf("Adding another range to a\n");
  ParameterAddRange("imp1", 
                    "a",
                    "imp1",
                    "1:7:1",
                    "Another nice range for a");

  printf("a is now\n");

  ParameterPrintDescription("a",
			    "thorn1", /*const char *thorn,*/
			    NULL, /* const char *implementation,*/
                            "..%s..%s\n",/*  const char *format,*/
                            stdout);

  return 0;
}

#endif
