 /*@@
   @header    flesh.h
   @date      Thu Sep 24 10:18:52 1998
   @author    Tom Goodale
   @desc 
   Header file for flesh functions.
   @enddesc 
 @@*/

/* $Id$ */

#ifndef _FLESH_H_
#define _FLESH_H_

/*  Typedefs */

typedef struct
{
  char *parameter_file_name;
} tFleshConfig;


typedef struct
{
  int dummy;
} cGF;


typedef struct
{
  int dummy;
} cGH;


/* Function prototypes */

#ifdef __cplusplus
extern "C" {
#endif

int InitialiseCactus(int *, char **, tFleshConfig *);

extern int (*Initialise)(tFleshConfig *);

extern int (*Evolve)(tFleshConfig *);

extern int (*Shutdown)(tFleshConfig *);

int ShutdownCactus(tFleshConfig *);

#ifdef __cplusplus
	   }
#endif

#endif
