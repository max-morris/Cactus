 /*@@
   @file      CreateGroup.c
   @date      Thu Jan 14 11:17:00 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "flesh.h"
#include "Misc.h"

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
int CCTK_CreateGroup(const char *fullname,
		     const char *gtype,
		     const char *vtype,
		     int dimension,
		     int n_variables,
		     ...)
{
  int retval;

  va_list ap;
  char *position;
  char *implementation;
  char *group_name;

  char *variable_name;

  cGroupDefinition *group;

  int variable;

  /* Find the name of the implementation and of the group. */
  CCTK_SplitString(&implementation, &group_name, fullname, "::");

  if(implementation)
  {
    /* Allocate storage for the group and setup some stuff. */
    if((group = CCTK_SetupGroup(implementation, group_name, n_variables)))
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
      };
    }
    else
    {
      retval = 2;
    }
  }
  else
  {
    retval = 1;
  }

  if(retval)
  {
    fprintf(stderr, "Error %d in CCTK_CreateGroup\n", retval);
  }

  return retval;

}


/* Need to define constants for all these. */

int CCTK_GTypeNumber(const char *type)
{
  int retval;

  if(!strcmp(type, "SCALAR"))
  {
    retval = 1;
  }

  if(!strcmp(type, "GF"))
  {
    retval = 2;
  }

  if(!strcmp(type, "ARRAY"))
  {
    retval = 3;
  }

  return retval;
}

int CCTK_VTypeNumber(const char *type)
{
  int retval;

  if(!strcmp(type, "INTEGER"))
  {
    retval = 1;
  }

  if(!strcmp(type, "REAL"))
  {
    retval = 2;
  }

  if(!strcmp(type, "LOGICAL"))
  {
    retval = 3;
  }

  if(!strcmp(type, "COMPLEX"))
  {
    retval = 4;
  }

  return retval;
}
