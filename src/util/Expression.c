 /*@@
   @file      Expression.c
   @date      Tue Sep 19 13:25:39 2000
   @author    Tom Goodale
   @desc 
   Expression evaluator.
   @enddesc
   @version $Header$
 @@*/

#ifndef TEST_CCTKI_EVALUATE
#include "cctk.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cctki_Expression.h"

static const char *rcsid = "$Header$";

CCTK_FILEVERSION(util_Expression_c)

/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/

typedef struct PToken
{
  struct PToken *last;
  struct PToken *next;
  char *token;
} pToken;

/********************************************************************
 ********************* Local Routine Prototypes *********************
 ********************************************************************/

static pToken *Tokenise(const char *expression);
static char *RPParse(pToken **current, char *stack, int *stacklength);
static int Evaluate(int val1, const char *operator, int val2);

static int isoperator(const char *token);
static int cmpprecendence(const char *op1, const char *op2);

static pToken *newtoken(const char *tokenstart, const char *tokenend);

#if 0
static void insertbefore(pToken *base, pToken *this);
#endif

#ifdef TEST_CCTKI_EVALUATE
static void printtokens(pToken *start);
#endif

static void insertafter(pToken *base, pToken *this);
static void FreeTokens(pToken *list);

/********************************************************************
 ********************* Other Routine Prototypes *********************
 ********************************************************************/

/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/

#define INITIAL_BUFFER_LENGTH 1000
#define MAX_STACK_SIZE 256
#define MAX_OPS 100

/********************************************************************
 *********************     External Routines   **********************
 ********************************************************************/

 /*@@
   @routine    CCTKi_ExpressionParse
   @date       Tue Sep 19 21:23:08 2000
   @author     Tom Goodale
   @desc 
   Parses an expression returning a predigested string in RPN.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
char *CCTKi_ExpressionParse(const char *expression)
{
  pToken *list;
  pToken *temp;

  char *buffer;
  int buffer_length;

  buffer_length = INITIAL_BUFFER_LENGTH;

  buffer = (char *)malloc(buffer_length);

  if(buffer)
  {
    /* FIXME: Don't really need a two pass algorithm here -
     *        it can all be done in one step.
     */

    /* Split the list into tokens */
    list = Tokenise(expression);

#ifdef TEST_CCTKI_EVALUATE    
    printtokens(list);
#endif

    temp = list;

    /* Convert the list into a string in RPN order */
    buffer = RPParse(&temp, buffer, &buffer_length);

    FreeTokens(list);
  }

  return buffer;
}

 /*@@
   @routine    CCTKi_ExpressionEvaluate
   @date       Tue Sep 19 21:23:40 2000
   @author     Tom Goodale
   @desc 
   Evaluates an parsed expression string created by CCTKI_ExpressionParse.
   The user passes in a function which is used to evaluate all operands.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int CCTKi_ExpressionEvaluate(char *buffer, 
                             int (*eval)(const char *, void *),
                             void *data)
{
  char *first;
  char *token;
  int stack[MAX_STACK_SIZE];
  int stackpointer;


  first = buffer;
  stackpointer = 0;

  /* Tokens are seperated by @ signs */
  while((token = strtok(first,"@")))
  {
    first = NULL;

    if(!isoperator(token))
    {
      /* Evaluate and put on stack */
      stack[stackpointer] = eval(token, data);
      stackpointer++;
    }
    else
    {
#ifdef TEST_CCTKI_EVALUATE
      printf("Stackpointer is %d, %f %s %f = ", 
             stackpointer, stack[stackpointer-2], token, stack[stackpointer-1]);
#endif
      /* Evaluate operation, clear operands from stack and add the result to the stack. */
      stack[stackpointer-2] = Evaluate(stack[stackpointer-2],token,stack[stackpointer-1]);
      stackpointer--;
#ifdef TEST_CCTKI_EVALUATE
      printf("%f\n", stack[stackpointer-1]); 
#endif
    }
  }
  
  return stack[stackpointer-1];
}


