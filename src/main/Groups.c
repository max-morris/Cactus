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

#include "cctk.h"
#include "cctk_Flesh.h"
#include "cctk_FortranString.h"

#include "cctk_Misc.h"
#include "cctk_Groups.h"
#include "cctk_WarnLevel.h"
#include "cctk_ParameterFunctions.h"

/*#define DEBUG_GROUPS*/

static char *rcsid = "$Header$";

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

  int dim;

  int n_timelevels;

  int n_variables;

  int stagger;

  /* *size[dim]  - pointers to parameter data*/
  CCTK_INT **size;

  /* variables[n_variables] */
  cVariableDefinition *variables;
} cGroupDefinition;

/* Static variables needed to hold group and variable data. */

static int n_groups = 0;
static cGroupDefinition *groups = NULL;

static int total_variables = 0;

static int *group_of_variable = NULL;

static int maxdim = 0;

static char staggered = 0;

/* When passing to fortran, must pass by reference
 * so need to define the odd global variable to pass 8-(
 */

int _cctk_one = 1;


static cGroupDefinition *CCTKi_SetupGroup(const char *implementation, 
                                          const char *name, 
                                          int n_variables);

static CCTK_INT **CCTKi_SetupGroupSize(int dimension, const char *thorn, const char *sizestring);


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

    CCTK_WARN(2,"Group name not in correct format implementation::group");
    retval = -3;
    break;

  case 2:

    CCTK_WARN(2,"Memory allocation failed");
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
      /*      char *message;*/
      /*      message = (char *)malloc( (100+strlen(fullgroupname))*sizeof(char) ); */
      /*      sprintf(message,"No group found with the name %s",fullgroupname); */
      CCTK_VWarn(6,__LINE__,__FILE__,CCTK_THORNSTRING, "No group found with the name %s", fullgroupname);
      /*      if (message) free(message); */
      retval = -1;
    }
  }

  /* Free memory from CCTK_DecomposeName */
  if (imp1) free(imp1);
  if (group1) free(group1);
 
  return retval;

}


void  FMODIFIER FORTRAN_NAME(CCTK_GroupIndex)(int *index,ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(name)
  *index = CCTK_GroupIndex(name);
  free(name);
}

