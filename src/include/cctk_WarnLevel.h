 /*@@
   @header    cctk_WarnLevel.h
   @date      Wed Feb 17 00:53:55 1999
   @author    Tom Goodale
   @desc 
   Header for the warning functions.
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTK_WARNLEVEL_H_
#define _CCTK_WARNLEVEL_H_

#ifdef __cplusplus 
extern "C" 
{
#endif

int CCTK_Warn(int level, 
               int line, 
               const char *file, 
               const char *thorn, 
               const char *message);
int CCTK_VWarn(int level, 
                int line, 
                const char *file, 
                const char *thorn, 
                const char *format, ...)
#ifdef __GNUC__
__attribute__ ((format (printf, 5, 6)))
#endif
;
int CCTK_VParamWarn (const char *thorn,
                     const char *format,
                     ...)
#ifdef __GNUC__
__attribute__ ((format (printf, 2, 3)))
#endif
;
int CCTK_ParamWarn(const char *thorn, const char *message);
int CCTK_Info(const char *thorn, const char *message);
int CCTK_VInfo(const char *thorn, const char *format, ...)
#ifdef __GNUC__
__attribute__ ((format (printf, 2, 3)))
#endif
;

#ifdef __cplusplus 
}
#endif

/* suggested values for warning levels (courtesy of Steve, PR#1742) */
#define CCTK_WARN_ABORT    0	/* abort the Cactus run */
#define CCTK_WARN_ALERT    1	/* the results of this run will probably */
				/* be wrong, but this isn't quite certain, */
				/* so we're not going to abort the run */
#define CCTK_WARN_COMPLAIN 2	/* the user should know about this, but */
				/* the results of this run are probably ok */
#define CCTK_WARN_PICKY    3	/* this is for small problems that can */
				/* probably be ignored, but that careful */
				/* people may want to know about */
#define CCTK_WARN_DEBUG    4	/* these messages are probably useful */
				/* only for debugging purposes */

#endif
