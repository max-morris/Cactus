 /*@@
   @file      GroupsOnGH.c
   @date      Tues April 6
   @author    Gabrielle Allen
   @desc 
   GH specific Routines to deal with groups.
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "flesh.h"
#include "Misc.h"
#include "Groups.h"
#include "GroupsOnGH.h"

/*#define GROUPSDEBUG*/

static char *rcsid = "$Header$";


 /*@@
   @routine    CCTK_GetVarDataPtr_ByName
   @date       Tue 6th April 1999
   @author     Gabrielle Allen
   @desc 
   Passes back a variable data pointer, given a full name and timelevel
   @enddesc 
   @calls     
   @history 
 
   @endhistory 

@@*/
void *CCTK_GetVarDataPtr_ByName(cGH *GH, char *fullvarname, int timelevel)
{
  return (GH->data[CCTK_GetVarNum(NULL,NULL,fullvarname)][timelevel]);
}

 /*@@
   @routine    CCTK_GetVarDataPtr_ByIndex
   @date       Tue 6th April 1999
   @author     Gabrielle Allen
   @desc 
   Passes back a variable data pointer, given a variable index and timelevel
   @enddesc 
   @calls     
   @history 
 
   @endhistory 

@@*/
void *CCTK_GetVarDataPtr_ByIndex(cGH *GH, int varindex, int timelevel)
{
  return GH->data[varindex][timelevel];
}

 /*@@
   @routine    CCTK_GetVarDataPtr
   @date       Tue 6th April 1999
   @author     Gabrielle Allen
   @desc 
   Passes back a variable data pointer, given either a  variable index 
   or a full name and timelevel
   @enddesc 
   @calls     
   @history 
 
   @endhistory 

@@*/
void *CCTK_GetVarDataPtr(cGH *GH, int varindex, char *fullvarname, int timelevel)
{
  if (fullvarname) 
  {
    return CCTK_GetVarDataPtr_ByName(GH, fullvarname, timelevel);
  }
  else
  {
    return CCTK_GetVarDataPtr_ByIndex(GH, varindex, timelevel);
  }
}






 /*@@
   @routine    CCTK_ArrayGroupSize
   @date       Mon Feb  8 12:05:50 1999
   @author     Tom Goodale
   @desc 
   Gets the size of a array in a specific direction.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int *CCTK_ArrayGroupSize(cGH *GH, const char *group, int dir)
{
  /* Quick fudge */
  return &(GH->local_shape[dir]);
}

 /*@@
   @routine    CCTK_QueryGroupStorage
   @date       
   @author     Tom Goodale
   @desc 

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_QueryGroupStorage(cGH *GH, const char *group)
{
  /* Quick fudge */  
  return 1;
}

