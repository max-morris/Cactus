 /*@@
   @header    cctki_Expression.h
   @date      Tue Sep 19 22:02:45 2000
   @author    Tom Goodale
   @desc 
   Header for expression stuff.
   @enddesc
   @version $Header$
 @@*/

#ifndef __CCTKI_EXPRESSION_H__
#define __CCTKI_EXPRESSION_H__ 1

#ifdef __cplusplus
extern "C" 
{
#endif

char *CCTKi_ExpressionParse(const char *expression);

int CCTKi_ExpressionEvaluate(char *buffer, 
                             int (eval)(const char *, void *),
                             void *data);

#ifdef __cplusplus
}
#endif

#endif /* __CCTKI_EXPRESSION_H__ */
