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
#include "WarnLevel.h"
#include "GroupsOnGH.h"

/*#define DEBUG_GROUPS*/

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

   @var        GH 
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @vcomment 
   @endvar 

   @var        fullvarname
   @vdesc      Full name of the grid variable
   @vtype      char *
   @vio        in
   @vcomment   Format <implementation>::<variable>
   @endvar 

   @var        timelevel
   @vdesc      Index of timelevel on which data is required
   @vtype      int
   @vio        in
   @vcomment 
   @endvar 

   @returntype void *
   @returndesc Pointer to the required data, should be cast to required type
   @endreturndesc

   @version    $Header$

@@*/

void *CCTK_GetVarDataPtr_ByName(cGH *GH, int timelevel, char *fullvarname)
{
  int index;
  void *retval=NULL;

  index = CCTK_GetVarIndex(fullvarname);
  if (index >= 0)
  {
     retval = GH->data[index][timelevel];
  }
  else
     CCTK_WARN(1,"Invalid index in CCTK_GetVarDataPtr_ByName");

#ifdef DEBUG_GROUPS
  CCTK_PRINTSEPARATOR
  printf("In CCTK_GetVarDataPtr_ByName\n----------------------------\n");
  printf("  Data pointer for %s (%d) is %x\n",fullvarname,index,retval);
  CCTK_PRINTSEPARATOR
#endif

  return retval;

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

   @var        GH 
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @vcomment 
   @endvar 

   @var        varindex
   @vdesc      Index of grid variable
   @vtype      int
   @vio        in
   @vcomment   Assumed to be in correct range
   @endvar 

   @var        timelevel
   @vdesc      Index of timelevel on which data is required
   @vtype      int
   @vio        in
   @vcomment 
   @endvar 

   @returntype void *
   @returndesc Pointer to the required data, should be cast to required type
   @endreturndesc

   @version    $Header$

@@*/

void *CCTK_GetVarDataPtr_ByIndex(cGH *GH, int timelevel, int varindex)
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
   @var        GH 
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @vcomment 
   @endvar 

   @var        varindex
   @vdesc      Index of grid variable
   @vtype      int
   @vio        in
   @vcomment   Assumed to be in correct range
   @endvar 

   @var        fullvarname
   @vdesc      Full name of the grid variable
   @vtype      char *
   @vio        in
   @vcomment   Format <implementation>::<variable>
   @endvar 

   @var        timelevel
   @vdesc      Index of timelevel on which data is required
   @vtype      int
   @vio        in
   @vcomment 
   @endvar 

   @returntype void *
   @returndesc Pointer to the required data, should be cast to required type
   @endreturndesc

   @version    $Header$

@@*/
void *CCTK_GetVarDataPtr(cGH *GH, int timelevel, int varindex, char *fullvarname)
{
  if (fullvarname) 
  {
    return CCTK_GetVarDataPtr_ByName(GH, timelevel, fullvarname);
  }
  else
  {
    return CCTK_GetVarDataPtr_ByIndex(GH, timelevel, varindex);
  }
}

