 /*@@
   @header    cctki_Groups.h
   @date      Mon Feb  8 14:47:10 1999
   @author    Tom Goodale
   @desc 
   Prototypes and constants for internal group functions.
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTKI_GROUPS_H_
#define _CCTKI_GROUPS_H_

/* Prototypes */

#ifdef __cplusplus 
extern "C" {
#endif

int CCTKi_CreateGroup(const char *gname, const char *thorn, const char *imp,
                      const char *gtype,
                      const char *vtype,
                      const char *gscope,
                      int dimension,
                      int ntimelevels,
                      const char *stype,
                      const char *size,
                      int n_variables,
                      ...);

#ifdef __cplusplus 
}
#endif


/* Group Types */

#define GROUP_SCALAR 1
#define GROUP_GF     2
#define GROUP_ARRAY  3

#define GROUP_PRIVATE   1
#define GROUP_PROTECTED 2
#define GROUP_PUBLIC    3

#endif