/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/

 /*@@
   @routine    Tokenise
   @date       Tue Sep 19 21:25:18 2000
   @author     Tom Goodale
   @desc 
   Split an expression into tokens
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static pToken *Tokenise(const char *expression)
{
  pToken *start;
  pToken *current;
  pToken *new;

  const char *tokenstart;
  const char *tokenend;
  const char *position;

  start   = NULL;
  current = NULL;

  tokenstart = expression;

  while(*tokenstart)
  {
    /* Remove leading whitespace */ 
    for(; *tokenstart == ' ' || *tokenstart == '\t'; tokenstart++);

    tokenend = NULL;

    position = tokenstart;

    for(position=tokenstart; *position && *(position+1); position++)
    {
      switch(*(position+1))
      {
        case '+' :
        case '-' :
        case '/' :
        case '*' :
        case '(' :
        case ')' :
        case '<' :
        case '>' :
          tokenend = position; break;
        case '=' :
          if(*position != '<' && *position != '>')
          {
            tokenend = position; 
          }
          break;
        case '&' :
          if(*position != '&')
          {
            tokenend = position;
          }
          break;
        case '|' :
          if(*position != '|')
          {
            tokenend = position;
          }
          break;
        default  :
          switch(*(position))
          {
            case '+' :
            case '-' :
            case '/' :
            case '*' :
            case '(' :
            case ')' :
            case '=' :
            case '&' :
            case '|' :
              tokenend = position; break;
            case '<' :
            case '>' :
              if(*(position+1) && *(position+1) != '=')
              {
                tokenend = position;
              }
              break;
            default  :
              ;
          }
      }

      if(tokenend)
      {
        break;
      }
    }

    /* Have we reached the end of the string ? */
    if(!tokenend)
    {
      tokenend = position;
    }
    
    /* Create a new token */
    new = newtoken(tokenstart, tokenend);

    /* Insert on list */
    if(current)
    {
      insertafter(current, new);
    }
    current = new;

    if(!start)
    {
      start = current;
    }

    if(*tokenend)
    {
      tokenstart = tokenend+1;
    }
    else
    {
      break;
    }
  }
    
  return start;
}

 /*@@
   @routine    RPParse
   @date       Tue Sep 19 21:28:36 2000
   @author     Tom Goodale
   @desc 
   Parses an toke list into Reverse Polish Notation (RPN).
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
#define PUSH(stack, stacklength, value) do                        \
{                                                                 \
  int len = strlen(stack)+strlen(value)+3;                        \
                                                                  \
  if(len > stacklength)                                           \
  {                                                               \
    stack = (char *)realloc(stack, len);                          \
  }                                                               \
  sprintf(stack,"%s@%s",stack,value);                             \
} while(0)


static char *RPParse(pToken **current, char *stack, int *stacklength)
{
  char *retval;
  pToken *this;
  char *operator;
  char *opstack[MAX_OPS];
  int numops;
  int precedence;

  numops = 0;
  this = *current;

  retval = stack;

  operator = NULL;

  if(*stacklength > 0)
  {
    stack[0] = 0;
  }
  else
  {
    stack = (char *)malloc(sizeof(char));
    *stacklength = 1;
  }

  for(this = *current; this && strcmp(this->token,")"); this = this->next)
  {
    if(!strcmp(this->token, "(") && this->next)
    {
      /* This is a sub-group, so parse recursively */
      this = this->next;
      retval = RPParse(&this, retval, stacklength);
      if(! this)
      {
        break;
      }
    }
    else if(!isoperator(this->token))
    {
      PUSH(retval, *stacklength, this->token);
    }
    else
    {
      /* It's an operator */
      if(operator)
      {
        /* We already have an operator */
        precedence = cmpprecendence(operator, this->token);

        if(precedence > 0)
        {
          /* Higher precedence than previous one so store previous one */
          numops++;
          opstack[numops-1] = operator;
          operator = this->token;
        }
        else
        {
          /* Lower or equal precedence */
          PUSH(retval, *stacklength, operator);
          operator = this->token;
          while(numops > 0)
          {
            if(cmpprecendence(opstack[numops-1], operator) <=0)
            {
              numops--;
              PUSH(retval, *stacklength, opstack[numops]);
            }
            else
            {
              break;
            }
          }
        }
      }
      else
      {
        operator = this->token;
      }
    }
  }

  if(operator)
  {
    PUSH(retval, *stacklength, operator);
    while(numops > 0)
    {
      numops--;
      PUSH(retval, *stacklength, opstack[numops]);
    }
  }

  *current=this;

  return retval;
}


 /*@@
   @routine    Evaluate
   @date       Tue Sep 19 21:35:34 2000
   @author     Tom Goodale
   @desc 
   Evaluates val1 op val2 
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int Evaluate(int val1, const char *operator, int val2)
{
  int retval;

  if(!strcmp(operator, "+"))
  {
    retval = (val1+val2);
  }
  else if(!strcmp(operator, "-"))
  {
    retval = (val1-val2);
  }
  else if(!strcmp(operator, "/"))
  {
    retval = (val1/val2);
  }
  else if(!strcmp(operator, "*"))
  {
    retval = (val1*val2);
  }
  else if(!strcmp(operator, "&&"))
  {
    retval = (val1 && val2);
  }
  else if(!strcmp(operator, "||"))
  {
    retval = (val1 || val2);
  }
  else if(!strcmp(operator, "="))
  {
    retval = ( val1 == val2);
  }
  else if(!strcmp(operator, "<"))
  {
    retval = (val1 < val2);
  }
  else if(!strcmp(operator, "<="))
  {
    retval = (val1 <= val2);
  }
  else if(!strcmp(operator, ">"))
  {
    retval = (val1 > val2);
  }
  else if(!strcmp(operator, ">="))
  {
    retval = (val1 >= val2);
  }
  else
  {
    fprintf(stderr, "Unknown operation %s", operator);
    retval = 0;
  }

  return retval;
}
  
 /*@@
   @routine    FreeTokens
   @date       Tue Sep 19 21:39:07 2000
   @author     Tom Goodale
   @desc 
   Frees a list of tokens.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void FreeTokens(pToken *list)
{
  pToken *token;
  pToken *next;

  for(token = list; token; token = next)
  {
    next = token->next;
    free(token->token);
    free(token);
  }
}  


 /*@@
   @routine    isoperator
   @date       Tue Sep 19 21:30:20 2000
   @author     Tom Goodale
   @desc 
   Tests if a string is an operator or not.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int isoperator(const char *token)
{
  int retval;
  
  if(!strcmp(token, "+") ||
     !strcmp(token, "-") ||
     !strcmp(token, "/") ||
     !strcmp(token, "*") ||
     !strcmp(token, "&&") ||
     !strcmp(token, "||") ||
     !strcmp(token, "=") ||
     !strcmp(token, "<") ||
     !strcmp(token, "<=") ||
     !strcmp(token, ">") ||
     !strcmp(token, ">="))
  {
    retval = 1;
  }
  else
  {
    retval = 0;
  }

  return retval;
}

 /*@@
   @routine    cmpprecendence
   @date       Wed Sep 20 09:05:59 2000
   @author     Tom Goodale
   @desc 
   Compare the precedence of two operators.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int cmpprecendence(const char *op1, const char *op2)
{
  int retval;
  const char *op;
  int op1prec;
  int op2prec;
  int *prec;

  /* Work out the precedence level for each operator */
  for(op = op1, prec = &op1prec; 
      op ; 
      op = (op == op1 ) ? op2 : NULL, 
      prec = (op == op1) ? &op1prec : &op2prec)
  {
    if(!strcmp(op, "=") ||
       !strcmp(op, "<") ||
       !strcmp(op, "<=") ||
       !strcmp(op, ">") ||
       !strcmp(op, ">="))
    {
      *prec = 1;
    }
    else if(!strcmp(op, "&&") ||
       !strcmp(op, "||"))
    {
      *prec = 2;
    }
    else if(!strcmp(op, "+") ||
            !strcmp(op, "-"))
    {
      *prec = 3;
    }
    else if(!strcmp(op, "/") ||
            !strcmp(op, "*"))
    {
      *prec = 4;
    }
    else
    {
      fprintf(stderr, "Unknown operator '%s'\n", op);
      *prec = 0;
    }
  }

  /* Now see which has the higher precedence */
  retval = op2prec-op1prec;

  return retval;
}
 
 /*@@
   @routine    newtoken
   @date       Tue Sep 19 21:30:42 2000
   @author     Tom Goodale
   @desc 
   Creates a new token item.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static pToken *newtoken(const char *tokenstart, const char *tokenend)
{
  pToken *this;
  const char *position;
  char *newpos;

  this = (pToken *)malloc(sizeof(pToken));

  if(this)
  {
    this->last = NULL;
    this->next = NULL;

    this->token = (char *)malloc(tokenend-tokenstart+2);
    if(this->token)
    {
      for(position=tokenstart, newpos=this->token; 
          position <= tokenend; 
          position++, newpos++)
      {
        *newpos = *position;
      }
      /* Just in case not already null terminated */
      *newpos = 0;

      /* Strip off any trailing spaces */
      for(; newpos >= this->token && 
            (*newpos == 0 || *newpos == ' ' || *newpos == '\t'); newpos--)
      {
        *newpos = 0;
      }
    }
  }

  return this;
}

 /*@@
   @routine    insertbefore
   @date       Tue Sep 19 21:33:39 2000
   @author     Tom Goodale
   @desc 
   Inserts a token before another one in a list.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
#if 0
static void insertbefore(pToken *base, pToken *this)
{
  if(base && this)
  {
    this->next = base;
    this->last = base->last;
    base->last = this;

    if(this->last)
    {
      this->last->next = this;
    }
  }
}
#endif
    
 /*@@
   @routine    insertafter
   @date       Tue Sep 19 21:34:02 2000
   @author     Tom Goodale
   @desc 
   Inserts a token after another one in a list.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static void insertafter(pToken *base, pToken *this)
{
  if(base && this)
  {
    this->last = base;
    this->next = base->next;
    base->next = this;
    
    if(this->next)
    {
      this->next->last = this;
    }
  }
}
  
 /*@@
   @routine    printtokens
   @date       Tue Sep 19 21:34:24 2000
   @author     Tom Goodale
   @desc 
   Debugging function to print out the tokens.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
#if TEST_CCTKI_EVALUATE
static void printtokens(pToken *start)
{
  pToken *token;

  for(token = start; token; token = token->next)
  {
    printf("->%s", token->token);
  }

  printf("\n");
}
#endif /* TEST_CCTKI_EVALUATE */

 /*@@
   @routine    printstack
   @date       Tue Sep 19 21:34:48 2000
   @author     Tom Goodale
   @desc 
   Debugging function to print out a predigested string.
   Note that is modifies the string, so it should be passed
   a copy.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
#if 0
static void printstack(char *stack)
{
  char *first;
  char *token;

  first = stack;

  while((token = strtok(first,"@")))
  {
    first = NULL;
    printf("Token is %s\n", token);
  }
}
#endif


/********************************************************************
 *********************     TEST FUNCTIONS   *************************
 ********************************************************************/

#ifdef TEST_CCTKI_EVALUATE

int evaluator(const char *token, void *data)
{
  int retval;

  retval = strtod(token, NULL);

  /*  fprintf(stderr, "Evaluated '%s' to %f\n", token,retval); */

  return retval;
}


int main(int argc, char *argv[])
{
  char *buffer;

  if(argc < 2)
  {
    printf("Usage: %s \"string\"\n", argv[0]);
    exit(0);
  }

  buffer = CCTKi_ExpressionParse(argv[1]);

  printf("Value is %f\n", CCTKi_ExpressionEvaluate(buffer, evaluator,NULL)); 
  
  free(buffer);

  return 0;
}
  
#endif /* TEST_CCTKI_EVALUATE */    
