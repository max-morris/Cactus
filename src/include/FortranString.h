 /*@@
   @header    FortranString.h
   @date      Wed Feb  17 12:55 1999
   @author    Gabrielle Allen, Paul Walker
   @desc 
   Macros for dealing with strings passed from fortran to C.
   The memory for the new arguments created should be freed after use   

   To ensure that passing strings from fortran to C has a chance of 
   working with all compilers, we assume that

   ** Strings are always at the end of the argument list **

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
#define THREE_FORTSTRINGS_ARGS\
    _fcd fcd_arg1, _fcd fcd_arg2, _fcd fcd_arg3
#define ONE_FORTSTRING_CREATE(argn)\
       int len = _fcdlen(fcdarg); \
       char *argn    = CCTK_NullTerminateString(_fcdtocp(fcdarg),len);;
#define TWO_FORTSTRINGS_CREATE(argn,argw)\
       int  nl    = _fcdlen(fcd_n);\
       int  wl    = _fcdlen(fcd_w);\
       char *argn = CCTK_NullTerminateString(_fcdtocp(fcd_n),nl);\
       char *argw = CCTK_NullTerminateString(_fcdtocp(fcd_w),wl);
#define THREE_FORTSTRINGS_CREATE(arg1,arg2,arg3)\
       int  narg1    = _fcdlen(fcd_arg1);\
       int  narg2    = _fcdlen(fcd_arg2);\
       int  narg3    = _fcdlen(fcd_arg3);\
       char *arg1 = CCTK_NullTerminateString(_fcdtocp(fcd_arg1),narg1);\
       char *arg2 = CCTK_NullTerminateString(_fcdtocp(fcd_arg2),narg2);\
       char *arg3 = CCTK_NullTerminateString(_fcdtocp(fcd_arg3),narg3);

#elif defined WIN32

#define ONE_FORTSTRING_ARG\
   char *n, unsigned int nl
#define TWO_FORTSTRINGS_ARGS\
   char *n, int nl, char *w, int wl
#define THREE_FORTSTRINGS_ARGS\
   const char *CCTK_str1,\
   const char *CCTK_str2,\
   const char *CCTK_str3,\
   unsigned int CCTK_len1,\
   unsigned int CCTK_len2,\
   unsigned int CCTK_len3
#define ONE_FORTSTRING_CREATE(argn)\
        char *argn = CCTK_NullTerminateString(n,nl);
#define TWO_FORTSTRINGS_CREATE(argn,argw)\
    char *argn = CCTK_NullTerminateString(n,nl);\
    char *argw = CCTK_NullTerminateString(w,wl);
#define THREE_FORTSTRINGS_CREATE(arg1,arg2,arg3)\
   char *arg1 = CCTK_NullTerminateString(CCTK_str1,CCTK_len1);\
   char *arg2 = CCTK_NullTerminateString(CCTK_str2,CCTK_len2);\
   char *arg3 = CCTK_NullTerminateString(CCTK_str3,CCTK_len3);

#else

#define ONE_FORTSTRING_ARG\
   const char *CCTK_str1,\
   unsigned int CCTK_len1
#define TWO_FORTSTRINGS_ARGS\
   const char *CCTK_str1,\
   const char *CCTK_str2,\
   unsigned int CCTK_len1,\
   unsigned int CCTK_len2
#define THREE_FORTSTRINGS_ARGS\
   const char *CCTK_str1,\
   const char *CCTK_str2,\
   const char *CCTK_str3,\
   unsigned int CCTK_len1,\
   unsigned int CCTK_len2,\
   unsigned int CCTK_len3
#define ONE_FORTSTRING_CREATE(arg1)\
   char *arg1 = CCTK_NullTerminateString(CCTK_str1,CCTK_len1);
#define TWO_FORTSTRINGS_CREATE(arg1,arg2)\
   char *arg1 = CCTK_NullTerminateString(CCTK_str1,CCTK_len1);\
   char *arg2 = CCTK_NullTerminateString(CCTK_str2,CCTK_len2);
#define THREE_FORTSTRINGS_CREATE(arg1,arg2,arg3)\
   char *arg1 = CCTK_NullTerminateString(CCTK_str1,CCTK_len1);\
   char *arg2 = CCTK_NullTerminateString(CCTK_str2,CCTK_len2);\
   char *arg3 = CCTK_NullTerminateString(CCTK_str3,CCTK_len3);

#endif







