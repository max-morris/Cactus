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

#include "cctk_Comm.h"
#include "cctk_Flesh.h"
#include "cctk_FortranString.h"
#include "cctk_Groups.h"
#include "cctk_GroupsOnGH.h"
#include "cctk_Misc.h"
#include "cctk_WarnLevel.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(main_GroupsOnGH_c)

 /*@@
   @routine    CCTK_VarDataPtr
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

   @var        varn
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

void *CCTK_VarDataPtr(cGH *GH, int timelevel, char *varn)
{
  int index;
  void *retval=NULL;

  index = CCTK_VarIndex(varn);
  if (index >= 0)
  {
     retval = GH->data[index][timelevel];
  }
  else
  {
     CCTK_Warn(1,__LINE__,__FILE__,"Cactus",
	       "Invalid index in CCTK_VarDataPtr");
  }

#ifdef DEBUG_GROUPS
  printf("In CCTK_VarDataPtr\n----------------------------\n");
  printf("  Data pointer for %s (%d) is %x\n",varn,index,retval);
#endif

  return retval;

}

 /*@@
   @routine    CCTK_VarDataPtrI
   @date       Tue 6th April 1999
   @author     Gabrielle Allen
   @desc 
   Passes back a variable data pointer, given a variable index and timelevel
   @enddesc 
   @calls     
   @history 
      A check for a valid index (i>=0) included by Gerd Lanfermann
   @endhistory 

   @var        GH 
   @vdesc      Pointer to Grid Hierachy
   @vtype      cGH *
   @vio        in
   @vcomment 
   @endvar 

   @var        vari
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

void *CCTK_VarDataPtrI(cGH *GH, int timelevel, int vari)
{ 
  if (vari < 0) 
    CCTK_Warn(1,__LINE__,__FILE__,"Cactus",
              "CCTK_VarPtrDataI: calling CCTK_VarDataPtrI with negative index");
  return GH->data[vari][timelevel];
}

 /*@@
   @routine    CCTK_VarDataPtrB
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

   @var        vari
   @vdesc      Index of grid variable
   @vtype      int
   @vio        in
   @vcomment   Assumed to be in correct range
   @endvar 

   @var        varn
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
void *CCTK_VarDataPtrB(cGH *GH, int timelevel, int vari, char *varn)
{
  if (varn) 
  {
    return CCTK_VarDataPtr(GH, timelevel, varn);
  }
  else
  {
    return CCTK_VarDataPtrI(GH, timelevel, vari);
  }
}

 /*@@
   @routine    CCTK_EnableGroupCommI
   @date       Sat Feb 13 17:06:30 1999
   @author     Tom Goodale
   @desc 
   Enables communication for a group based upon its name.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_EnableGroupCommI(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GroupName(group);
  if(group_name)
  {
    retcode = CCTK_EnableGroupComm(GH, group_name);

    free(group_name);
  }
  else
  {
    retcode = 0;
  }

  return retcode;
}

 /*@@
   @routine    CCTK_EnableGroupStorageI
   @date       Sat Feb 13 17:06:30 1999
   @author     Tom Goodale
   @desc 
   Enables storage for a group based upon its name.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_EnableGroupStorageI(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GroupName(group);
  if(group_name)
  {
#ifdef DEBUG_GROUPS
    printf("Turning on storage in %s for group %s (%d)\n",__FILE__,group_name,group);
#endif
    retcode = CCTK_EnableGroupStorage(GH, group_name);

    free(group_name);
  }
  else
  {
    retcode = 0;
  }

  return retcode;
}

 /*@@
   @routine    CCTK_DisableGroupCommI
   @date       Sat Feb 13 17:06:30 1999
   @author     Tom Goodale
   @desc 
   Routine to switch communication off for a group based upon its index
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_DisableGroupCommI(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GroupName(group);
  if(group_name)
  {
#ifdef DEBUG_GROUPS
    printf("Turning off comm in %s for group %s (%d)\n",__FILE__,group_name,group);
#endif
    retcode = CCTK_DisableGroupComm(GH, group_name);

    free(group_name);
  }
  else
  {
    retcode = 0;
  }

  return retcode;
}

 /*@@
   @routine    CCTK_DisableGroupStorageI
   @date       Sat Feb 13 17:06:30 1999
   @author     Tom Goodale
   @desc 
   Routine to switch storage off for a group based upon its index
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_DisableGroupStorageI(void *GH, int group)
{
  int retcode;
  char *group_name;

  group_name = CCTK_GroupName(group);
  if(group_name)
  {
    retcode = CCTK_DisableGroupStorage(GH, group_name);

    free(group_name);
  }
  else
  {
    retcode = 0;
  }

  return retcode;
}


const int *CCTK_ArrayGroupSizeI(cGH *GH, int dir, int groupi)
{
  return CCTK_ArrayGroupSizeB(GH,dir,groupi,NULL);
}

const int *CCTK_ArrayGroupSize(cGH *GH, int dir, const char *groupn)
{
  return CCTK_ArrayGroupSizeB(GH,dir,-1,groupn);
}

int CCTK_QueryGroupStorageI(cGH *GH, int groupi)
{
  return CCTK_QueryGroupStorageB(GH,groupi,NULL);
}

int CCTK_QueryGroupStorage(cGH *GH, const char *groupn)
{
  return CCTK_QueryGroupStorageB(GH, -1, groupn);
}

/********************************************************************
 ********************    Group Lower Bound    ***********************
 ********************************************************************/

 /*@@
   @routine    CCTK_GrouplbndGI
   @date       Mon June 19 June 2000
   @author     Gabrielle
   @desc 
   Returns the lower bound for a variable group
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_GrouplbndGI(cGH *cctkGH, 
		     int dim,
		     int *lbnd,
		     int groupindex)
{
  int retval = 0;
  int ierr;
  int usedim = dim;  /* Actual number of integers copied */
  cGroupDynamicData data;

  if (CCTK_GroupTypeI(groupindex) == GROUP_SCALAR)
  {
    retval = -3;
    CCTK_VWarn(2,__LINE__,__FILE__,"Cactus",
	       "CCTK_GrouplbndGI: Grid information called for scalar group %s",
	       CCTK_GroupName(groupindex));
  }
  else
  {
    ierr = CCTK_GroupDynamicData(cctkGH,groupindex,&data);
    
    if (ierr == 0 && data.dim && data.lbnd)
    {
      if (data.dim != dim)
      {
	retval = -1;
	usedim = (data.dim < dim) ? data.dim : dim;
	CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
		   "CCTK_GrouplbndGI: Incorrect dimension %d supplied, group %s has dimension %d, copying %d integers",
		   dim,data.dim,usedim);
      }
      memcpy(lbnd,(const int *)data.lbnd,usedim*sizeof(int));
    }
    else
    {
      retval = -2;
      CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
		 "CCTK_GrouplbndGI: Data not available from driver thorn");
    }
  }
  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GrouplbndGI)
     (int *ierr,cGH *cctkGH, int *dim, int *lbnd, int *groupindex)
{
  *ierr = CCTK_GrouplbndGI(cctkGH, *dim, lbnd, *groupindex);
}



