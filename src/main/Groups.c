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
		     int n_variables,
		     ...)
{
  int retval;

  va_list ap;
  char *position;

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
   @routine    CCTK_GetGroupNumb
   @date       Fri Jan 29 08:43:48 1999
   @author     Tom Goodale
   @desc 
   Gets the number for the specified group.
   @enddesc 
   @calls     
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
    if(!strcmp(implementation, groups[group_num].implementation) &&
       !strcmp(name, groups[group_num].name)) break;
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
  int group_num;
  int variable;

  retval = -1;

  group_num = CCTK_GetGroupNum(implementation, group_name);

  if(group_num > -1)
  {
    for(variable=0; variable<groups[group_num].n_variables;variable++)
    {
      if(!strcmp(variable_name, groups[group_num].variables[variable].name))
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

  return retval;
}


 /*@@
   @routine    CCTK_GetMaxDim
   @date       Mon Feb  8 12:04:01 1999
   @author     Tom Goodale
   @desc 
   Gets the maximum dimesion of all groups.
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
int CCTK_ArrayGroupSize(cGH *GH, const char *group, int dim)
{
  return 0;
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
int CCTK_GetGroupData(int group, int *gtype, int *vtype, int *dim, int *n_variables)
{
  int return_code;

  if(group >=0 && group < n_groups)
  {
    *gtype = groups[group].gtype;
    *vtype = groups[group].vtype;
    *dim   = groups[group].dim;
    *n_variables = groups[group].n_variables;

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

  group = group_of_variable[varnum];

  name = groups[group].variables[varnum-groups[group].variables[0].number].name;

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

char *CCTK_GetGroupName(int varnum)
{
  char *name;
  int group;

  group = group_of_variable[varnum];

  name = malloc((strlen(groups[group].implementation)+strlen(groups[group].name)+3)*sizeof(char))
    ;
  sprintf(name, "%s::%s",groups[group].implementation, groups[group].name);

  return name;
}

int CCTK_GetFirstVarNum(int group)
{
  return groups[group].variables[0].number;
}

int CCTK_GetVarGType(int var)
{
  int gtype;
  int group;

  group = group_of_variable[var];

  gtype = groups[group].gtype;

  return gtype;
}

int CCTK_GetVarVType(int var)
{
  int vtype;
  int group;

  group = group_of_variable[var];

  vtype = groups[group].vtype;

  return vtype;
}
