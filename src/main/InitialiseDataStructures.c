 /*@@
   @file      InitialiseDataStructures.c
   @date      Wed Jan 13 20:28:08 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
 @@*/

#include <stdio.h>

#include "flesh.h"


#define TEST_THORNREGISTRATION
#ifdef TEST_THORNREGISTRATION

int test_param_init();
int test_param_set(const char *, const char *);
int test_param_get(const char *, void **);
int test_private_group_setup(cGH *);
int test_protected_group_setup(cGH *);
int test_public_group_setup(cGH *);


static t_thorndata thorndata[] = {"test", "test", test_param_init, test_param_set, test_param_get, test_private_group_setup, test_protected_group_setup, test_public_group_setup};

static int n_thorns=1;

#else

#include "thorndata.h"

#endif

static char *rcsid = "$Id$";

int InitialiseDataStructures(tFleshConfig *ConfigData)
{
  int i;

  for(i=0; i < n_thorns; i++)
  {
    CCTK_RegisterThorn(thorndata[i].name, thorndata[i].implementation,
		       &(thorndata[i]));
  }

  return 0;
}

#ifdef TEST_THORNREGISTRATION

int test_param_init()
{
}

int test_param_set(const char *param, const char *value)
{
}
int test_param_get(const char *name, void **value)
{
}
int test_private_group_setup(cGH *GH)
{
}
int test_protected_group_setup(cGH *GH)
{
}
int test_public_group_setup(cGH *GH)
{
}


#endif

