 /*@@
   @header    StoreNamedData.h
   @date      Tue Sep  1 10:45:08 1998
   @author    Tom Goodale
   @desc 
   Contains structures and function prototypes necessary to use
   the StoreNamedData routines.
   @enddesc 

   @version $Id$
 @@*/

#ifndef _STORENAMEDDATA_H_

#define _STORENAMEDDATA_H_

/* Define a data type for the storage. */
typedef struct PNamedData
{
  struct PNamedData *last;
  struct PNamedData *next;

  char *name;

  void *data;
} pNamedData;

/* Function prototypes. */

/* Store the data. */
int StoreNamedData(pNamedData **list, const char *name, void *data);

/* Fet the data. */
void *GetNamedData(pNamedData *list, const char *name);

/* destroy the data. */
void DestroyNamedDataList(pNamedData *list);

#endif
