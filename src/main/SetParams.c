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

static int num_0errors=0; /* number of level 0 errors in parameter file */
static int num_1errors=0; /* number of level 1 errors in parameter file */

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
#if 0
  char thornname[101];
  const char *position;
  int length;
#endif
  int n_errors;
  int parameter_check;

  parameter_check = CCTK_ParameterLevel();

  retval = 0;
  
  if(CCTK_Equals(parameter, "ActiveThorns"))
  {
#if 0
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

#endif /*0*/

    n_errors = CCTKi_ActivateThorns(value);
    
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
    fprintf(stderr,"Range error setting parameter %s to %s\n",parameter,value);
    num_0errors++;
  }
  else if (retval == -2)
  {
    /* Parameter not defined in thorn */
    if (parameter_check==CCTK_PARAMETER_RELAXED)
    {
      fprintf(stderr,"Parameter %s not found\n",parameter);
      num_1errors++;
    }
    else 
    {
      fprintf(stderr,"Parameter %s not found\n",parameter);
      num_0errors++;
    }
  }
  else if (retval == -4)
  {
    /* Setting parameter twice */
    if (parameter_check==CCTK_PARAMETER_RELAXED)
    {
      fprintf(stderr,"Parameter %s set in two different thorns\n",parameter);
      num_1errors++;
    }
    else 
    {
      fprintf(stderr,"Parameter %s set in two different thorns\n",parameter);
      num_0errors++;
    }
  }
  else if (retval == -5)
  {
    /* Parameter not defined by any active thorn */
    if (parameter_check==CCTK_PARAMETER_STRICT)
    {
      fprintf(stderr,"Parameter %s is not associated with an active thorn\n",
	      parameter);
      num_0errors++;
    }
    else if (parameter_check==CCTK_PARAMETER_NORMAL)
    {
      fprintf(stderr,"Parameter %s is not associated with an active thorn\n",
	      parameter);
      num_1errors++;
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

 /*@@
   @routine    CCTKi_NumParameterFileErrors
   @date       Mon Dec 3 2001
   @author     Gabrielle Allen
   @desc 
   Returns the number of errors in the parameter file with a 
   given level
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     level
   @vdesc   Level of errors to report
   @vtype   int
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
   number of errors in parameter file with this level
   @endreturndesc
@@*/

int CCTKi_NumParameterFileErrors(int level)
{
  int retval;

  switch (level)
  {
  case 0:
    retval = num_0errors;
    break;
  case 1:
    retval = num_1errors;
    break;
  default:
    retval = 0;
  }

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


  retval = 0;

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
