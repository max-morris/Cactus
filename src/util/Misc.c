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

#include "Misc.h"

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

  retval = 1;
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

