 /*@@
   @file      datestamp.c
   @date      Mon May 11 10:20:58 1998
   @author    Paul Walker
   @desc 
   
   @enddesc 
 @@*/
#include <stdio.h>

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

/*#define MAKETEST*/
#ifdef MAKETEST
int main(void)
{
  printf("CCTK maketest compiled on %s\n", compileDate());

  return 0;
}
#endif
