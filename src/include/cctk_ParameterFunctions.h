 /*@@
   @file      cctk_ParameterFunctions.h
   @date      Wed Sep  8 10:46:19 MSZ 1999
   @author    Andre Merzky
   @desc 
   Public defines for parameter stuff
   @enddesc 
   @version $Header$
 @@*/

#ifndef _CCTK_PARAMETERFUNCTIONS_H
#define _CCTK_PARAMETERFUNCTIONS_H

/* this include file declares all parameter structs/defines which 
 * should be visible and useable for some thorn programmers.
 * At first of all this means (read) access to parameter properties.
 * Direct acces to parameter values (or its data pointers) are not 
 * allowed. The functions/declarations for this are in 
 * ParameterBindings.h, which in turn includes this file.
 */

/* thes SCOPE* defines are used as flags fo parameter scopes. */

#define SCOPE_GLOBAL        1 /* parameter is visible everywhere             */
#define SCOPE_RESTRICTED    2 /* parameter is visible for friend thorns only */
#define SCOPE_PRIVATE       3 /* parameter is visible for parent thorn only  */
#define SCOPE_NOT_GLOBAL    4 /* parameter is not visible everywhere         */
#define SCOPE_ANY           5 /* parameter scope is undefined/arbitrary      */

static char *cctk_parameter_scopes[] = {"GLOBAL", "RESTRICTED", "PRIVATE"};

/* parameter types  */

#define PARAMETER_KEYWORD   1 /* parameter is keyword  */
#define PARAMETER_STRING    2 /* parameter is string   */
#define PARAMETER_SENTENCE  3 /* parameter is sentence */
#define PARAMETER_INT       4 /* parameter is integer  */
#define PARAMETER_INTEGER   4 /* parameter is integer  */
#define PARAMETER_REAL      5 /* parameter is float    */
#define PARAMETER_BOOLEAN   6 /* parameter is bool     */

static const char *cctk_parameter_type_names[] = {"KEYWORD", 
                                                  "STRING", 
                                                  "SENTENCE", 
                                                  "INTEGER", 
                                                  "REAL", 
                                                  "BOOLEAN"};

/* what is a parameter range:
 * list of independent ranges, each with
 *  - orig string (range)
 *  - active flag
 *  - description
 */
typedef struct RANGE
{
    struct RANGE *last;
    struct RANGE *next;
    char         *range;
    char         *origin;
    int           active;
    char         *description;
} t_range;


/* what are parameter properties:
 * everything users may know about paras:
 * - name
 * - thorn it belongs to
 * - scope
 * - description
 * - default value
 * - type
 * - range (see above) 
 * - number of times it has been set
 * - steerable flag
 */
typedef struct PARAM_PROPS
{
  char    *name;
  char    *thorn;
  int      scope;
  
  char    *description;
  char    *defval;

  int      type;
  t_range *range;

  int      n_set;
  int      steerable;

} t_param_prop;

#ifdef __cplusplus
extern "C" {
#endif

/* set the value of a parameter */
int ParameterSet   (const char *name,        /* The name of the parameter  */
                    const char *thorn,       /* The originating thorn      */
                    const char *value);      /* The value of the parameter */

/* get the data pointer to and type of a parameter's value */
void *ParameterGet (const char *name,        /* The name of the parameter  */
                    const char *thorn,       /* The originating thorn      */
                    int *type);              /* Holds type of parameter    */

/* get the string representation of a parameter's value
   (string should be freed afterwards) */
char *ParameterValString (const char *name,  /* The name of the parameter  */
                    const char *thorn);      /* The originating thorn      */

/* walk through list of parameters */
const char *ParameterWalk(
                    int first,               /* Get first parameter or not */
                    const char *origin);     /* The origin of this walk    */

/* get list of parameter names for given thorn */
int CCTK_ParameterList (const char* thorn, char ***paramlist, int *n_param);

/* get parameter properties for gven parameter/thorn pair */
t_param_prop *CCTK_ParameterInfo (const char *name, 
                                  const char *thorn);

#ifdef __cplusplus
}
#endif

#endif /*  _CCTK_PARAMETERFUNCTIONS_H */
