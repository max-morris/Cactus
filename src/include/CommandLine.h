 /*@@
   @header    CommandLine.h
   @date      Wed Feb 17 00:53:55 1999
   @author    Tom Goodale
   @desc 
   Header for the warning functions.
   @enddesc 
 @@*/

#ifndef _COMMANDLINE_H_
#define _COMMANDLINE_H_

#ifdef __cplusplus 
extern "C" {
#endif

void CCTK_CommandLineTestThornCompiled(const char *optarg);
void CCTK_CommandLineDescribeAllParameters(void);
void CCTK_CommandLineDescribeParameter(const char *optarg);
void CCTK_CommandLineTestParameters(const char *optarg);
void CCTK_CommandLineWarningLevel(const char *optarg);
void CCTK_CommandLineErrorLevel(const char *optarg);
void CCTK_CommandLineRedirectStdout(void);
void CCTK_CommandLineListThorns(void);
void CCTK_CommandLineVersion(void);
void CCTK_CommandLineHelp(void);
void CCTK_CommandLineUsage(void);
void CCTK_CommandLineFinished(void);

#ifdef __cplusplus 
}
#endif

#endif
