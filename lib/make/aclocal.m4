dnl /*@@
dnl   @file      aclocal.m4
dnl   @date      Thu Oct 21 00:05:58 1999
dnl   @author    Tom Goodale
dnl   @desc 
dnl   Local Cactus macros
dnl   @enddesc
dnl   @version $Header$ 
dnl @@*/


dnl  These are copies of the standard autoconf macros, except they
dnl  use AC_TRY_COMPILE rather than AC_TRY_CPP to check for headers.
dnl  This gets round the problem on cygwin where the gnu cpp finds
dnl  the gcc headers and not the ones for the actual compiler.

dnl CCTK_CHECK_HEADER(HEADER-FILE, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
AC_DEFUN(CCTK_CHECK_HEADER,
[dnl Do the transliteration at runtime so arg 1 can be a shell variable.
cctk_safe=`echo "$1" | sed 'y%./+-%__p_%'`
AC_MSG_CHECKING([for $1])
AC_CACHE_VAL(cctk_cv_header_$cctk_safe,
[AC_TRY_COMPILE([#include <$1>], [return 0;], eval "cctk_cv_header_$cctk_safe=yes",
  eval "cctk_cv_header_$cctk_safe=no")])dnl
if eval "test \"`echo '$cctk_cv_header_'$cctk_safe`\" = yes"; then
  AC_MSG_RESULT(yes)
  ifelse([$2], , :, [$2])
else
  AC_MSG_RESULT(no)
ifelse([$3], , , [$3
])dnl
fi
])

dnl CCTK_CHECK_HEADERS(HEADER-FILE... [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
AC_DEFUN(CCTK_CHECK_HEADERS,
[for cctk_hdr in $1
do
CCTK_CHECK_HEADER($cctk_hdr,
[changequote(, )dnl
  cctk_tr_hdr=HAVE_`echo $cctk_hdr | sed 'y%abcdefghijklmnopqrstuvwxyz./-%ABCDEFGHIJKLMNOPQRSTUVWXYZ___%'`
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($cctk_tr_hdr) $2], $3)dnl
done
])


dnl CCTK_FIND_NULLDEVICE
dnl Have to do it in this rather bizarre way
dnl as cygwin emulates /dev/null 
AC_DEFUN(CCTK_FIND_NULLDEVICE,
[AC_MSG_CHECKING([for the null device])
AC_CACHE_VAL(cctk_cv_nulldevice,
[
if test -d /dev ; then
  eval "cctk_cv_nulldevice=/dev/null"
else
  cat > NUL <<EOF
test
EOF
  if eval "`cat NUL > /dev/null 2>/dev/null`" ; then
    eval "cctk_cv_nulldevice=NUL"
  fi
fi
])
if eval "test -n \"$cctk_cv_nulldevice\"" ; then
  AC_MSG_RESULT($cctk_cv_nulldevice)
  AC_DEFINE_UNQUOTED(NULL_DEVICE, "$cctk_cv_nulldevice")
else
  AC_MSG_RESULT("not found")
fi
])

AC_DEFUN(CCTK_TIME__FTIME,
[AC_MSG_CHECKING([for availability of _ftime timing])
AC_CACHE_VAL(cctk_cv_time_ftime,
[AC_TRY_LINK([#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>], 
[  struct _timeb timebs;
  _ftime(&timebs);
  printf("%f\n",(double)(timebs.time + timebs.millitm/1000.0));
  return 0;], eval "cctk_cv_time_ftime=yes",
  eval "cctk_cv_time_ftime=no")])dnl
if eval "test \"`echo '$cctk_cv_time_ftime'`\" = yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE_UNQUOTED(HAVE_TIME__FTIME)
else
  AC_MSG_RESULT(no)
fi
])dnl

AC_DEFUN(CCTK_TIME_GETRUSAGE,
[AC_MSG_CHECKING([for availability of getrusage timing])
AC_CACHE_VAL(cctk_cv_time_getrusage,
[AC_TRY_LINK([#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>], 
[struct rusage ru;
 getrusage(RUSAGE_SELF, &ru);
 printf("%f\n",(double)(ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec/1000000.0));
 return 0;], eval "cctk_cv_time_getrusage=yes",
  eval "cctk_cv_time_getrusage=no")])dnl
if eval "test \"`echo '$cctk_cv_time_getrusage'`\" = yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE_UNQUOTED(HAVE_TIME_GETRUSAGE)
else
  AC_MSG_RESULT(no)
fi
])dnl

AC_DEFUN(CCTK_TIME_GETTIMEOFDAY,
[AC_MSG_CHECKING([for availability of gettimeofday timing])
AC_CACHE_VAL(cctk_cv_time_gettimeofday,
[AC_TRY_LINK([], 
[gettimeofday();
 return 0;], eval "cctk_cv_time_gettimeofday=yes",
  eval "cctk_cv_time_gettimeofday=no")])dnl
if eval "test \"`echo '$cctk_cv_time_gettimeofday'`\" = yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE_UNQUOTED(HAVE_TIME_GETTIMEOFDAY)
else
  AC_MSG_RESULT(no)
fi
]dnl
if eval "test \"`echo '$cctk_cv_time_gettimeofday'`\" = yes"; then
[AC_MSG_CHECKING([if gettimeofday needs timezone])
AC_CACHE_VAL(cctk_cv_time_gettimeofday_timezone,
[AC_TRY_LINK([#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>], 
[struct timeval tp;
 struct timezone tzp;
 gettimeofday(&tp, &tzp);
 printf("%f\n", (double)(tp.tv_sec + (double)tp.tv_usec/1000000.0));
 return 0;], eval "cctk_cv_time_gettimeofday_timezone=yes",
  eval "cctk_cv_time_gettimeofday_timezone=no")])dnl
if eval "test \"`echo '$cctk_cv_time_gettimeofday_timezone'`\" = yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE_UNQUOTED(GETTIMEOFDAY_NEEDS_TIMEZONE)
else
  AC_MSG_RESULT(no)
fi
]dnl
fi
)dnl

AC_DEFUN(CCTK_PROG_CC_WORKS,
[AC_MSG_CHECKING([whether the C compiler ($CC $CFLAGS $LDFLAGS) works])
AC_LANG_SAVE
AC_LANG_C
AC_TRY_COMPILER([main(){return(0);} PilotMain(){return(0);}], ac_cv_prog_cc_works, ac_cv_prog_cc_cross)
AC_LANG_RESTORE
AC_MSG_RESULT($ac_cv_prog_cc_works)
if test $ac_cv_prog_cc_works = no; then
  AC_MSG_ERROR([installation or configuration problem: C compiler cannot create executables.])
fi
AC_MSG_CHECKING([whether the C compiler ($CC $CFLAGS $LDFLAGS) is a cross-compiler])
AC_MSG_RESULT($ac_cv_prog_cc_cross)
cross_compiling=$ac_cv_prog_cc_cross
])

AC_DEFUN(CCTK_PROG_CXX_WORKS,
[AC_MSG_CHECKING([whether the C++ compiler ($CXX $CXXFLAGS $LDFLAGS) works])
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILER([int main(){return(0);} extern "C" int PilotMain(){return(0);}], ac_cv_prog_cxx_works, ac_cv_prog_cxx_cross)
AC_LANG_RESTORE
AC_MSG_RESULT($ac_cv_prog_cxx_works)
if test $ac_cv_prog_cxx_works = no; then
  AC_MSG_ERROR([installation or configuration problem: C++ compiler cannot create executables.])
fi
AC_MSG_CHECKING([whether the C++ compiler ($CXX $CXXFLAGS $LDFLAGS) is a cross-compiler])
AC_MSG_RESULT($ac_cv_prog_cxx_cross)
cross_compiling=$ac_cv_prog_cxx_cross
])

AC_DEFUN(CCTK_HEADER_REGEX,
[AC_MSG_CHECKING([for regex.h])
AC_CACHE_VAL(cctk_cv_header_regex_h,
[AC_TRY_COMPILE([#include <stdio.h>
#include <regex.h>], [return 0;], eval "cctk_cv_header_regex_h=yes",
  eval "cctk_cv_header_regex_h=no")])dnl
if eval "test \"`echo '$cctk_cv_header_regex_h'`\" = yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE_UNQUOTED(HAVE_REGEX_H)
else
  AC_MSG_RESULT(no)
fi
])

dnl
dnl  Figure out if there is socklen_t by checking getsockname
dnl  -------------------------------------------------------------
AC_DEFUN(CCTK_CHECK_SOCK_LENGTH_TYPE,
[
AC_LANG_SAVE
AC_LANG_C

dnl Save old libs
dnl Add the socket and nsl libs if they exist (sun)
ac_save_LIBS="$LIBS"
AC_CHECK_LIB(socket,main)
AC_CHECK_LIB(nsl,main)

socket_argtype=
cat > socketHdrs.h << EOF
EOF
AC_CHECK_HEADER(unistd.h, [ echo "#include <unistd.h>" >> socketHdrs.h ])
AC_CHECK_HEADER(sys/types.h, [ echo "#include <sys/types.h>" >> socketHdrs.h ])
AC_CHECK_HEADER(sys/socket.h, [ echo "#include <sys/socket.h>" >> socketHdrs.h ])
if test "$ARCH" = "intelnt" ; then
        AC_CHECK_HEADER(winsock2.h, [ echo "#include <winsock2.h>" >> socketHdrs.h ])
fi
AC_LANG_SAVE
AC_LANG_CPLUSPLUS

try="socklen_t"
AC_TRY_LINK(
[
#include <stdio.h>
#include "socketHdrs.h"
],
[
$try *foo = NULL;
int i = getsockname(1, (struct sockaddr *)NULL, foo);
],
[ ],
[
try="int"
])

AC_DEFINE_UNQUOTED(CCTK_SOCKLEN_T, $try)
echo checking third arg to getsockname: is pointer to $try
rm socketHdrs.h
LIBS="$ac_save_LIBS"
AC_LANG_RESTORE
])


