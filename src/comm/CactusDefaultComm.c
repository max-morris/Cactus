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


int CactusDefaultSetupGH(cGH *GH)
{
  printf("I'm at line %d of file %s\n", __LINE__, __FILE__);
}


int CactusDefaultSetupGF(cGH *GH, cGF *GF)
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

