 /*@@
   @file      Groups.c
   @date      Mon Feb  1 12:16:28 1999
   @author    Tom Goodale
   @desc 
   Routines to deal with groups.
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "flesh.h"
#include "Misc.h"
#include "Groups.h"

/*#define GROUPSDEBUG*/

static char *rcsid = "$Header$";

/* Static variables needed to hold group and variable data. */

static int n_groups = 0;
static cGroupDefinition *groups = NULL;

static int total_variables = 0;

static int *group_of_variable = NULL;

static int maxdim = 0;


/* When passing to fortran, must pass by reference
 * so need to define the odd global variable to pass 8-(
 */

int _cctk_one = 1;


cGroupDefinition *CCTK_SetupGroup(const char *implementation, const char *group_name, int n_variables);


 /*@@
   @routine    CCTK_CreateGroup
   @date       Thu Jan 14 15:25:54 1999
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_CreateGroup(const char *gname, const char *thorn, const char *imp,
		     const char *gtype,
		     const char *vtype,
		     int dimension,
		     int ntimelevels,
		     int n_variables,
		     ...)
{
  int retval;

  va_list ap;
  char *variable_name;

  cGroupDefinition *group;

  int variable;

  retval = 0;

  /* Allocate storage for the group and setup some stuff. */
  if((group = CCTK_SetupGroup(imp, gname, n_variables)))
  {
    group->dim = dimension;
    group->gtype = CCTK_GTypeNumber(gtype);
    group->vtype = CCTK_VTypeNumber(vtype);

    group->n_timelevels = ntimelevels;
    
    /* Extract the variable names from the argument list. */
    va_start(ap, n_variables);

    for(variable = 0; variable < n_variables; variable++)
    {
      variable_name = va_arg(ap, char *);

      group->variables[variable].name = (char *)malloc((strlen(variable_name)+1*sizeof(char)));
      
      if(group->variables[variable].name)
      {
	strcpy(group->variables[variable].name, variable_name);
      }
      else
      {
	break;
      }
    }

    va_end(ap);

    if(variable < n_variables)
    {
      retval = 3;
    }
    else
    {
      if (dimension > maxdim) maxdim=dimension;
    }
  }
  else
  {
    retval = 2;
  }

  if(retval)
  {
    fprintf(stderr, "Error %d in CCTK_CreateGroup\n", retval);
  }

#ifdef GROUPSDEBUG
  printf("Created group %s\n",gname);
  printf("  CCTK_GetGroupNum(%s,%s) = %d\n",imp,gname,
	 CCTK_GetGroupNum(imp,gname));
  printf("  CCTK_GetGroupName(%d) = %s\n",CCTK_GetGroupNum(imp,gname),
         CCTK_GetGroupName(CCTK_GetGroupNum(imp,gname)));
