 /*@@
   @file      Misc.c
   @date      Wed Jan 20 10:06:35 1999
   @author    Tom Goodale
   @desc 
   Miscellaneuous routines.
   @enddesc 
 @@*/


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>

#include "Misc.h"
#include "FortranString.h"
#include "WarnLevel.h"

 /*@@
   @routine    CCTK_SplitString
   @date       Wed Jan 20 10:14:00 1999
   @author     Tom Goodale
   @desc 
   Splits a string into two parts at the given seperator.
   Assigns memory for the two resulting strings, so this should be freed 
   when no longer needed.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_SplitString(char **before, char **after, const char *string, const char *sep)
{
  int retval;
  char *position;

  /* Find location of the seperator */
  position = strstr(string, sep);

  if(position)
  {
    /*Allocate memory for return strings. */
    *before  = (char *)malloc((position-string+1)*sizeof(char));
    *after = (char *)malloc((strlen(string)-(position-string)-strlen(sep)+1)*sizeof(char));

    /* Check that the allocation succeeded. */
    if(!*before || !*after)
    {
      free(*before);
      *before = NULL;
      free(*after);
      *after = NULL;
      retval = 2;
    }
  }
  else
  {
    *before = NULL;
    *after = NULL;
    retval = 1;
  }

  if(position && before && after)
  {
    /* Copy the data */
    strncpy(*before, string, (int)(position-string));
    (*before)[(int)(position-string)] = '\0';
  
    strncpy(*after, position+strlen(sep), strlen(string)-(int)(position-string)-strlen(sep));
    (*after)[strlen(string)-(position-string)-strlen(sep)] = '\0';
    
    retval = 0;
  }

  return retval;
}

 /*@@
   @routine    CCTK_Equals
   @date       Wed Jan 20 10:25:30 1999
   @author     Tom Goodale
   @desc 
   Does a case independent comparison of strings.
   Returns true if they are equal.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_Equals(const char *string1, const char *string2)
{
  int retval;
  int position;
  char *message;

  retval = 1;

  /* Check that string1 isn't null */
  if (!string1 || !string2)
  {
    if (!string1 && string2)
    {
      message = (char *)malloc((100+sizeof(string2))*sizeof(char));
      sprintf(message,"First string null in CCTK_Equals (2nd is %s)",string2); 
      CCTK_Warn(0,message);
    }
    else if (string1 && !string2)
    { 
      message = (char *)malloc((100+sizeof(string1))*sizeof(char));
      sprintf(message,"Second string null in CCTK_Equals (1st is %s)",string1); 
      CCTK_Warn(0,message);
    }     
    else
    {
      CCTK_Warn(0,"Both strings null in CCTK_Equals");
    }
  }

  if(strlen(string1)==strlen(string2))
  {
    for(position = 0; position < strlen(string1);position++)
    {
      if(tolower(string1[position]) != tolower(string2[position]))
      {
	retval = 0;
	break;
      }
    }
  }
  else
  {
    retval = 0;
  }

  return retval;
}

int FORTRAN_NAME(CCTK_Equals)(const char **arg1,ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(arg2)
  int retval;
  retval = CCTK_Equals(*arg1,arg2);
  free(arg2); 
  return(retval);
}


/*@@
  @routine CCTK_NullTerminateString
  @author Paul Walker
  @desc 
  Null terminates a fortran string. Remember to free
  what it returns...
  @enddesc

@@*/

