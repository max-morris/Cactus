 /*@@
   @header    cctk_Groups.h
   @date      Mon Feb  8 14:47:10 1999
   @author    Tom Goodale
   @desc 
   Prototypes and constants for group functions.
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTK_GROUPS_H_
#define _CCTK_GROUPS_H_

/* Prototypes */

#ifdef __cplusplus 
extern "C" {
#endif

int CCTKi_CreateGroup(const char *gname, const char *thorn, const char *imp,
		     const char *gtype,
		     const char *vtype,
		     const char *gscope,
		     int dimension,
		     int ntimelevels,
		     int n_variables,
		     ...);

int CCTK_GroupIndex(const char *groupname);

int CCTK_VarIndex(const char *variablename);

char *CCTK_GroupNameFromVarI(int var);

int CCTK_GroupIndexFromVar(const char *var);

int CCTK_GroupIndexFromVarI(int var);

char *CCTK_FullName(int var);

char *CCTK_ImpFromVarI(int var);

int CCTK_MaxDim(void);

int CCTK_NumVars(void);

int CCTK_NumGroups(void);

int CCTK_GroupTypeNumber(const char *type);

int CCTK_VarTypeNumber(const char *type);

int CCTK_GroupScopeNumber(const char *type);

int CCTK_GroupData(int group, 
		      int *gtype, 
		      int *vtype, 
		      int *dim, 
		      int *n_variables,
		      int *n_timelevels);

char *CCTK_VarName(int varnum);

int CCTK_DecomposeName(const char *fullname, char **implementation, char **name);

char *CCTK_GroupName(int varnum);

int CCTK_GroupTypeFromVarI(int var);

int CCTK_VarTypeI(int var);

int CCTK_NumTimeLevelsFromVar(const char *var);
int CCTK_NumTimeLevelsFromVarI(int var);

int CCTK_FirstVarIndex(const char *group);
int CCTK_FirstVarIndexI(int group);

int CCTK_NumVarsInGroup(const char *group);
int CCTK_NumVarsInGroupI(int group);

#ifdef __cplusplus 
}
#endif


/* Group Types */

#define GROUP_SCALAR 1
#define GROUP_GF     2
#define GROUP_ARRAY  3

#define GROUP_PRIVATE   1
#define GROUP_PROTECTED 2
#define GROUP_PUBLIC    3

#endif
