/*@@
   @file      SetParams.c
   @date      Tue Jan 12 19:16:38 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
   @version $Header$
 @@*/

#include <stdio.h>
#include <stdlib.h>

/* FIXME - remove this when ActiveThorns doesn't need it */
#include "SKBinTree.h"

#include "cctk_Types.h"
#include "cctki_ActiveThorns.h"
#include "cctk_ActiveThorns.h"
#include "cctk_WarnLevel.h"
#include "cctk_Misc.h"
#include "cctk_Flesh.h"
#include "cctk_Parameter.h"

#include "cctki_Parameter.h"

#include "ParameterBindings.h"

static const char *rcsid = "$Header$";

CCTK_FILEVERSION(main_SetParams_c)

/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/

/********************************************************************
 ********************* Local Routine Prototypes *********************
 ********************************************************************/

static int ReallySetParameter(const char *parameter, 
                              const char *value);

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
   @routine    CCTKi_SetParameter
   @date       Tue Jan 12 19:25:37 1999
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     parameter
   @vdesc   Name of a parameter
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     value
   @vdesc   Value of the parameter
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   0  - success
   -1 - unknown parameter
   -? - other error
   @endreturndesc
@@*/
int CCTKi_SetParameter(const char *parameter, const char *value)
{
  int retval;
  char thornname[101];
  const char *position;
  int length;
  int n_errors;
  int parameter_check;

  parameter_check = CCTK_ParameterLevel();

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
                "CCTKi_SetParameter: Errors while activating thorns\n");
    }
  }
  else
  {     
    retval = ReallySetParameter(parameter, value); 
  }

  if (retval == -1)
  {
    CCTK_VWarn(0,__LINE__,__FILE__,"Cactus",
	       "CCTKi_SetParameter: Range error setting parameter %s to %s\n",
	       parameter,value);
  }
  else if (retval == -2)
  {
    /* Parameter not defined in thorn */
    if (parameter_check==CCTK_PARAMETER_RELAXED)
    {
      CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
		 "CCTKi_SetParameter: Parameter %s not found",
		 parameter);
    }
    else 
    {
      CCTK_VWarn(0,__LINE__,__FILE__,"Cactus",
		 "CCTKi_SetParameter: Parameter %s not found",
		 parameter);
    }
  }
  else if (retval == -4)
  {
    /* Setting parameter twice */
    if (parameter_check==CCTK_PARAMETER_RELAXED)
    {
      CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
		 "CCTKi_SetParameter: Parameter %s "
		 "set in two different thorns",
		 parameter);
    }
    else 
    {
      CCTK_VWarn(0,__LINE__,__FILE__,"Cactus",
		 "CCTKi_SetParameter: Parameter %s "
		 "set in two different thorns",
		 parameter);
    }
  }
  else if (retval == -5)
  {
    /* Parameter not defined by any active thorn */
    if (parameter_check==CCTK_PARAMETER_STRICT)
    {
      CCTK_VWarn(0,__LINE__,__FILE__,"Cactus",
		 "CCTKi_SetParameter: Parameter %s "
		 "is not associated with an active thorn",
		 parameter);
    }
    else if (parameter_check==CCTK_PARAMETER_NORMAL)
    {
      CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
		 "CCTKi_SetParameter: Parameter %s "
		 "is not associated with an active thorn",
		 parameter);
    }
  }

  /*  if(retval)
  {
    if(retval == -1)
    {
	fprintf(stderr, "Unknown parameter %s\n", parameter);
    }
    else
    {
      fprintf(stderr, "Error setting parameter %s to %s\n", parameter, value);
    }
    }*/

  return retval;
}

/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/


 /*@@
   @routine    ReallySetParameter
   @date       Tue Jan 12 19:25:37 1999
   @author     Tom Goodale
   @desc 
   Really sets the parameter value.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     parameter
   @vdesc   Name of a parameter
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 
   @var     value
   @vdesc   Value of the parameter
   @vtype   const char *
   @vio     in
   @vcomment 
 
   @endvar 

   @returntype int
   @returndesc
   0  = success
   -5 = thorn/imp not active
   -4 = tried to set parameter in two different thorns
   -3 = tried to steer nonsteerable parameter
   -2 = parameter not defined in the active thorn
   -1 = parameter out of range
   @endreturndesc
@@*/
static int ReallySetParameter(const char *parameter, const char *value)
{
  int retval;
  int retval_thorn=0;
  int found = 0;
  int retval_imp=0;
  const char *thorn;
  char *param;
  char *imp;

  Util_SplitString(&imp, &param, parameter, "::");

  if (!param)
  {
    /* must be global parameter */
    retval = CCTK_ParameterSet(parameter, imp, value);
  }
  else 
  {
    /* try and set parameter from implementation */
    if (CCTK_IsImplementationActive(imp))
    {
      thorn = CCTK_ActivatingThorn(imp);
      /* only do it if the thorn name is different to the imp */
      if (!CCTK_Equals(thorn,imp))
      {
	found++;
	retval_imp = CCTK_ParameterSet(param, thorn, value);
      }
    }

    /* try and set parameter from thorn */
    if (CCTK_IsThornActive(imp))
    {
      found++;
      retval_thorn = CCTK_ParameterSet(param, imp, value);
    }

    if (!found)
    {
      /* imp or thorn not found */
      retval = -5;
    }
    else if (found==2 && retval_imp>-1 && retval_thorn >-1)
    {
      /* tried to set parameter for both imp and thorn */
      retval = -4;
    }
    else if (found==2 && retval_imp<0 && retval_thorn <0)
    {
      /* failed to set parameter for both imp and thorn */
      /* FIXME: returning imp but loosing thorn info*/
      retval = retval_imp;
    }
    else if (found==2)
    {
      /* Only one succeeded */
      retval = (retval_imp>-1 ? retval_imp : retval_thorn);
    }
    else if (found==1)
    {
      retval = retval_imp + retval_thorn;
    }
  }

  /* Free any allocated memory. */
  free(imp);
  free(param);

  return retval;
}
