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
#include "cGH.h"


#ifndef _CCTK_FLESH_H_
#define _CCTK_FLESH_H_

/*  Typedefs */

typedef struct
{
  char *parameter_file_name;

  /* Array of pointers to cactus grid hierarchies. */
  cGH **GH;
  unsigned int nGHs;

  /*  cTimer *timer[3];*/
} tFleshConfig;


/* Function prototypes */

#ifdef __cplusplus
extern "C" 
{
#endif

int InitialiseCactus(int *, char ***, tFleshConfig *);

int InitialiseSubsystemDefaults(void);

void CactusBanner(void);

int InitialiseScheduler(tFleshConfig *ConfigData);

void CCTK_PrintBanners(void);

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
int CCTKi_BindingsScheduleInitialise(void);
int CCTKi_BindingsVariablesInitialise(void);
int CCTKi_BindingsParametersInitialise(void);

int CCTKi_RegisterDefaultTimerFunctions(void);

int CCTK_MainLoopIndex (void);

int CCTK_SetMainLoopIndex (int main_loop_index);

int SetupMainFunctions(void);
int SetupCommFunctions(void);
int SetupIOFunctions(void);

int CCTKi_SetupCache(void);

#ifdef __cplusplus
}
#endif

#endif
