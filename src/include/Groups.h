 /*@@
   @header    Groups.h
   @date      Mon Feb  8 14:47:10 1999
   @author    Tom Goodale
   @desc 
   Prototypes and constants for group functions.
   @enddesc 
   @version $Header$
 @@*/

#ifndef _GROUPS_H_
#define _GROUPS_H_

/* Prototypes */

#ifdef __cplusplus 
extern "C" {
#endif

int CCTK_CreateGroup(const char *gname, const char *thorn, const char *imp,
		     const char *gtype,
		     const char *vtype,
		     int dimension,
		     int ntimelevels,
		     int n_variables,
		     ...);

int CCTK_GetGroupIndex(const char *groupname);

int CCTK_GetVarIndex(const char *variablename);

char *CCTK_GetGroupNameFromVar_ByIndex(int var);

char *CCTK_GetFullName(int var);

char *CCTK_GetImplementationFromVar(int var);

int CCTK_GetMaxDim(void);

int CCTK_GetNumVars(void);

int CCTK_GetNumGroups(void);

int CCTK_GTypeNumber(const char *type);

int CCTK_VTypeNumber(const char *type);

int CCTK_GetGroupData(int group, 
		      int *gtype, 
		      int *vtype, 
		      int *dim, 
		      int *n_variables,
		      int *n_timelevels);

char *CCTK_GetVarName(int varnum);

int CCTK_DecomposeName(const char *fullname, char **implementation, char **name);

char *CCTK_GetGroupName(int varnum);

int CCTK_GetVarGType(int var);

int CCTK_GetVarVType(int var);

int CCTK_GetNumTimeLevels(int var);

int CCTK_GetFirstVarIndex(const char *group);
int CCTK_GetFirstVarIndex_ByIndex(int group);

int CCTK_GetNumVarsInGroup(const char *group);
int CCTK_GetNumVarsInGroup_ByIndex(int group);

#ifdef __cplusplus 
}
#endif


/* Group Types */

#define GROUP_SCALAR 1
#define GROUP_GF     2
#define GROUP_ARRAY  3

/* Variable Types */

#define VARIABLE_CHAR     1
#define VARIABLE_INTEGER  2
#define VARIABLE_REAL     3
#define VARIABLE_COMPLEX  4

#endif
