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
  
  retval = CCTK_BindingsParameterSet(parameter, value);


  if(retval)
  {
    fprintf(stderr, "Unknown parameter %s\n", parameter);
  }

  return retval;
}

