/*@@
   @file      Parameters.c
   @date      Mon Jun 28 21:44:17 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
 @@*/

const char *rcsid="$Header$";

#include "cctk_Config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "cctk.h"

#include "SKBinTree.h"

#include "cctk_ActiveThorns.h"

#include "ParameterBindings.h"
#include "cctk_ParameterFunctions.h"


#include "gnu_regex.h"

int CCTK_RegexMatch(const char *string, 
                    const char *pattern, 
                    const int nmatch,
                    regmatch_t *pmatch);


int STR_cmpi(const char *string1, const char *string2);

#define STR_CMP(a,b) STR_cmpi(a,b)

/*************************/
/* Structure Definitions */
/*************************/

/* what is a parameter:
 * - properties
 * - data
 * nothing else! 
 * Props are supposed to be public to thorn programmers, 
 * Data are supposed to be private to flesh!
 */
typedef struct PARAM
{
    t_param_prop*       props;
    void*               data;
    
} t_param;



/* what is a list of parameters:
 * just a normal linked list...
 */
typedef struct PARAMLIST
{
    t_param*            param;
    struct PARAMLIST*   last;
    struct PARAMLIST*   next;
    
} t_paramlist;



/* what is a node of parameter tree 
 * look at it: a list of linked lists... ;-)
 */
typedef struct PARAMTREENODE
{
    t_paramlist*        paramlist;
    
} t_paramtreenode;

static t_param *ParameterFind(const char *name, 
                              const char *thorn, 
                              int scope);

