 /*@@
   @header    util_Expression.h
   @date      Tue Sep 19 22:02:45 2000
   @author    Tom Goodale
   @desc 
   Header for expression stuff.
   @enddesc
   @version $Header$
 @@*/

#ifndef __UTIL_EXPRESSION_H__
#define __UTIL_EXPRESSION_H__ 1

#ifdef __cplusplus
extern "C" 
{
#endif

  /* These data types are for internal use only. */

  /* Defined operators */
typedef enum {OP_NONE,
              OP_EQUALS,
              OP_LESS_THAN,
              OP_GREATER_THAN,
              OP_LEQUALS,
              OP_GEQUALS,
              OP_AND,
              OP_OR,
              OP_PLUS,
              OP_MINUS,
              OP_DIV,
              OP_TIMES,
              OP_POWER,
              OP_ACOS,
              OP_ASIN,
              OP_ATAN,
              OP_CEIL,
              OP_COS, 
              OP_COSH,
              OP_EXP,
              OP_FABS,
              OP_FLOOR,
              OP_LOG,
              OP_LOG10,
              OP_SIN,
              OP_SINH,
              OP_SQRT,
              OP_TAN,
              OP_TANH}
  uExpressionOpcode;

  /* What sort of expression types we have. */
typedef enum {val,unary,binary} uExpressionType;

  /* RPN object. */
typedef struct 
{
  uExpressionType type;

  union
  {
    uExpressionOpcode opcode;
    int varnum;
  } token;
} uExpressionToken;

  /* Parsed expression object. */
typedef struct
{
  int ntokens;
  uExpressionToken *tokens;
  int nvars;
  const char **vars;
} uExpressionInternals;

/* Beginning of externally useable objects. */

  /* Structure to hold values. */
typedef struct 
{
  enum {rval,ival} type;
  
  union 
  {
    double rval;
    int    ival;
  } value;
} uExpressionValue;

  /* Externally visible representation of the expression. */
typedef uExpressionInternals *uExpression;

uExpression Util_ExpressionParse(const char *expression);

int Util_ExpressionEvaluate(const uExpression buffer,
                            uExpressionValue *retval,
                            int (*eval)(int, const char * const *, uExpressionValue *, void *),
                            void *data);

void Util_ExpressionFree(uExpression buffer);

#ifdef __cplusplus
}
#endif

#endif /* __UTIL_EXPRESSION_H__ */
