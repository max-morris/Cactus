 /*@@
   @header    flesh.h
   @date      Thu Sep 24 10:18:52 1998
   @author    Tom Goodale
   @desc 
   Header file for flesh functions.
   @enddesc 
 @@*/

#ifndef _FLESH_H_
#define _FLESH_H_

/*  Typedefs */

typedef struct
{
  char *parameter_file_name;
} tFleshConfig;

#ifdef __cplusplus
extern "C" {
#endif

int InitialiseCactus(int *, char **, tFleshConfig *);

int Initialise(tFleshConfig *);

int Evolve(tFleshConfig *);

int Shutdown(tFleshConfig *);

int ShutdownCactus(tFleshConfig *);

#ifdef __cplusplus
	   }
#endif

#endif