int CCTK_GrouplbndGN(cGH *cctkGH, 
		     int dim,
		     int *lbnd,
		     const char *groupname)
{
  int retval;
  int index = CCTK_GroupIndex(groupname);

  if (index > -1)
  {
    retval = CCTK_GrouplbndGI(cctkGH,dim,lbnd,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GrouplbndGN: Group index not found for %s",groupname);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GrouplbndGN)
     (int *ierr,cGH *cctkGH, int *dim, int *lbnd, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(groupname)
  *ierr = CCTK_GrouplbndGN(cctkGH, *dim, lbnd, groupname);
  free(groupname);
}



int CCTK_GrouplbndVI(cGH *cctkGH, 
		     int dim,
		     int *lbnd,
		     int varindex)
{
  int retval;
  int index = CCTK_GroupIndexFromVarI(varindex);

  if (index > -1)
  {
    retval = CCTK_GrouplbndGI(cctkGH,dim,lbnd,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GrouplbndVI: Group index not found for variable index %d",
	       varindex);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GrouplbndVI)
     (int *ierr,cGH *cctkGH, int *dim, int *lbnd, int *varindex)
{
  *ierr = CCTK_GrouplbndVI(cctkGH, *dim, lbnd, *varindex);
}



int CCTK_GrouplbndVN(cGH *cctkGH, 
		     int dim,
		     int *lbnd,
		     const char *varname)
{
  int retval;
  int index = CCTK_GroupIndexFromVar(varname);

  if (index > -1)
  {
    retval = CCTK_GrouplbndGI(cctkGH,dim,lbnd,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GrouplbndVN: Group index not found for %s",varname);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GrouplbndVN)
     (int *ierr,cGH *cctkGH, int *dim, int *lbnd, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(varname)
  *ierr = CCTK_GrouplbndVN(cctkGH, *dim, lbnd, varname);
  free(varname);
}



/********************************************************************
 ********************    Group Upper Bound    ***********************
 ********************************************************************/

 /*@@
   @routine    CCTK_GroupubndGI
   @date       Mon June 19 June 2000
   @author     Gabrielle
   @desc 
   Returns the upper bound for a variable group
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_GroupubndGI(cGH *cctkGH, 
		     int dim,
		     int *ubnd,
		     int groupindex)
{
  int retval = 0;
  int ierr;
  int usedim = dim;  /* Actual number of integers copied */
  cGroupDynamicData data;

  if (CCTK_GroupTypeI(groupindex) == GROUP_SCALAR)
  {
    retval = -3;
    CCTK_VWarn(2,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupubndGI: Grid information called for scalar group %s",
	       CCTK_GroupName(groupindex));
  }
  else
  {
    ierr = CCTK_GroupDynamicData(cctkGH,groupindex,&data);

    if (ierr == 0 && data.dim && data.ubnd)
    {
      if (data.dim != dim)
      {
	retval = -1;
	usedim = (data.dim < dim) ? data.dim : dim;
	CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
		   "CCTK_GroupubndGI: Incorrect dimension %d supplied, group %s has dimension %d, copying %d integers",
		   dim,data.dim,usedim);
      }
      memcpy(ubnd,(const int *)data.ubnd,usedim*sizeof(int));
    }
    else
    {
      retval = -2;
      CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
		 "CCTK_GroupubndGI: Data not available from driver thorn");
    }
  }
  return retval;
}

void CCTK_FCALL CCTK_FNAME(CCTK_GroupubndGI)
     (int *ierr,cGH *cctkGH, int *dim, int *ubnd, int *groupindex)
{
  *ierr = CCTK_GroupubndGI(cctkGH, *dim, ubnd, *groupindex);
}



int CCTK_GroupubndGN(cGH *cctkGH, 
		     int dim,
		     int *ubnd,
		     const char *groupname)
{
  int retval;
  int index = CCTK_GroupIndex(groupname);

  if (index > -1)
  {
    retval = CCTK_GroupubndGI(cctkGH,dim,ubnd,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupubndGN: Group index not found for %s",groupname);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GroupubndGN)
     (int *ierr,cGH *cctkGH, int *dim, int *ubnd, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(groupname)
  *ierr = CCTK_GroupubndGN(cctkGH, *dim, ubnd, groupname);
  free(groupname);
}



int CCTK_GroupubndVI(cGH *cctkGH, 
		     int dim,
		     int *ubnd,
		     int varindex)
{
  int retval;
  int index = CCTK_GroupIndexFromVarI(varindex);

  if (index > -1)
  {
    retval = CCTK_GroupubndGI(cctkGH,dim,ubnd,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupubndVI: Group index not found for variable index %d",
	       varindex);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GroupubndVI)
     (int *ierr,cGH *cctkGH, int *dim, int *ubnd, int *varindex)
{
  *ierr = CCTK_GroupubndVI(cctkGH, *dim, ubnd, *varindex);
}



int CCTK_GroupubndVN(cGH *cctkGH, 
		     int dim,
		     int *ubnd,
		     const char *varname)
{
  int retval;
  int index = CCTK_GroupIndexFromVar(varname);

  if (index > -1)
  {
    retval = CCTK_GroupubndGI(cctkGH,dim,ubnd,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupubndVN: Group index not found for %s",varname);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GroupubndVN)
     (int *ierr,cGH *cctkGH, int *dim, int *ubnd, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(varname)
  *ierr = CCTK_GroupubndVN(cctkGH, *dim, ubnd, varname);
  free(varname);
}



/********************************************************************
 ********************    Group Local Shape    ***********************
 ********************************************************************/

 /*@@
   @routine    CCTK_GrouplshGI
   @date       Mon June 19 June 2000
   @author     Gabrielle
   @desc 
   Returns the local shape for a variable group
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_GrouplshGI(cGH *cctkGH, 
		    int dim,
		    int *lsh,
		    int groupindex)
{
  int retval = 0;
  int ierr;
  int usedim = dim;  /* Actual number of integers copied */
  cGroupDynamicData data;

  if (CCTK_GroupTypeI(groupindex) == GROUP_SCALAR)
  {
    retval = -3;
    CCTK_VWarn(2,__LINE__,__FILE__,"Cactus",
	       "CCTK_GrouplshGI: Grid information called for scalar group %s",
	       CCTK_GroupName(groupindex));
  }
  else
  {
    ierr = CCTK_GroupDynamicData(cctkGH,groupindex,&data);
    
    if (ierr == 0 && data.dim && data.lsh)
    {
      if (data.dim != dim)
      {
	retval = -1;
	usedim = (data.dim < dim) ? data.dim : dim;
	CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
		   "CCTK_GrouplshGI: Incorrect dimension %d supplied, group %s has dimension %d, copying %d integers",
		   dim,data.dim,usedim);
      }
      memcpy(lsh,(const int *)data.lsh,usedim*sizeof(int));
    }
    else
    {
      retval = -2;
      CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
		 "CCTK_GrouplshGI: Data not available from driver thorn");
    }
  }
  return retval;
}

void CCTK_FCALL CCTK_FNAME(CCTK_GrouplshGI)
     (int *ierr,cGH *cctkGH, int *dim, int *lsh, int *groupindex)
{
  *ierr = CCTK_GrouplshGI(cctkGH, *dim, lsh, *groupindex);
}



int CCTK_GrouplshGN(cGH *cctkGH, 
		     int dim,
		     int *lsh,
		     const char *groupname)
{
  int retval;
  int index = CCTK_GroupIndex(groupname);

  if (index > -1)
  {
    retval = CCTK_GrouplshGI(cctkGH,dim,lsh,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GrouplshGN: Group index not found for %s",groupname);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GrouplshGN)
     (int *ierr,cGH *cctkGH, int *dim, int *lsh, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(groupname)
  *ierr = CCTK_GrouplshGN(cctkGH, *dim, lsh, groupname);
  free(groupname);
}



int CCTK_GrouplshVI(cGH *cctkGH, 
		     int dim,
		     int *lsh,
		     int varindex)
{
  int retval;
  int index = CCTK_GroupIndexFromVarI(varindex);

  if (index > -1)
  {
    retval = CCTK_GrouplshGI(cctkGH,dim,lsh,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GrouplshVI: Group index not found for variable index %d",
	       varindex);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GrouplshVI)
     (int *ierr,cGH *cctkGH, int *dim, int *lsh, int *varindex)
{
  *ierr = CCTK_GrouplshVI(cctkGH, *dim, lsh, *varindex);
}



int CCTK_GrouplshVN(cGH *cctkGH, 
		     int dim,
		     int *lsh,
		     const char *varname)
{
  int retval;
  int index = CCTK_GroupIndexFromVar(varname);

  if (index > -1)
  {
    retval = CCTK_GrouplshGI(cctkGH,dim,lsh,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GrouplshVN: Group index not found for %s",varname);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GrouplshVN)
     (int *ierr,cGH *cctkGH, int *dim, int *lsh, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(varname)
  *ierr = CCTK_GrouplshVN(cctkGH, *dim, lsh, varname);
  free(varname);
}



/********************************************************************
 *******************    Group Global Shape    ***********************
 ********************************************************************/

 /*@@
   @routine    CCTK_GroupgshGI
   @date       Mon June 19 June 2000
   @author     Gabrielle
   @desc 
   Returns the global shape for a variable group
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_GroupgshGI(cGH *cctkGH, 
		    int dim,
		    int *gsh,
		    int groupindex)
{
  int retval = 0;
  int ierr;
  int usedim = dim;  /* Actual number of integers copied */
  cGroupDynamicData data;

  if (CCTK_GroupTypeI(groupindex) == GROUP_SCALAR)
  {
    retval = -3;
    CCTK_VWarn(2,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupgshGI: Grid information called for scalar group %s",
	       CCTK_GroupName(groupindex));
  }
  else
  {
    ierr = CCTK_GroupDynamicData(cctkGH,groupindex,&data);
    
    if (ierr == 0 && data.dim && data.gsh)
    {
      if (data.dim != dim)
      {
	retval = -1;
	usedim = (data.dim < dim) ? data.dim : dim;
	CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
		   "CCTK_GroupgshGI: Incorrect dimension %d supplied, group %s has dimension %d, copying %d integers",
		   dim,data.dim,usedim);
      }
      memcpy(gsh,(const int *)data.gsh,usedim*sizeof(int));
    }
    else
    {
      retval = -2;
      CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
		 "CCTK_GroupgshGI: Data not available from driver thorn");
    }
  }
  return retval;
}

void CCTK_FCALL CCTK_FNAME(CCTK_GroupgshGI)
     (int *ierr,cGH *cctkGH, int *dim, int *gsh, int *groupindex)
{
  *ierr = CCTK_GroupgshGI(cctkGH, *dim, gsh, *groupindex);
}



int CCTK_GroupgshGN(cGH *cctkGH, 
		    int dim,
		    int *gsh,
		    const char *groupname)
{
  int retval;
  int index = CCTK_GroupIndex(groupname);

  if (index > -1)
  {
    retval = CCTK_GroupgshGI(cctkGH,dim,gsh,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupgshGN: Group index not found for %s",groupname);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GroupgshGN)
     (int *ierr,cGH *cctkGH, int *dim, int *gsh, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(groupname)
  *ierr = CCTK_GroupgshGN(cctkGH, *dim, gsh, groupname);
  free(groupname);
}



int CCTK_GroupgshVI(cGH *cctkGH, 
		    int dim,
		    int *gsh,
		    int varindex)
{
  int retval;
  int index = CCTK_GroupIndexFromVarI(varindex);

  if (index > -1)
  {
    retval = CCTK_GroupgshGI(cctkGH,dim,gsh,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupgshVI: Group index not found for variable index %d",
	       varindex);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GroupgshVI)
     (int *ierr,cGH *cctkGH, int *dim, int *gsh, int *varindex)
{
  *ierr = CCTK_GroupgshVI(cctkGH, *dim, gsh, *varindex);
}



int CCTK_GroupgshVN(cGH *cctkGH, 
		    int dim,
		    int *gsh,
		    const char *varname)
{
  int retval;
  int index = CCTK_GroupIndexFromVar(varname);

  if (index > -1)
  {
    retval = CCTK_GroupgshGI(cctkGH,dim,gsh,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupgshVN: Group index not found for %s",varname);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GroupgshVN)
     (int *ierr,cGH *cctkGH, int *dim, int *gsh, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(varname)
  *ierr = CCTK_GroupgshVN(cctkGH, *dim, gsh, varname);
  free(varname);
}



/********************************************************************
 ********************    Group nGhostzones    ***********************
 ********************************************************************/

 /*@@
   @routine    CCTK_GroupnghostzonesGI
   @date       Mon June 19 June 2000
   @author     Gabrielle
   @desc 
   Returns the ghostzone size for a variable group
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_GroupnghostzonesGI(cGH *cctkGH, 
			    int dim,
			    int *nghostzones,
			    int groupindex)
{
  int retval = 0;
  int ierr;
  int usedim = dim;  /* Actual number of integers copied */
  cGroupDynamicData data;

  if (CCTK_GroupTypeI(groupindex) == GROUP_SCALAR)
  {
    retval = -3;
    CCTK_VWarn(2,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupnghostzonesGI: Grid information called for scalar group %s",
	       CCTK_GroupName(groupindex));
  }
  else
  {
    ierr = CCTK_GroupDynamicData(cctkGH,groupindex,&data);

    if (ierr == 0 && data.dim && data.nghostzones)
    {
      if (data.dim != dim)
      {
	retval = -1;
	usedim = (data.dim < dim) ? data.dim : dim;
	CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
		   "CCTK_GroupnghostzonesGI: Incorrect dimension %d supplied, group %s has dimension %d, copying %d integers",
		   dim,data.dim,usedim);
      }
      memcpy(nghostzones,(const int *)data.nghostzones,usedim*sizeof(int));
    }
    else
    {
      retval = -2;
      CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
		 "CCTK_GroupnghostzonesGI: Data not available from driver thorn");
    }
  }
  return retval;
}

void CCTK_FCALL CCTK_FNAME(CCTK_GroupnghostzonesGI)
     (int *ierr,cGH *cctkGH, int *dim, int *nghostzones, int *groupindex)
{
  *ierr = CCTK_GroupnghostzonesGI(cctkGH, *dim, nghostzones, *groupindex);
}



int CCTK_GroupnghostzonesGN(cGH *cctkGH, 
			    int dim,
			    int *nghostzones,
			    const char *groupname)
{
  int retval;
  int index = CCTK_GroupIndex(groupname);

  if (index > -1)
  {
    retval = CCTK_GroupnghostzonesGI(cctkGH,dim,nghostzones,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupnghostzonesGN: Group index not found for %s",groupname);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GroupnghostzonesGN)
     (int *ierr,cGH *cctkGH, int *dim, int *nghostzones, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(groupname)
  *ierr = CCTK_GroupnghostzonesGN(cctkGH, *dim, nghostzones, groupname);
  free(groupname);
}



int CCTK_GroupnghostzonesVI(cGH *cctkGH, 
			    int dim,
			    int *nghostzones,
			    int varindex)
{
  int retval;
  int index = CCTK_GroupIndexFromVarI(varindex);

  if (index > -1)
  {
    retval = CCTK_GroupnghostzonesGI(cctkGH,dim,nghostzones,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupnghostzonesVI: Group index not found for variable index %d",
	       varindex);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GroupnghostzonesVI)
     (int *ierr,cGH *cctkGH, int *dim, int *nghostzones, int *varindex)
{
  *ierr = CCTK_GroupnghostzonesVI(cctkGH, *dim, nghostzones, *varindex);
}



int CCTK_GroupnghostzonesVN(cGH *cctkGH, 
			    int dim,
			    int *nghostzones,
			    const char *varname)
{
  int retval;
  int index = CCTK_GroupIndexFromVar(varname);

  if (index > -1)
  {
    retval = CCTK_GroupnghostzonesGI(cctkGH,dim,nghostzones,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupnghostzonesVN: Group index not found for %s",varname);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GroupnghostzonesVN)
     (int *ierr,cGH *cctkGH, int *dim, int *nghostzones, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(varname)
  *ierr = CCTK_GroupnghostzonesVN(cctkGH, *dim, nghostzones, varname);
  free(varname);
}


/********************************************************************
 ********************        Group bbox       ***********************
 ********************************************************************/

 /*@@
   @routine    CCTK_GroupbboxGI
   @date       Mon June 19 June 2000
   @author     Gabrielle
   @desc 
   Returns the bbox array for a variable group
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_GroupbboxGI(cGH *cctkGH, 
		     int size,
		     int *bbox,
		     int groupindex)
{
  int retval = 0;
  int ierr;
  int usesize = size;  /* Actual number of integers copied */
  cGroupDynamicData data;

  if (CCTK_GroupTypeI(groupindex) == GROUP_SCALAR)
  {
    retval = -3;
    CCTK_VWarn(2,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupbboxGI: Grid information called for scalar group %s",
	       CCTK_GroupName(groupindex));
  }
  else
  {
    ierr = CCTK_GroupDynamicData(cctkGH,groupindex,&data);

    if (ierr == 0 && data.dim && data.bbox)
    {
      if (2*data.dim != size)
      {
	retval = -1;
	usesize = (2*data.dim < size) ? 2*data.dim : size;
	CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
		   "CCTK_GroupbboxGI: Incorrect size %d supplied, group %s has dimension %d, copying %d integers",
		   size,data.dim,usesize);
      }
      memcpy(bbox,(const int *)data.bbox,usesize*sizeof(int));
    }
    else
    {
      retval = -2;
      CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
		 "CCTK_GroupbboxGI: Data not available from driver thorn");
    }
  }
  return retval;
}

void CCTK_FCALL CCTK_FNAME(CCTK_GroupbboxGI)
     (int *ierr,cGH *cctkGH, int *size, int *bbox, int *groupindex)
{
  *ierr = CCTK_GroupbboxGI(cctkGH, *size, bbox, *groupindex);
}



int CCTK_GroupbboxGN(cGH *cctkGH, 
		     int size,
		     int *bbox,
		     const char *groupname)
{
  int retval;
  int index = CCTK_GroupIndex(groupname);

  if (index > -1)
  {
    retval = CCTK_GroupbboxGI(cctkGH,size,bbox,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupbboxGN: Group index not found for %s",groupname);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GroupbboxGN)
     (int *ierr,cGH *cctkGH, int *size, int *bbox, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(groupname)
  *ierr = CCTK_GroupbboxGN(cctkGH, *size, bbox, groupname);
  free(groupname);
}



int CCTK_GroupbboxVI(cGH *cctkGH, 
		     int size,
		     int *bbox,
		     int varindex)
{
  int retval;
  int index = CCTK_GroupIndexFromVarI(varindex);

  if (index > -1)
  {
    retval = CCTK_GroupbboxGI(cctkGH,size,bbox,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupbboxVI: Group index not found for variable index %d",
	       varindex);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GroupbboxVI)
     (int *ierr,cGH *cctkGH, int *size, int *bbox, int *varindex)
{
  *ierr = CCTK_GroupbboxVI(cctkGH, *size, bbox, *varindex);
}



int CCTK_GroupbboxVN(cGH *cctkGH, 
		     int size,
		     int *bbox,
		     const char *varname)
{
  int retval;
  int index = CCTK_GroupIndexFromVar(varname);

  if (index > -1)
  {
    retval = CCTK_GroupbboxGI(cctkGH,size,bbox,index);
  }
  else
  {
    retval = -4;
    CCTK_VWarn(1,__LINE__,__FILE__,"Cactus",
	       "CCTK_GroupbboxVN: Group index not found for %s",varname);
  }

  return retval;
}
  
void CCTK_FCALL CCTK_FNAME(CCTK_GroupbboxVN)
     (int *ierr,cGH *cctkGH, int *size, int *bbox, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(varname)
  *ierr = CCTK_GroupbboxVN(cctkGH, *size, bbox, varname);
  free(varname);
}



