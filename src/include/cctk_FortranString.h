 /*@@
   @header    cctk_FortranString.h
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

#include "cctk_Misc.h"

#if defined T3E

#include <fortran.h>
#define ONE_FORTSTRING_ARG\
    _fcd cctk_str1
#define TWO_FORTSTRINGS_ARGS\
    _fcd cctk_str1, _fcd cctk_str2
#define THREE_FORTSTRINGS_ARGS\
    _fcd cctk_str1, _fcd cctk_str2, _fcd cctk_str3
#define ONE_FORTSTRING_CREATE(arg1)\
       int cctk_strlen1 = _fcdlen(arg1); \
       char *arg1 = Util_NullTerminateString(_fcdtocp(cctk_str1),cctk_strlen1);;
#define TWO_FORTSTRINGS_CREATE(arg1,arg2)\
       int  cctk_strlen1    = _fcdlen(arg1);\
       int  cctk_strlen2    = _fcdlen(arg2);\
       char *arg1 = Util_NullTerminateString(_fcdtocp(cctk_arg1),cctk_strlen1);\
       char *arg2 = Util_NullTerminateString(_fcdtocp(cctk_arg2),cctk_strlen2);
#define THREE_FORTSTRINGS_CREATE(cctk_str1,cctk_str2,cctk_str3)\
       int  cctk_strlen1    = _fcdlen(fcd_arg1);\
       int  cctk_strlen2    = _fcdlen(fcd_arg2);\
       int  cctk_strlen3    = _fcdlen(fcd_arg3);\
       char *arg1 = Util_NullTerminateString(_fcdtocp(cctk_str1),cctk_strlen1);\
       char *arg2 = Util_NullTerminateString(_fcdtocp(cctk_str2),cctk_strlen2);\
       char *arg3 = Util_NullTerminateString(_fcdtocp(cctk_str3),cctk_strlen3);

#elif defined WIN32

#define ONE_FORTSTRING_ARG\
   char *n, unsigned int cctk_strlen1
#define TWO_FORTSTRINGS_ARGS\
   char *n, int cctk_strlen1, char *w, int cctk_strlen2
#define THREE_FORTSTRINGS_ARGS\
   char *cctk_str1,\
   char *cctk_str2,\
   char *cctk_str3,\
   unsigned int cctk_strlen1,\
   unsigned int cctk_strlen2,\
   unsigned int cctk_strlen3
#define ONE_FORTSTRING_CREATE(arg1)\
        char *arg1 = Util_NullTerminateString(cctk_str1,cctk_strlen1);
#define TWO_FORTSTRINGS_CREATE(arg1,arg2)\
    char *arg1 = Util_NullTerminateString(cctk_str1,cctk_strlen1);\
    char *arg2 = Util_NullTerminateString(cctk_str2,cctk_strlen2);
#define THREE_FORTSTRINGS_CREATE(arg1,arg2,arg3)\
   char *arg1 = Util_NullTerminateString(cctk_str1,cctk_strlen1);\
   char *arg2 = Util_NullTerminateString(cctk_str2,cctk_strlen2);\
   char *arg3 = Util_NullTerminateString(cctk_str3,cctk_strlen3);

#else

#define ONE_FORTSTRING_ARG\
   char *cctk_str1,\
   unsigned int cctk_strlen1
#define TWO_FORTSTRINGS_ARGS\
   char *cctk_str1,\
   char *cctk_str2,\
   unsigned int cctk_strlen1,\
   unsigned int cctk_strlen2
#define THREE_FORTSTRINGS_ARGS\
   char *cctk_str1,\
   char *cctk_str2,\
   char *cctk_str3,\
   unsigned int cctk_strlen1,\
   unsigned int cctk_strlen2,\
   unsigned int cctk_strlen3
#define ONE_FORTSTRING_CREATE(arg1)\
   char *arg1 = Util_NullTerminateString(cctk_str1,cctk_strlen1);
#define TWO_FORTSTRINGS_CREATE(arg1,arg2)\
   char *arg1 = Util_NullTerminateString(cctk_str1,cctk_strlen1);\
   char *arg2 = Util_NullTerminateString(cctk_str2,cctk_strlen2);
#define THREE_FORTSTRINGS_CREATE(arg1,arg2,arg3)\
   char *arg1 = Util_NullTerminateString(cctk_str1,cctk_strlen1);\
   char *arg2 = Util_NullTerminateString(cctk_str2,cctk_strlen2);\
   char *arg3 = Util_NullTerminateString(cctk_str3,cctk_strlen3);

#endif