static t_param *ParameterNew(const char *thorn,
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

static void *ParameterGetSimple (t_param *param, 
                                 int *type);

static int ParameterSetSimple   (t_param *param, const char *value);

static t_paramtreenode *ParameterPTreeNodeFind(t_sktree *tree, 
                                               const char *name);

static t_sktree *ParameterPTreeNodeAdd(t_sktree **tree,
                                       const char *name,
                                       t_paramtreenode *new_node);

static int ParameterGetScope    (const char *scope);
static int ParameterGetType     (const char *type);

static int ParameterInsert      (t_sktree **tree, 
                                 t_param *newparam);

static int ParameterPrintSimple (t_param *param, 
                                 const char *format,
                                 FILE *file);

static int ParameterExtend      (t_param *param,  
                                 const char *range_origin, 
                                 const char *range, 
                                 const char *range_description);

static int ParameterListAddParam(t_paramlist **paramlist,
                                 t_param *newparam);


static int ParameterSetKeyword  (t_param *param, const char *value);
static int ParameterSetString   (t_param *param, const char *value);
static int ParameterSetSentence (t_param *param, const char *value);
static int ParameterSetInteger  (t_param *param, const char *value);
static int ParameterSetReal     (t_param *param, const char *value);
static int ParameterSetBoolean  (t_param *param, const char *value);

static t_sktree *paramtree=NULL;

/*@@
   @routine    CCTKi_ParameterCreate
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
int CCTKi_ParameterCreate(const char *name,
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
  t_param *param;
  va_list ranges;
  const char *rangeval;
  const char *rangedesc;

  iscope = ParameterGetScope(scope);

  param = ParameterFind(name, thorn, iscope);

  if(!param)
  {
    param = ParameterNew(thorn, name, 
                         type, scope, steerable, description, defval, data);

    if(n_ranges)
    {
      va_start(ranges, n_ranges);
      
      for(i=0; i < n_ranges; i++)
      {
        rangeval = (const char *)va_arg(ranges, const char *);
        rangedesc = (const char *)va_arg(ranges, const char *);
  
        ParameterExtend(param, thorn, rangeval, rangedesc);
      }
      va_end(ranges);

    }

    retval = ParameterSetSimple(param, defval);

  }
  else
  {
    retval = ParameterCheck(thorn, name, 
                            type, scope, steerable, description, defval, data);
  }

  return retval;
}

/*@@
   @routine    CCTKi_ParameterAddRange
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
int CCTKi_ParameterAddRange(const char *implementation, 
                            const char *name,
                            const char *range_origin,
                            const char *range,
                            const char *range_description)
{
  int retval;
  t_param *param;

  /* For the moment do this in the quick and dirty way 8-(  FIXME */
  t_sktree *thornlist;

  t_sktree *node;

  /*printf("Extending param %s::%s from thorn %s\n", implementation, name, range_origin);*/

  thornlist = CCTK_ImpThornList(implementation);

  retval = -1;

  if(thornlist)
  {
    for(node= SKTreeFindFirst(thornlist);
        node; 
        node = node->next)
    {
      param = ParameterFind(name, node->key, SCOPE_RESTRICTED);
      
      if(param)
      {
        retval = ParameterExtend(param,  range_origin, range, range_description);
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

int CCTK_ParameterSet(const char *name,
                      const char *thorn,
                      const char *value)
{
  int retval;
  t_param *param;

  param = ParameterFind(name, thorn, SCOPE_ANY);

  if(param)
  {
    retval = ParameterSetSimple(param, value);

    /* register another set operation */
    param->props->n_set++;

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
  t_param *param;

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
   @routine    CCTK_ParameterGet
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
void *CCTK_ParameterGet(const char *name,
                        const char *thorn,
                        int *type)
{
  void *retval;
  t_param *param;

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
   @routine    CCTK_ParameterValString
   @date       Thu Jan 21 2000
   @author     Thomas Radke
   @desc 
   Gets the string representation of a parameter's value
   - should be used for checkpointing and recovery.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     param_name
   @vdesc   The name of the parameter
   @vtype   const char *
   @vio     in
   @endvar
   @var     thorn
   @vdesc   The originating thorn
   @vtype   const char *
   @vio     in
   @endvar 
   @returntype char *
   @returndesc
   The address of the allocated string (should be freed after usage).
   @endreturndesc

@@*/
char *CCTK_ParameterValString (const char *param_name,
                               const char *thorn)
{
  int param_type;
  void *param_data;
  char *retval = NULL;
  char buffer [80];

  param_data = CCTK_ParameterGet (param_name, thorn, &param_type);
  if (param_data == NULL) return (NULL);

  switch (param_type)
  {
    case PARAMETER_KEYWORD:
    case PARAMETER_STRING:
    case PARAMETER_SENTENCE:
      retval = strdup (*(char **) param_data);
      break;

    case PARAMETER_BOOLEAN:
      retval = strdup ((int) (*(CCTK_INT *) param_data) ? "yes" : "no");
      break;

    case PARAMETER_INT:
      sprintf (buffer, "%d", (int) (*(CCTK_INT *) param_data));
      retval = strdup (buffer);
      break;

    case PARAMETER_REAL:
      sprintf (buffer, "%e", (double) (*(CCTK_REAL *) param_data));
      retval = strdup (buffer);
      break;

    default:
      CCTK_VWarn (3, __LINE__, __FILE__, CCTK_THORNSTRING,
                  "Unknown type %d for parameter '%s::%s'",
                  param_type, thorn, param_name);
      retval = NULL;
      break;
  }

  return (retval);
}


/*@@
   @routine    CCTK_ParameterWalk
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
const char *CCTK_ParameterWalk(int first,
                          const char *origin)
{
  int             return_found;
  t_sktree        *tnode;
  t_paramtreenode *node;
  t_paramlist     *paramlist;
  t_param         *startpoint;
  static t_param  *prev_startpoint_all = NULL;
  static t_param  *prev_startpoint_thorn = NULL;


  /* determine the startpoint for search */
  if (! first)
  {
    startpoint = origin ? prev_startpoint_thorn : prev_startpoint_all;

    if (startpoint == NULL)
    {
      CCTK_Warn(2,__LINE__,__FILE__,"Cactus", "CCTK_ParameterWalk: Cannot walk "
                "through parameter list without setting a startpoint at first");
      return NULL;
    }
  }
  else
  {
    startpoint = NULL;
  }
 
  /* say whether the startpoint should be returned (if found)
     or the next matching parameter */
  return_found = startpoint == NULL;


  /* begin the search */
  tnode = SKTreeFindFirst (paramtree);

  /* iterate over nodes */
  for ( ; tnode ; tnode = tnode->next) 
  {  
    /* get data and parameter paramlist */
    node  = (t_paramtreenode *) tnode->data;
    paramlist = node->paramlist;

    /* if startpoint is still unassigned set it to first parameter in list */
    if (paramlist && startpoint == NULL)
    {
      if (! origin || CCTK_Equals (origin, paramlist->param->props->thorn))
        startpoint = paramlist->param;
    }

    /* iterate over parameters in list */
    for ( ; paramlist; paramlist = paramlist->next)
    {

      /* Hey, we've found the startpoint ! */
      if (startpoint == paramlist->param)
      {

        /* Do we have to return this one ? 
           If not prepare finding the next matching param. */
        if (return_found)
        {
          char *retval;
          const char *implementation;

          implementation = CCTK_ThornImplementation (startpoint->props->thorn);
          retval = (char *) malloc (strlen (implementation) +
                                    strlen (startpoint->props->name) + 3);
          sprintf (retval, "%s::%s", implementation, startpoint->props->name);

          /* save the last startpoint */
          prev_startpoint_all = prev_startpoint_thorn = startpoint;

          return (const char *) retval;

        }
        else
        {
          startpoint = NULL;
          return_found = 1;
        }
      }
    } /* end looping over parameter list */
  } /* end looping over all nodes */

  return NULL;
}

/**********************************************************************/ 
/*@@
  @routine    CCTK_ParameterList
  @date       Mon Aug 30 17:16:58 MSZ 1999
  @author     Andre Merzky
  @desc 
  stores names of all parameters of given thorn into char** *list
  @enddesc 
  @calls
  @calledby   
  @history 
  @endhistory 
 
  @var     thorn
  @vdesc   thorn to get parameter names for
  @vtype   const char *
  @vio     in
  @vcomment 
  @endvar 
 
  @var     paramlist
  @vdesc   list to store parameter names into
  @vtype   char ***
  @vio     out
  @vcomment 
  @endvar 
 
  @var     n_param
  @vdesc   integer to store numbr of found parameters into
  @vtype   int *
  @vio     out
  @vcomment 
  @endvar 
 
  @returntype int
  @returndesc
  number of parameters found, <0 on failure
  @endreturndesc

  @@*/
int CCTK_ParameterList (const char *thorn, char ***paramlist, int *n_param)
{
  t_paramtreenode *node;
  t_sktree        *tnode;
  int              alloc_size;
    
  t_paramlist *tmp_paramlist;
    
  node    = NULL;

  *n_param = 0;

  /* FIXME */
#define _MY_PARAM_JUNK_SIZE  100
    
  alloc_size = _MY_PARAM_JUNK_SIZE;
  *paramlist = (char **) malloc (sizeof (char *) * alloc_size);
    
  if (! *paramlist) 
  {
    fprintf (stderr, "Cannot malloc paramlist* at line %d of %s\n", __LINE__, __FILE__);
    return (-1);
  }
    
  /* ok, get first node in sktree */
  tnode = SKTreeFindFirst (paramtree);
    
  /* iterate over nodes */
  for ( ; tnode ; tnode = tnode->next) 
  {  
    /* get data and parameter paramlist */
    node  = (t_paramtreenode *)(tnode->data);
    tmp_paramlist = node->paramlist;
  
    /* iterate over parameters */
    for ( ; tmp_paramlist ; tmp_paramlist = tmp_paramlist->next)
    {   
      /* is it a parameter of the given thorn? */
      if (!STR_CMP (thorn, tmp_paramlist->param->props->thorn)) 
      {  
        /* list long enough? */
        if ((*n_param) >= alloc_size)
        {
          alloc_size += _MY_PARAM_JUNK_SIZE;
          *paramlist = (char **) realloc (*paramlist, sizeof(char *)*alloc_size);
        
          if (! *paramlist) 
          {
            fprintf (stderr, "Cannot realloc paramlist* at line %d of %s\n", __LINE__, __FILE__);
            return (-1);
          }
        } 

        /* malloc actual string in paramlist */
        (*paramlist)[(*n_param)] = (char *) malloc 
          (strlen (tmp_paramlist->param->props->name) + 1);

        /* got memory? */    
        if (! (*paramlist)[(*n_param)]) 
        {
          fprintf (stderr, "Cannot malloc paramlist*[%d]at line %d of %s\n", 
                   (*n_param), __LINE__, __FILE__);
          return (-1);
        }
    
        /* save parameter name */
        strcpy ((*paramlist)[(*n_param)], tmp_paramlist->param->props->name);
    
        /* increase counter */
        (*n_param)++;
      }
    }
  }
    
  /* if necessary, shrink paramlist again. */
  if ((*n_param) < alloc_size)
  {
    alloc_size = (*n_param);
    *paramlist = (char **) realloc ((*paramlist), sizeof(char *)*alloc_size);

    if (*n_param && ! (*paramlist)) 
    {
      fprintf (stderr, "Cannot realloc paramlist* at line %d of %s\n", __LINE__, __FILE__);
      fprintf(stderr, "n_param is %d\n", *n_param);
      return (-1);
    }
  } 
    
  /* done */
  return (*n_param);
}


/**********************************************************************/ 
/*@@
  @routine    CCTK_ParameterInfo
  @date       Tue Aug 31 18:10:46 MSZ 1999
  @author     Andre Merzky
  @desc 
  For a given parameter, return description, type and range
  @enddesc 
  @calls
  @calledby   
  @history 
  @endhistory 
 
  @var     name
  @vdesc   parameter name
  @vtype   const char *
  @vio     in
  @vcomment 
  @endvar 
 
  @var     thorn
  @vdesc   thorn parameter belongs to
  @vtype   const char *
  @vio     in
  @vcomment 
  @endvar 
 
  @var     param_prop
  @vdesc   parameter descriptions
  @vtype   t_param_prop *
  @vio     out
  @vcomment 
  @endvar 
   
  @returntype int
  @returndesc
  1 on success, 0 on failure.
  @endreturndesc

  @@*/
t_param_prop *CCTK_ParameterInfo (const char *name, 
                                  const char *thorn) 
{
  t_param_prop *retval;

  t_param *param;

  param = ParameterFind (name, thorn, SCOPE_ANY);

  if (!param) 
  {
    retval = NULL;
  } 
  else 
  {
    retval = param->props;
  }

  return retval;
}


 /*@@
   @routine    ParameterFind
   @date       Sun Oct 17 16:20:48 1999
   @author     Tom Goodale
   @desc 
   Finds a parameter.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static t_param *ParameterFind(const char *name, 
                              const char *thorn, 
                              int scope)
{
  t_param *retval;
  t_paramtreenode *node;
  t_paramlist *list;

  retval = NULL;
  node = NULL;
  list = NULL;

  node = ParameterPTreeNodeFind (paramtree, name);

  if(node)
  {
    for(list = node->paramlist; list; list = list->next)
    {
      if(! thorn)
      {
        if(list->param->props->scope == SCOPE_GLOBAL)
        {
          break;
        }
      }
      else if(scope == SCOPE_ANY)
      {
        if(thorn && ! STR_CMP(thorn, list->param->props->thorn)) break;
      }
      else if(!STR_CMP(thorn, list->param->props->thorn) && list->param->props->scope == scope)
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
   Creates a new parameter
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static t_param *ParameterNew(const char *thorn,
                             const char *name,
                             const char *type,
                             const char *scope,
                             int        steerable,
                             const char *description,
                             const char *defval,
                             void       *data)
{
  int retval;
  t_param *newparam;
  char ** stringdata;
  
  retval = -1;

  newparam = (t_param *)malloc(sizeof(t_param));

  if(newparam)
  {
    newparam->props = (t_param_prop*) malloc (sizeof (t_param_prop));
      
    if (newparam->props) 
    {
    
      newparam->props->thorn       = (char*) malloc (sizeof (char) * (1 + strlen (thorn      )));
      newparam->props->name       = (char*) malloc (sizeof (char) * (1 + strlen (name       )));
      newparam->props->description    = (char*) malloc (sizeof (char) * (1 + strlen (description)));
      newparam->props->defval       = (char*) malloc (sizeof (char) * (1 + strlen (defval     )));
      newparam->props->scope       = ParameterGetScope(scope);
      newparam->props->type       = ParameterGetType(type);
      newparam->props->steerable      = steerable;
      newparam->props->range      = NULL;
      newparam->props->n_set      = 0;
    
      newparam->data         = data;
    
      if(newparam->props->type == PARAMETER_STRING ||
         newparam->props->type == PARAMETER_SENTENCE ||
         newparam->props->type == PARAMETER_KEYWORD)
      {
        stringdata = (char **)data;
        *stringdata = NULL;
      }
    
      if(newparam->props->thorn &&
         newparam->props->name &&
         newparam->props->description &&
         newparam->props->defval)
      {
        strcpy(newparam->props->name, name);
        strcpy(newparam->props->thorn, thorn);
        strcpy(newparam->props->description, description);
        strcpy(newparam->props->defval, defval);
        
        retval = ParameterInsert(&paramtree, newparam);
      }
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
                           t_param *newparam)
{
  int retval;
  t_sktree *treenode;
  t_paramtreenode *node;
  t_paramlist *list;

  treenode = SKTreeFindNode(*tree, newparam->props->name);

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
      treenode = SKTreeStoreData(*tree, *tree, newparam->props->name, node);
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
static void *ParameterGetSimple(t_param *param, 
                                int *type)
{
  *type = param->props->type;
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
static int ParameterExtend(t_param *param,  
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
    for(rangenode=param->props->range; rangenode; rangenode=rangenode->next)
    {
      lastnode = rangenode;

      order = STR_CMP(range_origin, rangenode->origin);

      if(order <= 0)
      {
        /* Insert before this node */
        newrange->next=rangenode;
        newrange->last=rangenode->last;
        rangenode->last=newrange;
        if(param->props->range == rangenode) param->props->range=newrange;
        if(newrange->last) newrange->last->next = newrange;
        break;
      }
    }

    if(!rangenode)
    {
      /* Insert at the end of the list */
      newrange->next=NULL;
      newrange->last=lastnode;
      if(param->props->range == NULL) param->props->range=newrange;
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

static int ParameterSetSimple(t_param *param, const char *value)
{
  int retval;

  retval = -2;

  /*  fprintf(stdout, "Setting value of parameter %s::%s\n", param->props->thorn, param->props->name);*/
  switch(param->props->type)
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
      fprintf(stderr, "Unknown parameter type %d\n", param->props->type);
  }

  return retval;
}

static int ParameterPrintSimple(t_param *param, 
                                const char *format,
                                FILE *file)
{
  t_range *range;

  fprintf(file, format, "Parameter", param->props->name);
  fprintf(file, format, "Thorn", param->props->thorn);
  fprintf(file, format, "Desc", param->props->description);
  fprintf(file, format, "Def", param->props->defval);

  for(range=param->props->range; range; range=range->next)
  {
    fprintf(file, format, "Range:", range->range);
  }

  return 0;
}

static int ParameterListAddParam(t_paramlist **paramlist,
                                 t_param *newparam)
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


static int ParameterSetKeyword(t_param *param, const char *value)
{
  int retval;
  t_range *range;

  retval = -1;
  for(range = param->props->range; range ; range = range->next)
  {
    if(CCTK_IsThornActive(range->origin)||CCTK_Equals(param->props->thorn, range->origin))
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
    char *msg;
    msg = (char *)malloc( 200*sizeof(char) );
    sprintf(msg,"Unable to set keyword %s::%s - %s not in any active range",
            param->props->thorn,
            param->props->name,
            value);
    CCTK_Warn(0,__LINE__,__FILE__,"Cactus",msg);
    free(msg);
    if(*((char **)param->data) == NULL)
    {
      fprintf(stderr, "Since this was the default value, setting anyway - please fix!\n");

      CCTK_SetString(param->data, value);
    }
  }

  return retval;
}

static int ParameterSetString(t_param *param, const char *value)
{
  int retval;
  t_range *range;

  retval = -1;
  for(range = param->props->range; range ; range = range->next)
  {
    if(CCTK_IsThornActive(range->origin)||CCTK_Equals(param->props->thorn, range->origin))
    {
#ifndef CCTK_PARAMUNCHECKED
      if(CCTK_RegexMatch(value, range->range, 0, NULL))
      {
#endif
        retval = CCTK_SetString(param->data, value);
#ifndef CCTK_PARAMUNCHECKED
      }
#endif
      break;
    }
  }

  if(retval == -1)
  {
    char *msg;
    msg = (char *)malloc( 200*sizeof(char) );
    sprintf(msg,"Unable to set string %s::%s - %s not in any active range",
            param->props->thorn,
            param->props->name,
            value);
    CCTK_Warn(0,__LINE__,__FILE__,"Cactus",msg);
    free(msg);

    if(*((char **)param->data) == NULL)
    {
      fprintf(stderr, "Since this was the default value, setting anyway - please fix!\n");

      CCTK_SetString(param->data, value);
    }
      
  }

  return retval;
}

static int ParameterSetSentence(t_param *param, const char *value)
{
  int retval;
  t_range *range;

  retval = -1;
  for(range = param->props->range; range ; range = range->next)
  {
    if(CCTK_IsThornActive(range->origin)||CCTK_Equals(param->props->thorn, range->origin))
    {
#ifndef CCTK_PARAMUNCHECKED
      if(CCTK_RegexMatch(value, range->range, 0, NULL))
      {
#endif
        retval = CCTK_SetString(param->data, value);
#ifndef CCTK_PARAMUNCHECKED
      }
#endif
      break;
    }
  }

  if(retval == -1)
  {

    char *msg;
    msg = (char *)malloc( 200*sizeof(char) );
    sprintf(msg,"Unable to set sentance %s::%s - %s not in any active range",
            param->props->thorn,
            param->props->name,
            value);
    CCTK_Warn(0,__LINE__,__FILE__,"Cactus",msg);
    free(msg);

    if(*((char **)param->data) == NULL)
    {
      fprintf(stderr, "Since this was the default value, setting anyway - please fix!\n");

      CCTK_SetString(param->data, value);
    }
  }

  return retval;
}

static int ParameterSetInteger(t_param *param, const char *value)
{
  int retval;
  t_range *range;
  int inval;
  CCTK_INT *val;

  inval = atoi(value);
  val = (CCTK_INT *)param->data;
  retval = -1;
  for(range = param->props->range; range ; range = range->next)
  {
    if(CCTK_IsThornActive(range->origin)||CCTK_Equals(param->props->thorn, range->origin))
    {
#ifndef CCTK_PARAMUNCHECKED
      if(Util_IntInRange(inval, range->range))
      {
#endif
        *val = inval;
        retval = 0;
#ifndef CCTK_PARAMUNCHECKED
      }
#endif
      break;
    }
  }

  if(retval == -1)
  {
    char *msg;
    msg = (char *)malloc( 200*sizeof(char) );
    sprintf(msg,"Unable to set integer %s::%s - %s not in any active range",
            param->props->thorn,
            param->props->name,
            value);
    CCTK_Warn(0,__LINE__,__FILE__,"Cactus",msg);
    free(msg);

  }

  return retval;
}



static int ParameterSetReal(t_param *param, const char *value)
{
  int retval,p;
  t_range *range;
  double inval;
  CCTK_REAL *val;
  char temp[1001];

  /*  Convert the value string to a double. Allow various formats.*/
  strncpy(temp, value, 1000);
  for (p=0;p<strlen(temp);p++) 
  {
    if (temp[p] == 'E' || 
        temp[p] == 'd' || 
        temp[p] == 'D') 
    {
      temp[p] = 'e';
      break;
    }
  }
  inval = atof(temp);

  val = (CCTK_REAL *)param->data;
  retval = -1;
  for(range = param->props->range; range ; range = range->next)
  {
    if(CCTK_IsThornActive(range->origin)||CCTK_Equals(param->props->thorn, range->origin))
    {
#ifndef CCTK_PARAMUNCHECKED
      if(Util_DoubleInRange(inval, range->range))
      {
#endif
        *val = inval;
        retval = 0;
#ifndef CCTK_PARAMUNCHECKED
      }
#endif
      break;
    }
  }

  if(retval == -1)
  {
    char *msg;
    msg = (char *)malloc( 200*sizeof(char) );
    sprintf(msg,"Unable to set real %s::%s - %s not in any active range",
            param->props->thorn,
            param->props->name,
            value);
    CCTK_Warn(0,__LINE__,__FILE__,"Cactus",msg);
    free(msg);

  }

  return retval;
}

static int ParameterSetBoolean(t_param *param, const char *value)
{
  int retval;

  retval = -1;
  retval = CCTK_SetBoolean(param->data, value);

  if(retval == -1)
  {
    char *msg;
    msg = (char *)malloc( 200*sizeof(char) );
    sprintf(msg,"Unable to set boolean %s::%s - %s not recognised",
            param->props->thorn,
            param->props->name,
            value);
    CCTK_Warn(0,__LINE__,__FILE__,"Cactus",msg);
    free(msg);
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
  CCTKi_ParameterCreate("a", "thorn1", "imp1", "int", "global", 0,
                        "The a param", "2", &(params.a));

  CCTKi_ParameterCreate("foo", "thorn2", "imp2", "keyword", "private", 0,
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
