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

void CCTKi_CommandLineTestThornCompiled(const char *optarg);
void CCTKi_CommandLineDescribeAllParameters(const char *optarg);
void CCTKi_CommandLineDescribeParameter(const char *optarg);
void CCTKi_CommandLineTestParameters(const char *optarg);
void CCTKi_CommandLineWarningLevel(const char *optarg);
void CCTKi_CommandLineErrorLevel(const char *optarg);
void CCTKi_CommandLineRedirectStdout(void);
void CCTKi_CommandLineListThorns(void);
void CCTKi_CommandLineVersion(void);
void CCTKi_CommandLineHelp(void);
void CCTKi_CommandLineUsage(void);
void CCTKi_CommandLineFinished(void);

#ifdef __cplusplus 
}
#endif

#endif