char *CCTK_NullTerminateString(const char *instring, unsigned int len) 
{
  char *outstring;
  unsigned int i;

  outstring = (char *)malloc((len+2)*sizeof(char));
  assert(outstring);
  for (i=0;i<len;i++) 
  {
    outstring[i] = instring[i];
  }
  outstring[len] = '\0';

  return(outstring);
}


 /*@@
   @routine    CCTK_InList
   @date       Wed Jan 20 10:31:25 1999
   @author     Tom Goodale
   @desc 
   Determines if a string is in a list of other strings. 
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_InList(const char *string1, int n_elements, ...)
{
  int retval;
  int arg;
  va_list ap;

  char *element;

  retval = 0;

  /* Walk through the element list. */
  va_start(ap, n_elements);
  
  for(arg = 0; arg < n_elements; arg++)
  {    
    element = va_arg(ap, char *);

    if(CCTK_Equals(string1, element))
    {
      retval = 1;
      break;
    }
  }
  
  va_end(ap);

  return retval;

}
	 

 /*@@
   @routine    CCTK_IntInRange
   @date       Wed Jan 20 10:32:36 1999
   @author     Tom Goodale
   @desc 
   This routine will determine if an integer is in the range specified
   in the range string.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_IntInRange(int inval, const char *range)
{
  return 1;
}

 /*@@
   @routine    CCTK_DoubleInRange
   @date       Wed Jan 20 10:32:36 1999
   @author     Tom Goodale
   @desc 
   This routine will determine if a double is in the range specified
   in the range string.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_DoubleInRange(double inval, const char *range)
{
  return 1;
}


 /*@@
   @routine    CCTK_IntInRangeList
   @date       Wed Jan 20 10:36:31 1999
   @author     Tom Goodale
   @desc 
   Determines if an integer is in a given list of ranges.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_IntInRangeList(int inval, int n_elements, ...)
{
  int retval;
  int arg;
  va_list ap;

  char *element;

  retval = 0;

  /* Walk through the element list. */
  va_start(ap, n_elements);
  
  for(arg = 0; arg < n_elements; arg++)
  {    
    element = va_arg(ap, char *);

    if(CCTK_IntInRange(inval, element))
    {
      retval = 1;
      break;
    }
  }
  
  va_end(ap);

  return retval;

}


 /*@@
   @routine    CCTK_DoubleInRangeList
   @date       Wed Jan 20 10:36:31 1999
   @author     Tom Goodale
   @desc 
   Determines if a double is in a given list of ranges.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_DoubleInRangeList(double inval, int n_elements, ...)
{
  int retval;
  int arg;
  va_list ap;

  char *element;

  retval = 0;

  /* Walk through the element list. */
  va_start(ap, n_elements);
  
  for(arg = 0; arg < n_elements; arg++)
  {    
    element = va_arg(ap, char *);

    if(CCTK_DoubleInRange(inval, element))
    {
      retval = 1;
      break;
    }
  }
  
  va_end(ap);

  return retval;

}


 /*@@
   @routine    CCTK_SetDoubleInRangeList
   @date       Thu Jan 21 09:41:21 1999
   @author     Tom Goodale
   @desc 
   Sets the value of a double if the desired value is in one of
   the specified ranges.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_SetDoubleInRangeList(double *data, const char *value, 
			      int n_elements, ...)
{
  int retval;
  char temp[1001];
  int p;
  int arg;
  va_list ap;

  char *element;

  double inval;

  retval = 1;

  /* Convert the value string to a double.
   * Allow various formats.
   */
  strncpy(temp, value, 1000);
  
  for (p=0;p<strlen(temp);p++) 
  {
    if (temp[p] == 'E' || 
	temp[p] == 'd' || 
	temp[p] == 'D') 
    {
      temp[p] = 'e';
      break;
    }
  }
    
  inval = atof(temp);

  /* Walk through the element list. */
  va_start(ap, n_elements);
  
  for(arg = 0; arg < n_elements; arg++)
  {    
    element = va_arg(ap, char *);

    if(CCTK_DoubleInRange(inval, element))
    {
      retval = 0;
      *data = inval;
      break;
    }
  }
  
  va_end(ap);

  return retval;
}

 /*@@
   @routine    CCTK_SetIntInRangeList
   @date       Thu Jan 21 10:27:26 1999
   @author     Tom Goodale
   @desc 
   Sets the value of a integer if the desired value is in one of
   the specified ranges.   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_SetIntInRangeList(int *data, const char *value, 
			   int n_elements, ...)
{
  int retval;
  int arg;
  va_list ap;

  char *element;

  int inval;

  retval = 1;

  /* Convert the value string to an int.*/
    
  inval = atoi(value);

  /* Walk through the element list. */
  va_start(ap, n_elements);
  
  for(arg = 0; arg < n_elements; arg++)
  {    
    element = va_arg(ap, char *);

    if(CCTK_IntInRange(inval, element))
    {
      retval = 0;
      *data = inval;
      break;
    }
  }
  
  va_end(ap);

  return retval;
}

 /*@@
   @routine    CCTK_SetKeywordInRangeList
   @date       Thu Jan 21 10:28:00 1999
   @author     Tom Goodale
   @desc 
   Sets the value of a keyword if the desired value is in one of
   the specified ranges.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_SetKeywordInRangeList(char **data, const char *value, 
			       int n_elements, ...)
{
  int retval;
  int arg;
  va_list ap;

  char *element;

  int inval;

  retval = 1;

  /* Walk through the element list. */
  va_start(ap, n_elements);
  
  for(arg = 0; arg < n_elements; arg++)
  {    
    element = va_arg(ap, char *);

    if(CCTK_Equals(value, element))
    {
      if(*data) free(*data);
      *data = (char *)malloc((strlen(value)+1)*sizeof(char));
      if(*data)
      {
	strcpy(*data, value);
	retval = 0;
      }
      else
      {
	retval =-1;
      }
      break;
    }
  }
  
  va_end(ap);

  return retval;
}


 /*@@
   @routine    CCTK_SetString
   @date       Thu Jan 21 10:28:27 1999
   @author     Tom Goodale
   @desc 
   Sets the value of a string
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_SetString(char **data, const char *value)
{
  int retval;

  retval = 1;

  if(*data) free(*data);
  *data = (char *)malloc((strlen(value)+1)*sizeof(char));
  if(*data)
  {
    strcpy(*data, value);
    retval = 0;
  }
  else
  {
    retval = -1;
  }

  return retval;
}

 /*@@
   @routine    CCTK_PrintString
   @date       Fri Apr 1 1999
   @author     Gabrielle Allen
   @desc
   Prints the value of a string (this is for fortran)
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTK_PrintString(char *data)
{
  printf("%s",data);
}

void FORTRAN_NAME(CCTK_PrintString)(char **arg1)
{
  CCTK_PrintString(*arg1);
}

 /*@@
   @routine    CCTK_SetLogical
   @date       Thu Jan 21 10:35:11 1999
   @author     Tom Goodale
   @desc 
   Sets the value of a logical to true or false according to
   the value of the value string.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_SetLogical(int *data, const char *value)
{
  int retval = 1;

  if(CCTK_InList(value, 5, "true", "t", "yes", "y", "1"))
  {
    *data = 1;
    retval = 0;
  }
  else if(CCTK_InList(value, 5, "false", "f", "no", "n", "0"))
  {
    *data = 0;
    retval = 0;
  }
  else
  {
    CCTK_Warn(1,"Logical not set in CCTK_SetLogical");
    retval = -1;
  }

  return retval;
}
