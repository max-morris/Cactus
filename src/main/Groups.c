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

#include "cctk_Constants.h"
#include "cctk_Config.h"
#include "cctk_WarnLevel.h"
#include "cctk_Flesh.h"
#include "cctk_FortranString.h"
#include "cctk_Groups.h"
#include "cctk_Parameter.h"
#include "cctk_Types.h"

#include "cctki_Stagger.h"

/*#define DEBUG_GROUPS*/

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_Groups_c)

/* Typedefs. */
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

  int dtype;

  int dim;

  int n_timelevels;

  int n_variables;

  int staggertype;

  /* *size[dim]  - pointers to parameter data*/
  CCTK_INT **size;

  /* *ghostsize[dim]  - pointers to parameter data*/
  CCTK_INT **ghostsize;

  /* variables[n_variables] */
  cVariableDefinition *variables;
} cGroupDefinition;

/* Static variables needed to hold group and variable data. */

static int n_groups = 0;
static cGroupDefinition *groups = NULL;

static int total_variables = 0;

static int *group_of_variable = NULL;

static int maxdim = 0;

static int staggered = 0;

/* When passing to fortran, must pass by reference
 * so need to define the odd global variable to pass 8-(
 */

int _cctk_one = 1;


static cGroupDefinition *CCTKi_SetupGroup(const char *implementation, 
                                          const char *name, 
                                          int staggercode,
                                          int n_variables);

static CCTK_INT **CCTKi_ExtractSize(int dimension, const char *thorn, const char *sizestring);

 /*@@
   @routine    CCTK_StaggerVars
   @date       
   @author     Gerd Lanfermann
   @desc 

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_StaggerVars(void) 
{
  return(staggered);
}




 /*@@
   @routine    CCTK_GroupIndex
   @date       Fri Jan 29 08:43:48 1999
   @author     Tom Goodale
   @desc 
   Gets the index number for the specified group.
   @enddesc 
   @calls CCTK_Equals   
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_GroupIndex(const char *fullgroupname)
{
  int group_num;
  int retval=-1;
  char *imp1         = NULL;
  char *group1       = NULL;
  const char *imp2   = NULL;
  const char *group2 = NULL;
  
  switch(CCTK_DecomposeName(fullgroupname,&imp1,&group1))
  {
  case 1:

    CCTK_VWarn(2,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupIndex: Group name %s not in format implementation::group",
	       fullgroupname);
    retval = -3;
    break;

  case 2:

    CCTK_Warn(2,__LINE__,__FILE__,"Cactus","CCTK_GroupIndex: Memory allocation failed");
    retval = -4;
    break;

  default:

    imp2 = (const char *)imp1;
    group2 = (const char *)group1;

    for(group_num = 0; group_num < n_groups; group_num++)
    {
      if(CCTK_Equals(imp2, groups[group_num].implementation) &&
         CCTK_Equals(group2, groups[group_num].name)) break;
    }

    if (group_num < n_groups)
    {
      retval = group_num;
    }
    else
    {
      CCTK_VWarn(2,__LINE__,__FILE__,"Cactus", 
		 "CCTK_GroupIndex: No group %s found", 
		 fullgroupname);
      retval = -1;
    }
  }

  /* Free memory from CCTK_DecomposeName */
  if (imp1) free(imp1);
  if (group1) free(group1);
 
  return retval;

}


void  CCTK_FCALL CCTK_FNAME(CCTK_GroupIndex)
     (int *index,ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name)
  *index = CCTK_GroupIndex(name);
  free(name);
}

/*@@
   @routine    CCTKi_PrintGroupInfo
   @date       Thu Jan 14 15:25:54 1999
   @author     Gerd Lanfermann
   @desc
     Debugging info on the Groups.
   @enddesc
   @calls
   @calledby
   @history

   @endhistory

@@*/

