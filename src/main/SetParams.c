 /*@@
   @file      SetParams.c
   @date      Tue Jan 12 19:16:38 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
 @@*/

#include <stdio.h>

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
  char thornname[101];
  const char *position;
  int length;
  int n_errors;

  if(CCTK_Equals(parameter, "ActiveThorns"))
  {
    n_errors = 0;
    position = value;

    while(*position)
    {
      length=0;

      for(;*position && *position != ' ';position++)
      {
	thornname[length] = *position;
	if(length < 100) length++;
      }

      thornname[length] = '\0';
      n_errors += CCTK_ActivateThorn(thornname) != 0;
      if(*position) position++;
    }
  }
  else
  {     
    retval = CCTK_BindingsParameterSet(parameter, value);
  }

  if(retval)
  {
    if(retval == -1)
    {
      fprintf(stderr, "Unknown parameter %s\n", parameter);
    }
  }

  return retval;
}

