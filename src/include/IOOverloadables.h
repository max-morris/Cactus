 /*@@
   @header    IOOverloadables.h
   @date      Thu Feb  4 09:59:34 1999
   @author    Tom Goodale
   @desc 
   The overloadable functions for the IO layer.
   See OverloadMacros.h to see how to use these.
   @enddesc 
   @version $Header$
 @@*/

#ifdef ARGUMENTS
#undef ARGUMENTS
#endif

#ifdef RETURN_TYPE
#undef RETURN_TYPE
#endif

#define RETURN_TYPE int
#define ARGUMENTS cGH *GH
OVERLOADABLE(OutputGH)

#undef ARGUMENTS
#define ARGUMENTS cGH *GH,            \
                  const char *var,    \
                  const char *method, \
                  const char *alias
OVERLOADABLE(OutputVarAsByMethod)

#undef ARGUMENTS
#undef RETURN_TYPE