#endif

  return retval;

}

 /*@@
   @routine    CCTK_SetupGroup
   @date       Thu Jan 14 16:38:40 1999
   @author     Tom Goodale
   @desc 
   Stores the data associated with a group.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
cGroupDefinition *CCTK_SetupGroup(const char *implementation, 
				  const char *name, 
				  int n_variables)
{
  int *temp_int;
  cGroupDefinition *temp;
  cGroupDefinition *returndata;
  int variable;
  int group_num;

  if((group_num = CCTK_GetGroupNum(implementation, name)) == -1)
  {
    /* Resize the array of groups */
    if(temp = (cGroupDefinition *)realloc(groups, (n_groups+1)*sizeof(cGroupDefinition)))
    {
      groups = temp;
      
      /* Allocate memory to various fields */
      groups[n_groups].implementation = (char *)malloc((strlen(implementation)+1)*sizeof(char));
      
      groups[n_groups].name = (char *)malloc((strlen(name)+1)*sizeof(char));
      
      groups[n_groups].variables = (cVariableDefinition *)malloc(n_variables*sizeof(cVariableDefinition));
      
      /* Resize the array holding correspondence between variables and groups. */

      temp_int = (int *)realloc(group_of_variable, (total_variables+n_variables)*sizeof(int));

      if(groups[n_groups].implementation && 
	 groups[n_groups].name && 
	 groups[n_groups].variables &&
	 temp_int)
      {
	/* Fill in the data structures. */
	group_of_variable = temp_int;
	
	strcpy(groups[n_groups].implementation, implementation);
	strcpy(groups[n_groups].name, name);
	
	groups[n_groups].number = n_groups;
	
	groups[n_groups].n_variables = n_variables;
	
	/* Fill in global variable numbers. */
	for(variable = 0; variable < n_variables; variable++)
	{
	  groups[n_groups].variables[variable].number = total_variables;
	  
	  group_of_variable[total_variables] = n_groups;
	  
	  total_variables++;
	}
	
	n_groups++;
      }
      else
      {
	/* Memory allocation failed, so free any which may have been allocated. */
	free(groups[n_groups].implementation);
	groups[n_groups].implementation = NULL;

	free(groups[n_groups].name);
	groups[n_groups].name = NULL;
    
	free(groups[n_groups].variables);
	groups[n_groups].variables = NULL;

      }
    }
    
    /* Return the new group definition structure if successful, otherwise NULL.*/
    if(temp && groups[n_groups-1].name)
    {
      returndata =  &(groups[n_groups-1]);
    }
    else
    {
      returndata =  NULL;
    }
  }
  else
  {
    returndata = &(groups[group_num]);
  }

  return returndata;
}

 /*@@
   @routine    CCTK_GetGroupNum
   @date       Fri Jan 29 08:43:48 1999
   @author     Tom Goodale
   @desc 
   Gets the number for the specified group.
   @enddesc 
   @calls CCTK_Equals   
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_GetGroupNum(const char *implementation,
		     const char *name)
{
  int group_num;

  for(group_num = 0; group_num < n_groups; group_num++)
  {
    if(CCTK_Equals(implementation, groups[group_num].implementation) &&
       CCTK_Equals(name, groups[group_num].name)) break;
  }

  if (group_num < n_groups)
  {
    return group_num;
  }
  else
  {
    return -1;
  }
}


 /*@@
   @routine    CCTK_GetVarNum
   @date       Mon Feb  8 12:03:22 1999
   @author     Tom Goodale
   @desc 
   Gets the global number associated wth a variable.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_GetVarNum(const char *implementation,
	           const char *group_name,
		   const char *variable_name)
{
  int retval;
  int gnum,group_num;
  int variable;

  retval = -1;

  if (group_name == NULL)
  {	
    for (gnum = 0; gnum < n_groups; gnum++)
    {			
      
      for(variable=0; variable<groups[gnum].n_variables;variable++)
      {
	if(CCTK_Equals(variable_name, groups[gnum].variables[variable].name)
	   && CCTK_Equals(implementation,groups[gnum].implementation))
	{
	  retval  = groups[gnum].variables[variable].number;
	  break;
	}
      }
    }
  } 
  else

  {
    group_num = CCTK_GetGroupNum(implementation, group_name);
    
    if(group_num > -1)
    {
      for(variable=0; variable<groups[group_num].n_variables;variable++)
      {
	if(CCTK_Equals(variable_name, groups[group_num].variables[variable].name))
	{
	  retval  = groups[group_num].variables[variable].number;
	  break;
	}
      }
    }
    else
    {
      retval = -2;
    }
  }

  return retval;
}

 /*@@
   @routine    CCTK_GetMaxDim
   @date       Mon Feb  8 12:04:01 1999
   @author     Tom Goodale
   @desc 
   Gets the maximum dimension of all groups.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_GetMaxDim(void)
{
  return maxdim;
}


 /*@@
   @routine    CCTK_GetNumVars
   @date       Mon Feb  8 12:04:50 1999
   @author     Tom Goodale
   @desc 
   Gets the total number of variables.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_GetNumVars(void)
{
  return total_variables;
}

 /*@@
   @routine    CCTK_GetNumGroups
   @date       Mon Feb  8 12:04:50 1999
   @author     Tom Goodale
   @desc 
   Gets the total number of groups.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_GetNumGroups(void)
{
  return n_groups;
}

 /*@@
   @routine    CCTK_GetGroupFromVar
   @date       Mon Feb 22
   @author     Gabrielle Allen
   @desc 
   Given a variable index return a group name.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
char *CCTK_GetGroupFromVar(int var)
{
  char *retval;
  int group_num;

  if (var<0 || var>total_variables-1)
  {
    retval = NULL;
  }
  else
  {  
    group_num = group_of_variable[var];
    retval = groups[group_num].name;
  } 

  return retval;
}

 /*@@
   @routine    CCTK_GetImplementationFromVar
   @date       Mon Feb 22
   @author     Gabrielle Allen
   @desc 
   Given a variable index return a implementation name.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
char *CCTK_GetImplementationFromVar(int var)
{
  char *retval;
  int group_num;

  if (var<0 || var>total_variables-1)
  {
    retval = NULL;
  }
  else
  {
    group_num = group_of_variable[var];
    retval = groups[group_num].implementation;
  }

  return retval;
}

 /*@@
   @routine    CCTK_GetFullName(i)
   @date       Mon Feb 22
   @author     Gabrielle Allen
   @desc 
   Given a variable index return the implementation 
   and the variable name together.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
char *CCTK_GetFullName(int var)
{
  char *impname;
  char *varname;
  int group_num;
  char *fullname=NULL;

  varname = CCTK_GetVarName(var);
  if (varname)
  {
    group_num = group_of_variable[var];
    impname = groups[group_num].implementation;

    fullname = malloc((strlen(varname)+strlen(impname)+3)*sizeof(char));
    if (fullname)
      sprintf(fullname,"%s::%s",impname,varname);
  }
  else
  {  
    fullname = NULL;
  }

  return fullname;
}


 /*@@
   @routine    CCTK_ArrayGroupSize
   @date       Mon Feb  8 12:05:50 1999
   @author     Tom Goodale
   @desc 
   Gets the size of a array in a specific direction.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int *CCTK_ArrayGroupSize(cGH *GH, const char *group, int dir)
{
  /* Quick fudge */
  return &(GH->local_shape[dir]);
}

 /*@@
   @routine    CCTK_QueryGroupStorage
   @date       
   @author     Tom Goodale
   @desc 

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_QueryGroupStorage(cGH *GH, const char *group)
{
  /* Quick fudge */  
  return 1;
}


 
 /*@@
   @routine    CCTK_GTypeNumber
   @date       Mon Feb  8 14:44:45 1999
   @author     Tom Goodale
   @desc 
   Gets the type number associated with a group.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_GTypeNumber(const char *type)
{
  int retval;

  if(!strcmp(type, "SCALAR"))
  {
    retval = GROUP_SCALAR;
  }

  if(!strcmp(type, "GF"))
  {
    retval = GROUP_GF;
  }

  if(!strcmp(type, "ARRAY"))
  {
    retval = GROUP_ARRAY;
  }

  return retval;
}

 /*@@
   @routine    CCTK_VTypeNumber
   @date       Mon Feb  8 14:44:45 1999
   @author     Tom Goodale
   @desc 
   Gets the type number associated with a variable.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_VTypeNumber(const char *type)
{
  int retval;

  if(!strcmp(type, "INTEGER"))
  {
    retval = VARIABLE_INTEGER;
  }

  if(!strcmp(type, "REAL"))
  {
    retval = VARIABLE_REAL;
  }

  if(!strcmp(type, "COMPLEX"))
  {
    retval = VARIABLE_COMPLEX;
  }

  if(!strcmp(type, "CHAR"))
  {
    retval = VARIABLE_CHAR;
  }

  return retval;
}


 /*@@
   @routine    CCTK_GetGroupData
   @date       Mon Feb  8 15:56:01 1999
   @author     Tom Goodale
   @desc 
   Gets the group type, the variable type, and the number of variables
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_GetGroupData(int group, 
		      int *gtype, 
		      int *vtype, 
		      int *dim, 
		      int *n_variables,
		      int *n_timelevels)
{
  int return_code;

  if(group >=0 && group < n_groups)
  {
    *gtype = groups[group].gtype;
    *vtype = groups[group].vtype;
    *dim   = groups[group].dim;
    *n_variables = groups[group].n_variables;
    *n_timelevels = groups[group].n_timelevels;

    return_code = 1;
  }
  else
  {
    return_code = 0;
  }

  return return_code;
}

 /*@@
   @routine    CCTK_GetVarName
   @date       Tue Feb  9 15:34:56 1999
   @author     Tom Goodale
   @desc 
   Gets the name of a variable.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
char *CCTK_GetVarName(int varnum)
{
  char *name;
  int group;

  if(varnum < total_variables)
  {
    group = group_of_variable[varnum];
    name  = groups[group].variables[varnum-groups[group].variables[0].number].name;
  }
  else
  {
    name = NULL;
  }

  return name;
}


 /*@@
   @routine    CCTK_DecomposeGroupName
   @date       Tue Feb  9 15:39:14 1999
   @author     Tom Goodale
   @desc 
   Decomposes a group name of the form imp::group
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_DecomposeGroupName(const char *fullname, char **implementation, char **name)
{
  return CCTK_SplitString(implementation, name, fullname, "::");
}



char *CCTK_GetGroupName(int group)
{
  char *name;

  if (group < 0 || group >= n_groups) 
  {
    name = NULL;
  }
  else
  {
    name = malloc((strlen(groups[group].implementation)+strlen(groups[group].name)+3)*sizeof(char));
    if (name)
    {
      sprintf(name, "%s::%s",groups[group].implementation, groups[group].name);
    }
    else
    {
      name = NULL;
    }
  }
  return name;
}

int CCTK_GetFirstVarNum(int group)
{
  if (0 <= group && group<n_groups)
  {
    return groups[group].variables[0].number;
  }
  else
  {
    return -1;
  }
}

int CCTK_GetVarGType(int var)
{
  int gtype;
  int group;

  if(var < total_variables)
  {
    group = group_of_variable[var];

    gtype = groups[group].gtype;
  }
  else
  {
    gtype = -1;
  }

  return gtype;
}

int CCTK_GetVarVType(int var)
{
  int vtype;
  int group;

  if(var < total_variables)
  {
    group = group_of_variable[var];

    vtype = groups[group].vtype;
  }
  else
  {
    vtype = -1;
  }

  return vtype;
}

int CCTK_GetNumTimeLevels(int var)
{
  int ntimelevels;
  int group;

  if(var < total_variables)
  {
    group = group_of_variable[var];
    ntimelevels = groups[group].n_timelevels;
  }
  else
  {
    ntimelevels = -1;
  }
  
  return ntimelevels;
}
  
