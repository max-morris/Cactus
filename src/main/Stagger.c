 /*@@
   @file      Stagger.c
   @date      Thu Jan 27 15:32:28 2000
   @author    Gerd Lanfermann
   @desc 
   Stuff to deal with staggering.
   @enddesc 
   @version $Header$
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cctk_Groups.h"
#include "cctk_Types.h"
#include "cctk_FortranString.h"
#include "cctk_WarnLevel.h"
#include "cctki_Stagger.h"

static char *rcsid = "$Header$";


/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/

static int staggered = 0;


/********************************************************************
 *********************     External Routines   **********************
 ********************************************************************/

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
   @routine    CCTK_GroupStaggerIndexGI
   @date       
   @author     
   @desc       retuns the stagger code for a given group index

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_GroupStaggerIndexGI(int gindex)
{
  cGroup group;
  int sc;
  CCTK_GroupData(gindex, &group);
  sc = group.stagtype;
  return(sc);
}

void FMODIFIER FORTRAN_NAME(CCTK_GroupStaggerIndexGI)
     (int *stagcode, int *gindex) 
{
  *stagcode = CCTK_GroupStaggerIndexGI(*gindex);
}


 /*@@
   @routine    CCTK_GroupStaggerIndexGN
   @date       
   @author     Gerd Lanfermann
   @desc       returns the stagger index for a given group name
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
 
int CCTK_GroupStaggerIndexGN(const char *gname) 
{
  int gindex;
  gindex = CCTK_GroupIndex(gname);
  return(CCTK_GroupStaggerIndexGI(gindex));
}

void FMODIFIER FORTRAN_NAME(CCTK_GroupStaggerIndexGN)(int *scode, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(gname)
  int gindex;
  gindex = CCTK_GroupIndex(gname);
  *scode = CCTK_GroupStaggerIndexGI(gindex);
  free(gname);
}
  

 /*@@
   @routine    CCTK_StaggerIndex
   @date       
   @author     Gerd Lanfermann
   @desc       returns the stagger index for a given stagger name

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
 
int CCTK_StaggerIndex(const char *stype) 
{
  int i,scode,base,dim,m;
  char *info;

  base = 1;
  scode= 0;
  dim  = strlen(stype);

  for (i=0;i<dim;i++) 
  {

    switch (toupper(stype[i]))
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
    base  = CCTK_NUM_STAGGER * base;
  }
  return(scode);
}

void FMODIFIER FORTRAN_NAME(CCTK_StaggerIndex)(int *scode, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(sname);
  *scode = CCTK_StaggerIndex(sname);
  free(sname);
}


/*@@
   @routine    CCTK_StaggerDirIndex
   @date       
   @author     Gerd Lanfermann
   @desc       returns the stagger index in a direction <dir>
               when given the staggerindex <sc>.

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
 
int CCTK_StaggerDirIndex(int dir, int si) 
{
  int val,b,dsi;
  static int hash[4],hashed=0;

  if (hashed==0) 
  {
    hash[0]= 1;
    hash[1]= 3;
    hash[2]= 9;
    hash[3]=27;
    hashed = 1;
  }

  for (b=CCTK_NSTAG;b>=0;b--) 
  {
    val = (int)(si / hash[b]);
    si  = si % hash[b];
    if (dir==b) 
    {
      dsi = val;
      break;
    }
  }
  return(dsi);
}



void FMODIFIER FORTRAN_NAME(CCTK_StaggerDirIndex)
     ( int *dsi, int *dir, int *gsi) 
{
  /* accept fortran indexing [1..]: decrease the directional index
     for the call to the C routine.  */
  *dsi  = CCTK_StaggerDirIndex((*dir)-1, *gsi);
} 



/*@@
   @routine    CCTK_StaggerDirIndexArray
   @date       
   @author     Gerd Lanfermann
   @desc       returns the stagger index for all direction in 
               an array <dindex> of size <dim> when given the staggerindex <sc>.

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
 
int CCTK_StaggerDirArray(int *dindex , int dim, int sindex) 
{
  int val,b;
  static int hash[4],hashed=0;

  if (hashed==0) 
  {
    hash[0]= 1;
    hash[1]= 3;
    hash[2]= 9;
    hash[3]=27;
    hashed = 1;
  }

  if (dim>4) {
    CCTK_Warn(1,__LINE__,__FILE__,"Cactus", "Max. Staggerdims: 4");
    return(-1);
  }

  for (b=CCTK_NSTAG;b>=0;b--) 
  {
    val       = (int)(sindex / hash[b]);
    sindex    = sindex % hash[b];
    if (b<dim) dindex[b] = val;
  }
  return(0);
}


void FMODIFIER FORTRAN_NAME(CCTK_GroupStaggerDirArray)
     (int *ierr, int *dindex, int *dim, int *gsc) 
{
  /* accept fortran indexing [1..]: decrease the directional index
     for the call to the C routine.  */
  *ierr = CCTK_StaggerDirArray(dindex, *dim, *gsc);
} 

 /*@@
   @routine    CCTK_GroupStaggerDirArrayGI
   @date       
   @author     Gerd Lanfermann
   @desc       returns the stagger index for all direction in 
               an array <dindex> of size <dim> when given the group
               index <gi>

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
 
int CCTK_GroupStaggerDirArrayGI(int *dindex, int dim, int gi) 
{
  int si,ierr;
  si  = CCTK_GroupStaggerIndexGI(gi);
  ierr= CCTK_StaggerDirArray(dindex, dim, si);
}

void FMODIFIER FORTRAN_NAME(CCTK_GroupStaggerDirArrayGI)
     (int *ierr, int *dindex, int *dim, int *gi) 
{
  *ierr = CCTK_GroupStaggerDirArrayGI(dindex, *dim, *gi);
} 


 /*@@
   @routine    CCTK_StaggerDirName
   @date       
   @author     Gerd Lanfermann
   @desc       returns the directional staggering in direction <dir> 
               for a given stagger name <stype>.

   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
 
int CCTK_StaggerDirName(int dir, const char *stype) 
{
  int scode;
  char hs[7]="MMMMMM",*info;

  sprintf(hs,"%s",stype);

  if (dir>strlen(hs)) 
  {
    CCTK_Warn(1,__LINE__,__FILE__,"Cactus","Stagger name too short for requested direction");
  }

  switch (toupper(hs[dir]))
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

void FMODIFIER FORTRAN_NAME(CCTK_StaggerDirName)
     (int *dsc, int *dir, ONE_FORTSTRING_ARG) 
{
  ONE_FORTSTRING_CREATE(sname);

  *dsc = CCTK_StaggerDirName((*dir)-1,sname);

  free(sname);
}






 /*@@
   @routine    CCTKi_ParseStaggerString
   @date       
   @author     Gerd Lanfermann
   @desc       returns the stagger index for a string. Similar routines 
               as CCTK_StaggerIndexName, but does more error checking since 
               it is called during Group setup and if things go wrong, the used
               has a better idea where he specified wrong settings.

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
  if (CCTK_Equals(stype,"NONE"))
  {
    strncpy(hs,"MMMMMM",dim);
  }
  else if (CCTK_Equals(stype,"CELL")==0) 
  {
    strncpy(hs,"CCCCCC",dim);
  }
  else 
  {
    sprintf(hs,"%s",stype);
  }

  for (i=0;i<dim;i++) 
  {
    switch (toupper(hs[i]))
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
