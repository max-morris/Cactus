 /*@@
   @file      SKBinTree.c
   @date      Mon Oct  5 11:00:01 1998
   @author    Tom Goodale
   @desc 
   Routines to deal with binary trees
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SKBinTree.h"

int STR_cmpi(const char *string1, const char *string2);

#define STR_CMP(a,b) STR_cmpi(a,b)

static char *rcsid = "$Id$";


 /*@@
   @routine    SKTreeStoreData
   @date       Mon Oct  5 11:04:55 1998
   @author     Tom Goodale
   @desc 
   Stores data in a binary tree.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
t_sktree *SKTreeStoreData(t_sktree *root, t_sktree *subtree, 
		      const char *key, void *data)
{
  int order;

  if(!subtree)
  {
    /* Create a new element. */ 
    subtree = (t_sktree *)malloc(sizeof(t_sktree));
    if(subtree)
    {
      subtree->left=NULL;
      subtree->right=NULL;

      subtree->data = data;

      subtree->key= (char *)malloc(sizeof(char)*(strlen(key)+1));
      strcpy(subtree->key, key);
      if(root) 
      {
	if((order = STR_CMP(key, root->key)) < 0)
	{
	  root->left = subtree;
	}
	else
	{
	  root->right = subtree;
	}
      }
    }
  }
  else
  {
    /* Go down left or right branch. */
    if((order = STR_CMP(key, root->key)) < 0)
    {
      subtree = SKTreeStoreData(subtree, subtree->left, key, data);
    }
    else if(order > 0) 
    {
      subtree = SKTreeStoreData(subtree, subtree->right, key, data);
    }
    else if(order==0)
    {
      /* Duplicate key. */
      subtree = NULL;
    }
  }

  /* Return either the new node, or NULL if either a duplicate value or
   * memory failure.
   */

  return subtree;

}

 /*@@
   @routine    SKTreeTraverseInorder
   @date       Mon Oct  5 11:05:54 1998
   @author     Tom Goodale
   @desc 
   Traverse a tree 'inorder'
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int SKTreeTraverseInorder(t_sktree *root, int (*process)(void *, void *), void *info)
{
  int terminate;

  terminate = 0;

  if(root)
  {
    terminate = SKTreeTraverseInorder(root->left, process, info);
    if(!terminate) terminate = process(root->data,info);
    if(!terminate) terminate = SKTreeTraverseInorder(root->right, process, info);
  }

  return terminate;
}

 /*@@
   @routine    SKTreeTraversePreorder
   @date       Mon Oct  5 11:05:54 1998
   @author     Tom Goodale
   @desc 
   Traverse a tree 'preorder'
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int SKTreeTraversePreorder(t_sktree *root, int (*process)(void *, void *), void *info)
{
  int terminate;

  terminate = 0;

  if(root)
  {
    terminate = process(root->data, info);
    if(!terminate) terminate = SKTreeTraversePreorder(root->left, process, info);
    if(!terminate) terminate = SKTreeTraversePreorder(root->right, process,info);
  }

  return terminate;
}
		     
 /*@@
   @routine    SKTreeTraversePostorder
   @date       Mon Oct  5 11:05:54 1998
   @author     Tom Goodale
   @desc 
   Traverse a tree 'postorder'
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int SKTreeTraversePostorder(t_sktree *root, int (*process)(void *, void *), void *info)
{
  int terminate;

  terminate = 0;

  if(root)
  {
    terminate = SKTreeTraversePostorder(root->left, process, info);
    if(!terminate) terminate = SKTreeTraversePostorder(root->right, process, info);
    if(!terminate) terminate = process(root->data, info);
  }

  return terminate;
}

 /*@@
   @routine    SKTreePrintNodes
   @date       Mon Oct  5 11:06:52 1998
   @author     Tom Goodale
   @desc 
   Allows a binary tree to be printed out.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void SKTreePrintNodes(t_sktree *root, int depth, void (*print_node)(void *, int))
{
  if(root)
  {
    SKTreePrintNodes(root->left, depth+1,print_node);
    print_node(root->data,depth);
    SKTreePrintNodes(root->right, depth+1, print_node);
  }
}


t_sktree *SKTreeFindNode(t_sktree *root, const char *key)
{
  int order;

  t_sktree *node;

  if(root)
  {
    /* Go down left or right branch. */
    if((order = STR_CMP(key, root->key)) < 0)
    {
      node = SKTreeFindNode(root->left, key);
    }
    else if(order > 0) 
    {
      node = SKTreeFindNode(root->right, key);
    }
    else if(order==0)
    {
      /* Found it. */
      node = root;
    }
  }
  else
  {
    node = NULL;
  }

  return node;
}  



int STR_cmpi(const char *string1, const char *string2)
{
  int retval;
  int position;

  retval = 1;

  for(position = 0; 
      position < strlen(string1) && position < strlen(string2);
      position++)
  {
    if(tolower(string1[position]) < tolower(string2[position]))
    {
      retval = -1;
      break;
    }
    else if(tolower(string1[position]) > tolower(string2[position]))
    {
      retval = 1;
    }
    else
    {
      retval = 0;
    }
  }

  if(retval == 0 && position < strlen(string1))
  {
    retval = 1;
  }
  else if(retval == 0 && position < strlen(string2))
  {
    retval = 1;
  }

  return retval;
}


/* Stuff to test the routines. */

/*#define TEST_SKBinTree*/
#ifdef TEST_SKBinTree

typedef struct 
{
  int i;
} t_infodata ;

int process(char *data, t_infodata *infodata)
{
  printf("%d, %s\n", infodata->i, data);

  infodata->i++;

  return 0;
}

void print_node(char *data, int depth)
{
  int i;
  for(i=0; i < depth; i++) printf("*");
  printf("%s\n", data);
}

int main(void)
{
  t_sktree *root;
  t_infodata infodata;
  char instring[500];
  char *newstring;
  t_sktree *node;

  infodata.i=0;

  root = NULL;
  
  while(scanf("%s", instring) && strcmp("quit", instring))
  {
    newstring = malloc(strlen(instring)*sizeof(char));
    strcpy(newstring, instring);

    if(!root)
    {
      root = SKTreeStoreData(root, root, newstring, (int (*)(const void *, const void *))strcmp);
    }
    else
    {
      SKTreeStoreData(root, root, newstring, (int (*)(const void *, const void *))strcmp);
    }  
  }

  SKTreeTraverseInorder(root, (int (*)(void *, void *))process, (void *)&infodata);

  SKTreePrintNodes(root, 0, (void (*)(void *, int))print_node);

  printf("String to find ? ");
  scanf("%s", instring);

  node = SKTreeFindNode(root, instring, (int (*)(const void *, const void *))strcmp);

  if(node)
  {
    printf("Found a node, node->data is %s\n", node->data);
  }
  else
  {
    printf("Unable to find node with %s\n", instring);
  }

  return 0;
}

#endif
