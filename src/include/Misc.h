 /*@@
   @header    Misc.h
   @date      Wed Jan 20 10:39:01 1999
   @author    Tom Goodale
   @desc 
   header file for miscellaneous routines.
   @enddesc 
   @version $Id$
 @@*/

#ifndef _MISC_H_
#define _MISC_H_

#ifdef __cplusplus
extern "C" {
#endif

int CCTK_SplitString(char **before, char **after, const char *string, const char *sep);

int CCTK_Equals(const char *string1, const char *string2);

char *CCTK_NullTerminateString(const char *, unsigned int);

int CCTK_InList(const char *string1, int n_elements, ...);

int CCTK_IntInRange(int inval, const char *range);
int CCTK_DoubleInRange(double inval, const char *range);
int CCTK_IntInRangeList(int inval, int n_elements, ...);
int CCTK_DoubleInRangeList(double inval, int n_elements, ...);

int CCTK_SetDoubleInRangeList(double *data, const char *value, 
			      int n_elements, ...);
int CCTK_SetIntInRangeList(int *data, const char *value, 
			   int n_elements, ...);
int CCTK_SetKeywordInRangeList(char **data, const char *value, 
			       int n_elements, ...);
int CCTK_SetString(char **data, const char *value);
int CCTK_SetLogical(int *data, const char *value);

#ifdef __cplusplus
           }   
#endif

#endif
