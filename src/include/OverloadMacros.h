 /*@@
   @header    OverloadMacros.h
   @date      Thu Feb  4 08:02:29 1999
   @author    Tom Goodale
   @desc 
   Macros used for the overload functions
   @enddesc 
   @version $Header$
 @@*/

#ifndef _OVERLOADMACROS_H_
#define _OVERLOADMACROS_H_

/* These are a load of macros used to make overloadable functions. 
 *
 * Basically define ARGUMENTS with the arguments of the function,
 * and RETURN_TYPE as the return type
 * then put lines of the form OVERLOADABLE(function)
 * in a header file.
 * Defining OVERLOADABLE(name) as OVERLOADABLE_<macro>(name)
 * and then including the header will create functions, prototypes
 * dummy functions or some checking code as required.
 */

/* This macro defines a global variable with the name of the function
 * and a function which allows people to set its value.
 */
#define OVERLOADABLE_FUNCTION(name)                               \
RETURN_TYPE (*CCTK_##name)(ARGUMENTS) = NULL;                     \
int CCTK_Overload##name(RETURN_TYPE (*func)(ARGUMENTS))           \
{                                                                 \
  int return_code;                                                \
  if(! CCTK_##name)                                               \
  {                                                               \
     CCTK_##name = func;                                          \
     return_code = 1;                                             \
  }                                                               \
  else                                                            \
  {                                                               \
     char *message = malloc( (200+strlen(#name))*sizeof(char) );  \
     sprintf(message,                                             \
	     "Warning: Attempted to overload function %s twice\n",\
             #name);                                              \
     CCTK_Warn(1,__LINE__,__FILE__,"Cactus",message);             \
     free(message);                                               \
     return_code = 0;                                             \
  }                                                               \
                                                                  \
  return return_code;                                             \
}

/* This macro creates an extern declaration for an overloadable function */
#define OVERLOADABLE_PROTOTYPE(name)                              \
extern RETURN_TYPE (*CCTK_##name)(ARGUMENTS);

/* This macro defines a dummy function */
#define OVERLOADABLE_DUMMY(name)                                  \
RETURN_TYPE CCTK_Dummy##name(ARGUMENTS)                           \
{                                                                 \
  fprintf(stderr, "Dummy %s called.\n", #name);                   \
  return 0;                                                       \
}

/* This macro defines the prototype for a dummy function. */
#define OVERLOADABLE_DUMMYPROTOTYPE(name)                         \
RETURN_TYPE CCTK_Dummy##name(ARGUMENTS);

/* This macro defines a check line which will set the overloadable
 * function to be the dummy if it hasn't been set.
 */
#define OVERLOADABLE_CHECK(name)                                  \
  if(!CCTK_##name) CCTK_##name = CCTK_Dummy##name;


/* This macro defines the prototype for the overloading function itself */
#define OVERLOADABLE_OVERLOADPROTO(name)                          \
int CCTK_Overload##name(RETURN_TYPE (*func)(ARGUMENTS));


#endif
