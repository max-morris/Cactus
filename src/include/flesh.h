 /*@@
   @header    flesh.h
   @date      Thu Sep 24 10:18:52 1998
   @author    Tom Goodale
   @desc 
   Header file for flesh functions.
   @enddesc 
 @@*/

/* $Id$ */

#include "CactusTimers.h"

#ifndef _FLESH_H_
#define _FLESH_H_

/*  Typedefs */

typedef struct
{
  int dummy;
  char *name;
} cGF;

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
  int *local_shape;
  int *lower_bound;
  int *upper_bound;

  /* The bounding box - 1 => a real boundary, 0 => a local grid boundary. */
  /* bbox[2*dim] */
  int *bbox;

  /* The refinement factor over the top level grid. */
  int levfac;

  /* data[var_num][xyz]*/
  void **data;

  /* The extension array */
  void **extensions;

  /* All the group data for this GH (storage, comm, etc. */
  cGHGroupData *GroupData;

  /* Rfr tree for this GH. */
  void *rfr_top;

} cGH;

typedef struct 
{
  char *name;
  int number;

  /* dimensional_comm_array[dim] */
  char *dimensional_comm_array;
} cVariableDefinition;

typedef struct
{
  /* The various names of the thing. */
 
  char *thorn;
  char *implementation;
  char *name;

  /* The group number. */
  int number;

  /* The types. */
  int gtype;
  
  int vtype;

  int dim;

  int n_variables;

  /* variables[n_variables] */
  cVariableDefinition *variables;
} cGroupDefinition;

typedef struct
{
  char *parameter_file_name;

  /* Array of pointers to cactus grid hierarchies. */
  cGH **GH;
  unsigned int nGHs;

  cTimer *timer[3];
} tFleshConfig;


/* Function prototypes */

#ifdef __cplusplus
extern "C" {
#endif

int InitialiseCactus(int *, char ***, tFleshConfig *);

int CCTK_SetParameter(const char *parameter, const char *value);

int ShutdownCactus(tFleshConfig *);

int ProcessCommandLine(int *argc, char ***argv, tFleshConfig *ConfigData);

int InitialiseDataStructures(tFleshConfig *ConfigData);

int ProcessParameterDatabase(tFleshConfig *ConfigData);

int CallStartupFunctions(tFleshConfig *ConfigData);

int CCTK_AddGH(tFleshConfig *config, int convergence_level, cGH *GH);

int CCTK_GetMaxDim(void);

int CCTK_GetNVariables(void);

int CCTK_GetNGroups(void);

int CCTK_VTypeNumber(const char *type);

int CCTK_GTypeNumber(const char *type);

#ifdef __cplusplus
	   }
#endif

#endif
