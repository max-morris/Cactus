 /*@@
   @file      MemAllocate.c
   @date      Tue Mar  7 11:58:03 2000
   @author    Gerd lanfermann
   @desc 
      Cactus memory allocation routines to monitor memory consumption
      by C routines. No fortran support.
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

/*@@
   @routine    CCTKi_Malloc
   @date       Wed Mar  8 12:46:06 2000
   @author     Gerd Lanfermann
   @desc 
      Allocates memory, updates the total memory variable (static)
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

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

/*@@
   @routine    CCTKi_Free
   @date       Wed Mar  8 12:46:55 2000
   @author     Gerd Lanfermann
   @desc 
     Frees  memory, updates the total memory variable (static)
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

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

 /*@@
   @routine    CCTK_MemStat
   @date       Wed Mar  8 12:47:23 2000
   @author     Gerd Lanfermann
   @desc 
     prints a info string, statign current, past total memory
     and difference.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

void CCTK_MemStat(void) 
{
  char mess[138];
  sprintf(mess,"total: %d  past: %d  diff %d \n",
	  totmem, pastmem, totmem-pastmem);
  printf("CCTK_Memstat: %s ",mess);
}

 /*@@
   @routine    CCTK_MemStat
   @date       Wed Mar  8 12:47:23 2000
   @author     Gerd Lanfermann
   @desc 
     returns total memory allocated by C routines 
     (which use CCTK_MALLOC)
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
unsigned long int CCTK_TotalMemory(void)
{
  return(totmem);
}
    
