/*@@
   @file      Parameters.c
   @date      Mon Jun 28 21:44:17 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ParameterBindings.h"

#include "SKBinTree.h"

const char *rcsid="$Header$";

int STR_cmpi(const char *string1, const char *string2);

#define STR_CMP(a,b) STR_cmpi(a,b)

#define SCOPE_GLOBAL        1
#define SCOPE_RESTRICTED    2
#define SCOPE_PRIVATE       3
#define SCOPE_NOT_GLOBAL    4

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
  char *implementation;
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
                                  const char *implementation, 
                                  int scope);

static int ParameterNew(const char *thorn,
                        const char *implementation,
                        const char *name,
                        const char *type,
                        const char *scope,
                        int        steerable,
                        const char *description,
                        const char *defval,
                        void       *data);

static int ParameterCheck(const char *thorn,
                          const char *implementation,
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
   @var     implementation
   @vdesc   The originating implementation
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
                    const char *implementation,
                    const char *type,
                    const char *scope,
                    int        steerable,
                    const char *description,
                    const char *defval,
                    void       *data)
{
  int retval;
  int iscope;
  t_parameter *parameter;

  iscope = ParameterGetScope(scope);

  parameter = ParameterFind(name, thorn, implementation, iscope);

  if(!parameter)
  {
    retval = ParameterNew(thorn, implementation, name, 
                          type, scope, steerable, description, defval, data);
  }
  else
  {
    retval = ParameterCheck(thorn, implementation, name, 
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
int ParameterAddRange(const char *origin, 
                      const char *name,
                      const char *range_origin,
                      const char *range,
                      const char *range_description)
{
  int retval;
  t_parameter *parameter;

  if(!STR_CMP(origin, range_origin))
  {
    parameter = ParameterFind(name, origin, NULL, SCOPE_PRIVATE);
    if(!parameter) parameter = ParameterFind(name, origin, NULL, SCOPE_RESTRICTED);
    if(!parameter) parameter = ParameterFind(name, origin, NULL, SCOPE_GLOBAL);
  }
  else
  {
    parameter = ParameterFind(name, NULL, origin, SCOPE_RESTRICTED);
  }

  if(parameter)
  {
    retval = ParameterExtend(parameter,  range_origin, range, range_description);
  }
  else
  {
    retval = -1;
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
		 const char *implementation,
                 const char *value)
{
  int retval;
  int iscope;
  t_parameter *param;
  t_parameter *param2;

  param = NULL;
  param2 = NULL;

  /* See what info we have */
  if(!thorn && !implementation)
  {
    /* No info, so must be a global parameter. */
    iscope = SCOPE_GLOBAL;
    param = ParameterFind(name, NULL, NULL, iscope);
  }
  else
  {
    iscope = SCOPE_NOT_GLOBAL;
    if(thorn && implementation)
    {
      /* Have both thorn and implementation so check both. */
      param = ParameterFind(name, NULL, implementation, iscope);
      param2 = ParameterFind(name, thorn, NULL, iscope);
      if(param2 && ! param)
      {
	/* Only one found, so put into primary setting mode */
	param = param2;
      }
    }
    else
    {
      /* Only have one of thorn or imp, so look for it */
      param = ParameterFind(name, thorn, implementation, iscope);
    }
  }

  if(param)
  {
    retval = ParameterSetSimple(param, value);
  }
  else
  {
    retval = -1;
  }

  /* Should produce a warning for this case. */
  if(param2)
  {
    retval = ParameterSetSimple(param2, value);
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
			      const char *implementation,
                              const char *format,
                              FILE *file)
{
  int retval;
  int iscope;
  t_parameter *param;
  t_parameter *param2;

  param = NULL;
  param2 = NULL;

  /* See what info we have */
  if(!thorn && !implementation)
  {
    /* No info, so must be a global parameter. */
    iscope = SCOPE_GLOBAL;
    param = ParameterFind(name, NULL, NULL, iscope);
  }
  else
  {
    iscope = SCOPE_NOT_GLOBAL;
    if(thorn && implementation)
    {
      /* Have both thorn and implementation so check both. */
      param = ParameterFind(name, NULL, implementation, iscope);
      param2 = ParameterFind(name, thorn, NULL, iscope);
      if(param2 && ! param)
      {
	/* Only one found, so put into primary setting mode */
	param = param2;
      }
    }
    else
    {
      /* Only have one of thorn or imp, so look for it */
      param = ParameterFind(name, thorn, implementation, iscope);
    }
  }

  if(param)
  {
    retval = ParameterPrintSimple(param, format, file);
  }
  else
  {
    retval = -1;
  }

  /* Should produce a warning for this case. */
  if(param2)
  {
    retval = ParameterPrintSimple(param2, format, file);
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
		   const char *implementation,
		   int *type)
{
  void *retval;
  int iscope;
  t_parameter *param;
  t_parameter *param2;

  param = NULL;
  param2 = NULL;

  /* See what info we have */
  if(!thorn && !implementation)
  {
    /* No info, so must be a global parameter. */
    iscope = SCOPE_GLOBAL;
    param = ParameterFind(name, NULL, NULL, iscope);
  }
  else
  {
    iscope = SCOPE_NOT_GLOBAL;
    if(thorn && implementation)
    {
      /* Have both thorn and implementation so check both. */
      param = ParameterFind(name, NULL, implementation, iscope);
      param2 = ParameterFind(name, thorn, NULL, iscope);
      if(param2 && ! param)
      {
	/* Only one found, so put into primary setting mode */
	param = param2;
      }
    }
    else
    {
      /* Only have one of thorn or imp, so look for it */
      param = ParameterFind(name, thorn, implementation, iscope);
    }
  }

  if(param)
  {
    retval = ParameterGetSimple(param, type);
  }
  else
  {
    retval = NULL;
  }

  if(param2)
  {
    fprintf(stderr, "Ambiguous parameter name %s\n", name);
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
                                  const char *implementation, 
                                  int scope)
{
  t_parameter *retval;
  t_paramtreenode *node;
  t_paramlist *list;

  retval = NULL;
  node = NULL;
  list = NULL;

  if(scope == SCOPE_NOT_GLOBAL && thorn && implementation)
  {
    fprintf(stderr, "Calling error\n");
    retval = NULL;
  }
  else
  {
    node = ParameterPTreeNodeFind(paramtree, name);

    if(node)
    {
      for(list = node->paramlist; list; list = list->next)
      {
	if(scope == SCOPE_GLOBAL ) 
	{
	  if(scope == list->param->scope)
	  {
	    break;
	  }	    
	}
	else if(scope == SCOPE_NOT_GLOBAL)
	{
	  if(thorn && ! STR_CMP(thorn, list->param->thorn)) break;
	  if(implementation && ! STR_CMP(implementation, list->param->implementation)) break;
	}
	else if(scope == SCOPE_RESTRICTED)
	{
	  if(implementation && ! STR_CMP(implementation, list->param->implementation) && scope == list->param->scope) break;
	}	  
	else
	{
	  if(thorn && ! STR_CMP(thorn, list->param->thorn) && scope == list->param->scope) break;
	}	  
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

static int ParameterNew(const char *thorn,
                        const char *implementation,
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
  
  retval = -1;

  newparam = (t_parameter *)malloc(sizeof(t_parameter));

  if(newparam)
  {
    newparam->thorn = (char *)malloc(sizeof(char)*(1+strlen(thorn)));
    newparam->implementation = (char *)malloc(sizeof(char)*(1+strlen(implementation)));
    newparam->name = (char *)malloc(sizeof(char)*(1+strlen(name)));
    newparam->scope = ParameterGetScope(scope);
    newparam->type = ParameterGetType(type);
    newparam->steerable=steerable;
    newparam->description = (char *)malloc(sizeof(char)*(1+strlen(description)));
    newparam->defval = (char *)malloc(sizeof(char)*(1+strlen(defval)));
    newparam->data = data;

    if(newparam->thorn &&
       newparam->implementation &&
       newparam->name &&
       newparam->description &&
       newparam->defval)
    {
      strcpy(newparam->name, name);
      strcpy(newparam->thorn, thorn);
      strcpy(newparam->implementation, implementation);
      strcpy(newparam->description, description);
      strcpy(newparam->defval, defval);

      retval = ParameterInsert(&paramtree, newparam);
    }
  }
    
  return retval;
}

static int ParameterCheck(const char *thorn,
                          const char *implementation,
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
  PTYPE(INTEGER);
  PTYPE(REAL);
  PTYPE(LOGICAL);

#undef PTYPE

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
  switch(param->type)
  {
    case PARAMETER_KEYWORD  :
    case PARAMETER_STRING   :
    case PARAMETER_SENTENCE :
    case PARAMETER_INTEGER  :
    case PARAMETER_REAL     :
    case PARAMETER_LOGICAL  :
      fprintf(stderr, "Setting value of parameter %s::%s", param->thorn, param->name);
    default            :
      fprintf(stderr, "Unknown parameter type %d\n", param->type);
  }

  return 0;
}

static int ParameterPrintSimple(t_parameter *param, 
				const char *format,
				FILE *file)
{
  t_range *range;

  fprintf(file, format, "Parameter", param->name);
  fprintf(file, format, "Thorn", param->thorn);
  fprintf(file, format, "Imp", param->implementation);
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
  return 0;
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