void CCTKi_PrintGroupInfo(void) {
  int group_num;

  for(group_num = 0; group_num < n_groups; group_num++) {
    printf("GROUP INFO: GrpNo./imp_name/name/stag %d   >%s<   >%s<  %d\n",
           group_num,
           groups[group_num].implementation,
           groups[group_num].name,
           groups[group_num].staggertype);
  }
}


 /*@@
   @routine    CCTKi_CreateGroup
   @date       Thu Jan 14 15:25:54 1999
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTKi_CreateGroup(const char *gname, 
		      const char *thorn, 
		      const char *imp,
		      const char *gtype,
		      const char *vtype,
		      const char *gscope,
		      int         dimension,
		      int         ntimelevels,
		      const char *stype,
		      const char *dtype,
		      const char *size,
		      const char *ghostsize,
		      int         n_variables,
		      ...
		      )
{
  int     retval;
  int     groupscope;
  int     staggercode;
  int     variable;

  va_list ap;

  char*   variable_name;

  cGroupDefinition* group=NULL;

  retval = 0;

  /* get the staggercode */
  staggercode = CCTKi_ParseStaggerString(dimension, imp, gname, stype);

  /* Allocate storage for the group */
  groupscope = CCTK_GroupScopeNumber(gscope);
  if (groupscope == CCTK_PUBLIC || groupscope == CCTK_PROTECTED)
  {
    group = CCTKi_SetupGroup(imp, gname, staggercode, n_variables);
  }
  else if (groupscope == CCTK_PRIVATE)
  {
    group = CCTKi_SetupGroup(thorn, gname, staggercode, n_variables);
  }
  else
  {
    CCTK_Warn(1,__LINE__,__FILE__,"Cactus",
	      "CCTKi_CreateGroup: Unrecognised group scope");
  }

  /* Allocate storage for the group and setup some stuff. */
  if(group)
  {
    group->dim          = dimension;
    group->gtype        = CCTK_GroupTypeNumber(gtype);
    group->vtype        = CCTK_VarTypeNumber(vtype);
    group->gscope       = groupscope;
    group->staggertype  = staggercode;
    group->dtype        = CCTK_GroupDistribNumber(dtype);
    group->n_timelevels = ntimelevels;
    
    /* Extract the variable names from the argument list. */
    va_start(ap, n_variables);

    for(variable = 0; variable < n_variables; variable++)
    {
      variable_name = va_arg(ap, char *);

      group->variables[variable].name = 
	(char *)malloc((strlen(variable_name)+1*sizeof(char)));
      
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
      if (dimension > maxdim) 
      {
	maxdim    = dimension;
      }
      if (staggercode > 0)    
      {
	staggered = 1;
      }
      group->size      = CCTKi_ExtractSize(dimension, thorn, size);
      group->ghostsize = CCTKi_ExtractSize(dimension, thorn, ghostsize);
    }
  }
  else
  {
    retval = 2;
  }

  if(retval)
  {
    CCTK_Warn(4,__LINE__,__FILE__,"Cactus","CCTK_CreateGroup: Error");
  }

  return retval;

}

 /*@@
   @routine    CCTKi_SetupGroup
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
static cGroupDefinition *CCTKi_SetupGroup(const char *implementation, 
                                          const char *name,
                                          int staggercode,
                                          int n_variables)
{
  int *temp_int;
  cGroupDefinition *temp;
  cGroupDefinition *returndata;
  int variable;
  int group_num;
  char *fullname1;

  fullname1 = (char *) malloc( (strlen(implementation)+strlen(name)+3)
			       *sizeof(char));
  sprintf(fullname1,"%s::%s",implementation,name); 

  if((group_num = CCTK_GroupIndex(fullname1)) == -1)
  {
    /* Resize the array of groups */
    if((temp = (cGroupDefinition *)realloc(groups, (n_groups+1)*sizeof(cGroupDefinition))))
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
        
        groups[n_groups].number     = n_groups;
        groups[n_groups].staggertype= staggercode;
        groups[n_groups].n_variables= n_variables;
        
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

#ifdef DEBUG_GROUPS
  printf("Setting up group %s\n",fullname1);
