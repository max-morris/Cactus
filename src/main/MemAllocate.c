
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "cctk_Config.h"
#include "cctki_Cache.h"
#include "cctk_Parameters.h"


#define MEMDEBUG 

static int totmem=0;

typedef struct
{
  int size;
  int line;
  char *file;
} iMemData;

void *CCTKi_malloc(size_t size, int line, const char *file)
{
  iMemData *memdata;
  char *foo;
  
  foo = malloc(size+sizeof(iMemData));
  if (!foo) printf("Allocation error! ");
  memdata = foo;
  memdata->size = size;
  memdata->line = line;
  memdata->file = file;
  totmem +=size;
#ifdef MEMDEBUG
  printf("Allocating %d - by %s in line %d TOTAL: %d\n",
	 memdata->size,memdata->file,memdata->line, CCTKi_TotalMemory());
#endif

  return((void*)(foo+sizeof(iMemData)));
}

void CCTKi_free(void *foo)
{
  iMemData *memdata;
  memdata = ((char*)foo)-sizeof(iMemData);
#ifdef MEMDEBUG
  printf("Freeing %d - allocated by %s in line %d TOTAL: %d\n",
	 memdata->size,memdata->file,memdata->line, CCTKi_TotalMemory());
#endif
  totmem-=memdata->size;
  free(memdata);
}

int CCTK_TotalMemory()
{
  return(totmem);
}
    
