 /*@@
   @header    cctk_Flesh.h
   @date      Thu Sep 24 10:18:52 1998
   @author    Tom Goodale
   @desc 
   Header file for flesh functions.
   @enddesc 
   @version $Header$
 @@*/

#include "CactusTimers.h"
#include "cctk.h"

#ifndef _FLESH_H_
#define _FLESH_H_

#include "cGH.h"

/*  Typedefs */

typedef struct
{
  int dummy;
  char *name;
} cGF;

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

  int gscope;

  int dim;

  int n_timelevels;

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

int CCTKi_SetParameter(const char *parameter, const char *value);

int ShutdownCactus(tFleshConfig *);

int ProcessCommandLine(int *argc, char ***argv, tFleshConfig *ConfigData);

int InitialiseDataStructures(tFleshConfig *ConfigData);

int ProcessParameterDatabase(tFleshConfig *ConfigData);

int CallStartupFunctions(tFleshConfig *ConfigData);

int CCTKi_AddGH(tFleshConfig *config, int convergence_level, cGH *GH);

int CCTK_MaxDim(void);

int CCTK_NumVars(void);

int CCTK_NumGroups(void);

int CCTK_VarTypeNumber(const char *type);

int CCTK_GroupTypeNumber(const char *type);

int CCTKi_BindingsImplementationsInitialise(void);

int CCTK_MainLoopIndex (void);

int CCTK_SetMainLoopIndex (int main_loop_index);

#ifdef __cplusplus
}
#endif

#endif
