 /*@@
   @file      String.c
   @date      Tue May  2 10:44:19 2000
   @author    Tom Goodale
   @desc 
   Routines dealing with strings.
   @enddesc
   @version $Header$
 @@*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "util_String.h"

#include "cctk_Flesh.h"

static const char *rcsid = "$Header$";

CCTK_FILEVERSION(util_String_c)

/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/

/********************************************************************
 ********************* Local Routine Prototypes *********************
 ********************************************************************/

/********************************************************************
 ********************* Other Routine Prototypes *********************
 ********************************************************************/

/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/

/********************************************************************
 *********************     External Routines   **********************
 ********************************************************************/

 /*@@
   @routine    CCTK_StrSep
   @date       Tue May  2 10:29:07 2000
   @author     Tom Goodale
   @desc 
     The strsep() function returns the next token from the string stringp which is delimited by delim.  The token
     is terminated with a `\0' character and stringp is updated to point past the token.

     RETURN VALUE
     The strsep() function returns a pointer to the token, or NULL if delim is not found in stringp.

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     stringp
   @vdesc   The string to search for a token in.
   @vtype   const char **stringp
   @vio     inout
   @vcomment 
 
   @endvar 
   @var     delim
   @vdesc   The delimiter
   @vtype   const char *delim
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype const char *
   @returndesc
   a pointer to the token, or NULL if delim is not found in stringp.
   @endreturndesc

@@*/
const char *Util_StrSep(const char **stringp, const char *delim)
{
  int retlength = 0;
  static char *retval = NULL;
  char *temp;
  const char *start;
  const char *end;

  start = *stringp;

  end = strstr(start, delim);

  /* Is the delimiter part of the string */
  if(end)
  {
    if(retlength < (end-start)+1)
    {
      temp = realloc(retval, (end-start+1));

      if(temp)
      {
        retval = temp;
        retlength = end-start+1;
      }
      else
      {
        free(retval);
        retval = NULL;
        retlength = 0;
      }
    }

    if(retval)
    {
      strncpy(retval, start, (size_t)(end-start));
      retval[end-start] = '\0';

      *stringp = end+strlen(delim);
    }

  }
  else
  {
    free(retval);
    retval = NULL;
    retlength = 0;
  }
  
  return retval;
}

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
   @var     before
   @vdesc   String before seperator
   @vtype   char **
   @vio     out
   @vcomment 
 
   @endvar 
   @var     after
   @vdesc   String after seperator
   @vtype   char **
   @vio     out
   @vcomment 
 
   @endvar 
   @var     string
   @vdesc   String to seperate
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     sep
   @vdesc   String seperator
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   0 - success
   1 - seperator not found
   2 - out of memory
   @endreturndesc

@@*/
int Util_SplitString(char **before, char **after, const char *string, const char *sep)
{
  int retval=0;
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
   @routine    Util_Strdup
   @date       Thu Mar 28 11:20:27 2000
   @author     Gerd Lanfermann
   @desc 
   Homegrown ersion of strdup, since it's not guaranteed to be there.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     s
   @vdesc   string to be duplicated
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype char *
   @returndesc
   the duplicate string.
   @endreturndesc
@@*/
char *Util_Strdup(const char *s) 
{
  char *retstr=NULL;
  
  retstr = (char*) malloc((strlen(s)+1)*sizeof(char));
  if(retstr)
  {
    strcpy(retstr,s);
  }
    
  return retstr;
}

 /*@@
   @routine    Util_StrCmpi
   @date       Mon Jul  5 01:19:00 1999
   @author     Tom Goodale
   @desc 
   Case independent strcmp
   @enddesc 
   @calls     
   @calledby   
   @history 
   @hdate Wed Oct 13 15:30:57 1999 @hauthor Tom Goodale
   @hdesc Checks the length of the two string first. 
   @endhistory 
   @var     string1
   @vdesc   First string in comparison
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     string2
   @vdesc   Second string in comparison
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   +ve - string1 > string2
   0   - string1 = string2
   -ve - string1 < string2
   @endreturndesc
@@*/
int Util_StrCmpi(const char *string1, const char *string2)
{
  int retval;
  int position;

  retval = 0;

  if(! retval)
  {
    for(position = 0; 
        string1[position] && string2[position];
        position++)
    {
      if((retval = (tolower(string1[position]) - tolower(string2[position]))))
      {
        break;
      }
    }
  }

  if(! retval)
  {
    retval = strlen(string1) - strlen(string2);
  }


  return retval;
}

 /*@
   @routine    Util_SplitFilename
   @date       Wed Oct 4 10:14:00 2000
   @author     Gabrielle Allen
   @desc 
   Splits a filename into its directory and basic filename parts.
   Assigns memory for the two resulting strings, so this should be freed 
   when no longer needed.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     dir
   @vdesc   The directory part
   @vtype   char **
   @vio     out
   @vcomment 
 
   @endvar 
   @var     file
   @vdesc   The file part
   @vtype   char **
   @vio     out
   @vcomment 
 
   @endvar 
   @var     string
   @vdesc   The string to split
   @vtype   const char *
   @vio     out
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   0  - success
   -1 - out of memory
   @endreturndesc
@@*/
int Util_SplitFilename(char **dir, char **file, const char *string)
{
  int retval=-1;
  char *position=NULL;
  char *copy;

  copy = Util_Strdup(string);

  /* Find location of the seperator */
  position = strrchr(copy, '/');
  if(position)
  {
    retval = 0;
    *file = position+1;
    *dir = copy;
    strcpy(position,"");
  }
  else
  {
    *file = copy;
    *dir = NULL;
  }
  
  return retval;
}


/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/



#ifdef TEST_Util_STRSEP

#include <stdio.h>

int main(int argc, char *argv[])
{
  const char *argument;
  char *delim;
  const char *token;

  if(argc < 3)
  {
    printf("Usage: %s <string> <delim>\n", argv[0]);
    exit(1);
  }

  argument = argv[1];
  delim = argv[2];

  while((token = Util_StrSep(&argument, delim)))
  {
    printf("Token is     '%s'\n", token);
  }

  if(argument - argv[1] < strlen(argv[1]))
  {
    printf("Remainder is '%s'\n", argument);
  }

  return 0;

}

#endif /*TEST_CCTK_STRSEP */

