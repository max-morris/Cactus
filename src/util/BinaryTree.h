 /*@@
   @header    BinaryTree.h
   @date      Mon Oct  5 11:01:20 1998
   @author    Tom Goodale
   @desc 
   Prototypes and data definitions for binary tree routines.
   @enddesc 
 @@*/

#ifndef _BINARYTREE_H_
#define _BINARYTREE_H_

typedef struct T_TREE
{
  struct T_TREE *left;
  struct T_TREE *right;

  void *data;
} t_tree;

#ifdef _cplusplus
extern "C" {
#endif

t_tree *TreeStoreData(t_tree *root, t_tree *subtree, void *data, int (*compare)(const void *, const void *));

int TreeTraverseInorder(t_tree *root, int (*process)(void *, void *), void *info);

int TreeTraversePreorder(t_tree *root, int (*process)(void *, void *), void *info);

int TreeTraversePostorder(t_tree *root, int (*process)(void *, void *), void *info);

void TreePrintNodes(t_tree *root, int depth, void (*print_node)(void *, int));

#ifdef _cplusplus
	   }
#endif

#endif