#endif 

  if (fullname1) free(fullname1);

  return returndata;
}




 /*@@
   @routine    CCTK_VarIndex
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

int CCTK_VarIndex(const char *variable_name)
{
  int retval;
  int gnum;
  int variable;
  int ierr;
  char *realimpname;
  char *realvarname;
  const char *impname;
  const char *varname;

  retval = -1;

  ierr = CCTK_DecomposeName(variable_name,&realimpname,&realvarname);

  if (ierr == 0)
  {

    /* Store the pointers to these strings in const char *s */
    impname = realimpname;
    varname = realvarname;

    for (gnum = 0; gnum < n_groups; gnum++)
    {                   
    
      for(variable=0; variable<groups[gnum].n_variables;variable++)
      {
        if(CCTK_Equals(varname, groups[gnum].variables[variable].name)
           && CCTK_Equals(impname,groups[gnum].implementation))
        {
          retval  = groups[gnum].variables[variable].number;
          break;
        }
      }
    }

  }
  else if (ierr == 1)
  {
    CCTK_VWarn(2,__LINE__,__FILE__,"Cactus",
            "CCTK_VarIndex: Full name %s in wrong format", variable_name);
    retval = -3; 
  }
  else if (ierr == 2)
  {
    CCTK_Warn(2,__LINE__,__FILE__,"Cactus","CCTK_VarIndex: Memory allocation failed");
    retval = -4;
  }
  else
  {
    CCTK_Warn(1,__LINE__,__FILE__,"Cactus","CCTK_VarIndex: Error failed to be caught");
  }

#ifdef DEBUG_GROUPS
  printf(" In VarIndex\n"," ------------\n");
  printf("   impname -%s-\n",impname);
  printf("   varname -%s-\n",varname);
#endif
    

  if (realimpname) free(realimpname);
  if (realvarname) free(realvarname);
    
  return retval;

}

