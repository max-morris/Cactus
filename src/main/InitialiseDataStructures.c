 /*@@
   @file      InitialiseDataStructures.c
   @date      Wed Jan 13 20:28:08 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flesh.h"

#include "Implementations.h"
#include "StoreVariableData.h"

static char *rcsid = "$Id$";

#define TEST_THORNREGISTRATION
#ifdef TEST_THORNREGISTRATION

int test_startup();
int test_rfr_init(cGH *);
int test_param_init();
int test_param_set(const char *, const char *);
int test_param_get(const char *, void **);
int test_private_group_setup(cGH *);
int test_protected_group_setup(cGH *);
int test_public_group_setup(cGH *);


static t_thorndata thorndata[] = {"test", "test", test_startup, test_rfr_init, test_param_init, test_param_set, test_param_get, test_private_group_setup, test_protected_group_setup, test_public_group_setup};

static int n_thorns=1;

#else

#include "thorndata.h"

#endif


int InitialiseDataStructures(tFleshConfig *ConfigData)
{
  int i;

  ConfigData->nGHs = 0;
  ConfigData->GH = NULL;

  for(i=0; i < n_thorns; i++)
  {
    CCTK_RegisterThorn(thorndata[i].name, thorndata[i].implementation,
		       &(thorndata[i]));
  }

  return 0;
}

#ifdef TEST_THORNREGISTRATION

static struct
{
  int test_int;
  double test_double;
  char test_keyword[20];
} test_params = {1,1.5,"alpha"};

int test_startup()
{
}

int test_rfr_init(cGH *GH)
{
}

int test_param_init()
{
}

int test_param_set(const char *param, const char *value)
{
  char temp[1001];
  char *test;
  int p;

  if(!strcmp(param, "test_int"))
  {
    test_params.test_int  = atoi(value);
  }

  if(!strcmp(param, "test_double"))
  {
    strncpy(temp, value, 1000);
 
    for (p=0;p<strlen(temp);p++) 
    if (temp[p] == 'E' || temp[p] == 'd' || temp[p] == 'D')
      temp[p] = 'e';
    test_params.test_double  = atof(temp);
  }

  if(!strcmp(param, "test_keyword"))
  {
    if(!strcmp(value,"alpha")||
       !strcmp(value,"beta"))
    {
      strcpy(test_params.test_keyword, value);
    }	 
  }

}

int test_param_get(const char *param, void **value)
{
  int retval;

  int *temp_int;
  double *temp_double;
  char *temp_char;

  retval = 0;
  if(!strcmp(param, "test_int"))
  {
    temp_int = (int *)malloc(sizeof(int));
    *temp_int = test_params.test_int;
    *value = (void *)temp_int;
    retval = 1;
  }

  if(!strcmp(param, "test_double"))
  {
    temp_double = (double *)malloc(sizeof(double));
    *temp_double = test_params.test_double;
    *value = (void *)temp_double;
    retval = 2;
  }

  if(!strcmp(param, "test_keyword"))
  {
    temp_char = (char *)malloc(strlen((test_params.test_keyword)+1)*sizeof(char));
    strcpy(temp_char, test_params.test_keyword);
    *value = (void *)temp_char;
    retval = 3;
  }
  
  return retval;
    
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

