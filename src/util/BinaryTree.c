 /*@@
   @file      BinaryTree.c
   @date      Mon Oct  5 11:00:01 1998
   @author    Tom Goodale
   @desc 
   Routines to deal with binary trees
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BinaryTree.h"

static char *rcsid = "$Header$";


 /*@@
   @routine    TreeStoreData
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
t_tree *TreeStoreData(t_tree *root, t_tree *subtree, void *data, int (*compare)(const void *, const void *))
{
  int order;

  if(!subtree)
  {
    /* Create a new element. */ 
    subtree = (t_tree *)malloc(sizeof(t_tree));
    if(subtree)
    {
      subtree->left=NULL;
      subtree->right=NULL;

      subtree->data = data;

      if(root) 
      {
        if((order = compare(data, root->data)) < 0)
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
    if((order = compare(data, root->data)) < 0)
    {
      subtree = TreeStoreData(subtree, subtree->left, data, compare);
    }
    else if(order > 0) 
    {
      subtree = TreeStoreData(subtree, subtree->right, data, compare);
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
   @routine    TreeTraverseInorder
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
int TreeTraverseInorder(t_tree *root, int (*process)(void *, void *), void *info)
{
  int terminate;

  terminate = 0;

  if(root)
  {
    terminate = TreeTraverseInorder(root->left, process, info);
    if(!terminate) terminate = process(root->data,info);
    if(!terminate) terminate = TreeTraverseInorder(root->right, process, info);
  }

  return terminate;
}

 /*@@
   @routine    TreeTraversePreorder
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
int TreeTraversePreorder(t_tree *root, int (*process)(void *, void *), void *info)
{
  int terminate;

  terminate = 0;

  if(root)
  {
    terminate = process(root->data, info);
    if(!terminate) terminate = TreeTraversePreorder(root->left, process, info);
    if(!terminate) terminate = TreeTraversePreorder(root->right, process,info);
  }

  return terminate;
}
                     
 /*@@
   @routine    TreeTraversePostorder
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
int TreeTraversePostorder(t_tree *root, int (*process)(void *, void *), void *info)
{
  int terminate;

  terminate = 0;

  if(root)
  {
    terminate = TreeTraversePostorder(root->left, process, info);
    if(!terminate) terminate = TreeTraversePostorder(root->right, process, info);
    if(!terminate) terminate = process(root->data, info);
  }

  return terminate;
}

 /*@@
   @routine    TreePrintNodes
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
void TreePrintNodes(t_tree *root, int depth, void (*print_node)(void *, int))
{
  if(root)
  {
    TreePrintNodes(root->left, depth+1,print_node);
    print_node(root->data,depth);
    TreePrintNodes(root->right, depth+1, print_node);
  }
}


 /*@@
   @routine    TreeFindNode
   @date       Mon Oct  5 11:06:52 1998
   @author     Tom Goodale
   @desc 
   Find a node.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
t_tree *TreeFindNode(t_tree *root, void *data, int (*compare)(const void *, const void *))
{
  int order;

  t_tree *node;

  /* Go down left or right branch. */
  if((order = compare(data, root->data)) < 0)
  {
    node = TreeFindNode(root->left, data, compare);
  }
  else if(order > 0) 
  {
    node = TreeFindNode(root->right, data, compare);
  }
  else if(order==0)
  {
    /* Found it. */
    node = root;
  }
  else
  {
    node = NULL;
  }

  return node;
}  


/* Stuff to test the routines. */

/*#define TEST_BinaryTree*/
#ifdef TEST_BinaryTree

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
  t_tree *root;
  t_infodata infodata;
  char instring[500];
  char *newstring;
  t_tree *node;

  infodata.i=0;

  root = NULL;
  
  while(scanf("%s", instring) && strcmp("quit", instring))
  {
    newstring = malloc(strlen(instring)*sizeof(char));
    strcpy(newstring, instring);

    if(!root)
    {
      root = TreeStoreData(root, root, newstring, (int (*)(const void *, const void *))strcmp);
    }
    else
    {
      TreeStoreData(root, root, newstring, (int (*)(const void *, const void *))strcmp);
    }  
  }

  TreeTraverseInorder(root, (int (*)(void *, void *))process, (void *)&infodata);

  TreePrintNodes(root, 0, (void (*)(void *, int))print_node);

  printf("String to find ? ");
  scanf("%s", instring);

  node = TreeFindNode(root, instring, (int (*)(const void *, const void *))strcmp);

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
