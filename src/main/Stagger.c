
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "cctk_Groups.h"
#include "cctk_Types.h"
#include "cctk_FortranString.h"
#include "cctk_ParameterFunctions.h"
#include "cctk_WarnLevel.h"

static int staggered = 0;

 /*@@
   @routine    CCTK_StaggerVars
   @date       
   @author     Gerd Lanfermann
   @desc 

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_StaggerVars(void) 
{
  return(staggered);
}


 /*@@
   @routine    
   @date       
   @author     
   @desc 

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_StaggerCodeGrpIdx(int gindex) 
{
  cGroup group;
  int sc;
  CCTK_GroupData(gindex, &group);
  sc = group.stagtype;
  return(sc);
}

void FMODIFIER FORTRAN_NAME(CCTK_StaggerCodeGrpIdx)
     (int *stagcode, int *gindex) 
{
  *stagcode = CCTK_StaggerCodeGrpIdx(*gindex);
}

 /*@@
   @routine    
   @date       
   @author     Gerd Lanfermann
   @desc 

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
 
int CCTK_StaggerCodeGrp(const char *gname) 
{
  int gindex;
  gindex = CCTK_GroupIndex(gname);
  return(CCTK_StaggerCodeGrpIdx(gindex));
}

void FMODIFIER FORTRAN_NAME(CCTK_StaggerCodeGrp)(int *stagcode, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(gname)
  int gindex;
  gindex    = CCTK_GroupIndex(gname);
  *stagcode = CCTK_StaggerCodeGrpIdx(gindex);
  free(gname);
}
  

 /*@@
   @routine    
   @date       
   @author     Gerd Lanfermann
   @desc 

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
 
int CCTK_StaggerCodeName(const char *stype) 
{
  int i,scode,base,dim,m;
  char *info;

  base =1;
  scode=0;
  dim  =strlen(stype);

  for (i=0;i<dim;i++) 
  {

    switch (stype[i])
    {
      case 'M':m=0; break;
      case 'C':m=1; break;
      case 'P':m=2; break;
      default:
        info   = (char*)malloc (256*sizeof(char));
        sprintf(info,"Unknown stagger type: >%s< \n", stype);
        CCTK_Warn(1,__LINE__,__FILE__,"Cactus",info);
        free(info);
        return(-1);
    }
    scode+= m*base;
    base  = 3 * base;
  }
  return(scode);
}

void FMODIFIER FORTRAN_NAME(CCTK_StaggerCodeName)(int *scode, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(sname);
  *scode = CCTK_StaggerCodeName(sname);
  free(sname);
}


 /*@@
   @routine    
   @date       
   @author     Gerd Lanfermann
   @desc 

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
 
int CCTK_DirStaggerCodeVal(int dir, int sc) 
{
  int val,b,dsc;
  static int hash[4],hashed=0;

  if (hashed==0) 
  {
    hash[0]= 1;
    hash[1]= 3;
    hash[2]= 9;
    hash[3]=27;
    hashed = 1;
  }

  for (b=3;b>=0;b--) 
  {
    val = (int)(sc / hash[b]);
    sc  = sc % hash[b];
    if (dir==b) 
    {
      dsc = val;
      break;
    }
  }
  return(dsc);
}

void FMODIFIER FORTRAN_NAME(CCTK_DirStaggerCodeVal)
     ( int *dsc, int *dir, int *gsc) 
{
  *dsc  = CCTK_DirStaggerCodeVal((*dir)-1, *gsc);
  if ((*dsc)>=0) (*dsc)++;
}



 /*@@
   @routine    
   @date       
   @author     Gerd Lanfermann
   @desc 

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
 
int CCTK_DirStaggerCodeName(int dir, const char *stype) {
  int scode;
  char hs[7]="MMMMMM",*info;

  sprintf(hs,"%s",stype);

  if (dir>strlen(hs)) 
  {
    CCTK_Warn(1,__LINE__,__FILE__,"Cactus","Not enough letters in stagger code");
  }

  switch (hs[dir])
    {
    case 'M': scode = 0; break;
    case 'C': scode = 1; break;
    case 'P': scode = 2; break;
    default:
        info   = (char*)malloc (256*sizeof(char));
        sprintf(info,"Unknown stagger type: >%s< \n", hs);
        CCTK_Warn(1,__LINE__,__FILE__,"Cactus",info);
        free(info);
        return(-1);
    }
  return(scode);
}

void FMODIFIER FORTRAN_NAME(CCTK_DirStaggerCodeName)
     (int *ierr, int *dsc, int *dir, ONE_FORTSTRING_ARG) 
{
  ONE_FORTSTRING_CREATE(sname);
  *ierr= 0;
  *dsc = CCTK_DirStaggerCodeName((*dir)-1,sname);
  if ((*dsc)>=0) 
  {
    (*dsc)++;
  }
  free(sname);
}

 /*@@
   @routine    
   @date       
   @author     Gerd Lanfermann
   @desc 

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
 
int CCTKi_ParseStaggerString(int dim,
			     const char *imp, 
			     const char *gname,
			     const char *stype) 
{
  int i,m;
  int base  = 1;
  int scode = 0;
  char hs[7]="MMMMMM", *info;

  /* change possible SHORTCUTS into the official notation, allow for dim=6 */
  if (strcmp(stype,"NONE")==0)
  {
    strncpy(hs,"MMMMMM",dim);
  }
  else if (strcmp(stype,"CELL")==0) 
  {
    strncpy(hs,"CCCCCC",dim);
  }
  else 
  {
    sprintf(hs,"%s",stype);
  }

  for (i=0;i<dim;i++) 
  {
    switch (hs[i])
    {
      case 'M':m=0; break;
      case 'C':m=1; break;
      case 'P':m=2; break;
      default:
        info   = (char*)malloc (256*sizeof(char));
        sprintf(info,
              "Unknown stagger type: >%s< for group: >%s::%s< \n",
              stype,imp,gname);
        CCTK_Warn(1,__LINE__,__FILE__,"Cactus",info);
        free(info);
        return(-1);
    }
    scode+= m*base;
    base  = 3 * base;
  }

  return(scode);
}
