 /*@@
   @file      ActiveThorns.c
   @date      Sun Jul  4 16:15:36 1999
   @author    Tom Goodale
   @desc 
   Stuff to deal with activethorns.
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SKBinTree.h"

#include "config.h"
#include "cctk_ActiveThorns.h"
#include "cctk_FortranString.h"

static char *rcsid = "$Header$";

/* Local routine */
static int CCTK_RegisterImp(const char *name, const char *thorn);

/* Structures used to store data */
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


static t_sktree *thornlist = NULL;
static t_sktree *implist   = NULL;

static int n_thorns = 0;
static int n_imps   = 0;

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

	if(!thornlist) thornlist = temp;

	if(temp)
	{

	  /* Register the implementation */
	  CCTK_RegisterImp(imp, name);

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
   @routine    CCTK_RegisterImp
   @date       Sun Jul  4 17:44:42 1999
   @author     Tom Goodale
   @desc 
   Registers an implementation.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int CCTK_RegisterImp(const char *name, const char *thorn)
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

void FMODIFIER FORTRAN_NAME(CCTK_IsThornActive)(int *retval, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name) 
  *retval = CCTK_IsThornActive(name);
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

void FMODIFIER FORTRAN_NAME(CCTK_IsImplementationActive)
     (int *retval, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name) 
  *retval = CCTK_IsImplementationActive(name);
  free(name);
}

 /*@@
   @routine    CCTKi_ListThorns
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

@@*/
int CCTKi_ListThorns(FILE *file, const char *format, int active)
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
   @routine    CCTKi_ListImplementations
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

@@*/
int CCTKi_ListImplementations(FILE *file, const char *format, int active)
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
   @routine    CCTK_ImpList
   @date       Thu Oct 14 16:14:22 1999
   @author     Tom Goodale
   @desc 
   Returns the list of implementations.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_ImpList(int active, char ***list, int *n_implementations)
{
  int retval;
  t_sktree *node;

  struct IMPLEMENTATION *imp;

  retval = 0;


  *list = (char **)malloc(n_imps*sizeof(char *));
  
  for(node= SKTreeFindFirst(implist),   *n_implementations = 0;
      node; 
      node = node->next, retval++)
  {
    imp = (struct IMPLEMENTATION *)(node->data);

    if(imp->active || !active)
    {
      (*list)[*n_implementations] = (char *) malloc(strlen(node->key)+1);
      strcpy((*list)[*n_implementations], node->key);
      (*n_implementations)++;
    }
  }

  return retval;
}



 /*@@
   @routine    CCTK_ThornList
   @date       Thu Oct 14 16:04:59 1999
   @author     Andre Merzky
   @desc 
   Returns a list of thorns.
   @enddesc 
   @calls     
   @calledby   
   @history 

   @endhistory 

@@*/
int CCTK_ThornList (const char* imp, char ***list, int *n_thorns)
{
  int       retval;
  t_sktree *node;
  t_sktree *thornlist;
  int	    alloc_size = 0;
    
  /* FIXME */
#define _MY_THORN_JUNK_SIZE 10

  /* find all thorns for implementation */
  thornlist = (t_sktree*) CCTK_ImpThornList (imp);
    
    
  /* got thornlist? */	
  if (thornlist)
  {
    /* then we can start allocatin list */
    alloc_size += _MY_THORN_JUNK_SIZE;
    *list = (char **) malloc (alloc_size * sizeof (char *));
	
    /* success? */
    if (! (*list)) 
    {
      fprintf (stderr, "Cannot malloc paramlist*\n");
      return (-1);
    }
    /* recourse thorn tree */
    for (node = SKTreeFindFirst (thornlist), *n_thorns = 0;
         node; 
         node = node->next, retval++)
    {
      /* list long enough? */
      if ((*n_thorns) >= alloc_size)
      {
        /* no: realloc! */
        alloc_size += _MY_THORN_JUNK_SIZE;
        *list = (char **) realloc ((*list), alloc_size);

        /* success? */
        if (! (*list)) {
          fprintf (stderr, "Cannot realloc paramlist*\n");
          return (-1);
        }
          
      } 

      /* store thorn */
      (*list)[*n_thorns] = (char *) malloc ((strlen (node->key) + 1) * sizeof (char));
      strcpy ((*list)[*n_thorns], node->key);
      (*n_thorns)++;
    }
  }

  /* if necessary, shrink paramlist again. */
  if ((*n_thorns) < alloc_size)
  {
    alloc_size += (*n_thorns);
    *list = (char **) realloc ((*list), alloc_size);
    
    if (! (*list)) 
    {
      fprintf (stderr, "Cannot realloc list*\n");
      return (-1);
    }
  } 

  /* done */
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
