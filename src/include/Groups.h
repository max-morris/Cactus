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

#ifdef _cplusplus 
extern "C" {
#endif

int CCTK_CreateGroup(const char *gname, const char *thorn, const char *imp,
		     const char *gtype,
		     const char *vtype,
		     int dimension,
		     int n_variables,
		     ...);

int CCTK_GetGroupNum(const char *implementation,
		     const char *name);

int CCTK_GetVarNum(const char *implementation,
		    const char *group_name,
		    const char *variable_name);

int CCTK_GetMaxDim(void);

int CCTK_GetNumVars(void);

int CCTK_GetNumGroups(void);

int CCTK_ArrayGroupSize(cGH *GH, const char *group, int dim);

int CCTK_GTypeNumber(const char *type);

int CCTK_VTypeNumber(const char *type);

int CCTK_GetGroupData(int group, int *gtype, int *vtype, int *dim, int *n_variables);

char *CCTK_GetVarName(int varnum);

#ifdef _cplusplus 
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
