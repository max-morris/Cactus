 /*@@
   @file      datestamp.c
   @date      Mon May 11 10:20:58 1998
   @author    Paul Walker
   @desc 
   
   @enddesc 
 @@*/
#include <stdio.h>
#include "cctk_version.h"

static char *rcsid = "$Id$";

 /*@@
   @routine    datastamp
   @date       Mon May 11 10:20:58 1998
   @author     Paul Walker
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void datestamp(void) {
  printf ("  Compiled on %s at %s\n", __DATE__, __TIME__);
}

 /*@@
   @routine    compileTime
   @date       Mon May 11 10:20:58 1998
   @author     Paul Walker
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
char *compileTime(void) {
  return (__TIME__);
}

 /*@@
   @routine    compileDate
   @date       Mon May 11 10:20:58 1998
   @author     Paul Walker
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
char *compileDate(void) {
  return (__DATE__);
}


/* Macros to turn things into strings. */

#define STRINGIFY(a) REALSTRINGIFY(a)

#define REALSTRINGIFY(a) #a


char *CCTK_FullVersion(void)
{
  return (STRINGIFY(CCTK_VERSION));
}

char *CCTK_MajorVersion(void)
{
  return (STRINGIFY(CCTK_VERSION_MAJOR));
}

char *CCTK_MinorVersion(void)
{
  return (STRINGIFY(CCTK_VERSION_MINOR));
}

char *CCTK_OtherVersion(void)
{
  return (STRINGIFY(CCTK_VERSION_OTHER));
}

/*#define MAKETEST*/
#ifdef MAKETEST
int main(void)
{
  printf("CCTK maketest compiled on %s\n", compileDate());

  return 0;
}
#endif
