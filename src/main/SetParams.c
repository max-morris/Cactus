 /*@@
   @file      SetParams.c
   @date      Tue Jan 12 19:16:38 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flesh.h"
#include "Implementations.h"

static char *rcsid = "$Id$";

 /*@@
   @routine    CCTK_SetParameter
   @date       Tue Jan 12 19:25:37 1999
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_SetParameter(const char *parameter, const char *value)
{
  int retval;
  
  retval = CCTK_BindingsParameterSet(parameter, value);


  if(!retval)
  {
    fprintf(stderr, "Unknown parameter %s\n", parameter);
  }

  return retval;
}

 /*@@
   @routine    CCTK_ExtractImplementation
   @date       Wed Jan 13 11:23:37 1999
   @author     Tom Goodale
   @desc 
   Takes a parameter name of the form imp::name and splits it.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_ExtractImplementation(char **imp, char **name, const char *parameter)
{
  int retval;
  char *position;

  /* Find location of the seperator */
  position = strstr(parameter, "::");

  if(position)
  {
    /*Allocate memory for return strings. */
    *imp  = (char *)malloc((position-parameter+1)*sizeof(char));
    *name = (char *)malloc((strlen(parameter)-(position-parameter)-2+1)*sizeof(char));

    /* Check that the allocation succeeded. */
    if(!*imp || !*name)
    {
      free(*imp);
      *imp = NULL;
      free(*name);
      *name = NULL;
      retval = 2;
    }
  }
  else
  {
    *imp = NULL;
    *name = NULL;
    retval = 1;
  }

  if(position && imp && name)
  {
    /* Copy the data */
    strncpy(*imp, parameter, (int)(position-parameter));
    (*imp)[(int)(position-parameter)] = '\0';
  
    strncpy(*name, position+2, strlen(parameter)-(int)(position-parameter)-2);
    (*name)[strlen(parameter)-(position-parameter)-2] = '\0';
    
    retval = 0;
  }

  return retval;
}

