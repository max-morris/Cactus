/*@@
   @file      SetParams.c
   @date      Tue Jan 12 19:16:38 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
 @@*/

#include <stdio.h>
#include <malloc.h>

/* FIXME - remove this when ActiveThorns doesn't need it */
#include "SKBinTree.h"

#include "cctk_Types.h"
#include "cctki_ActiveThorns.h"
#include "cctk_ActiveThorns.h"
#include "cctk_WarnLevel.h"
#include "cctk_Misc.h"

#include "ParameterBindings.h"

static int ReallySetParameter(
   const char *parameter, 
   const char *value
   );
int CCTK_ParameterSet(
   const char *name,
   const char *thorn,
   const char *value
   );
       
static char *rcsid = "$Id$";

 /*@@
   @routine    CCTKi_SetParameter
   @date       Tue Jan 12 19:25:37 1999
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTKi_SetParameter(const char *parameter, const char *value)
{
  int retval;
  char thornname[101];
  const char *position;
  int length;
  int n_errors;
  
  retval = 0;
  
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
      
      if (length > 0)
      {
        thornname[length] = '\0';
        n_errors += CCTKi_ActivateThorn(thornname) != 0;
      }
      if(*position) position++;

    }
    
    if(n_errors)
    {
      CCTK_Warn(0,__LINE__,__FILE__,"Cactus",
		"Errors while activating thorns\n");
    }
  }
  else
  {     
    /*   retval = CCTKi_BindingsParameterSet(parameter, value);*/
    retval = ReallySetParameter(parameter, value); 
  }
  
  if(retval)
  {
    if(retval == -1)
    {
      fprintf(stderr, "Unknown parameter %s\n", parameter);
    }
    else
    {
      fprintf(stderr, "Error setting parameter %s to %s\n", parameter, value);
    }
  }

  return retval;
}

static int ReallySetParameter(const char *parameter, const char *value)
{
  int retval;
  const char *thorn;
  char *param;
  char *imp;

  int retval_imp;
  int retval_thorn;


  /*
    CCTKi_BindingsParameterHelp(optarg,"%s",stdout);
  */
 
  Util_SplitString(&imp, &param, parameter, "::");

  retval = -1;
  /* If param is null, there were no colons in the input */

  if(!param)
  {
    retval = CCTK_ParameterSet(parameter, imp, value);
  }
  else
  {
    /* Set if this is an implementation one */
    if(CCTK_IsImplementationActive(imp))
    {
      thorn = CCTK_ActivatingThorn(imp);
      retval_imp = CCTK_ParameterSet(param, thorn, value);
    }
    else
    {
      thorn = NULL;
      retval_imp = -1;
    }


    /* Set if this is a thorn one. */
    if(!thorn || !CCTK_Equals(thorn,imp))
    {
      if(CCTK_IsThornActive(imp))
      {
        retval_thorn = CCTK_ParameterSet(param, imp, value);
      }
      else
      {
        retval_thorn = -1;
      }
    }
    else
    {
      /* Don't need to set it twice if the name of the imp is the same as the thorn providing it */
      retval_thorn = retval_imp;
    }

    if(!retval_imp || !retval_thorn)
    {
      retval = 0;
    }
    else
    {
      retval = retval_imp ? retval_imp : retval_thorn;
    }
  }

  /* Free any allocated memory. */
  free(imp);
  free(param);

  return retval;

}
