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

typedef struct
{
  int grouptype;
  int vartype;
  int stagtype;
  int dim;
  int numvars;
  int numtimelevels;
} cGroup;

/* Prototypes */

#ifdef __cplusplus 
extern "C" 
{
#endif

#include "cctk_Types.h"

int        CCTK_DecomposeName(const char *fullname, 
                              char **implementation, 
                              char **name);

int        CCTK_FirstVarIndex(const char *group);
int        CCTK_FirstVarIndexI(int group);
char      *CCTK_FullName(int var);

int        CCTK_GroupData(int group, cGroup *gp);
CCTK_INT **CCTK_GroupGhostsizesI(int group);
int        CCTK_GroupIndex(const char *groupname);
int        CCTK_GroupIndexFromVar(const char *var);
int        CCTK_GroupIndexFromVarI(int var);
char      *CCTK_GroupName(int varnum);
char      *CCTK_GroupNameFromVarI(int var);
int        CCTK_GroupScopeNumber(const char *type);
CCTK_INT **CCTK_GroupSizesI(int group);
int        CCTK_GroupTypeFromVarI(int var);
int        CCTK_GroupTypeNumber(const char *type);
int        CCTK_GroupTypeI(int group);

char      *CCTK_ImpFromVarI(int var);

int        CCTK_MaxDim(void);

int        CCTK_NumGroups(void);
int        CCTK_NumTimeLevelsFromVar(const char *var);
int        CCTK_NumTimeLevelsFromVarI(int var);
int        CCTK_NumVars(void);
int        CCTK_NumVarsInGroup(const char *group);
int        CCTK_NumVarsInGroupI(int group);

int        CCTK_VarIndex(const char *variablename);
char      *CCTK_VarName(int varnum);
int        CCTK_VarTypeI(int var);
int        CCTK_VarTypeNumber(const char *type);
char      *CCTK_VarTypeName(int vartype);

int        CCTK_VarTypeSize(int vtype);

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

#define CCTK_SCALAR 1
#define CCTK_GF     2
#define CCTK_ARRAY  3

#define CCTK_PRIVATE   1
#define CCTK_PROTECTED 2
#define CCTK_PUBLIC    3

#endif /* _CCTK_GROUPS_H_ */
