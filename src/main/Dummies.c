 /*@@
   @file      Dummies.c
   @date      Tue Feb  2 18:56:38 1999
   @author    Tom Goodale
   @desc 
   A file to contain some dummy functions until the real ones are written. 
   @enddesc 
 @@*/

#include "Dummies.h"

static char *rcsid = "$Id$";

int CCTK_DummyStorageOn(void *GH, int group)
{
  return 0;
}

int CCTK_DummyStorageOff(void *GH, int group)
{
  return 0;
}

int CCTK_DummyCommunicationOn(void *GH, int group)
{
  return 0;
}

int CCTK_DummyCommunicationOff(void *GH, int group)
{
  return 0;
}

int CCTK_DummyTriggerable(int variable)
{
  return 0;
}

int CCTK_DummyTriggerSaysGo(void *GH, int variable)
{
  return 1;
}

int CCTK_DummyTriggerAction(void *GH, int group)
{
  return 0;
}


int CCTK_CallFunc(void *GH, int language, void *function)
{

  void (*calledfunc)(void *);

  calledfunc = (void (*)(void *))function;

  /* Call the function. */
  
  calledfunc(GH);

  return 0;
}
