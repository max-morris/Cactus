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
#include <string.h>

#include "cctk_WarnLevel.h"
#include "cctk_Malloc.h"
#include "cctk_Flesh.h"
#include "StoreHandledData.h"

static char *rcsid = "$Header$";

CCTK_FILEVERSION(util_Malloc_c)

int CCTK_Abort(void *GH);

/*$#define MEMDEBUG$*/ 

/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/
/* Datastructure to track the allocation per file */
typedef struct
{
  unsigned long int size;
  unsigned long int tsize;
  const char *file;
} t_memhash;


/* Database structure (d), is kept at the beginning of 
   the allocated memory chunk (m):   ddddmmmmmmm
   pointer points to beginning of m:     |_pointer
*/
typedef struct            
{
  unsigned long int ok;      /* magic number */
  unsigned long int size;    /* requested size */
  unsigned long int tsize;   /* requested size + database */
  unsigned int line;         /* allocated on line # */
  const char *file;          /* allocated by file # */
} t_mallocinfo;

/* used for integrity checking of the data */
#define OK_INTEGRITY 424242

/* Datastructure to track the allocation size at ticket
   request time */
typedef struct
{
  unsigned long int size;
} t_memticket;


/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/

static cHandledData *ticketDB = NULL;
static cHandledData *memfileDB = NULL;

static int n_tickets = 0;

static unsigned long int totmem=0;
static unsigned long int pastmem=0;

/********************************************************************
 *********************   internal prototypes     ********************
 ********************************************************************/
void *CCTKi_Malloc(size_t size, int line, const char *file);
void CCTKi_Free(void *pointer, int line, const char *file);

int CCTKi_UpdateMemByFile(int size, int line, const char *file);


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
  t_mallocinfo *info;
  char *data;
  int retval;

  data = (char*)malloc(size+sizeof(t_mallocinfo));
  if(!data) 
  {
    fprintf(stderr, "Allocation error! ");
  }
  info = (t_mallocinfo*) data;
  info->ok   = OK_INTEGRITY;
  info->size = size;
  info->tsize= size+sizeof(t_mallocinfo);
  info->line = line;
  info->file = file;

  pastmem = totmem;
  totmem += size;

  retval = CCTKi_UpdateMemByFile(info->size, line, file);

#ifdef MEMDEBUG
  printf("Allocating %lu - by %s in line %d TOTAL: %lu\n",
         info->size, info->file, info->line, CCTK_TotalMemory());
#endif

  /* return the pointer plus an OFFSET (t_mallocinfo) to hide the info*/
  return((void*)(data+sizeof(t_mallocinfo)));
}


/*@@
   @routine    CCTKi_Realloc
   @date       Wed Mar  8 12:46:06 2000
   @author     Gerd Lanfermann
   @desc 
      ReAllocates memory, updates the total memory variable (static)
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

void *CCTKi_Realloc(void *pointer, size_t size, int line, const char *file)
{
  t_mallocinfo *info;
  char *data;
  char mess[256];
 
  /* Realloc called with NULL equiv. to malloc */
  if (pointer==NULL)
    return(CCTKi_Malloc(size, line, file));

  /* Realloc called with size zero equiv. to free */
  if (size==0) {
    CCTKi_Free(pointer, line, file);
    return(NULL);
  }

  /* Realloc failure */
 
  /* get the info section */
  info = (t_mallocinfo *)((char*)pointer-sizeof(t_mallocinfo));

  /* make a sanity check: memory could have be allocated with standard malloc */
  if (info->ok!=OK_INTEGRITY)  
  {
    sprintf(mess,"Malloc database corrupted. \n     Reallocation called from %s, line %d. \n     Was this memory allocated with CCTK_MALLOC \n?!",file,line);
    CCTK_Warn(1,__LINE__,__FILE__,",routine: CCTKi_Realloc",mess);
    CCTK_Abort(NULL);
  }
  
  /* reallocate starting at info pointer */
  data = (char*)realloc(info, size+sizeof(t_mallocinfo));
  if (!data)
  {
     printf(mess,"Could not reallocate memory.  Reallocation called from %s, line %d. \n", file,line);
    CCTK_Warn(0,__LINE__,__FILE__,",routine: CCTKi_Realloc",mess);
  }

  /* get the info section again and update */
  info = (t_mallocinfo*) data;
  info->size = size;
  info->tsize= size+sizeof(t_mallocinfo);

  /* update some static variables */
  pastmem = totmem;
  totmem += size;
  CCTKi_UpdateMemByFile(info->size, line, file);
 