void  CCTK_FCALL CCTK_FNAME(CCTK_VarIndex)
     (int *index,ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name)
  *index = CCTK_VarIndex(name);
  free(name);
}


 /*@@
   @routine    CCTK_MaxDim
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
int CCTK_MaxDim(void)
{
  return maxdim;
}

void  CCTK_FCALL CCTK_FNAME(CCTK_MaxDim)
     (int *dim)
{
  *dim = CCTK_MaxDim();
}


 /*@@
   @routine    CCTK_NumVars
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
int CCTK_NumVars(void)
{
  return total_variables;
}

void  CCTK_FCALL CCTK_FNAME(CCTK_NumVars)
     (int *total_variables)
{
  *total_variables = CCTK_NumVars();
}


 /*@@
   @routine    CCTK_NumGroups
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
int CCTK_NumGroups(void)
{
  return n_groups;
}

void  CCTK_FCALL CCTK_FNAME(CCTK_NumGroups)
     (int *n_groups)
{
  *n_groups = CCTK_NumGroups();
}


 /*@@
   @routine    CCTK_GroupNameFromVarI
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

char *CCTK_GroupNameFromVarI(int var)
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
    retval = (char *)malloc( ( strlen(groups[group_num].name) 
                             + strlen(groups[group_num].implementation) + 3)
                             * sizeof(char) );
    sprintf(retval,"%s::%s",groups[group_num].implementation,
            groups[group_num].name);
  } 

  return retval;
}


 /*@@
   @routine    CCTK_GroupIndexFromVarI
   @date       Mon Feb 22
   @author     Gabrielle Allen
   @desc 
   Given a variable index return a group index.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_GroupIndexFromVarI(int var)
{
  int retval;

  if (var<0 || var>total_variables-1)
  {
    retval = -1;
  }
  else
  {  
    retval = group_of_variable[var];
  } 

  return retval;

}

void  CCTK_FCALL CCTK_FNAME(CCTK_GroupIndexFromVarI)
     (int *gindex,int *var)
{
  *gindex = CCTK_GroupIndexFromVarI(*var);
}



 /*@@
   @routine    CCTK_GroupIndexFromVar
   @date       Mon Feb 22
   @author     Gabrielle Allen
   @desc 
   Given a variable name returns a group index.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_GroupIndexFromVar(const char *var)
{
  return CCTK_GroupIndexFromVarI(CCTK_VarIndex(var));
}

void  CCTK_FCALL CCTK_FNAME(CCTK_GroupIndexFromVar)
     (int *index,ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(var)
  *index = CCTK_GroupIndexFromVar(var);
  free(var);
}


 /*@@
   @routine    CCTK_ImpFromVarI
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
const char *CCTK_ImpFromVarI(int var)
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
   @routine    CCTK_FullName
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

char *CCTK_FullName(int var)
{
  char *impname;
  const char *varname;
  int group_num;
  char *fullname=NULL;

  varname = CCTK_VarName(var);
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
   @routine    CCTK_GroupTypeNumber
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

int CCTK_GroupTypeNumber(const char *type)
{
  int retval;

  retval = -1;

  if(!strcmp(type, "SCALAR"))
  {
    retval = CCTK_SCALAR;
  }

  if(!strcmp(type, "GF"))
  {
    retval = CCTK_GF;
  }

  if(!strcmp(type, "ARRAY"))
  {
    retval = CCTK_ARRAY;
  }

  return retval;
}

void  CCTK_FCALL CCTK_FNAME(CCTK_GroupTypeNumber)
     (int *number,ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(type)
  *number = CCTK_GroupTypeNumber(type);
  free(type);
}


 /*@@
   @routine    CCTK_VarTypeNumber
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

int CCTK_VarTypeNumber(const char *type)
{
  int retval;

  retval = -1;

  if(!strcmp(type, "INT"))
  {
    retval = CCTK_VARIABLE_INT;
  }

  if(!strcmp(type, "INT2"))
  {
    retval = CCTK_VARIABLE_INT2;
  }

  if(!strcmp(type, "INT4"))
  {
    retval = CCTK_VARIABLE_INT4;
  }

  if(!strcmp(type, "INT8"))
  {
    retval = CCTK_VARIABLE_INT8;
  }

  if(!strcmp(type, "REAL"))
  {
    retval = CCTK_VARIABLE_REAL;
  }

  if(!strcmp(type, "REAL4"))
  {
    retval = CCTK_VARIABLE_REAL4;
  }

  if(!strcmp(type, "REAL8"))
  {
    retval = CCTK_VARIABLE_REAL8;
  }

  if(!strcmp(type, "REAL16"))
  {
    retval = CCTK_VARIABLE_REAL16;
  }

  if(!strcmp(type, "COMPLEX"))
  {
    retval = CCTK_VARIABLE_COMPLEX;
  }

  if(!strcmp(type, "CHAR"))
  {
    retval = CCTK_VARIABLE_CHAR;
  }

  return retval;
}

void  CCTK_FCALL CCTK_FNAME(CCTK_VarTypeNumber)
     (int *number,ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(type)
  *number = CCTK_VarTypeNumber(type);
  free(type);
}

 /*@@
   @routine    CCTK_VarTypeName
   @date       Mon Jan  3 13:50:56 CET 2000
   @author     Gabrielle Allen
   @desc
   Gets the variable type name associated with a variable type number.
   @enddesc
   @calls
   @calledby
   @history

   @endhistory

@@*/
const char *CCTK_VarTypeName(int vtype)
{
  char *retval;

  switch(vtype)
  {
    case CCTK_VARIABLE_INT:
      retval = "CCTK_VARIABLE_INT";
      break;

    case CCTK_VARIABLE_INT2:
      retval = "CCTK_VARIABLE_INT2";
      break;

    case CCTK_VARIABLE_INT4:
      retval = "CCTK_VARIABLE_INT4";
      break;

    case CCTK_VARIABLE_INT8:
      retval = "CCTK_VARIABLE_INT8";
      break;

    case CCTK_VARIABLE_REAL:
      retval = "CCTK_VARIABLE_REAL";
      break;

    case CCTK_VARIABLE_REAL4:
      retval = "CCTK_VARIABLE_REAL4";
      break;

    case CCTK_VARIABLE_REAL8:
      retval = "CCTK_VARIABLE_REAL8";
      break;

    case CCTK_VARIABLE_COMPLEX:
      retval = "CCTK_VARIABLE_COMPLEX";
      break;

    case CCTK_VARIABLE_CHAR:
      retval = "CCTK_VARIABLE_CHAR";
      break;

    case CCTK_VARIABLE_STRING:
      retval = "CCTK_VARIABLE_STRING";
      break;

    default:
      retval = NULL;
      break;
  }

  return retval;

}


 /*@@
   @routine    CCTK_GroupScopeNumber
   @date       Tuesday June 22 1999
   @author     Gabrielle Allen
   @desc 
   Gets the scope number associated with a group.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_GroupScopeNumber(const char *type)
{
  int retval;

  retval = -1;
 
  if(!strcmp(type, "PRIVATE"))
  {
    retval = CCTK_PRIVATE;
  }

  if(!strcmp(type, "PROTECTED"))
  {
    retval = CCTK_PROTECTED;
  }

  if(!strcmp(type, "PUBLIC"))
  {
    retval = CCTK_PUBLIC;
  }

  return retval;
}

void  CCTK_FCALL CCTK_FNAME(CCTK_GroupScopeNumber)
     (int *number,ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(type)
  *number = CCTK_GroupScopeNumber(type);
  free(type);
}


 /*@@
   @routine    CCTK_GroupScopeNumber
   @date       Tuesday June 22 1999
   @author     Gabrielle Allen
   @desc 
   Gets the distribution number associated with a group.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_GroupDistribNumber(const char *dtype)
{
  int retval;

  retval = -1;
 
  if(!strcmp(dtype, "CONSTANT"))
  {
    retval = CCTK_DISTRIB_CONSTANT;
  }

  if(!strcmp(dtype, "DEFAULT"))
  {
    retval = CCTK_DISTRIB_DEFAULT;
  }

  return retval;
}

void  CCTK_FCALL CCTK_FNAME(CCTK_GroupDistribNumber)
     (int *number,ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(type)
  *number = CCTK_GroupDistribNumber(type);
  free(type);
}


 /*@@
   @routine    CCTK_GroupData
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


int CCTK_GroupData(int group, cGroup *gp)
{
  int retval=0;

  if(group >=0 && group < n_groups)
  {
      gp->grouptype     = groups[group].gtype;
      gp->vartype       = groups[group].vtype;
      gp->disttype      = groups[group].dtype;
      gp->dim           = groups[group].dim;
      gp->numvars       = groups[group].n_variables;
      gp->numtimelevels = groups[group].n_timelevels;
      gp->stagtype      = groups[group].staggertype;
  }
  else
  {
    gp = NULL;
    retval = -1;
  }

  return retval;

}


 /*@@
   @routine    CCTK_VarName
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

const char *CCTK_VarName(int varnum)
{
  char *name;
  int group;

  if (varnum<0 || varnum>total_variables-1)
  {
    name = NULL;
  }
  else
  {
    group = group_of_variable[varnum];
    name  = groups[group].variables[varnum-groups[group].variables[0].number].name;
  }

  return name;
}


 /*@@
   @routine    CCTK_DecomposeName
   @date       Tue Feb  9 15:39:14 1999
   @author     Tom Goodale
   @desc 
   Decomposes a group name of the form imp::name
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_DecomposeName(const char *fullname, char **implementation, char **name)
{
  return Util_SplitString(implementation, name, fullname, "::");
}


 /*@@
   @routine    CCTK_GroupName
   @date       Tue Apr  9 15:39:14 1999
   @author     Gabrielle Allen
   @desc 
   Given a group index returns the group name
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

char *CCTK_GroupName(int group)
{
  char *name;

  if (group < 0 || group >= n_groups) 
  {
    name = NULL;
  }
  else
  {
    name = (char *)malloc((strlen(groups[group].implementation)
                           +strlen(groups[group].name)+3)*sizeof(char));
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


 /*@@
   @routine    CCTK_FirstVarIndexI
   @date       
   @author     Gabrielle Allen
   @desc 
   Given a group index returns the first variable index in the group
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_FirstVarIndexI(int group)
{
  int retval;

  if (0 <= group && group<n_groups)
  {
    retval = groups[group].variables[0].number;
  }
  else
  {
    retval = -1;
  }

  return retval;
}

void  CCTK_FCALL CCTK_FNAME(CCTK_FirstVarIndexI)
     (int *first, int *group)
{
  *first = CCTK_FirstVarIndexI(*group);
}


int CCTK_FirstVarIndex(const char *groupname)
{
  return CCTK_FirstVarIndexI(CCTK_GroupIndex(groupname));
}


int CCTK_NumVarsInGroupI(int group)
{
  int retval;

  if (0 <= group && group<n_groups)
  {
    retval =  groups[group].n_variables;
  }
  else
  {
    retval = -1;
  }

  return retval;
}


 /*@@
   @routine    CCTK_NumVarsInGroup
   @date       
   @author     Gabrielle Allen
   @desc 
   Given a group name returns the number of variables in the group
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_NumVarsInGroup(const char *groupname)
{
  return CCTK_NumVarsInGroupI(CCTK_GroupIndex(groupname));
}

void  CCTK_FCALL CCTK_FNAME(CCTK_NumVarsInGroup)
     (int *num,ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(groupname)
  *num = CCTK_NumVarsInGroup(groupname);
  free(groupname);
}


 /*@@
   @routine    CCTK_GroupTypeFromVarI
   @date       
   @author     
   @desc 
   Given a variable index return the type of group
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_GroupTypeFromVarI(int var)
{
  int gtype;
  int group;

  if (var<0 || var>total_variables-1)
  {
    gtype = -1;
  }
  else
  {
    group = group_of_variable[var];

    gtype = groups[group].gtype;
  }

#ifdef DEBUG_GROUPS

  printf("\nIn CCTK_GroupTypeFromVarI\n");
  printf("-------------------------\n");
  printf("Variable index = %d\n",var);
  printf("Variable name = %s\n",CCTK_FullName(var));
  printf("Group type = %d\n\n",gtype);

#endif

  return gtype;
}

 /*@@
   @routine    CCTK_GroupTypeI
   @date       
   @author     
   @desc 
   Given a group index return the type of group
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_GroupTypeI(int group)
{
  return groups[group].gtype;
}


 /*@@
   @routine    CCTK_VarTypeI
   @date       
   @author     
   @desc 
   Given a variable index return the variable type
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_VarTypeI(int var)
{
  int vtype;
  int group;

  if (var<0 || var>total_variables-1)
  {
    vtype = -1;
  }
  else
  {
    group = group_of_variable[var];

    vtype = groups[group].vtype;
  }

  return vtype;
}

void  CCTK_FCALL CCTK_FNAME(CCTK_VarTypeI)
     (int *type,int *var)
{
  *type = CCTK_VarTypeI(*var);
}


 /*@@
   @routine    CCTK_NumTimeLevelsI
   @date       
   @author     
   @desc 
   Given a variable index return the number of timelevels
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_NumTimeLevelsFromVarI(int var)
{
  int ntimelevels;
  int group;

  if (var<0 || var>total_variables-1)
  {
    ntimelevels = -1;
  }
  else
  {
    group = group_of_variable[var];
    ntimelevels = groups[group].n_timelevels;
  }
  
  return ntimelevels;
}
  
void  CCTK_FCALL CCTK_FNAME(CCTK_NumTimeLevelsFromVarI)
     (int *num,int *var)
{
  *num = CCTK_NumTimeLevelsFromVarI(*var);
}


 /*@@
   @routine    CCTK_NumTimeLevelsFromVar
   @date       
   @author     
   @desc 
   Given a variable name return the number of timelevels
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_NumTimeLevelsFromVar(const char *var)
{
  return CCTK_NumTimeLevelsFromVarI(CCTK_VarIndex(var));
}

void  CCTK_FCALL CCTK_FNAME(CCTK_NumTimeLevelsFromVar)
     (int *num,ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(var)
  *num = CCTK_NumTimeLevelsFromVar(var);
  free(var);
}


 /*@@
   @routine    CCTK_PrintGroup
   @date       3 July 1999
   @author     Gabrielle Allen
   @desc 
   Given a group index print the group name. This is for debugging
   purposes for Fortran routines.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

void CCTK_PrintGroup(int group)
{
  fprintf(stdout,"Group %d is %s\n",group,CCTK_GroupName(group));
}

void  CCTK_FCALL CCTK_FNAME(CCTK_PrintGroup)
     (int *group)
{
  CCTK_PrintGroup(*group);
}


 /*@@
   @routine    CCTK_PrintVar
   @date       3 July 1999
   @author     Gabrielle Allen
   @desc 
   Given a group index print the variable name. This is for debugging
   purposes for Fortran.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

void CCTK_PrintVar(int var)
{
  fprintf(stdout,"Variable %d is %s\n",var,CCTK_VarName(var));
}

void  CCTK_FCALL CCTK_FNAME(CCTK_PrintVar)
     (int *var)
{
  CCTK_PrintGroup(*var);
}

 /*@@
   @routine    CCTKi_ExtractSize
   @date       Sun Nov 28 12:38:38 1999
   @author     Tom Goodale
   @desc 
   Extracts the size array from a comma-separated list.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static CCTK_INT **CCTKi_ExtractSize(int dimension, 
				    const char *thorn, 
				    const char *sizestring)
{
  int         i;
  int         type;
  CCTK_INT    *this_size;
  CCTK_INT  **size_array;
  const char *last_comma;
  const char *next_comma;  
  char        tmp[200];

  if(strlen(sizestring))
  {

    size_array = (CCTK_INT **)malloc(dimension*sizeof(CCTK_INT *));
    
    next_comma = sizestring;
    
    if(size_array)
    {
      for(i=0; i < dimension; i++)
      {
        {
          last_comma = next_comma[0] == ',' ? next_comma+1 : next_comma;
          next_comma = strstr(last_comma, ",");
          
          if(next_comma)
          {
            strncpy(tmp, last_comma, (size_t)(next_comma-last_comma));
            tmp[next_comma-last_comma] = '\0';
          }
          else
          {
            strcpy(tmp, last_comma);
          }

	  this_size = (CCTK_INT *)CCTK_ParameterGet(tmp, thorn, &type);
	  if (this_size)
	  {
	    size_array[i] = this_size;
	  }
	  else
	  {
	    CCTK_VWarn(0,__LINE__,__FILE__,"Cactus",
		      "CCTKi_ExtractSize: %s is not a parameter",tmp);
	  }
        }
      }
    }
  }
  else
  {
    /* No size specified */
    size_array = NULL;
  }

  return size_array;
}


 /*@@
   @routine    CCTK_GroupSizesI
   @date       Sun Nov 28 12:56:44 1999
   @author     Tom Goodale
   @desc 
   Returns the size array for a group.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
CCTK_INT **CCTK_GroupSizesI(int group)
{
  CCTK_INT **sizes;

  if (0 <= group && group<n_groups)
  {
    sizes = groups[group].size;
  }
  else
  {
    sizes = NULL;
  }

  return sizes;
}


 /*@@
   @routine    CCTK_GroupGhostsizesI
   @date       Sun Jan 23 12:56:44 2000
   @author     Gabrielle Allen
   @desc 
   Returns the ghostsize array for a group.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
CCTK_INT **CCTK_GroupGhostsizesI(int group)
{
  CCTK_INT **ghostsizes;

  if (0 <= group && group<n_groups)
  {
    ghostsizes = groups[group].ghostsize;
  }
  else
  {
    ghostsizes = NULL;
  }

  return ghostsizes;
}

 /*@@
   @routine    CCTK_VarTypeSize
   @date       Sun Dec  5 10:08:05 1999
   @author     Gabrielle Allen
   @desc 
   Returns the size of a given variable type
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_VarTypeSize(int vtype)
{

  int var_size=0;

  switch(vtype)
  {
    case CCTK_VARIABLE_CHAR: 
      var_size = sizeof(CCTK_CHAR); 
      break;

    case CCTK_VARIABLE_INT: 
      var_size = sizeof(CCTK_INT) ; 
      break;

#ifdef CCTK_INT2
    case CCTK_VARIABLE_INT2: 
      var_size = sizeof(CCTK_INT2); 
      break;
#endif

#ifdef CCTK_INT4
    case CCTK_VARIABLE_INT4: 
      var_size = sizeof(CCTK_INT4); 
      break;
#endif

#ifdef CCTK_INT8
    case CCTK_VARIABLE_INT8: 
      var_size = sizeof(CCTK_INT8); 
      break;
#endif

    case CCTK_VARIABLE_REAL: 
      var_size = sizeof(CCTK_REAL); 
      break;

#ifdef CCTK_REAL4
    case CCTK_VARIABLE_REAL4: 
      var_size = sizeof(CCTK_REAL4); 
      break;
#endif

#ifdef CCTK_REAL8
    case CCTK_VARIABLE_REAL8: 
      var_size = sizeof(CCTK_REAL8); 
      break;
#endif

#ifdef CCTK_REAL16
    case CCTK_VARIABLE_REAL16: 
      var_size = sizeof(CCTK_REAL16); 
      break;
#endif

    case CCTK_VARIABLE_COMPLEX: 
      var_size = sizeof(CCTK_COMPLEX); 
      break;

    default:
      CCTK_VWarn(0,__LINE__,__FILE__,
		 "Cactus",
		 "CCTK_VarTypeSize: Unknown variable type (%d)",vtype);
      var_size = 1;

  }

  return var_size;

}


/* DEPRECATED: 4.0b6 */

