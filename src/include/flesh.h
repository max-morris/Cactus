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

  /* bbox[2*dim] */
  int *bbox;

  int levfac;

  /* data[var_num][xyz]*/
  void **data;

  void *extensions;

  cGHGroupData *GroupData;
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
  char *implementation;
  char *name;
  int number;

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


typedef struct
{
  char *name;
  char *implementation;
  int (*startup)();
  int (*rfr_init)(cGH *);
  int (*param_init)();
  int (*param_set)(const char *, const char *);
  int (*param_get)(const char *, void **);
  int (*private_group_setup)(cGH *);
  int (*protected_group_setup)(cGH *);
  int (*public_group_setup)(cGH *);
} t_thorndata;

/* Function prototypes */

#ifdef __cplusplus
extern "C" {
#endif

int InitialiseCactus(int *, char ***, tFleshConfig *);

int CCTK_SetParameter(const char *parameter, const char *value);

extern int (*Initialise)(tFleshConfig *);

extern int (*Evolve)(tFleshConfig *);

extern int (*Shutdown)(tFleshConfig *);

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
