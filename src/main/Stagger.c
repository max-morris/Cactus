
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

int CCTK_StaggeredGrids(void) {
  return(staggered);
}

int CCTK_StaggerCodeGrpIdx(int gindex) {
  cGroup group;
  int sc;
  CCTK_GroupData(gindex, &group);
  sc = group.stagtype;
  return(sc);
}
 
void FMODIFIER FORTRAN_NAME(CCTK_StaggerCodeGrpIdx)(int *stagcode, int *gindex) 
{
  *stagcode = CCTK_StaggerCodeGrpIdx(*gindex);
}

int CCTK_StaggerCodeGrp(const char *gname) {
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
  

int CCTK_StaggerCodeName(const char *stype) {
  int i,scode,base,dim,m;
  char *info;

  base =1;
  scode=0;
  dim  =strlen(stype);

  for (i=0;i<dim;i++) {

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


int CCTK_DirStaggerCodeVal(int dir, int sc) {
  int val,b,dsc;
  static int hash[4],hashed=0;

  if (hashed==0) {
    hash[0]= 1;
    hash[1]= 3;
    hash[2]= 9;
    hash[3]=27;
    hashed = 1;
  }

  for (b=3;b>=0;b--) {
    val = (int)(sc / hash[b]);
    sc  = sc % hash[b];
    if (dir==b) {
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



int CCTK_DirStaggerCodeName(int dir, const char *stype) {
  int scode;
  char hs[7]="MMMMMM",*info;

  sprintf(hs,"%s",stype);

  if (dir>strlen(hs)) CCTK_Warn(1,__LINE__,__FILE__,"Cactus","Not enough letters in stagger code");

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
  if ((*dsc)>=0) (*dsc)++;
  free(sname);
}

