 /*@@
   @file      SKBinTree.c
   @date      Mon Oct  5 11:00:01 1998
   @author    Tom Goodale
   @desc 
   Routines to deal with binary trees keyed by strings.
   The tree is a threaded tree, i.e. it can also be
   traversed, inorder, like a linked list.
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
   Stores data in string-keyed binary tree.
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
      subtree->next=NULL;

      subtree->data = data;

      subtree->key= (char *)malloc(sizeof(char)*(strlen(key)+1));
      strcpy(subtree->key, key);
      if(root) 
      {
	if((order = STR_CMP(key, root->key)) < 0)
	{
	  root->left = subtree;
	  subtree->next = root ;
	}
	else
	{
	  root->right = subtree;
	  subtree->next = root->next;
	  root->next = subtree;
	}
      }
    }
  }
  else
  {
    /* Go down left or right branch. */
    if((order = STR_CMP(key, subtree->key)) < 0)
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


 /*@@
   @routine    SKTreeFindNode
   @date       Mon Jul  5 10:09:30 1999
   @author     Tom Goodale
   @desc 
   Finds a given node in the tree.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
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

 /*@@
   @routine    SKTreeFindFirst
   @date       Mon Jul  5 10:09:57 1999
   @author     Tom Goodale
   @desc 
   Finds the first node in the tree (the leftmost one).
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
t_sktree *SKTreeFindFirst(t_sktree *root)
{
  for(; root->left ; root = root->left);

  return root;
}




 /*@@
   @routine    STR_cmpi
   @date       Mon Jul  5 01:19:00 1999
   @author     Tom Goodale
   @desc 
   Case independent strcmp
   @enddesc 
   @calls     
   @calledby   
   @history 
   @hdate Wed Oct 13 15:30:57 1999 @hauthor Tom Goodale
   @hdesc Checks the length of the two string first. 
   @endhistory 

@@*/
int STR_cmpi(const char *string1, const char *string2)
{
  int retval;
  int position;
 
  retval = strlen(string1) - strlen(string2);

  if(! retval)
  {
    for(position = 0; 
        string1[position] && string2[position];
        position++)
    {
      if((retval = (tolower(string1[position]) - tolower(string2[position]))))
      {
        break;
      }
    }
  }

  return retval;
}

