 /*@@
   @file      Traverse.c
   @date      Mon Apr 17 19:16:40 2000
   @author    Tom Goodale
   @desc 
   Routines for traversal.
   @enddesc 
   @version $Header$
 @@*/

#include "cctk_Config.h"
#include "cGH.h"
#include "cctk_Flesh.h"
#include "cctki_GHExtensions.h"

static char *rcsid="$Header$";

CCTK_FILEVERSION(main_Traverse_c)

 /*@@
   @routine    CCTK_Traverse
   @date       Thu Jan 27 14:42:16 2000
   @author     Tom Goodale
   @desc 
   Routine called to traverse functions 
   @enddesc 
   @calls     CCTKi_ScheduleTraverseGHExtensions
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_Traverse(cGH *GH, const char *where)
{
  int retcode;

  retcode = CCTKi_ScheduleTraverseGHExtensions(GH, where);

  return retcode;
}
