 /*@@
   @header    util_String.h
   @date      Tue May  2 11:00:39 2000
   @author    Tom Goodale
   @desc 
   String routines
   @enddesc 
   @version $History$
 @@*/

#ifndef _UTIL_STRING_H_
#define _UTIL_STRING_H_ 1

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

char *Util_Strdup(const char *s);

int Util_StrCmpi(const char *string1, 
                 const char *string2);

#ifdef __cplusplus
}
#endif

#endif /* _UTIL_STRING_H_ */
