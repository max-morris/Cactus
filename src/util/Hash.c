 /*@@
   @file      Hash.c
   @date      Wed Oct 27 20:27:36 1999
   @author    Tom Goodale
   @desc 
   Generic hash stuff.
   @enddesc 
 @@*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Hash.h"

static char *rcsid = "$Header$";


/* Local routine prototypes */
static t_hash_entry *HashFind(t_hash *hash, 
                              unsigned int klen, 
                              char *key, 
                              unsigned int hashval);
static int HashRehash(t_hash *hash);

/********************************************************************
 ********************    External Routines   ************************
 ********************************************************************/


 /*@@
   @routine    HashCreate
   @date       Wed Oct 27 23:31:11 1999
   @author     Tom Goodale
   @desc 
   Creates a new hash table. 
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
t_hash *HashCreate(unsigned int initial_size)
{
  t_hash *retval;

  retval = (t_hash *)malloc(sizeof(t_hash));

  if(retval)
  {
    retval->size = 2;

    /* Set the initial size to be the first power of two greater than
     * the specified initial size. 
     */
    while(retval->size < initial_size) retval->size *=2;

    retval->fill = 0;
    retval->keys = 0;

    retval->array = (t_hash_entry **)calloc(sizeof(t_hash_entry *), retval->size);

    if(! retval->array)
    {
      free(retval);

      retval = NULL;
    }

  }

  return retval;
}

 /*@@
   @routine    HashDestroy
   @date       Wed Oct 27 23:32:12 1999
   @author     Tom Goodale
   @desc 
   Destroys a hash table.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void HashDestroy(t_hash *hash)
{
  unsigned int size;

  t_hash_entry **array;
  t_hash_entry *entry;

  unsigned int location;

  size  = hash->size;
  array = hash->array;
  
  for(location = 0; location < size; location++)
  {
    for(entry = array[location]; entry; entry = entry->next)
    {
      free(entry->key);
      free(entry);
    }
  }
  free(hash->array);
}

 /*@@
   @routine    HashStore
   @date       Wed Oct 27 23:44:14 1999
   @author     Tom Goodale
   @desc 
   Stores a value in the hash table.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int HashStore(t_hash *hash, 
              unsigned int klen, 
              char *key, 
              unsigned int hashval,
              void *data)
{
  int retval;
  t_hash_entry *entry;

  entry = HashFind(hash, klen, key, hashval);

  if(entry)
  {
    entry->data = data;
    retval = 0;
  }
  else
  {
    retval = HashAdd(hash, klen, key, hashval, data);
  }

  return retval;
}

 /*@@
   @routine    HashAdd
   @date       Wed Oct 27 23:32:40 1999
   @author     Tom Goodale
   @desc 
   Adds a new entry to the hash table.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int HashAdd(t_hash *hash, 
            unsigned int klen, 
            char *key, 
            unsigned int hashval, 
            void *data)
{
  int retval;
  t_hash_entry *entry;
  t_hash_entry *lastentry;
  unsigned int location;
  int duplicate;
  int i;

  /* Calculate the hash value if necessary */
  if(!hashval)
  {
    hashval = HashHash(klen, key);
  }

  /* Get its location in the table */
  location = hashval % hash->size;

  /* See what's there */
  entry = hash->array[location];

  duplicate = 0;

  /* Find the last entry on the list, checking for duplicates 
   * on the way 
   */
  lastentry = NULL;
  while(entry)
  {
    lastentry = entry;
    if(hashval == entry->hash)
    {
      duplicate = 1;
      break;
    }
    else
    {
      entry = entry->next;
    }
  }
  
  if(!duplicate)
  {
    /* Create a new entry */
    entry = (t_hash_entry *)malloc(sizeof(t_hash_entry));

    if(entry)
    {
      entry->hash = hashval;
      entry->klen = klen;
      entry->key  = (char *)malloc(sizeof(char)*klen);
      entry->data = data;
      entry->next = NULL;

      if(entry->key)
      {
        for(i=0; i < klen; i++)
        {
          entry->key[i] = key[i];
        }

        /* Link it in */
        if(lastentry)
        {
          lastentry->next = entry;
        }
        else
        {
          hash->array[location] = entry;
          hash->fill++;
        }

        entry->last = lastentry;
        hash->keys++;
      }
      else
      {
        free(entry);
        entry = NULL;
      }
    }
  }

  if(duplicate)
  {
    retval = -1;
  }
  else if(!entry)
  {
    retval = -2;
  }
  else
  {
    retval = 0;
  }

  HashRehash(hash);

  return retval;
}

 /*@@
   @routine    HashDelete
   @date       Wed Oct 27 23:33:42 1999
   @author     Tom Goodale
   @desc 
   Deletes an entry from a hash table.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
int HashDelete(t_hash *hash, 
               unsigned int klen, 
               char *key, 
               unsigned int hashval)
{
  t_hash_entry *entry;
  unsigned int location;

  /* Calculate the hash value if necessary */
  if(!hashval)
  {
    hashval = HashHash(klen, key);
  }

  /* Get its location in the table */
  location = hashval % hash->size;

  /* See what's there */
  entry = hash->array[location];

  /* Find the entry on the list.*/
  for(entry = hash->array[location]; entry; entry = entry->next)
  {
    if(hashval == entry->hash)
    {
      break;
    }
  }

  if(entry)
  {
    /* Maintain the linked list */
    if(entry->next && entry->last)
    {
      entry->next->last = entry->last;
      entry->last->next = entry->next;
    }
    else if(entry->last)
    {
      entry->last->next = NULL;
    }
    else if(entry->next)
    {
      entry->next->last = NULL;
      hash->array[location] = entry->next;
    }
    else
    {
      hash->array[location] = NULL;
      hash->fill--;
    }

    free(entry->key);
    free(entry);
    
    hash->keys--;
  }

  return (entry != NULL);

}

 /*@@
   @routine    HashGet
   @date       Wed Oct 27 23:35:17 1999
   @author     Tom Goodale
   @desc 
   Gets the data associated with a hash table entry.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
void *HashGet(t_hash *hash, 
              unsigned int klen, 
              char *key, 
              unsigned int hashval)
{
  void *retval;
  t_hash_entry *entry;

  entry = HashFind(hash, klen, key, hashval);

  if(entry)
  {
    retval = entry->data;
  }
  else
  {
    retval = NULL;
  }

  return retval;
}



 /*@@
   @routine    HashHash
   @date       Wed Oct 27 22:15:17 1999
   @author     Tom Goodale
   @desc 
   Hashing function.  I took this from the book 
      'Advanced Perl Programming' 
   published by O'Reilly, and it is apparently the
   algorithm used in Perl, and that is GPL.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
unsigned int HashHash(unsigned int klen, 
                      char *key)
{
  unsigned int hash;
  int i;
  char *pos;

  i   = klen;  
  pos = key;

  hash = 0;

  while(i--)
  {
    hash = hash*33 + *pos++;
  }

#ifdef DEBUG_HASH
  printf("key '%s', length %d, has hash %u\n", key, klen, hash);
#endif

  return hash;
}


/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/

 /*@@
   @routine    HashFind
   @date       Wed Oct 27 23:34:09 1999
   @author     Tom Goodale
   @desc 
   Finds an entry in a hash table.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static t_hash_entry *HashFind(t_hash *hash, 
                              unsigned int klen, 
                              char *key, 
                              unsigned int hashval)
{
  t_hash_entry *entry;
  unsigned int location;

  /* Calculate the hash value if necessary */
  if(!hashval)
  {
    hashval = HashHash(klen, key);
  }

  /* Get its location in the table */
  location = hashval % hash->size;

  /* See what's there */
  entry = hash->array[location];

  /* Find the entry on the list.*/
  for(entry = hash->array[location]; entry; entry = entry->next)
  {
    if(hashval == entry->hash)
    {
      break;
    }
  }

  return entry;
}


 /*@@
   @routine    HashRehash
   @date       Wed Oct 27 23:34:32 1999
   @author     Tom Goodale
   @desc 
   Resizes a hash table.
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

@@*/
static int HashRehash(t_hash *hash)
{
  int retval;

  unsigned int old_size;
  unsigned int new_size;

  unsigned int old_location;
  unsigned int location;

  unsigned int new_fill;

  t_hash_entry **oldarray;
  t_hash_entry **newarray;

  t_hash_entry *entry;
  t_hash_entry *next;
  t_hash_entry *entry2;

  if(hash->keys > hash->fill)
  {
    old_size = hash->size;
    new_size = hash->size*2;
    newarray = (t_hash_entry **)calloc(sizeof(t_hash_entry *), new_size);
    oldarray = hash->array;
    new_fill = 0;

#ifdef DEBUG_HASH
    printf("Resizing hash from %u to %u - fill is %u\n", old_size, new_size, hash->fill);
#endif


    if(newarray)
    {
      for(old_location = 0; old_location < old_size; old_location++)
      {
        /* Move these entries to new array */
        for(entry = oldarray[old_location]; entry; entry = next)
        {
          next = entry->next;

          location = entry->hash % new_size;

          if(!newarray[location])
          {
            new_fill++;
            newarray[location] = entry;
            entry->last = NULL;
            entry->next = NULL;
          }
          else
          {
            /* Find the end of this list */
            for(entry2 = newarray[location]; entry2->next; entry2 = entry2->next);

            /* Add the entry at the end. */
            entry2->next = entry;
            entry->last  = entry2;
            entry->next  = NULL;
          }
        }
      }
      
      /* Free the old data and update the hash. */
      free(hash->array);

      hash->array = newarray;
      hash->fill  = new_fill;
      hash->size  = new_size;
      retval = 0;
    }
    else
    {
      retval = -1;
    }
  }
  else
  {
    retval = 0;
  }

  return retval;
}


