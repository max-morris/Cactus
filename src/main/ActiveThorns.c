 /*@@
   @file      ActiveThorns.c
   @date      Sun Jul  4 16:15:36 1999
   @author    Tom Goodale
   @desc 
   Stuff to deal with activethorns.
   @enddesc 
   @version $Header$
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SKBinTree.h"

#include "cctk_Flesh.h"
#include "cctk_Config.h"
#include "cctk_ActiveThorns.h"
#include "cctk_FortranString.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_ActiveThorns_c)

/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/

struct THORN
{
  int active;
  char *implementation;
};

struct IMPLEMENTATION
{
  int active;
  t_sktree *thornlist;
  char *activating_thorn;
};

/********************************************************************
 ********************* Local Routine Prototypes *********************
 ********************************************************************/

static int RegisterImp(const char *name, const char *thorn);

/********************************************************************
 ********************* Other Routine Prototypes *********************
 ********************************************************************/

/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/

static t_sktree *thornlist = NULL;
static t_sktree *implist   = NULL;

static int n_thorns = 0;
static int n_imps   = 0;

/********************************************************************
 *********************     External Routines   **********************
 ********************************************************************/

 /*@@
   @routine    CCTKi_RegisterThorn
   @date       Sun Jul  4 17:44:14 1999
   @author     Tom Goodale
   @desc 
   Registers a thorn with the flesh.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     name
   @vdesc   Thorn name
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     imp
   @vdesc   Implementation name
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
    0 - success
   -1 - Duplicate thorn
   -2 - memory failure storing thorn
   -3 - memory failure storing implementation name
   -4 - failed to store thorn in tree
   @endreturndesc
@@*/
int CCTKi_RegisterThorn(const char *name, const char *imp)
{
  int retval;
  t_sktree *node;
  t_sktree *temp;

  struct THORN *thorn;

  /*  printf("Registering thorn %s, which provides %s\n", name, imp);*/

  /* Does the thorn already exist ? */
  node = SKTreeFindNode(thornlist, name);

  if(!node)
  {
    n_thorns++;
    /* Create the structure to hold thorn info. */
    thorn = (struct THORN *)malloc(sizeof(struct THORN));

    if(thorn)
    {
      thorn->implementation = (char *)malloc(sizeof(char)*(strlen(imp)+1));

      if(thorn->implementation)
      {
        /* Fill out data for the thorn. */
        strcpy(thorn->implementation, imp);
        thorn->active = 0;

        /* Store the data in the tree */
        temp = SKTreeStoreData(thornlist, thornlist, name, thorn);

        if(!thornlist) 
        {
          thornlist = temp;
        }

        if(temp)
        {

          /* Register the implementation */
          RegisterImp(imp, name);

          retval = 0;
        }
        else
        {
          retval = -4;
        }
      }
      else
      {
        retval = -3;
      }
    }
    else
    {
      retval = -2;
    }
  }
  else
  {
    retval = -1;
  }

  return retval;
}

 /*@@
   @routine    CCTKi_ActivateThorn
   @date       Sun Jul  4 17:46:15 1999
   @author     Tom Goodale
   @desc 
   Activates a thorn and the associated implementation assuming
   the implementation isn't already active.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     name
   @vdesc   Name of thorn to activate
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
    0 - success
   -1 - non-existent thorn
   -2 - internal error
   -3 - thorn already active
   -4 - implementation already active
   @endreturndesc
@@*/
int CCTKi_ActivateThorn(const char *name)
{
  int retval;
  t_sktree *thornnode;
  t_sktree *impnode;
  
  struct THORN *thorn;
  struct IMPLEMENTATION *imp;

  printf("Activating thorn %s...", name);

  /* Find the thorn */
  thornnode = SKTreeFindNode(thornlist, name);

  if(thornnode)
  {
    thorn = (struct THORN *)(thornnode->data);

    /* Find the implementation */
    impnode = SKTreeFindNode(implist, thorn->implementation);

    if(impnode)
    {
      imp = (struct IMPLEMENTATION *)(impnode->data);
          
      if(!thorn->active)
      {
        if(!imp->active)
        {
          /* Activate the thorn. */
          printf("Success -> active implementation %s\n", thorn->implementation);
          thorn->active = 1;
          imp->active = 1;
          /* Remember which thorn activated this imp. */
          imp->activating_thorn = (char *)malloc(sizeof(char)*(strlen(name)+1));
          strcpy(imp->activating_thorn, name);
          retval = 0;
        }
        else
        {
          printf("Failure -> Implementation %s already activated by %s\n", thorn->implementation, imp->activating_thorn);
          retval = -4;
        }
      }
      else
      {
        printf("Failure -> Thorn %s already active\n", name);
        retval = -3;
      }
    }
    else
    {
      printf("Internal error - can't find imp %s from thorn %s\n", thorn->implementation, name);
      retval = -2;
    }
  }
  else
  {
    printf("Failure -> non-existent thorn.\n");
    retval = -1;
  }

  return retval;

}


