 /*@@
   @file      ConfigData.c
   @date      Fri Jan 15 13:27:50 1999
   @author    Tom Goodale
   @desc 
   Miscellaneous routines to deal with configuration data
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>

#include "cctk_Flesh.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_ConfigData_c)

 /*@@
   @routine    CCTKi_AddGH
   @date       Fri Jan 15 13:43:11 1999
   @author     Tom Goodale
   @desc 
   Adds a GH to the config_data structure at a particular convergence level.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTKi_AddGH(tFleshConfig *config, int convergence_level, cGH *GH)
{
  int retval;

  cGH **temp;

  int i;

  retval = 0;

  if(config->nGHs == 0 || convergence_level > config->nGHs-1)
  {
    temp = (cGH **)realloc(config->GH, (convergence_level+1)*sizeof(cGH *));

    if(temp)
    {
      config->GH = temp;
      for(i=config->nGHs; i<convergence_level+1;i++)
      {
        config->GH[i] = NULL;
      }
      config->nGHs=convergence_level+1;
    }
    else
    {
      retval = 1;
    }
  }

  if(!retval && !config->GH[convergence_level])
  {
    config->GH[convergence_level] = GH;
  }
  else
  {
    fprintf(stderr, "Tried to store two GHs at the same convergence level !\n");
    retval = 2;
  }

  return retval;
}