/********************************************************************
 *********************     Test Routines    *************************
 ********************************************************************/
  
#ifdef TEST_HASH

int main(int argc, char *argv[])
{
  t_hash *hash;
  char key[40];

  char *value;
  int i;

  int n_strings;
  int initial_size;

  n_strings = 5;
  initial_size = 0;

  switch(argc)
  {
    case 3: initial_size = atoi(argv[2]);
    case 2: n_strings    = atoi(argv[1]);
  } 

  hash = HashCreate(initial_size);

  for(i = 0; i < n_strings; i++)
  {
    sprintf(key, "key_%d", i);

    value = (char *)malloc(sizeof(char)*(strlen(key)+7));

    sprintf(value, "%s_value", key);

    printf("Adding key %d\n", i);

    HashStore(hash, strlen(key), key, 0, (void *)value);
  }

  for(i = 0; i < n_strings; i++)
  {
    sprintf(key, "key_%d", i);

    printf("Key %s -> '%s'\n", key, (char *)HashGet(hash, strlen(key), key, 0));
  }

  for(i = 0; i < n_strings; i++)
  {
    sprintf(key, "key_%d", i);
    HashDelete(hash, strlen(key), key, 0);
  }

  for(i = 0; i < n_strings; i++)
  {
    sprintf(key, "key_%d", i);

    value = (char *)HashGet(hash, strlen(key), key, 0);

    if(value)
    {
      printf("Key %s -> '%s', but should be NULL!\n", key, value);
    }
  }

  return 0;
}

#endif /* TEST_HASH */