int CCTK_OldGroupData(int group, 
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
   @routine    CCTK_GroupDimI
   @date       Wed Feb 2 2000
   @author     Gabrielle Allen
   @desc 
   Given a group index returns the group dimension
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_GroupDimI(int group)
{
  int retval;

  if (0 <= group && group<n_groups)
  {
    retval = groups[group].dim;
  }
  else
  {
    retval = -1;
  }

  return retval;
}

void  CCTK_FCALL CCTK_FNAME(CCTK_GroupDimI)
     (int *dim, int *group)
{
  *dim = CCTK_GroupDimI(*group);
}

int CCTK_GroupDimFromVarI(int vi)
{
  int retval,group;
  
  group = CCTK_GroupIndexFromVarI(vi);

  if (0 <= group && group<n_groups)
  {
    retval = groups[group].dim;
  }
  else
  {
    retval = -1;
  }

  return retval;
}

void  CCTK_FCALL CCTK_FNAME(CCTK_GroupDimFromVarI)(int *dim, int *vi)
{
  *dim = CCTK_GroupDimFromVarI(*vi);
}


 /*@@
   @routine    CCTK_TraverseString
   @date       Wed 20 Sep 2000
   @author     Thomas Radke
   @desc
               Traverse through all variables and/or groups whose names
               appear in the given string, and call the callback routine
               with those indices and an optstring string appended the
               the variable/group name.
               The special keyword "all" in the string can be used to
               indicate that the callback should be called for all
               variables/groups.
   @enddesc
   @history
   @endhistory
   @var        string
   @vdesc      list of variable and/or group names
   @vtype      const char *
   @vio        in
   @endvar
   @var        callback
   @vdesc      routine to call for every variable and/or group found
   @vtype      int (*) (int index, const char *optstring, void *callback_arg)
   @vio        int
   @endvar
   @var        callback_arg
   @vdesc      an arbitrary argument which gets passed to the callback routine
   @vtype      void *
   @vio        in
   @endvar
   @var        selection
   @vdesc      decides whether group and/or variable names are accepted
               in the string
   @vtype      int
   @vio        in
   @endvar
@@*/

int CCTK_TraverseString (const char *parsestring,
                         void (*callback) (int index,
                                           const char *optstring,
                                           void *callback_arg),
                         void *callback_arg,
                         int selection)
{
  int retval;
  char *before;
  char *after;
  char *splitstring;
  char *optstring;
  int index, first, last;


  if (callback == NULL)
  {
    CCTK_VWarn (1, __LINE__, __FILE__, "Cactus", "No callback given");
    return (-1);
  }

  retval = 0;

  splitstring = (char *) parsestring;
  after = NULL;

  while (splitstring && *splitstring)
  {

    if (Util_SplitString (&before, &after, splitstring, " "))
    {
      before = splitstring;
      if (after)
      {
        free (after);
        after = NULL;
      }
    }

#ifdef DEBUG_GROUPS
    printf ("   String is '%s'\n", splitstring);
    printf ("   Split is '%s' and '%s'\n", before, after);
#endif

    if (strlen (before) > 0)
    {

      optstring = strchr (before, '[');
      if (optstring)
      {
        *optstring = '\0';
      }

      /* See if this name is "<implementation>::<variable>" */
      if (selection == CCTK_VAR || selection == CCTK_GROUP_OR_VAR)
      {
        first = last = CCTK_VarIndex (before);
      }
      else
      {
        first = last = -1;
      }
      if (first < 0)
      {

        /* See if this name is "<implementation>::<group>" */
        if (selection == CCTK_GROUP || selection == CCTK_GROUP_OR_VAR)
        {
          index = CCTK_GroupIndex (before);
        }
        else
        {
          index = -1;
        }
        if (index >= 0)
        {
          /* We have a group so now need all the variables in the group */
          first = CCTK_FirstVarIndexI (index);
          last = first + CCTK_NumVarsInGroupI (index) - 1;
        }
        else if (CCTK_Equals (before, "all")) /* Look for any special tokens */
        {
          first = 0;
          if (selection == CCTK_GROUP)
          {
            last = CCTK_NumGroups () - 1;
          }
          else
          {
            last = CCTK_NumVars () - 1;
          }
        }
        else
        {
          first = last = -1;
        }
      }

      if (optstring)
      {
        *optstring = '[';
      }
      if (first >= 0)
      {
        for (index = first; index <= last; index++)
        {
          (*callback) (index, optstring, callback_arg);
        }
        retval += last - first + 1;
      }
      else
      {
        CCTK_VWarn (1, __LINE__, __FILE__, "Cactus",
                    "Ignoring '%s' in string (invalid token)", before);
      }
    }

    if (before != splitstring)
    {
      free (before);
    }
    if (splitstring != parsestring)
    {
      free (splitstring);
    }
    splitstring = after;
  }

  if (after)
  {
    free (after);
  }

  return (retval);
}