/*@@
   @routine    CCTK_DumpGroupInfo
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

void CCTK_DumpGroupInfo(void) {
  int group_num;

  for(group_num = 0; group_num < n_groups; group_num++) {
    printf("GROUP INFO: GrpNo./imp_name/name:  %d   >%s<   >%s<\n",
           group_num,groups[group_num].implementation,groups[group_num].name);
  }
}

 /*@@
   @routine    CCTKi_StaggerCode
   @date       Fri Jan  7 15:59:26 2000
   @author     Gerd Lanfermann
   @desc 
      gets the stagger string and returns a number identifying the 
      staggering. 
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTKi_StaggerCode(int dim, 
		      const char *imp, const char *gname, 
		      const char *stype) {
  int i,m;
  int base  = 1;
  int scode = 0;
  char *hs, *info;
  
  hs     = (char*)malloc ((dim+1) *sizeof (char));

  /* change possible SHORTCUTS into the official notation, allow for dim=6 */
  if 
    (strcmp(stype,"NONE")==0) strncpy(hs,"MMMMMM",dim); 
  else if 
    (strcmp(stype,"CELL")==0) strncpy(hs,"CCCCCC",dim);
  else {
    sprintf(hs,"%s",stype);
  }

  for (i=0;i<dim;i++) {

    switch (hs[i]) 
    {
      case 'M':m=0; break;
      case 'C':m=1; break;
      case 'P':m=2; break;
      default:
        info   = (char*)malloc (256*sizeof(char));
        sprintf(info,
      	      "Unknown stagger type: >%s< for group: >%s::%s< \n",
	      stype,imp,gname);
        CCTK_WARN(1,info);
        free(info);
        return(-1);
    }
    scode+= m*base;
    base  = 3 * base;
  }

  free(hs);
  return(scode);
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
int CCTKi_CreateGroup(const char *gname, const char *thorn, const char *imp,
                      const char *gtype,
                      const char *vtype,
                      const char *gscope,
                      int dimension,
                      int ntimelevels,
                      const char *stype,
                      const char *size,
                      int n_variables,
                      ...)
{
  int retval;
  int groupscope;
  int staggercode;
  va_list ap;
  char *variable_name;

  cGroupDefinition *group=NULL;

  int variable;

  retval = 0;

  /* get the staggercode */
  staggercode = CCTKi_StaggerCode(dimension, imp, gname, stype);

  /* Allocate storage for the group */
  groupscope = CCTK_GroupScopeNumber(gscope);
  if (groupscope == GROUP_PUBLIC || groupscope == GROUP_PROTECTED)
  {
    group = CCTKi_SetupGroup(imp, gname, n_variables);

#ifdef DEBUG_GROUPS
  {
    char *fullname = (char *)malloc( (200+strlen(gname)+strlen(imp))*sizeof(char));
    sprintf(fullname,"%s::%s",imp,gname);
    printf("Created implementation group %s\n",fullname);
    printf("  CCTK_GroupIndex(%s) = %d\n",fullname,
           CCTK_GroupIndex(fullname));
    printf("  CCTK_GroupName(%d) = %s\n",CCTK_GroupIndex(fullname),
           CCTK_GroupName(CCTK_GroupIndex(fullname)));
    free(fullname);
  }
#endif

  }
  else if (groupscope == GROUP_PRIVATE)
  {
    group = CCTKi_SetupGroup(thorn, gname, n_variables);

#ifdef DEBUG_GROUPS
  {
    char *fullname = (char *)malloc( (200+strlen(gname)+strlen(imp))*sizeof(char));
    sprintf(fullname,"%s::%s",thorn,gname);
    printf("Created thorn group %s\n",fullname);
    printf("  CCTK_GroupIndex(%s) = %d\n",fullname,
           CCTK_GroupIndex(fullname));
    printf("  CCTK_GroupName(%d) = %s\n",CCTK_GroupIndex(fullname),
           CCTK_GroupName(CCTK_GroupIndex(fullname)));
    free(fullname);
  }
#endif

  }
  else
  {
    CCTK_WARN(1,"Unrecognised group scope in CCTK_CreateGroup");
  }

  /* Allocate storage for the group and setup some stuff. */
  if(group)
  {
    group->dim    = dimension;
    group->gtype  = CCTK_GroupTypeNumber(gtype);
    group->vtype  = CCTK_VarTypeNumber(vtype);
    group->gscope = groupscope;
    group->stagger= staggercode;

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
      if (dimension > maxdim) maxdim    = dimension;
      if (staggercode > 0)    staggered = 1;

      group->size = CCTKi_SetupGroupSize(dimension, thorn, size);

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
                                          int n_variables)
{
  int *temp_int;
  cGroupDefinition *temp;
  cGroupDefinition *returndata;
  int variable;
  int group_num;
  char *fullname1;
  const char *fullname2;

  fullname1 = (char *) malloc( (strlen(implementation)+strlen(name)+2)
                                    *sizeof(const char *));
  sprintf(fullname1,"%s::%s",implementation,name); 
  fullname2 = (const char *)fullname1;

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
  char *message;
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
    message = (char *)malloc( (100+strlen(variable_name))*sizeof(char) );
    sprintf(message,"Full name %s in wrong format in CCTK_VarNum",
            variable_name);
    CCTK_WARN(2,message);
    if (message) free(message);
    retval = -3; 
  }
  else if (ierr == 2)
  {
    CCTK_WARN(2,"Memory allocation failed");
    retval = -4;
  }
  else
  {
    CCTK_WARN(1,"Error failed to be caught");
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

void  FMODIFIER FORTRAN_NAME(CCTK_VarIndex)(int *index,ONE_FORTSTRING_ARG)
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

void  FMODIFIER FORTRAN_NAME(CCTK_MaxDim)(int *dim)
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

void  FMODIFIER FORTRAN_NAME(CCTK_NumVars)(int *total_variables)
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

void  FMODIFIER FORTRAN_NAME(CCTK_NumGroups)(int *n_groups)
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

void  FMODIFIER FORTRAN_NAME(CCTK_GroupIndexFromVarI)(int *gindex,int *var)
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

void  FMODIFIER FORTRAN_NAME(CCTK_GroupIndexFromVar)(int *index,ONE_FORTSTRING_ARG)
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
char *CCTK_ImpFromVarI(int var)
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
  char *varname;
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

void  FMODIFIER FORTRAN_NAME(CCTK_GroupTypeNumber)(int *number,ONE_FORTSTRING_ARG)
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

void  FMODIFIER FORTRAN_NAME(CCTK_VarTypeNumber)(int *number,ONE_FORTSTRING_ARG)
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
char *CCTK_VarTypeName(int vtype)
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
    retval = GROUP_PRIVATE;
  }

  if(!strcmp(type, "PROTECTED"))
  {
    retval = GROUP_PROTECTED;
  }

  if(!strcmp(type, "PUBLIC"))
  {
    retval = GROUP_PUBLIC;
  }

  return retval;
}

