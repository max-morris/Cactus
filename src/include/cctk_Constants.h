 /*@@
   @header    cctk_Constants.h
   @date      Fri Oct 15 21:29:23 CEST 1999
   @author    Gabrielle Allen
   @desc 
   Constants used by Cactus
   @enddesc
   @version $Header$
 @@*/

#ifndef _CCTK_CONSTANTS_H_
#define _CCTK_CONSTANTS_H_

#define CCTK_VARIABLE_VOID       0
#define CCTK_VARIABLE_CHAR       1
#define CCTK_VARIABLE_INT        2
#define CCTK_VARIABLE_INT2       3
#define CCTK_VARIABLE_INT4       4
#define CCTK_VARIABLE_INT8       5
#define CCTK_VARIABLE_REAL       6
#define CCTK_VARIABLE_REAL4      7
#define CCTK_VARIABLE_REAL8      8
#define CCTK_VARIABLE_REAL16     9
#define CCTK_VARIABLE_COMPLEX   10
#define CCTK_VARIABLE_COMPLEX8  11
#define CCTK_VARIABLE_COMPLEX16 12
#define CCTK_VARIABLE_COMPLEX32 13
#define CCTK_VARIABLE_STRING    14

/* stagger flags: yes/no, number of staggerings: 3 */
#define CCTK_NO_STAGGER    0
#define CCTK_STAGGER       0
#define CCTK_NSTAGGER      3

/* stagger code in one direction */
#define CCTK_STAG_M        0
#define CCTK_STAG_C        1
#define CCTK_STAG_P        2

/* stagger code of three directions */
#define CCTK_STAG_MMM 0
#define CCTK_STAG_CMM 1
#define CCTK_STAG_PMM 2

#define CCTK_STAG_MCM 3
#define CCTK_STAG_CCM 4
#define CCTK_STAG_PCM 5

#define CCTK_STAG_MPM 6
#define CCTK_STAG_CPM 7
#define CCTK_STAG_PPM 8

#define CCTK_STAG_MMC 9
#define CCTK_STAG_CMC 11
#define CCTK_STAG_PMC 12

#define CCTK_STAG_MCC 13
#define CCTK_STAG_CCC 14
#define CCTK_STAG_PCC 15

#define CCTK_STAG_MPC 16
#define CCTK_STAG_CPC 17
#define CCTK_STAG_PPC 18

#define CCTK_STAG_MMP 19
#define CCTK_STAG_CMP 20
#define CCTK_STAG_PMP 21

#define CCTK_STAG_MCP 22
#define CCTK_STAG_CCP 23
#define CCTK_STAG_PCP 24

#define CCTK_STAG_MPP 25
#define CCTK_STAG_CPP 26
#define CCTK_STAG_PPP 27


#endif /* _CCTK_CONSTANTS_ */

