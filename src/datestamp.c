 /*@@
   @file      datestamp.c
   @date      Mon May 11 10:20:58 1998
   @author    Paul Walker
   @desc 
   
   @enddesc 
   #version $Header$
 @@*/

#include <stdio.h>

#include "cctk_Version.h"
#include "cctki_version.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(datestamp_c)

const char *CCTKi_version_src_datestamp_c(void);
const char *CCTKi_version_src_datestamp_c(void) { return rcsid; }

 /*@@
   @routine    CCTKi_DataStamp
   @date       Mon May 11 10:20:58 1998
   @author     Paul Walker
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void CCTKi_DateStamp(void) 
{
  printf ("  Compiled on %s at %s\n", __DATE__, __TIME__);
}

 /*@@
   @routine    CCTK_CompileTime
   @date       Mon May 11 10:20:58 1998
   @author     Paul Walker
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
const char *CCTK_CompileTime(void) 
{
  return (__TIME__);
}

 /*@@
   @routine    CCTK_CompileDate
   @date       Mon May 11 10:20:58 1998
   @author     Paul Walker
   @desc 
   
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
const char *CCTK_CompileDate(void) 
{
  return (__DATE__);
}


/* Macros to turn things into strings. */

#define STRINGIFY(a) REALSTRINGIFY(a)

#define REALSTRINGIFY(a) #a


const char *CCTK_FullVersion(void)
{
  return (STRINGIFY(CCTK_VERSION));
}

const char *CCTK_MajorVersion(void)
{
  return (STRINGIFY(CCTK_VERSION_MAJOR));
}

const char *CCTK_MinorVersion(void)
{
  return (STRINGIFY(CCTK_VERSION_MINOR));
}

const char *CCTK_MicroVersion(void)
{
  return (STRINGIFY(CCTK_VERSION_MICRO));
}

/*#define MAKETEST*/
#ifdef MAKETEST
int main(void)
{
  printf("CCTK maketest compiled on %s\n", compileDate());

  return 0;
}
#endif
