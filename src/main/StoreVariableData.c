 /*@@
   @file      StoreVariableData.c
   @date      Thu Jan 14 15:36:40 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
 @@*/

#include <stdlib.h>
#include <string.h>

#include "flesh.h"
#include "StoreNamedData.h"

static n_groups = 0;
static cGroupDefinition *groups = NULL;

static int total_variables;

static int *group_of_variable = NULL;


 /*@@
   @routine    CCTK_SetupGroup
   @date       Thu Jan 14 16:38:40 1999
   @author     Tom Goodale
   @desc 
   
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
  int variable;

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
    return &(groups[n_groups-1]);
  }
  else
  {
    return NULL;
  }

}

int CCTK_GetGroupNumber(const char *implementation,
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

int CCTK_GetVariableNumber(const char *implementation,
			   const char *group_name,
			   const char *variable_name)
{
  int retval;
  int group_num;
  int variable;

  retval = -1;

  group_num = CCTK_GetGroupNumber(implementation, group_name);

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