/*@@
   @routine    CCTK_IsThornActive
   @date       Sun Jul  4 17:46:56 1999
   @author     Tom Goodale
   @desc 
   Checks if a thorn is active.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     name
   @vdesc   Name of thorn
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - inactive
   1 - active
   @endreturndesc
@@*/
int CCTK_IsThornActive(const char *name)
{
  int retval;
  t_sktree *node;
  
  struct THORN *thorn;

  /* Find the thorn */
  node = SKTreeFindNode(thornlist, name);

  retval = 0;

  if(node)
  {
    thorn = (struct THORN *)(node->data);

    if(thorn->active)
    {
      retval = 1;
    }
  }

  return retval;
}

int CCTK_FCALL CCTK_FNAME(CCTK_IsThornActive)
     (ONE_FORTSTRING_ARG)
{
  int retval;
  ONE_FORTSTRING_CREATE(name) 
  retval = CCTK_IsThornActive(name);
  free(name);
  return retval;
}

 /*@@
   @routine    CCTK_ThornImplementation
   @date       Sun Oct 17 21:10:19 1999
   @author     Tom Goodale
   @desc 
   Returns the implementation provided by the thorn.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     name
   @vdesc   Name of the thorn
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype const char *
   @returndesc 
   Name of the implementation or NULL
   @endreturndesc
@@*/
const char *CCTK_ThornImplementation(const char *name)
{
  const char *retval;
  t_sktree *node;
  
  struct THORN *thorn;

  /* Find the thorn */
  node = SKTreeFindNode(thornlist, name);

  retval = NULL;

  if(node)
  {
    thorn = (struct THORN *)(node->data);

    retval = thorn->implementation;
  }

  return retval;
}

 /*@@
   @routine    CCTK_ImplementationThorn
   @date       Sun Oct 17 22:04:13 1999
   @author     Tom Goodale
   @desc 
   Returns the name of one thorn providing an implementation.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     name
   @vdesc   Name of the implementation
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype const char *
   @returndesc 
   Name of the thorn or NULL
   @endreturndesc
@@*/
const char *CCTK_ImplementationThorn(const char *name)
{
  const char *retval;

  t_sktree *node;
  
  struct IMPLEMENTATION *imp;

  /* Find the implementation */
  node = SKTreeFindNode(implist, name);

  retval = NULL;

  if(node)
  {
    imp = (struct IMPLEMENTATION *)(node->data);

    retval = imp->thornlist->key;
  }

  return retval;
}


/*@@
   @routine    CCTK_IsThornCompiled
   @date       Sun Jul  4 17:46:56 1999
   @author     Tom Goodale
   @desc 
   Checks if a thorn is compiled in.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     name
   @vdesc   Name of thorn
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - not compiled
   1 - compiled
   @endreturndesc
@@*/
int CCTK_IsThornCompiled(const char *name)
{
  int retval;
  t_sktree *node;
  
  /* Find the thorn */
  node = SKTreeFindNode(thornlist, name);

  retval = 0;

  if(node)
  {
    retval = 1;
  }

  return retval;
}

void CCTK_FCALL CCTK_FNAME(CCTK_IsThornCompiled)
     (int *retval, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name) 
  *retval = CCTK_IsThornCompiled(name);
  free(name);
}


