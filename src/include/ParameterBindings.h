 /*@@
   @file      ParameterBindings.h
   @date      Mon Feb 22 15:01:14 1999
   @author    Tom Goodale
   @desc 
   Defines for parameter stuff
   @enddesc 
 @@*/

#ifndef _PARAMETERBINDINGS_H_
#define _PARAMETERBINDINGS_H_


#ifdef __cplusplus
extern "C" {
#endif

int ParameterCreate(const char *name,        /* The parameter name */
		    const char *thorn,       /* The thorn          */ 
		    const char *type,       /* The parameter type */
                    const char *scope,       /* The scoping block  */
                    int steerable,           /* Is it steerable ?  */
                    const char *description, /* The description    */ 
                    const char *defval,      /* The default value  */ 
                    void *datapointer,       /* The actual data    */
                    int n_ranges,            /* How many allowed ranges it has */
		    ...);

int ParameterSet   (const char *name,        /* The name of the parameter  */
                    const char *thorn,       /* The originating thorn      */
                    const char *value);      /* The value of the parameter */

void *ParameterGet (const char *name,        /* The name of the parameter  */
                    const char *thorn,       /* The originating thorn      */
                    int *type);              /* Holds type of parameter    */

const char *ParameterWalk(
                    int first,               /* Get first parameter or not */
                    const char *origin);     /* The origin of this walk    */

#ifdef __cplusplus
}
#endif

#define PARAMETER_KEYWORD  1
#define PARAMETER_STRING   2
#define PARAMETER_SENTENCE 3
#define PARAMETER_INT      4
#define PARAMETER_INTEGER  4
#define PARAMETER_REAL     5
#define PARAMETER_BOOLEAN  6

#endif
