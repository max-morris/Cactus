 /*@@
   @header    FortranString.h
   @date      Wed Feb  17 12:55 1999
   @author    Gabrielle Allen, Paul Walker
   @desc 
   Macros for dealing with strings passed from fortran to C.
   The memory for the new arguments created should be freed after use   
   @enddesc 
   @version $Header$
 @@*/

#include "Misc.h"

#if defined T3E
#include <fortran.h>
#define ONE_FORTSTRING_ARG\
    _fcd fcdarg
#define TWO_FORTSTRINGS_ARGS\
    _fcd fcd_n, _fcd fcd_w
#define ONE_FORTSTRING_CREATE(argn)\
       int len = _fcdlen(fcdarg); \
       char *argn    = CCTK_NullTerminateString(_fcdtocp(fcdarg),len);;
#define TWO_FORTSTRINGS_CREATE(argn,argw)\
       int  nl    = _fcdlen(fcd_n);\
       int  wl    = _fcdlen(fcd_w);\
       char *argn = CCTK_NullTerminateString(_fcdtocp(fcd_n),nl);\
       char *argw = CCTK_NullTerminateString(_fcdtocp(fcd_w),wl);
#elif defined WIN32
#define ONE_FORTSTRING_ARG\
   char *n, unsigned int nl
#define TWO_FORTSTRINGS_ARGS\
   char *n, int nl, char *w, int wl
#define ONE_FORTSTRING_CREATE(argn)\
        char *argn = CCTK_NullTerminateString(n,nl);
#define TWO_FORTSTRINGS_CREATE(argn,argw)\
    char *argn = CCTK_NullTerminateString(n,nl);\
    char *argw = CCTK_NullTerminateString(w,wl);
#else
#define ONE_FORTSTRING_ARG\
   char *n, unsigned int nl
#define TWO_FORTSTRINGS_ARGS\
   const char *n, const char *w, int nl, int wl
#define ONE_FORTSTRING_CREATE(argn)\
        char *argn = CCTK_NullTerminateString(n,nl);
#define TWO_FORTSTRINGS_CREATE(argn,argw)\
    char *argn = CCTK_NullTerminateString(n,nl);\
    char *argw = CCTK_NullTerminateString(w,wl);
#endif