/*@@
   @routine    CCTK_IsImplementationActive
   @date       Sun Jul  4 17:46:56 1999
   @author     Tom Goodale
   @desc 
   Checks if an implementation is active.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     name
   @vdesc   Name of implementation
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
   0 - inactive
   1 - active
   @endreturndesc
@@*/
int CCTK_IsImplementationActive(const char *name)
{
  int retval;

  t_sktree *node;
  
  struct IMPLEMENTATION *imp;

  /* Find the implementation */
  node = SKTreeFindNode(implist, name);

  retval = 0;

  if(node)
  {
    imp = (struct IMPLEMENTATION *)(node->data);

    if(imp->active)
    {
      retval = 1;
    }
  }

  return retval;
}

void CCTK_FCALL CCTK_FNAME(CCTK_IsImplementationActive)
     (int *retval, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name) 
  *retval = CCTK_IsImplementationActive(name);
  free(name);
}

 /*@@
   @routine    CCTKi_PrintThorns
   @date       Mon Jul  5 10:02:15 1999
   @author     Tom Goodale
   @desc 
   Prints a list of thorns.
   Only lists active ones if the 'active' parameter is true.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     file
   @vdesc   File stream to print to
   @vtype   FILE *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     format
   @vdesc   format string for file
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     active
   @vdesc   Just print active thorns ?
   @vtype   int
   @vio     in
   @vcomment 
   Set to 0 to print all thorns. 
   @endvar 

   @returntype int
   @returndesc 
   0 - success
   @endreturndesc
@@*/
int CCTKi_PrintThorns(FILE *file, const char *format, int active)
{
  int retval;
  t_sktree *node;

  struct THORN *thorn;

  retval = 0;

  for(node= SKTreeFindFirst(thornlist);
      node; 
      node = node->next, retval++)
  {
    thorn = (struct THORN *)(node->data);

    if(thorn->active || !active)
    {
      fprintf(file, format, node->key);
    }
  }

  return retval;
}

 /*@@
   @routine    CCTKi_PrintImps
   @date       Mon Jul  5 10:08:19 1999
   @author     Tom Goodale
   @desc 
   Prints a list of implementations.
   Only lists active ones if the 'active' parameter is true.   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     file
   @vdesc   File stream to print to
   @vtype   FILE *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     format
   @vdesc   format string for file
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     active
   @vdesc   Just print active implementations ?
   @vtype   int
   @vio     in
   @vcomment 
   Set to 0 to print all implementations. 
   @endvar 

   @returntype int
   @returndesc 
   0 - success
   @endreturndesc
@@*/
int CCTKi_PrintImps(FILE *file, const char *format, int active)
{
  int retval;
  t_sktree *node;

  struct IMPLEMENTATION *imp;

  retval = 0;

  for(node= SKTreeFindFirst(implist);
      node; 
      node = node->next, retval++)
  {
    imp = (struct IMPLEMENTATION *)(node->data);

    if(imp->active || !active)
    {
      fprintf(file, format, node->key);
    }
  }

  return retval;
}


 /*@@
   @routine    CCTK_ActivatingThorn
   @date       Thu Oct 14 16:08:42 1999
   @author     Tom Goodale
   @desc 
   Finds the thorn which activated a particular implementation 
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     name
   @vdesc   implementation name
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype const char *
   @returndesc 
   Name of activating thorn or NULL if inactive
   @endreturndesc
@@*/
const char *CCTK_ActivatingThorn(const char *name)
{
  const char *retval;

  t_sktree *node;
  
  struct IMPLEMENTATION *imp;

  /* Find the implementation */
  node = SKTreeFindNode(implist, name);

  retval = NULL;

  if(node)
  {
    imp = (struct IMPLEMENTATION *)(node->data);

    if(imp->active)
    {
      retval = imp->activating_thorn;
    }
  }

  return retval;
}



 /*@@
   @routine    CCTK_ImpThornList
   @date       Tue Jul 27 09:15:58 1999
   @author     Tom Goodale
   @desc 
   Return the thorns for an implementation.
   For now return an sktree - FIXME
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     name
   @vdesc   Name of implementation
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype t_sktree *
   @returndesc 
   Thornlist
   @endreturndesc
@@*/
t_sktree *CCTK_ImpThornList(const char *name)
{
  t_sktree *retval;

  t_sktree *node;
  
  struct IMPLEMENTATION *imp;
  

  /* Find the implementation */
  node = SKTreeFindNode(implist, name);

  if(node)
  {
    imp = (struct IMPLEMENTATION *)(node->data);

    retval = imp->thornlist;
  }
  else
  {
    retval = NULL;
  }

  return retval;
}


 /*@@
   @routine    CCTK_NumCompiledThorns
   @date       Tue Feb 02 2000
   @author     Thomas Radke
   @desc 
   Return the number of thorns compiled in.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

   @returntype int
   @returndesc 
   Number of thorns compiled in
   @endreturndesc
@@*/
int CCTK_NumCompiledThorns(void)
{
  return n_thorns;
}

  
 /*@@
   @routine    CCTK_CompiledThorn
   @date       Tue Feb 02 2000
   @author     Thomas Radke
   @desc 
   Return the name of the compiled thorn with given index.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     tindex
   @vdesc   thorn index
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype const char *
   @returndesc 
   Name of thorn
   @endreturndesc
@@*/
const char *CCTK_CompiledThorn(int tindex)
{
  int i;
  t_sktree *node;
  const char *ret_val;

  ret_val = NULL;

  for(node = SKTreeFindFirst(thornlist), i = 0;
      node;
      node = node->next, i++)
  {
    if (i == index)
    {
      ret_val = node->key;
      break;
    }
  }

  return ret_val;
}

  
 /*@@
   @routine    CCTK_NumCompiledImplementations
   @date       Tue Feb 02 2000
   @author     Thomas Radke
   @desc 
   Return the number of implementations compiled in.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

   @returntype int
   @returndesc 
   Number of implementations compiled in
   @endreturndesc
@@*/
int CCTK_NumCompiledImplementations(void)
{
  return n_imps;
}

  
 /*@@
   @routine    CCTK_CompiledImplementation
   @date       Tue Feb 02 2000
   @author     Thomas Radke
   @desc 
   Return the name of the compiled implementation with given index.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     tindex
   @vdesc   implementation index
   @vtype   int
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype const char *
   @returndesc 
   Name of implementation
   @endreturndesc
@@*/
const char *CCTK_CompiledImplementation(int index)
{
  int i;
  t_sktree *node;
  const char *ret_val;

  ret_val = NULL;

  for(node = SKTreeFindFirst(implist), i = 0;
      node;
      node = node->next, i++)
  {
    if (i == index)
    {
      ret_val = node->key;
      break;
    }
  }

  return ret_val;
}

