 /*@@
   @header    util_String.h
   @date      Tue May  2 11:00:39 2000
   @author    Tom Goodale
   @desc 
   String routines
   @enddesc 
   @version $Header$
 @@*/

#ifndef _UTIL_STRING_H_
#define _UTIL_STRING_H_ 1

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" 
{
#endif

const char *Util_StrSep(const char **stringp, 
                        const char *delim);

int Util_SplitString(char **before, 
                     char **after, 
                     const char *string, 
                     const char *sep);

int Util_SplitFilename(char **dir, 
                       char **file, 
                       const char *string);

#define Util_Strdup strdup

size_t Util_Strlcpy(char* dst, const char* src, size_t dst_size);
size_t Util_Strlcat(char* dst, const char* src, size_t dst_size);

int Util_StrCmpi(const char *string1, 
                 const char *string2);
int Util_StrMemCmpi(const char *string1, 
                    const char *string2,
                    size_t len2);

int Util_vsnprintf (char *str, size_t count, const char *fmt, va_list args);
#define Util_snprintf snprintf   /* For backward compatibility */

int Util_asprintf(char **buffer, const char *fmt, ...);
int Util_asnprintf(char **buffer, size_t size, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* _UTIL_STRING_H_ */
