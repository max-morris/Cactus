 /*@@
   @header    cctk_IOMethods.h
   @date      
   @author    
   @desc 
   header file for handling IO methods
   @enddesc 
   @version $Id$
 @@*/

#ifndef _CCTK_IOMETHODS_H_
#define _CCTK_IOMETHODS_H_

struct IOMethod
{
  const char *implementation;
  int    (*OutputGH)(cGH *);
  int    (*OutputVarAs)(cGH *, const char *, const char *);
  int    (*TriggerOutput)(cGH *, int);
  int    (*TimeToOutput)(cGH *,int);
};

#ifdef __cplusplus
extern "C" {
#endif

#define CCTK_RegisterIOMethod(a) CCTKi_RegisterIOMethod(CCTK_THORNSTRING,a)
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