#ifdef MEMDEBUG
  printf("ReAllocating %lu - by %s in line %d TOTAL: %lu\n",
         info->size, info->file, info->line, CCTK_TotalMemory());  
#endif

  /* return the pointer starting at the datasection, behind the info section */
  return((void*)(data+sizeof(t_mallocinfo)));
}

/*@@
   @routine    CCTKi_cMalloc
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

void *CCTKi_Calloc(size_t nmemb, size_t size, int line, const char *file)
{
  t_mallocinfo *info;
  size_t nsize;
  char *data;
  int retval;

  /* instead of a cmalloc(nmem,size) , we do a malloc(nmem*size) */
  nsize = nmemb*size;
  data  = (char*)malloc(nsize+sizeof(t_mallocinfo));
  if(!data) 
  {
    fprintf(stderr, "Allocation error! ");
  }
  info = (t_mallocinfo*) data;
  info->size = OK_INTEGRITY;
  info->size = nsize;
  info->tsize= nsize+sizeof(t_mallocinfo);
  info->line = line;
  info->file = file;

  pastmem = totmem;
  totmem += nsize;

  retval = CCTKi_UpdateMemByFile(info->size, line, file);

#ifdef MEMDEBUG
  printf("Allocating %lu - by %s in line %d TOTAL: %lu\n",
         info->size, info->file, info->line, CCTK_TotalMemory());
#endif

  return((void*)(data+sizeof(t_mallocinfo)));
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

void CCTKi_Free(void *pointer, int line, const char *file)
{
  t_mallocinfo *info;
  char mess[256];

  info = (t_mallocinfo *)((char*)pointer-sizeof(t_mallocinfo));

  if (info->ok!=OK_INTEGRITY)  
  {    
    sprintf(mess,"Malloc database corrupted. \n     Reallocation called from %s, line %d.\n     Was this memory allocated with CCTK_[RE]MALLOC ?!",file,line);
    CCTK_Warn(1,__LINE__,__FILE__,",routine CCTKi_Free",mess);
    CCTK_Abort(NULL);
  }

#ifdef MEMDEBUG
  printf("Freeing %lu - allocated by %s in line %d TOTAL: %lu\n",
         info->size, info->file, info->line, CCTK_TotalMemory());  
#endif

  pastmem  = totmem;
  totmem  -= info->size;
  
  free(info);
} 

/*@@
   @routine    CCTKi_UpdateMemByFile
   @date       Wed Mar  8 12:46:06 2000
   @author     Gerd Lanfermann
   @desc 
      Keeps track of the allocated memory on a per file bases.
    @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTKi_UpdateMemByFile(int size, int line, const char *file)
{
  t_memhash *memfile;
  int handle, retval=-1;
  
  if ((handle=Util_GetHandle(memfileDB, file, (void**)&memfile)) > -1) 
  {
    memfile = (t_memhash*) Util_GetHandledData(memfileDB, handle);

    /* Memory entry under <file> exists */
    if (memfile)
    {
      memfile->size +=size;
      memfile->tsize+=size;
      retval = 0;
    }
    else {
      retval = -3;
    }
  }
  else
    {
      /* Memory entry under <file> has to be created */
      memfile = (t_memhash*) malloc( sizeof(t_memhash));
      if (memfile)
      {
	memfile->size += size;
	memfile->tsize+= size+sizeof(t_memhash);
	memfile->file  = file;
	retval = Util_NewHandle(&memfileDB, file, memfile);
      }
      else
      {
	retval = -1;
      }
    }
  return retval;
}

