struct IOMethod
{
  int    (*OutputGH)(cGH *);
  int    (*OutputVarAs)(cGH *, const char *, const char *);
  int    (*TriggerOutput)(cGH *, int);
  int    (*TimeToOutput)(cGH *,int);
};
