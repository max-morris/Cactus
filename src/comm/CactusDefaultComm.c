 /*@@
   @file      CactusDefaultComm.c
   @date      Tue Sep 29 15:06:22 1998
   @author    Tom Goodale
   @desc 
   Default communication routines.
   @enddesc 
 @@*/


#include <stdio.h>
#include <stdlib.h>

#include "flesh.h"
#include "CactusMainDefaults.h"

static char *rcsid = "$Id$";


cGH *CactusDefaultSetupGH(tFleshConfig *config, int convergence_level)
{
  cGH *thisGH;
  
  cGFconfig *GF;
  
  cGF **tempGFs;

  cScala
  /* Create a new Grid Hierarchy */
  thisGH = (cGH *)malloc(sizeof(cGH));

  if(thisGH)
  {
    /* Traverse list of GH setup rroutines. */
    CactusSetupGHTraverse(config, convergence_level, thisGH);

    /* Setup GFs */
    thisGH->GFs = NULL;
    thisGH->nGFs = 0;
    for(GF = config->GFs; GF; GF = GF->next)
    {
      thisGH->nGFs++;
      tempGFs = (cGF **)realloc(thisGH->GFs, thisGH->nGFs*sizeof(cGF *));
      if(tempGFs)
      {
	thisGH->GFs = tempGFs;
	thisGH->GFs[thisGH->nGFs-1] = SetupGF(GH, GF);
      }
    }

  return thisGH;
}


cGF *CactusDefaultSetupGF(cGH *GH, cGFconfig *configdata)
{
  
  printf("I'm at line %d of file %s\n", __LINE__, __FILE__);
}

int CactusDefaultSyncAllFuncs(cGH *GH)
{
  printf("I'm at line %d of file %s\n", __LINE__, __FILE__);
}

int CactusDefaultSyncGroupFuncs(cGH *GH, const char *group)
{
  printf("I'm at line %d of file %s\n", __LINE__, __FILE__);
}

int CactusDefaultSyncOneFunc(cGH *GH, cGF *GF)
{
  printf("I'm at line %d of file %s\n", __LINE__, __FILE__);
}

int CactusDefaultParallelInit(tFleshConfig *config)
{
  printf("I'm at line %d of file %s\n", __LINE__, __FILE__);
}

int CactusDefaultParallelFinalise(tFleshConfig *config)
{
  printf("I'm at line %d of file %s\n", __LINE__, __FILE__);
}

int CactusDefaultReduce(cGH *GH, cGF *GF, int operation, void *result)
{
  printf("I'm at line %d of file %s\n", __LINE__, __FILE__);
}