void  FMODIFIER FORTRAN_NAME(CCTK_GroupScopeNumber)(int *number,ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(type)
  *number = CCTK_GroupScopeNumber(type);
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
int CCTK_GroupData(int group, 
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

char *CCTK_VarName(int varnum)
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

void  FMODIFIER FORTRAN_NAME(CCTK_FirstVarIndexI)(int *first, int *group)
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

void  FMODIFIER FORTRAN_NAME(CCTK_NumVarsInGroup)(int *num,ONE_FORTSTRING_ARG)
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
  CCTK_PRINTSEPARATOR
  printf("In CCTK_GroupTypeFromVarI\n");
  printf("-------------------------\n");
  printf("Variable index = %d\n",var);
  printf("Variable name = %s\n",CCTK_FullName(var));
  printf("Group type = %d\n",gtype);
  CCTK_PRINTSEPARATOR
#endif

  return gtype;
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

void  FMODIFIER FORTRAN_NAME(CCTK_VarTypeI)(int *type,int *var)
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
  
void  FMODIFIER FORTRAN_NAME(CCTK_NumTimeLevelsFromVarI)(int *num,int *var)
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

void  FMODIFIER FORTRAN_NAME(CCTK_NumTimeLevelsFromVar)(int *num,ONE_FORTSTRING_ARG)
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

void  FMODIFIER FORTRAN_NAME(CCTK_PrintGroup)(int *group)
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

void  FMODIFIER FORTRAN_NAME(CCTK_PrintVar)(int *var)
{
  CCTK_PrintGroup(*var);
}

 /*@@
   @routine    CCTKi_SetupGroupSize
   @date       Sun Nov 28 12:38:38 1999
   @author     Tom Goodale
   @desc 
   Extracts the size of an array group from a comma-seperated list.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static CCTK_INT **CCTKi_SetupGroupSize(int dimension, const char *thorn, const char *sizestring)
{
  int i;
  CCTK_INT **size_array;
  const char *last_comma;
  const char *next_comma;  
  char tmp[200];
  int type;

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
            strncpy(tmp, last_comma, (next_comma-last_comma));
            tmp[next_comma-last_comma] = '\0';
          }
          else
          {
            strcpy(tmp, last_comma);
          }

          size_array[i] = (CCTK_INT *)ParameterGet(tmp, thorn, &type);
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
  CCTK_INT **retval;

  if (0 <= group && group<n_groups)
  {
    retval = groups[group].size;
  }
  else
  {
    retval = NULL;
  }

  return retval;
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

int CCTK_VarTypeSize(vtype)
{

  int var_size=0;
  char *msg;

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
      msg = malloc( 200*sizeof(char) );
      sprintf(msg,"Unknown variable type (%d) in CCTK_VarTypeSize",vtype);
      CCTK_Warn(0,__LINE__,__FILE__,"Cactus",msg);
      free(msg);
      var_size = 1;

  }

  return var_size;

}
