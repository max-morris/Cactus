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
#define _HASH_H_ 1

#ifdef __cplusplus
extern "C" 
{
#endif

typedef struct T_HASH_ENTRY
{
  struct T_HASH_ENTRY *last;
  struct T_HASH_ENTRY *next;
 
  unsigned int hash;

  unsigned int klen;
  char *key;

  void *data;
} iHashEntry;

typedef struct T_HASH
{
  unsigned int size;
  unsigned int fill;
  unsigned int keys;

  iHashEntry **array;
} uHash;

uHash *Util_HashCreate(unsigned int initial_size);
int Util_HashDestroy(uHash *hash);

int Util_HashStore(uHash *hash, 
                   unsigned int klen, 
                   char *key, 
                   unsigned int hashval,
                   void *data);

int Util_HashAdd(uHash *hash, 
                 unsigned int klen, 
                 char *key, 
                 unsigned int hashval, 
                 void *data);

int Util_HashDelete(uHash *hash, 
                    unsigned int klen, 
                    char *key, 
                    unsigned int hashval);

void *Util_HashData(uHash *hash, 
                    unsigned int klen, 
                    char *key, 
                    unsigned int hashval);

unsigned int Util_HashHash(unsigned int klen, 
                           char *key);

#ifdef __cplusplus
}
#endif

#endif /* _HASH_H_ */
