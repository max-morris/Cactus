 /*@@
   @header    Hash.h
   @date      Wed Oct 27 23:28:53 1999
   @author    Tom Goodale
   @desc 
   Header file for hash stuff.
   @enddesc 
   @version $Header$
 @@*/

#ifndef _HASH_H_
#define _HASH_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct T_HASH_ENTRY
{
  struct T_HASH_ENTRY *last;
  struct T_HASH_ENTRY *next;
 
  unsigned int hash;

  unsigned int klen;
  char *key;

  void *data;
} t_hash_entry;

typedef struct T_HASH
{
  unsigned int size;
  unsigned int fill;
  unsigned int keys;

  t_hash_entry **array;
} t_hash;

t_hash *HashCreate(unsigned int initial_size);
void HashDestroy(t_hash *hash);

int HashStore(t_hash *hash, 
              unsigned int klen, 
              char *key, 
              unsigned int hashval,
              void *data);

int HashAdd(t_hash *hash, 
            unsigned int klen, 
            char *key, 
            unsigned int hashval, 
            void *data);

int HashDelete(t_hash *hash, 
               unsigned int klen, 
               char *key, unsigned int hashval);

void *HashGet(t_hash *hash, 
              unsigned int klen, 
              char *key, 
              unsigned int hashval);

unsigned int HashHash(unsigned int klen, 
                      char *key);

#ifdef __cplusplus
}
#endif

#endif /* _HASH_H_ */
