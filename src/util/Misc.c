 /*@@
   @file      Misc.c
   @date      Wed Jan 20 10:06:35 1999
   @author    Tom Goodale
   @desc 
   Miscellaneuous routines.
   @enddesc 
   @version $Header$
 @@*/

/*#define DEBUG_MISC*/

/*#include "cctk.h"*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <float.h>

#include "cctk_GNU.h"

#include "cctk_Config.h"
#include "cctk_Misc.h"
#include "cctk_FortranString.h"
#include "cctk_WarnLevel.h"

static char *rcsid = "$Header$";

int CCTK_RegexMatch(const char *string, 
                    const char *pattern, 
                    const int nmatch,
                    regmatch_t *pmatch); 

 /*@@
   @routine    Util_SplitString
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
int Util_SplitString(char **before, char **after, const char *string, const char *sep)
{
  int retval;
  char *position;

  /* Find location of the seperator */
  position = strstr(string, sep);

  if(position)
  {
    /*Allocate memory for return strings. */
    *before = (char *)malloc((size_t)((position-string+1)*sizeof(char)));
    *after  = (char *)malloc((size_t)((strlen(string)-(position-string)-strlen(sep)+1)*sizeof(char)));

    /* Check that the allocation succeeded. */
    if(!*before || !*after)
    {
      free(*before);
      *before = NULL;
      free(*after);
      *after = NULL;
      retval = 2;
    }
    else
    {
      retval = 3;
    }
  }
  else
  {
    *before = NULL;
    *after = NULL;
    retval = 1;
  }

  if(position && *before && *after)
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
      CCTK_Warn(0,__LINE__,__FILE__,"Cactus",message);
    }
    else if (string1 && !string2)
    { 
      message = (char *)malloc((100+sizeof(string1))*sizeof(char));
      sprintf(message,"Second string null in CCTK_Equals (1st is %s)",string1); 
      CCTK_Warn(0,__LINE__,__FILE__,"Cactus",message);
    }     
    else
    {
      CCTK_Warn(0,__LINE__,__FILE__,"Cactus","Both strings null in CCTK_Equals");
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

int FMODIFIER FORTRAN_NAME(CCTK_Equals)(const char **arg1,ONE_FORTSTRING_ARG)
{
  int retval;
  ONE_FORTSTRING_CREATE(arg2)
  retval = CCTK_Equals(*arg1,arg2);
  free(arg2); 
  return(retval);
}


/*@@
  @routine Util_NullTerminateString
  @author Paul Walker
  @desc 
  Null terminates a fortran string. Remember to free
  what it returns...
  @enddesc

@@*/

char *Util_NullTerminateString(const char *instring, unsigned int len) 
{
  char *outstring;
  unsigned int i;

  if (len > 100000)
  {
    char *message;
    message = malloc(1024*sizeof(char));
    sprintf(message,"You are Null Terminating a string with length %d !!\n"
                    "This is probably an error in calling a C routine from Fortran",len);
    CCTK_Warn(1,__LINE__,__FILE__,"Cactus",message);
    free(message);
  }

#ifdef DEBUG_MISC
  printf("Util_NullTerminateString: -%s-, (%u)\n",instring,len);
#endif

  outstring = (char *)malloc((len+2)*sizeof(char));
  assert(outstring);

  for (i=0;i<len;i++) 
    outstring[i] = instring[i];
  
  outstring[len] = '\0';

  return(outstring);
}


 /*@@
   @routine    Util_InList
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
int Util_InList(const char *string1, int n_elements, ...)
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
   @routine    Util_IntInRange
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
int Util_IntInRange(int inval, const char *range)
{
  int retval;
  int matched;
  regmatch_t pmatch[6];
  int start_closed, end_closed;
  int start, end, step;

  retval = 0;

  /* Valid ranges are of the form start:end:step 
   * possibly preceeded by a [ or ( and ended by a ) or ] to indicate
   * closure.  The end and step are optional.  A * can also be used
   * to indicate any value.
   *
   * The following regular expression may match five substrings:
   *
   * 1 - [ or (
   * 2 - start
   * 3 - end
   * 4 - step
   * 5 - ) or ]
   */

  if((matched = CCTK_RegexMatch(range, 
                     "(\\[|\\()?([^]):]*):?([^]):]*)?:?([^]):]*)?(\\]|\\))?", 
                     6, pmatch)) != 0)
  {
    /* First work out if the range is closed at the lower end. */
    if(pmatch[1].rm_so != -1)
    {
      switch(range[pmatch[1].rm_so])
      {
        case '(' : start_closed = 0; break;
        case '[' : 
        default  : start_closed = 1;
      }
    }
    else
    {
      start_closed = 1;
    }

    /* Next find the start of the range */
    if(pmatch[2].rm_so != -1 && 
       (pmatch[2].rm_eo-pmatch[2].rm_so > 0) &&
       range[pmatch[2].rm_so] != '*')
    {
      start = atoi(range+pmatch[2].rm_so);
    }
    else
    {
      /* No start range given, so use the smallest integer available. */
      start = INT_MIN;
    }

    /* Next find the end of the range */
    if(pmatch[3].rm_so != -1 && 
       (pmatch[3].rm_eo-pmatch[3].rm_so > 0) &&
       range[pmatch[3].rm_so] != '*')
    {
      end = atoi(range+pmatch[3].rm_so);
    }
    else
    {
      /* No end range given, so use the largest integer available. */
      end = INT_MAX;
    }
          
    /* Next find the step of the range */
    if(pmatch[4].rm_so != -1 && (pmatch[4].rm_eo-pmatch[4].rm_so > 0))
    {
      step = atoi(range+pmatch[4].rm_so);
    }
    else
    {
      /* No step given, so default to 1. */
      step = 1;
    }
    
    /* Finally work out if the range is closed at the upper end. */
    if(pmatch[5].rm_so != -1)
    {
      switch(range[pmatch[5].rm_so])
      {
        case ')' : end_closed = 0; break;
        case ']' : 
        default  : end_closed = 1;
      }
    }
    else
    {
      end_closed = 1;
    }
  
    if(inval >= start + !start_closed && 
       inval <= end   - !end_closed   &&
       ! ((inval-start) % step))
    {
      retval = 1;
    }

  } 
  else
  {
    CCTK_Warn(1, __LINE__, __FILE__, "Flesh", "Invalid range");
  }
    
  return retval;
}

 /*@@
   @routine    Util_DoubleInRange
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
int Util_DoubleInRange(double inval, const char *range)
{
  int retval;
  int matched;
  regmatch_t pmatch[6];
  int start_closed, end_closed;
  double start, end;
#if 0
  double step;
#endif

  retval = 0;

  /* Valid ranges are of the form start:end:step 
   * possibly preceeded by a [ or ( and ended by a ) or ] to indicate
   * closure.  The end and step are optional.  A * can also be used
   * to indicate any value.
   *
   * The following regular expression may match five substrings:
   *
   * 1 - [ or (
   * 2 - start
   * 3 - end
   * 4 - step
   * 5 - ) or ]
   */

  if((matched = CCTK_RegexMatch(range, 
                     "(\\[|\\()?([^]):]*):?([^]):]*)?:?([^]):]*)?(\\]|\\))?", 
                     6, pmatch)) != 0)
  {
    /* First work out if the range is closed at the lower end. */
    if(pmatch[1].rm_so != -1)
    {
      switch(range[pmatch[1].rm_so])
      {
        case '(' : start_closed = 0; break;
        case '[' : 
        default  : start_closed = 1;
      }
    }
    else
    {
      start_closed = 1;
    }

    /* Next find the start of the range */
    if(pmatch[2].rm_so != -1 && 
       (pmatch[2].rm_eo-pmatch[2].rm_so > 0) &&
       range[pmatch[2].rm_so] != '*')
    {
      start = atof(range+pmatch[2].rm_so);
    }
    else
    {
      /* No start range given, so use the smallest float available. */
      start = -FLT_MAX;
    }

    /* Next find the end of the range */
    if(pmatch[3].rm_so != -1 && 
       (pmatch[3].rm_eo-pmatch[3].rm_so > 0) &&
       range[pmatch[3].rm_so] != '*')
    {
      end = atof(range+pmatch[3].rm_so);
    }
    else
    {
      /* No end range given, so use the largest float available. */
      end = FLT_MAX;
    }

#if 0     
    /* Next find the step of the range */
    if(pmatch[4].rm_so != -1 && (pmatch[4].rm_eo-pmatch[4].rm_so > 0))
    {
      step = atof(range+pmatch[4].rm_so);
    }
    else
    {
      /* No step given, so default to 1. */
      step = 1;
    }
#endif
    
    /* Finally work out if the range is closed at the upper end. */
    if(pmatch[5].rm_so != -1)
    {
      switch(range[pmatch[5].rm_so])
      {
        case ')' : end_closed = 0; break;
        case ']' : 
        default  : end_closed = 1;
      }
    }
    else
    {
      end_closed = 1;
    }
  
    if(inval >= start /*+ !start_closed */&& 
       inval <= end  /* - !end_closed */ /* &&
                                       ! ((inval-start) % step)*/)
    {
      retval = 1;
    }

  } 
  else
  {
    CCTK_Warn(1, __LINE__, __FILE__, "Flesh", "Invalid range");
  }
    
  return retval;
}


 /*@@
   @routine    Util_IntInRangeList
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
int Util_IntInRangeList(int inval, int n_elements, ...)
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

    if(Util_IntInRange(inval, element))
    {
      retval = 1;
      break;
    }
  }
  
  va_end(ap);

  return retval;

}


 /*@@
   @routine    Util_DoubleInRangeList
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
int Util_DoubleInRangeList(double inval, int n_elements, ...)
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

    if(Util_DoubleInRange(inval, element))
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
int CCTK_SetDoubleInRangeList(CCTK_REAL *data, const char *value, 
                              int n_elements, ...)
{
  int retval;
  char temp[1001];
  int p;
  int arg;
  va_list ap;

  char *element;

  CCTK_REAL inval;

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

    if(Util_DoubleInRange(inval, element))
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
int CCTK_SetIntInRangeList(CCTK_INT *data, const char *value, 
                           int n_elements, ...)
{
  int retval;
  int arg;
  va_list ap;

  char *element;

  CCTK_INT inval;

  retval = 1;

  /* Convert the value string to an int.*/
    
  inval = atoi(value);

  /* Walk through the element list. */
  va_start(ap, n_elements);
  
  for(arg = 0; arg < n_elements; arg++)
  {    
    element = va_arg(ap, char *);

    if(Util_IntInRange(inval, element))
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
   @routine    CCTK_SetStringInRegexList
   @date       Fri Apr 16 08:37:02 1999
   @author     Tom Goodale
   @desc 
   Sets the value of a string if it matches any of the given regular
   expressions.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_SetStringInRegexList(char **data, const char *value, 
                               int n_elements, ...)
{
  int retval;
  int arg;
  va_list ap;

  char *element;

  retval = 1;

  /* Walk through the element list. */
  va_start(ap, n_elements);
  
  for(arg = 0; arg < n_elements; arg++)
  {    
    element = va_arg(ap, char *);

    if(CCTK_RegexMatch(value, element, 0, NULL))
    {
      retval = CCTK_SetString(data, value);
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
  printf("%s\n",data);
}

void FMODIFIER FORTRAN_NAME(CCTK_PrintString)(char **arg1)
{
  CCTK_PrintString(*arg1);
}

 /*@@
   @routine    CCTK_SetBoolean
   @date       Thu Jan 21 10:35:11 1999
   @author     Tom Goodale
   @desc 
   Sets the value of a boolean to true or false according to
   the value of the value string.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_SetBoolean(CCTK_INT *data, const char *value)
{
  int retval = 1;

  if(Util_InList(value, 5, "true", "t", "yes", "y", "1"))
  {
    *data = 1;
    retval = 0;
  }
  else if(Util_InList(value, 5, "false", "f", "no", "n", "0"))
  {
    *data = 0;
    retval = 0;
  }
  else
  {
    CCTK_Warn(1,__LINE__,__FILE__,"Cactus","Boolean not set in CCTK_SetBoolean");
    retval = -1;
  }

  return retval;
}

 /*@@
   @routine    CCTK_RegexMatch
   @date       Fri Apr 16 08:40:14 1999
   @author     Tom Goodale
   @desc 
   Perform a regular expression match of string against pattern.
   Also returns the specified number of matched substrings as
   give by regexec.
   This is a modified form of the example routine given in the SGI 
   man page for regcomp.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_RegexMatch(const char *string, 
                    const char *pattern, 
                    const int nmatch,
                    regmatch_t *pmatch) 
{
  int status;
  regex_t re;
  
  if (regcomp(&re, pattern, REG_EXTENDED) != 0) 
  {
    return(0);      /* report error */
  }
  status = regexec(&re, string, (size_t)nmatch, pmatch, 0);
  regfree(&re);
  if (status != 0) 
  {
    return(0);      /* report error */
  }
  return(1);
}


/*@@
   @routine    CCTK_FortranString
   @date       Thu Jan 22 14:44:39 1998
   @author     Paul Walker
   @desc
   Change a C string into a Fortran string
   @enddesc
@@*/
 
 
void FORTRAN_NAME(CCTK_FortranString)(CCTK_INT *nchar, 
                                      char **cstring, 
                                      ONE_FORTSTRING_ARG)
{
  int i;
  ONE_FORTSTRING_CREATE(fstring)
  ONE_FORTSTRING_PTR(fptr)

  if (strlen(*cstring) > cctk_strlen1) 
  {
    char *message;
    message = (char *)malloc( (200+strlen(*cstring))*sizeof(char) );
    sprintf(message,"Cannot output %s to char* of length %d",
            *cstring,cctk_strlen1);
    CCTK_Warn (1,__LINE__,__FILE__,"Cactus",message);
    free(message);
    *nchar = -1;
  }

  for (i=0;i<strlen(*cstring);i++) 
  {
    fptr[i] = (*cstring)[i];
  }

  for (i=strlen(*cstring);i<cctk_strlen1;i++)
  {
    fptr[i] = ' ';
  }

  fptr[strlen(*cstring)] = '\0';

  *nchar = strlen(*cstring);

}
                           
