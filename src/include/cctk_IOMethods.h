 /*@@
   @header    cctk_IOMethods.h
   @date      1999/07/30
   @author    Gabrielle Allen
   @desc
              header file for handling IO methods
   @enddesc
   @version   $Version: cctk_IOMethods.h,v 1.3 2001/10/23 15:40:08 allen Exp $
 @@*/

#ifndef _CCTK_IOMETHODS_H_
#define _CCTK_IOMETHODS_H_

#ifdef __cplusplus
extern "C" {
#endif

struct IOMethod
{
  const char *implementation;
  int    (*OutputGH)(cGH *);
  int    (*OutputVarAs)(cGH *, const char *, const char *);
  int    (*TriggerOutput)(cGH *, int);
  int    (*TimeToOutput)(cGH *,int);
};

#define CCTK_RegisterIOMethod(a) CCTKi_RegisterIOMethod(CCTK_THORNSTRING, a)
int CCTKi_RegisterIOMethod(const char *thorn, const char *name);

int CCTK_RegisterIOMethodOutputGH(int handle, int (*func)(cGH *));

int CCTK_RegisterIOMethodTimeToOutput(int handle, int (*func)(cGH *, int));

int CCTK_RegisterIOMethodTriggerOutput(int handle, int (*func)(cGH *, int));

int CCTK_RegisterIOMethodOutputVarAs(int handle, int (*func)(cGH *,
                                     const char *,const char *));

const char *CCTK_IOMethodImplementation(int handle);

int CCTK_NumIOMethods(void);

#ifdef __cplusplus
}
#endif

#endif
