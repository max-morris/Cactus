 /*@@
   @header    flesh.h
   @date      Thu Sep 24 10:18:52 1998
   @author    Tom Goodale
   @desc 
   Header file for flesh functions.
   @enddesc 
 @@*/

/* $Id$ */

#include "CactusTimers.h"

#ifndef _FLESH_H_
#define _FLESH_H_

/*  Typedefs */

typedef struct
{
  int dummy;
  char *name;
} cGF;


typedef struct
{
  int dummy;
  unsigned long int iteration;

  char *output_prefix;
} cGH;


typedef struct
{
  char *parameter_file_name;

  /* Array of pointers to cactus grid hierarchies. */
  cGH **GH;
  unsigned int nGHs;

  cTimer *timer[3];
} tFleshConfig;


typedef struct
{
  char *name;
  char *implementation;
  int (*startup)();
  int (*rfr_init)(cGH *);
  int (*param_init)();
  int (*param_set)(const char *, const char *);
  int (*param_get)(const char *, void **);
  int (*private_group_setup)(cGH *);
  int (*protected_group_setup)(cGH *);
  int (*public_group_setup)(cGH *);
} t_thorndata;

/* Function prototypes */

#ifdef __cplusplus
extern "C" {
#endif

int InitialiseCactus(int *, char **, tFleshConfig *);

int CCTK_SetParameter(const char *parameter, const char *value);

extern int (*Initialise)(tFleshConfig *);

extern int (*Evolve)(tFleshConfig *);

extern int (*Shutdown)(tFleshConfig *);

int ShutdownCactus(tFleshConfig *);

#ifdef __cplusplus
	   }
#endif

#endif
