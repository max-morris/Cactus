 /*@@
   @file      FortranWrappers.c
   @date      Sat Sep 18 00:42:12 1999
   @author    Tom Goodale
   @desc 
   File for dealing with fortran wrapper functions.
   @enddesc 
 @@*/

static char *rcsid = "$Header$";

#include <stdlib.h>

#include "cctk_FortranWrappers.h"

#include "StoreNamedData.h"

static pNamedData *registry = NULL;

 /*@@
   @routine    CCTK_RegisterFortranWrapper
   @date       Sat Sep 18 00:51:21 1999
   @author     Tom Goodale
   @desc 
   Registers a Fortran wrapper function.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTK_RegisterFortranWrapper(const char *name, int (*function)(void *, void *))
{
  int retcode;
  retcode = StoreNamedData(&registry, name, (void *)function);

  return retcode;
}

 /*@@
   @routine    CCTK_FortranWrapper
   @date       Sat Sep 18 00:51:57 1999
   @author     Tom Goodale
   @desc 
   Gets a Fortran wrapper function. 
   (A function taking (void *,void *), and returning an int. )
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int (*CCTK_FortranWrapper(const char *name))(void *, void *)
{
  return (int (*)(void *,void *))GetNamedData(registry, name);
}
