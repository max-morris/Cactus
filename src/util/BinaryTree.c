 /*@@
   @file      BinaryTree.c
   @date      Mon Oct  5 11:00:01 1998
   @author    Tom Goodale
   @desc 
   Routines to deal with binary trees
   @enddesc 
   @version $Header$
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cctk_Flesh.h"
#include "util_BinaryTree.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(util_BinaryTree_c)

/********************************************************************
 ********************    External Routines   ************************
 ********************************************************************/


 /*@@
   @routine    Util_BinTreeStoreData
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
uBinTree *Util_BinTreeStoreData(uBinTree *root, 
                                uBinTree *subtree, 
                                void *data, 
                                int (*compare)(const void *, const void *))
{
  int order;

  if(!subtree)
  {
    /* Create a new element. */ 
    subtree = (uBinTree *)malloc(sizeof(uBinTree));
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
      subtree = Util_BinTreeStoreData(subtree, subtree->left, data, compare);
    }
    else if(order > 0) 
    {
      subtree = Util_BinTreeStoreData(subtree, subtree->right, data, compare);
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
   @routine    Util_BinTreeTraverseInorder
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
int Util_BinTreeTraverseInorder(uBinTree *root, 
                                int (*process)(void *, void *), 
                                void *info)
{
  int terminate;

  terminate = 0;

  if(root)
  {
    terminate = Util_BinTreeTraverseInorder(root->left, process, info);
    if(!terminate) terminate = process(root->data,info);
    if(!terminate) terminate = Util_BinTreeTraverseInorder(root->right, process, info);
  }

  return terminate;
}

 /*@@
   @routine    Util_BinTreeTraversePreorder
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
int Util_BinTreeTraversePreorder(uBinTree *root, 
                                 int (*process)(void *, void *), 
                                 void *info)
{
  int terminate;

  terminate = 0;

  if(root)
  {
    terminate = process(root->data, info);
    if(!terminate) terminate = Util_BinTreeTraversePreorder(root->left, process, info);
    if(!terminate) terminate = Util_BinTreeTraversePreorder(root->right, process,info);
  }

  return terminate;
}
                     
 /*@@
   @routine    Util_BinTreeTraversePostorder
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
int Util_BinTreeTraversePostorder(uBinTree *root, 
                                  int (*process)(void *, void *), 
                                  void *info)
{
  int terminate;

  terminate = 0;

  if(root)
  {
    terminate = Util_BinTreeTraversePostorder(root->left, process, info);
    if(!terminate) terminate = Util_BinTreeTraversePostorder(root->right, process, info);
    if(!terminate) terminate = process(root->data, info);
  }

  return terminate;
}

 /*@@
   @routine    Util_BinTreePrintNodes
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
int Util_BinTreePrintNodes(uBinTree *root, 
                           int depth, 
                           void (*print_node)(void *, int))
{
  if(root)
  {
    Util_BinTreePrintNodes(root->left, depth+1,print_node);
    print_node(root->data,depth);
    Util_BinTreePrintNodes(root->right, depth+1, print_node);
  }
  return 0;
}


 /*@@
   @routine    Util_BinTreeFindNode
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
uBinTree *Util_BinTreeFindNode(uBinTree *root, 
                               void *data, 
                               int (*compare)(const void *, const void *))
{
  int order;

  uBinTree *node;

  /* Go down left or right branch. */
  if((order = compare(data, root->data)) < 0)
  {
    node = Util_BinTreeFindNode(root->left, data, compare);
  }
  else if(order > 0) 
  {
    node = Util_BinTreeFindNode(root->right, data, compare);
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
  uBinTree *root;
  t_infodata infodata;
  char instring[500];
  char *newstring;
  uBinTree *node;

  infodata.i=0;

  root = NULL;
  
  while(scanf("%s", instring) && strcmp("quit", instring))
  {
    newstring = malloc(strlen(instring)*sizeof(char));
    strcpy(newstring, instring);

    if(!root)
    {
      root = Util_BinTreeStoreData(root, root, newstring, (int (*)(const void *, const void *))strcmp);
    }
    else
    {
      Util_BinTreeStoreData(root, root, newstring, (int (*)(const void *, const void *))strcmp);
    }  
  }

  Util_BinTreeTraverseInorder(root, (int (*)(void *, void *))process, (void *)&infodata);

  Util_BinTreePrintNodes(root, 0, (void (*)(void *, int))print_node);

  printf("String to find ? ");
  scanf("%s", instring);

  node = Util_BinTreeFindNode(root, instring, (int (*)(const void *, const void *))strcmp);

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
