 /*@@
   @file      MemAllocate.c
   @date      Tue Mar  7 11:58:03 2000
   @author    Gerd lanfermann
   @desc 
   
   @enddesc 
   @version $Header$
 @@*/

#include <stdio.h>
#include <stdlib.h>

#include "cctk_Config.h"
#include "cctk_Malloc.h"

static char *rcsid = "$Header$";

#define MEMDEBUG 

/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/

typedef struct
{
  unsigned long int size;
  int line;
  const char *file;
} iMemData;


/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/

static unsigned long int totmem=0;
static unsigned long int pastmem=0;

/********************************************************************
 *********************     External Routines   **********************
 ********************************************************************/

void *CCTKi_Malloc(size_t size, int line, const char *file)
{
  iMemData *memdata;
  char *data;
  
  data = (char*)malloc(size+sizeof(iMemData));
  if(!data) 
  {
    fprintf(stderr, "Allocation error! ");
  }
  memdata = (iMemData *)data;
  memdata->size = size;
  memdata->line = line;
  memdata->file = file;

  pastmem = totmem;
  totmem += size;

#ifdef MEMDEBUG
  printf("Allocating %lu - by %s in line %d TOTAL: %lu\n",
         memdata->size,memdata->file,memdata->line, CCTK_TotalMemory());
#endif

  return((void*)(data+sizeof(iMemData)));
}

void CCTKi_Free(void *pointer)
{
  iMemData *memdata;

  memdata = (iMemData *)((char*)pointer-sizeof(iMemData));
#ifdef MEMDEBUG
  printf("Freeing %lu - allocated by %s in line %d TOTAL: %lu\n",
         memdata->size,memdata->file,memdata->line, CCTK_TotalMemory());
#endif
  pastmem  = totmem;
  totmem  -= memdata->size;
  
  free(memdata);
}

void CCTK_MemStat(void) 
{
  char mess[138];
  sprintf(mess,"total: %d  past: %d  diff %d \n",
	  totmem, pastmem, totmem-pastmem);
  printf("CCTK_Memstat: %s ",mess);
}

unsigned long int CCTK_TotalMemory(void)
{
  return(totmem);
}
    
