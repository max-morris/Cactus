 /*@@
   @file      RecordImplementation.c
   @date      Wed Jan 13 21:03:55 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "StoreNamedData.h"

static char *rcsid = "$Header$";

typedef struct 
{
  int n_thorns;
  char **thornlist;
} t_ImplementationData;

static pNamedData *implementation_data = NULL;


 /*@@
   @routine    CCTK_RecordImplementation
   @date       Wed Jan 13 23:23:00 1999
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_RecordImplementation(const char *implementation,
                              const char *thorn)
{
  int retval;
  char **temp;

  t_ImplementationData *data;

  if((data = (t_ImplementationData *)GetNamedData(implementation_data, implementation)))
  {
    data->n_thorns++;
    temp = (char **)realloc(data->thornlist,data->n_thorns*sizeof(char *));
    if(temp)
    {
      data->thornlist = temp;
      data->thornlist[data->n_thorns-1] = (char *)malloc((strlen(thorn)+1)*sizeof(char));
      if(data->thornlist[data->n_thorns-1])
      {
        strcpy(data->thornlist[data->n_thorns-1], thorn);
        retval = 0;
      }
      else
      {
        retval = 4;
      }
    }
    else
    {
      fprintf(stderr, "Unable to allocate memory for new thorn %s\n", thorn);
      retval = 1;
    }
  }
  else
  {
    data = (t_ImplementationData *)malloc(sizeof(t_ImplementationData));

    if(data)
    {
      data->thornlist = (char **)malloc(sizeof(char *));
      if(data->thornlist)
      {
        data->thornlist[0] = (char *)malloc((strlen(thorn)+1)*sizeof(char));
        if(data->thornlist[0])
        {
          strcpy(data->thornlist[0], thorn);
          data->n_thorns = 1;
          StoreNamedData(&implementation_data,implementation, data);
          retval = 0;
        }
        else
        {
          retval = 4;
        }
      }
      else
      {
        fprintf(stderr, "Unable to allocate memory for new thorn %s\n", thorn);
        retval = 3;
      }
    }
    else
    {
      fprintf(stderr, "Unable to allocate memory for new thorn %s\n", thorn);
      retval = 2;
    }

  }

  return retval;
}
  
 /*@@
   @routine    GetImplementationThorns
   @date       Wed Jan 13 23:22:43 1999
   @author     Tom Goodale
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int GetImplementationThorns(const char *implementation, char ***thornlist)
{
  int retval;
  t_ImplementationData *data;

  if((data = (t_ImplementationData *)GetNamedData(implementation_data, implementation)))
  {
    *thornlist = data->thornlist;
    retval = data->n_thorns;
  }
  else
  {
    *thornlist = NULL;
    retval = 0;
  }

  return retval;
}
