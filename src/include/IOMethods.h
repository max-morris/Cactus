struct IOMethod
{
  int    (*OutputGH)(cGH *);
  int    (*OutputVarAs)(cGH *, const char *, const char *);
  int    (*TimeToOutput)(cGH *,int);
};
