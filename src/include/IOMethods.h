struct IOMethod
{
  int    (*OutputGH)(cGH *);
  int    (*OutputVarAs)(cGH *, const char *, const char *);
  int    (*TriggerOutput)(cGH *, int);
  int    (*TimeToOutput)(cGH *,int);
};

#ifdef __cplusplus
extern "C" {
#endif

int CCTK_RegisterIOMethod(const char *name);
int CCTK_RegisterIOMethodOutputGH(int handle, int (*func)(cGH *));
int CCTK_RegisterIOMethodTimeToOutput(int handle, int (*func)(cGH *, int));
int CCTK_RegisterIOMethodTriggerOutput(int handle, int (*func)(cGH *, int));
int CCTK_RegisterIOMethodOutputVarAs(int handle, int (*func)(cGH *,
                                     const char *,const char *));
#ifdef __cplusplus
}
#endif