/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/

 /*@@
   @routine    RegisterImp
   @date       Sun Jul  4 17:44:42 1999
   @author     Tom Goodale
   @desc 
   Registers an implementation.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     name
   @vdesc   name of the implementation
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     thorn
   @vdesc   name of the thorn
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc 
    0 - success
   -1 - failed to store thorn in implementation
   -2 - memory failure creating implementation
   -3 - failed to store implementtion in tree
   @endreturndesc
@@*/
static int RegisterImp(const char *name, const char *thorn)
{
  int retval;
  t_sktree *node;
  t_sktree *temp;

  struct IMPLEMENTATION *imp;

  /* Does the implementation already exist ? */
  node = SKTreeFindNode(implist, name);

  if(!node)
  {
    n_imps++;

    /* Create the structure to hold info about it. */
    imp = (struct IMPLEMENTATION *)malloc(sizeof(struct IMPLEMENTATION));

    if(imp)
    {
      imp->active = 0;

      /* Store the name of this thorn in a tree */      
      imp->thornlist = SKTreeStoreData(NULL,NULL, thorn, NULL);

      /* Store the info in the tree. */
      temp = SKTreeStoreData(implist, implist, name, imp);

      if(!implist) implist = temp;

      if(temp)
      {
        retval = 0;
      }
      else
      {
        retval = -3;
      }
    }
    else
    {
      retval = -2;
    }
  }
  else
  {
    imp = (struct IMPLEMENTATION *)(node->data);
    SKTreeStoreData(imp->thornlist,imp->thornlist, thorn, NULL);

    retval = -1;
  }

  return retval;
}

