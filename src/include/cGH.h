 /*@@
   @header    cGH.h
   @date      Wed Feb 17 03:17:47 1999
   @author    Tom Goodale
   @desc 
   The cGH structure.
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CGH_H_
#define _CGH_H_

#include "cctk.h"

typedef struct
{
  char storage;
  char comm;
} cGHGroupData;

typedef struct
{
  int dim;
  unsigned long int iteration;

  /* ...[dim]*/
  int *global_shape;
  int *local_shape;
  int *lower_bound;
  int *upper_bound;

  /* The grid spacings */
  Double delta_time;
  Double *delta_space;

  /* The bounding box - 1 => a real boundary, 0 => a local grid boundary. */
  /* bbox[2*dim] */
  int *bbox;

  /* The refinement factor over the top level (coarsest) grid. */
  int levfac;

  /* The convergence level */
  int convlevel;

  /* The number of ghostzones in each direction */
  int nghostzones;

  /* The coordinate time */
  Double time;

  /* data[var_num][TIMELEVEL][xyz]*/
  /* TIMELEVEL  I believe, xyz is linear */
  void ***data;

  /* The extension array */
  void **extensions;

  /* All the group data for this GH (storage, comm, etc. */
  cGHGroupData *GroupData;

  /* Rfr tree for this GH. */
  void *rfr_top;

} cGH;

#endif