/*@@
   @routine    CCTK_MemTicketRequest
   @date       Sun Mar 12 17:22:08 2000
   @author     Gerd Lanfermann
   @desc 
     Request a ticket: save the current total memory to a database.
     Return an integer (ticket). Use the ticket to calculate the 
     difference in memory allocation between the two instances in 
     CCTK_MemTicketCash.

     This only tracks the real data memory, which is the same as in 
     undebug mode. It does not keep track of the internal allocations 
     done to provide the database, bc. this is not allocated either if 
     you compile undebugged.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_MemTicketRequest(void)
{
  int this_ticket;
  t_memticket *tmem;
  char tname[20];

  sprintf(tname, "ticket_%d",n_tickets++);

  if (Util_GetHandle(ticketDB, tname, (void**)&tmem)>-1)
  {
    this_ticket=-3;
  }
  else  {
    tmem =(t_memticket*) malloc(sizeof(t_memticket));

    if (tmem) 
    {
      tmem->size  = CCTK_TotalMemory();
      this_ticket = Util_NewHandle(&ticketDB, tname, tmem);
    }
    else {
      this_ticket = -2;
    } 
  }
  return(this_ticket);
}

/*@@
   @routine    CCTK_MemTicketCash
   @date       Sun Mar 12 17:22:08 2000
   @author     Gerd Lanfermann
   @desc 
     Cash in your ticket: return the memory difference between now and the 
     time the ticket was requested.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

long int CCTK_MemTicketCash(int this_ticket)
{
  long int tdiff;
  unsigned long int tsize;
  t_memticket *tmem;
  
  tmem = (t_memticket*) Util_GetHandledData(ticketDB, this_ticket);

  if (tmem)
  {
    tsize = tmem->size;
    tdiff = CCTK_TotalMemory() - tsize;
  }
  else 
  {
    printf("CCTK_MemTicketCash: Cannot find ticket %d \n", this_ticket);
    tdiff = 42;
  }
  return(tdiff);
}

/*@@
   @routine    CCTK_MemTicketDelete
   @date       Sun Mar 12 17:22:08 2000
   @author     Gerd Lanfermann
   @desc 
      Delete the memory ticket. The ticket-id will not be reused, since
      it's incremented with every ticket request, but the memory
      is freed.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/

int CCTK_MemTicketDelete(int this_ticket)
{
  int ret_val;
  t_memticket *tmem;

  tmem = (t_memticket*)Util_GetHandledData(ticketDB, this_ticket);

  if (tmem)
  {
    Util_DeleteHandle(ticketDB, this_ticket); 
    ret_val = 0;
  }
  else
  {  
    ret_val = -1;
  }
  return(ret_val);
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
  char mess[1024];
  sprintf(mess,"total: %ld  past: %ld  diff %+ld \n",
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






 /*@@
   @routine    testmalloc
   @date       Sun Mar 12 17:31:44 2000
   @author     Gerd Lanfermann
   @desc 
      test routine 
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
    
#ifdef TESTMALLOC

int main(int argc, char *argv[])
{
  int    *myint;
  double *mydouble;
  char   *mychar;
  
  int i,n,ticket[3];
   
  n = 10;
  
  printf("### Start Allocating ...\n");
  ticket[0]=CCTKi_MemTicketRequest();
  myint   = (int*)   CCTKi_Malloc(n*sizeof(int),42,"My int");
  printf("check Ticket1:  %d\n",CCTK_MemTicketCash(ticket[0]));

  ticket[1]=CCTKi_MemTicketRequest();
  mydouble= (double*)CCTKi_Malloc(n*sizeof(double), 42,"My double");
  printf("check Ticket1:  %d\n",CCTK_MemTicketCash(ticket[0]));
  printf("check Ticket2:  %d\n",CCTK_MemTicketCash(ticket[1]));

  ticket[2]=CCTKi_MemTicketRequest();
  mychar  = (char*)  CCTKi_Malloc(n*sizeof(char), 42, "My char");
  printf("check Ticket1:  %d\n",CCTK_MemTicketCash(ticket[0]));
  printf("check Ticket2:  %d\n",CCTK_MemTicketCash(ticket[1]));
  printf("check Ticket3:  %d\n",CCTK_MemTicketCash(ticket[2]));

  for (i=0;i<n;i++)
  {
    myint[i]   = i;
    mydouble[i]= i/3.1415;
    mychar[i]  = "i";
  }

  printf("Total Memory allocated: %d ", CCTK_TotalMemory);
  printf("### Start Freeing ...\n");
  CCTKi_Free(myint, 43, "My int");
  CCTKi_Free(mydouble, 43, "My double");
  CCTKi_Free(mychar, 43, "My char");

  printf("check Ticket1:  %d\n",CCTK_MemTicketCash(ticket[0]));
  printf("check Ticket2:  %d\n",CCTK_MemTicketCash(ticket[1]));
  printf("check Ticket3:  %d\n",CCTK_MemTicketCash(ticket[2]));

  printf("Total Memory allocated: %d ", CCTK_TotalMemory);
  
  CCTK_MemTicketDelete(ticket[0]);
  CCTK_MemTicketDelete(ticket[1]);
  CCTK_MemTicketDelete(ticket[2]);

  return(0);
}

#endif /* TESTMALLOC */
