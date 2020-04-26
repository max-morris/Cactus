 /*@@
   @file      CactusDefaultHasAccess
   @date      Sun Apr 26 11:25:36 CDT 2020
   @author    Roland Haas
   @desc
              The default variable access check routines
   @enddesc
   @version   $Id$
 @@*/

#include "cctk_Flesh.h"

static const char *rcsid = "$Header$";

CCTK_FILEVERSION(main_CactusDefaultHasAccess_c);

int CactusDefaultHasAccess (const cGH* GH, int index);


 /*@@
   @routine    CactusDefaultHasAccess
   @date       Sun Apr 26 11:26:40 CDT 2020
   @author     Roland Haas
   @desc
               Default access check routine
   @enddesc

   @var        index
   @vdesc      The index of the variable
   @vtype      int
   @vio        in
   @endvar

   @returntype int
   @returndesc
   This function returns a non-zero value if the variable is accessible.
   @endreturndesc
@@*/
int CactusDefaultHasAccess (const cGH* GH, int index)
{
  return 1;
  (void)GH;
  (void)index;
}
