 /*@@
   @header    StoreHandledData.h
   @date      Tue Feb  2 10:56:31 1999
   @author    Tom Goodale
   @desc 
   Header file for handled data routines
   @enddesc
   @version $Id$
 @@*/

#ifndef _STOREHANDLEDDATA_H_

#define _STOREHANDLEDDATA_H_

/* Define a data types for the storage. */
typedef struct 
{
  unsigned int in_use;
  char *name;
  void *data;
} cHandleStorage;

typedef struct
{
  cHandleStorage *array;
  unsigned array_size;
  unsigned first_unused;
} cHandledData;

/* Function prototypes. */

#ifdef _cplusplus 
extern "C" 
{
#endif

int CCTK_NewHandle(cHandledData **storage, const char *name, void *data);
int CCTK_DeleteHandle(cHandledData *storage, int handle);
void *CCTK_GetHandledData(cHandledData *storage, int handle);
int CCTK_GetHandle(cHandledData *storage, const char *name, void **data);

#ifdef _cplusplus 
}
#endif

#endif
