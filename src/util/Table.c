/* Table.c -- implementation for key-value tables */
/* $Header$ */

/*@@
 @file          Table.c
 @seeheader     util_Table.h
 @date          Wed Oct 31 16:17:45 MET 2001
 @author        Jonathan Thornburg <jthorn@aei.mpg.de>
 @desc
                This program implements the key-value table API defined
                in util_Table.h and in the Cactus User's Guide.  A slightly
                earlier version of this is documented in
                  http://www.cactuscode.org/Development/Specs/KeyValueLookup.txt
 @enddesc
 @version       $Id$
 @@*/

/*
 * ***** table of contents for this file *****
 *
 * Growable Array Data Structures
 * Table Data Structures
 * Iterator Data Structures
 * Misc Macros for This File
 * Prototypes for Functions Private to This File
 * Main Table API
 *   Util_TableCreate
 *   Util_TableClone
 *   Util_TableDestroy
 *   Util_TableQueryFlags
 *   Util_TableQueryNKeys
 *   Util_TableQueryMaxKeyLength
 *   Util_TableQueryValueInfo
 *   Util_TableDeleteKey
 *   Util_TableCreateFromString
 *   Util_TableSetFromString
 *   Util_TableSetString
 *   Util_TableGetString
 *   Util_TableSetGeneric
 *   Util_TableSetGenericArray
 *   Util_TableGetGeneric
 *   Util_TableGetGenericArray
 *   Util_TableSet*
 *   Util_TableSet*Array
 *   Util_TableGet*
 *   Util_TableGet*Array
 * Table Iterator API
 *   Util_TableItCreate
 *   Util_TableItClone
 *   Util_TableItDestroy
 *   Util_TableItQueryIsNull
 *   Util_TableItQueryIsNonNull
 *   Util_TableItQueryTableHandle
 *   Util_TableItQueryKeyValueInfo
 *   Util_TableItAdvance
 *   Util_TableItResetToStart
 *   Util_TableItSetToNull
 *   Util_TableItSetToKey
 * Internal Support Functions
 *   internal_set
 *   internal_get
 *   get_table_header_ptr
 *   is_bad_key
 *   find_table_entry
 *   insert_table_entry
 *   delete_table_entry_by_key
 *   delete_table_entry_by_ptr
 *   get_iterator_ptr
 *   grow_pointer_array
 *   convert_string_to_number
#ifdef UTIL_TABLE_TEST
 * Table and Iterator Dump Routines
 *   print_all_tables
 *   print_table
 *   print_all_iterators
 * Standalone Test Driver
 *   CHECK_SET_GET_{INT,GENERIC_INT,REAL,COMPLEX}
 *   CHECK_SET_GET_{INT,REAL,GENERIC_REAL,COMPLEX}_ARRAY
 *   main
 *   test_nonexistent_tables
 *   test_table_create_destroy
 *   test_set_get
 *   test_set_get_array
 *   test_iterators
 *   test_delete_table_entry
 *   test_set_create_from_string
 *   test_set_get_string
 *   test_set_get_pointers
 *   test_set_get_fpointers
 *   test_clone
 *   check_table_contents
 *   check_table_contents_ij
 *   check_table_contents_real1
 *   check_table_contents_real_e
#endif
 */

#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* FIXME: C99 defines <stdbool.h>, we should include that or a fake version */
typedef int bool;
#define true    1
#define false   0

#ifndef CCODE
  #define CCODE       /* signal Cactus header files that we're C, not Fortran */
#endif

#include "cctk_Types.h"
#include "cctk_Constants.h"
#include "cctk_Groups.h"
#include "cctk_Flesh.h"
#include "cctk_FortranString.h"

#include "util_ErrorCodes.h"
#include "util_String.h"
#include "util_Table.h"

#ifdef UTIL_TABLE_TEST
  #include "cctk_Version.h"
#endif

#ifdef UTIL_TABLE_TEST
/* we build a standalone test driver */
#endif

#ifdef UTIL_TABLE_DEBUG
/* we print various debugging information */
#endif

#ifdef UTIL_TABLE_DEBUG2
  /* we print very verbose debugging information */
  #define UTIL_TABLE_DEBUG
#endif

#ifndef UTIL_TABLE_TEST
  static const char *rcsid = "$Header$";
  CCTK_FILEVERSION(util_Table_c);
#endif

/******************************************************************************/
/***** Growable Array Data Structures *****************************************/
/******************************************************************************/

/*
 * We use "growable arrays" to keep track of all tables and all table
 * iterators.  In both cases we use the same data structure:
 *
 *      int N_objects;          // actual number of tables/iterators
 *      int N_elements;            // actual size of growable array
 *      void *array;            // pointer to malloc-allocated growable array
 *                              // indexed by handle/ihandle
 *
 * Note that the pointer must be  void *  so we can use the  grow_array()
 * function; this pointer should be cast into an actual usable type for
 * normal uses.  Null pointers in the array mark unused array elements.
 */

/*
 * growth policy for growable arrays
 * sequence is
#ifdef UTIL_TABLE_TEST
 *      0, 1, 3, 7, 15, ... entries     (very slow growth
 *                                       ==> better exercise growing code)
#else
 *      0, 10, 30, 70, 150, ... entries
#endif
 * n.b. this grows >= a geometric series
 *      ==> total time in realloc is linear in max array size
 *      (if we just grew in an arithmetic progression then the total
 *       time in realloc() would be quadratic in the max array size)
 */
#ifdef UTIL_TABLE_TEST
  #define GROW(old_n)   (2*(old_n) + 1)
#else
  #define GROW(old_n)   (2*(old_n) + 10)
#endif

/******************************************************************************/
/***** Table Data Structures **************************************************/
/******************************************************************************/

/*
 * The present implementation represents a table as a singly-linked
 * list of table entries.  The code is generally programmed for simplicity,
 * not for maximum performance: linear searches are used everywhere.
 * In practice, we don't expect tables to have very many entries, so
 * this shouldn't be a problem.
 */

struct table_entry
{
  struct table_entry *next;
  char *key;
  int type_code;
  int N_elements;
  void *value;
};

struct table_header
{
  struct table_entry *head;
  int flags;
  int handle;
};

struct scalar_value
{
  int datatype;
  union
  {
    CCTK_INT  int_scalar;
    CCTK_REAL real_scalar;
  } value;
};

/*
 * We keep track of all tables with the following variables
 * (all are static ==> private to this file)
 */

/* number of tables */
static int N_tables = 0;

/* number of elements in the following array */
static int N_thp_array = 0;

/*
 * pointer to growable array of pointers to table headers,
 *            indexed by table handle,
 * with unused array elements set to NULL pointers
 * ... name abbreviates "table-header-pointer array"
 */
void **thp_array = NULL;

/******************************************************************************/
/***** Iterator Data Structures ***********************************************/
/******************************************************************************/

/*
 * This structure represents a table interator.
 *
 * Note that we never modify the table through an iterator,
 * so all the pointers here are to const objects
 */
struct iterator
{
  const struct table_header *thp;   /* must always be non-NULL */
  const struct table_entry *tep;    /* NULL for iterator in */
                                    /* "null-pointer" state */
};

/*
 * We keep track of all iterators with the following variables
 * (all are static ==> private to this file)
 */

/* number of iterators */
static int N_iterators = 0;

/* number of elements in the following array */
static int N_ip_array = 0;

/*
 * pointer to growable array of pointers to iterators,
 *            indexed by iterator handle,
 * with unused array elements set to NULL pointers
 * ... name abbreviates "iterator-pointer array"
 */
void **ip_array = NULL;

/******************************************************************************/
/***** Misc Macros for This File **********************************************/
/******************************************************************************/

#define min(x,y)        ((x < y) ? (x) : (y))


/******************************************************************************/
/***** Prototypes for Functions Private to This File **************************/
/******************************************************************************/

/*
 * This is the internal function implementing all the
 *      Util_TableSet*()
 *      Util_TableSet*Array()
 * functions.  It returns their desired return value, i.e.
 *      1 for key was already in table before this call
 *        (old value was replaced)
 *        (it doesn't matter what the old value's type_code and
 *         N_elements were, i.e. these do *not* have to match the
 *         new value),
 *      0 for key was not in table before this call,
 *      UTIL_ERROR_BAD_HANDLE           handle is invalid
 *      UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character
 *      UTIL_ERROR_BAD_INPUT            N_elements < 0
 *      UTIL_ERROR_NO_MEMORY            unable to allocate memory
 */
static
  int internal_set(int handle,
                   int type_code, int N_elements, const void *value,
                   const char *key);

/*
 * This is the internal function implementing all the
 *      Util_TableGet*()
 *      Util_TableGet*Array()
 * functions.  It returns their desired return value, i.e.
 *      number of values stored in  array[]  if ok,
 *      -ve for error, including
 *      UTIL_ERROR_BAD_HANDLE           handle is invalid
 *      UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character
 *      UTIL_ERROR_BAD_INPUT            array != NULL and N_elements < 0
 *      UTIL_ERROR_TABLE_NO_SUCH_KEY    no such key in table
 *      UTIL_ERROR_TABLE_WRONG_DATA_TYPE value has wrong data type
 * If any of the error conditions is returned, the value buffer is unchanged.
 */
static
  int internal_get(int handle,
                   int type_code, int N_value_buffer, void *value_buffer,
                   const char *key);

/* check table handle for validity, return pointer to table header */
static
  struct table_header *get_table_header_ptr(int handle);

/*
 * check if key is syntactically "bad" (eg contains '/' character)
 * returns true for bad key, false for ok
 */
static
  bool is_bad_key(const char *key);

/*
 * delete an entry (specified by its key) from a table
 * return same as Util_TableDeleteKey(), i.e.
 *      0 for ok (key existed before this call, and has now been deleted)
 *      -ve for error, including
 *      UTIL_ERROR_TABLE_NO_SUCH_KEY    no such key in table
 */
static
  int delete_table_entry_by_key(struct table_header *thp, const char *key);

/*
 * delete an entry from a table,
 * specifying the entry by a pointer to the entry *before* it,
 * or NULL to delete the starting entry in the list
 */
static
  void delete_table_entry_by_ptr(struct table_header *thp,
                                 struct table_entry *prev_tep);

/*
 * find table entry for a given key
 * return pointer to it, or NULL if no such key is present in table
 * if  prev_tep_ptr != NULL,
 *         also set *prev_tep_ptr to point to table entry one *before*
 *         the one with the given key, or to NULL if the given key is
 *         the starting entry in the table
 */
static
  struct table_entry *find_table_entry
          (const struct table_header *thp, const char *key,
           struct table_entry **prev_tep_ptr);

/* allocate a new table entry, set its fields to copies of arguments */
static
  int insert_table_entry(struct table_header *thp,
                           const char *key,
                           int type_code, int N_elements, const void *value);

/* check iterator handle for validity, return pointer to iterator */
static
  struct iterator *get_iterator_ptr(int ihandle);

/*
 * This function grows an malloc-allocated array of  void *  pointers
 * via realloc(), initializing the new space to NULL pointers.
 *
 * Arguments:
 * *pN = (in out) array size
 * *pvp_array = (in out) Pointer to growable array of  void *  pointers.
 *
 * Results:
 * This function returns
 *      0 for ok,
 *      -ve for error, including
 *      UTIL_ERROR_NO_MEMORY            can't allocate memory to grow table
 */
static
  int grow_pointer_array(int *pN, void ***pvp_array);

/*
 * This function converts the given string into a scalar value of type
 * CCTK_INT or CCTK_REAL.
 *
 * Arguments:
 * string = (in) null-terminated string to be converted into a number
 * scalar = (out) structure defining the type and value of the number
 */
static
  void convert_string_to_number(const char *string, struct scalar_value *scalar);

#ifdef UTIL_TABLE_TEST
/*
 * Print out the table and iterator data structures.
 */
static void print_all_tables(void);
static void print_table(int handle);
static void print_all_iterators(void);
#endif

#ifdef UTIL_TABLE_TEST
/*
 * test drivers
 */
static void test_nonexistent_tables(void);
static void test_table_create_destroy(void);
static void test_set_get(int handle, bool case_insensitive);
static void test_set_get_array(int handle);
static void test_iterators(int handle);
static void test_delete_table_entry(int handle, bool case_insensitive);
static int test_set_create_from_string(void);
static void test_set_get_string(int handle, bool case_insensitive);
static void test_set_get_pointers(int handle);
static void test_set_get_fpointers(int handle);
static void test_clone(int handle);
static void check_table_contents(int handle, bool order_up_flag);
static void check_table_contents_ij(int handle, int ihandle);
static void check_table_contents_real1(int handle, int ihandle);
static void check_table_contents_real_e(int handle, int ihandle);
#endif

/******************************************************************************/
/***** Main Table API *********************************************************/
/******************************************************************************/

/*@@
  @routine      Util_TableCreate
  @desc
                This function creates a new (empty) table.
  @enddesc

  @var          flags
  @vtype        int
  @vdesc        inclusive-or of UTIL_TABLE_FLAGS_* bit flags, must be >= 0
                (n.b. for Fortran users: inclusive-or is the same as sum here,
                since the bit masks are all disjoint)
  @endvar

  @comment
                We require flags >= 0 so other functions can distinguish
                flags from (negative) error codes
  @endcomment

  @returntype   int
  @returndesc
                a handle to the newly-created table,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_NO_MEMORY            unable to allocate memory<BR>
                UTIL_ERROR_TABLE_BAD_FLAGS      flags < 0
  @endreturndesc
  @@*/
int Util_TableCreate(int flags)
{
  #ifdef UTIL_TABLE_DEBUG
  printf("Util_TableCreate()\n");
  #endif

  if (flags < 0)
    return UTIL_ERROR_TABLE_BAD_FLAGS;

  if (N_tables == N_thp_array)
  {
    /* grow  thp_array  to get some room to create the new table */
    #ifdef UTIL_TABLE_DEBUG
    printf("   growing thp_array[] from old size %d\n", N_thp_array);
    #endif
    if (grow_pointer_array(&N_thp_array, &thp_array) < 0)
    {
      return UTIL_ERROR_NO_MEMORY;
    }
    #ifdef UTIL_TABLE_DEBUG
    printf("                         to new size %d\n", N_thp_array);
    #endif
  }

  /* we should now have space to create the new table */
  assert(N_tables < N_thp_array);

  /* find an unused handle */
  #ifdef UTIL_TABLE_DEBUG
  printf("   searching for an unused handle (N_tables=%d N_thp_array=%d)\n",
         N_tables, N_thp_array);
  #endif
    {
  int handle;
  for (handle = 0 ; handle < N_thp_array ; ++handle)
  {
    #ifdef UTIL_TABLE_DEBUG2
    printf("      checking handle=%d\n", handle);
    #endif
    if (thp_array[handle] == NULL)
    {
      /* we've found an unused handle ==> create the table */
      struct table_header *const thp = (struct table_header *)
                                       malloc(sizeof(struct table_header));
      if (thp == NULL)
      {
        return UTIL_ERROR_NO_MEMORY;
      }

      #ifdef UTIL_TABLE_DEBUG
      printf("   using handle=%d\n", handle);
      #endif

      thp->head = NULL;
      thp->flags = flags;
      thp->handle = handle;

      ++N_tables;
      thp_array[handle] = (void *) thp;

      return handle;
    }
  }

  /* we should never get to here! */
  assert(false);
  abort();                                /* internal error (core dump) */
  /* prevent compiler warning 'function should return a value' */
  return(0);
    }
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableCreate)
                           (int *retval, const int *flags);
void CCTK_FCALL CCTK_FNAME (Util_TableCreate)
                           (int *retval, const int *flags)
{
  *retval = Util_TableCreate (*flags);
}
#endif	/* !UTIL_TABLE_TEST */

/******************************************************************************/

/*@@
  @routine      Util_TableClone
  @desc
                This function clones (makes an exact copy of) a table.
                (N.b. the order in which an interator sequences through
                a table may differ in the clone.)
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table to be cloned
  @endvar

  @returntype   int
  @returndesc
                a handle to the clone table, or<BR>
                UTIL_ERROR_NO_MEMORY            unable to allocate memory<BR>
                UTIL_ERROR_TABLE_BAD_FLAGS      flags < 0 in the to-be-cloned
                                                table (this should never happen)
  @endreturndesc
  @@*/
int Util_TableClone(int handle)
{
  const struct table_header *const thp = get_table_header_ptr(handle);
  if (thp == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

    {
  const int clone_handle = Util_TableCreate(thp->flags);
  if (clone_handle < 0)
  {
    return clone_handle;                        /* error creating clone table */
  }

  #ifdef UTIL_TABLE_DEBUG
  printf("Util_TableClone(handle=%d) ==> clone_handle=%d\n",
         handle, clone_handle);
  #endif

  /* copy all the table entries */
    {
  struct table_header *const clone_thp = get_table_header_ptr(clone_handle);
  const struct table_entry *tep;
  for (tep = thp->head ; tep != NULL ; tep = tep->next)
  {
    #ifdef UTIL_TABLE_DEBUG2
    printf("   copying key \"%s\"\n", tep->key);
    #endif
      {
    /* insert_table_entry() does the actual copying */
    int status
        = insert_table_entry(clone_thp,
                             tep->key,
                             tep->type_code, tep->N_elements, tep->value);
    if (status < 0)
    {
      /* this should cleanup as much as we've done so far */
      Util_TableDestroy(clone_handle);
      return status;          /* error inserting table entry into clone table */
    }
      }
  }

  return clone_handle;
    }
    }
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableClone)
                           (int *retval, const int *handle);
void CCTK_FCALL CCTK_FNAME (Util_TableClone)
                           (int *retval, const int *handle)
{
  *retval = Util_TableClone (*handle);
}
#endif	/* !UTIL_TABLE_TEST */

/******************************************************************************/

/*@@
  @routine      Util_TableDestroy
  @desc
                This function destroys a table.
                (Of course, this invalidates any iterators for this table.)
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @returntype   int
  @returndesc
                0 for ok,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid
  @endreturndesc
  @@*/
int Util_TableDestroy(int handle)
{
  struct table_header *const thp = get_table_header_ptr(handle);
  if (thp == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

  #ifdef UTIL_TABLE_DEBUG
  printf("Util_TableDestroy(handle=%d)\n", handle);
  #endif

  /* delete all the keys */
  while (thp->head != NULL)
  {
    delete_table_entry_by_ptr(thp, NULL);
  }

  --N_tables;
  thp_array[handle] = NULL;
  free(thp);

  return 0;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableDestroy)
                           (int *retval, const int *handle);
void CCTK_FCALL CCTK_FNAME (Util_TableDestroy)
                           (int *retval, const int *handle)
{
  *retval = Util_TableDestroy (*handle);
}
#endif	/* !UTIL_TABLE_TEST */

/******************************************************************************/

/*@@
  @routine      Util_TableQueryFlags
  @desc
                This function queries a table's flags word.
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @returntype   int
  @returndesc
                flags if table exists,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid
  @endreturndesc
  @@*/
int Util_TableQueryFlags(int handle)
{
  const struct table_header *const thp = get_table_header_ptr(handle);
  if (thp == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

  return thp->flags;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableQueryFlags)
                           (int *retval, const int *handle);
void CCTK_FCALL CCTK_FNAME (Util_TableQueryFlags)
                           (int *retval, const int *handle)
{
  *retval = Util_TableQueryFlags (*handle);
}
#endif	/* !UTIL_TABLE_TEST */

/******************************************************************************/

/*@@
  @routine      Util_TableQueryNKeys
  @desc
                This function queries the total number of key/value entries
                in a table.
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @returntype   int
  @returndesc
                number of entries (>= 0),<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid
  @endreturndesc
  @@*/
int Util_TableQueryNKeys(int handle)
{
  const struct table_header *const thp = get_table_header_ptr(handle);
  if (thp == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

    {
  int N = 0;
  const struct table_entry *tep;
  for (tep = thp->head ; tep != NULL ; tep = tep->next)
  {
    ++N;
  }

  return N;
    }
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableQueryNKeys)
                           (int *retval, const int *handle);
void CCTK_FCALL CCTK_FNAME (Util_TableQueryNKeys)
                           (int *retval, const int *handle)
{
  *retval = Util_TableQueryNKeys (*handle);
}
#endif	/* !UTIL_TABLE_TEST */

/******************************************************************************/

/*@@
  @routine      Util_TableQueryMaxKeyLength
  @desc
                This function queries the maximum key length in a table.
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @returntype   int
  @returndesc
                maximum key length (>= 0),<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid
  @endreturndesc
  @@*/
int Util_TableQueryMaxKeyLength(int handle)
{
  const struct table_header *const thp = get_table_header_ptr(handle);
  if (thp == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

    {
  int max_length = 0;
  const struct table_entry *tep;
  for (tep = thp->head ; tep != NULL ; tep = tep->next)
  {
    const int length = strlen(tep->key);
    if (length > max_length)
    {
      max_length = length;
    }
  }

  return max_length;
    }
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableQueryMaxKeyLength)
                           (int *retval, const int *handle);
void CCTK_FCALL CCTK_FNAME (Util_TableQueryMaxKeyLength)
                           (int *retval, const int *handle)
{
  *retval = Util_TableQueryMaxKeyLength (*handle);
}
#endif	/* !UTIL_TABLE_TEST */

/******************************************************************************/

/*@@
  @routine      Util_TableQueryValueInfo
  @desc
                This function queries the type and number of elements
                of the value corresponding to a specified key in a table.
                It can also be used to "just" determine whether or not
                a specified key is present in a table.
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @var          type_code
  @vtype        int *
  @vdesc        pointer to where this function should store
                the value's type code
                (one of the CCTK_VARIABLE_* constants from "cctk_Types.h"),
                or NULL pointer to skip storing this
  @endvar

  @var          N_elements
  @vtype        int *
  @vdesc        pointer to where this function should store
                the number of array elements in the value,
                or NULL pointer to skip storing this
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to the key (a C-style null-terminated string)
  @endvar

  @returntype   int
  @returndesc
                1 for key is in table,<BR>
                0 for no such key in table
                  (in this case nothing is stored in *type and *N_elements)<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid<BR>
                UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character
  @endreturndesc

  @comment
                Unlike all the other query functions, this function
                returns 0 for no such key in table.  The rationale
                for this design is that by passing NULL pointers for
                type_code and N_elements, this function is then a
                Boolean "is key in table?" predicate.

                If any error code is returned, the user's buffers
                pointed to by type_code and N_elements (if these pointers
                are non-NULL) are unchanged.
  @endcomment
  @@*/
int Util_TableQueryValueInfo(int handle,
                             CCTK_INT *type_code, CCTK_INT *N_elements,
                             const char *key)
{
  const struct table_header *const thp = get_table_header_ptr(handle);
  if (thp == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

  if (is_bad_key(key))
  {
    return UTIL_ERROR_TABLE_BAD_KEY;
  }

    {
  const struct table_entry *const tep = find_table_entry(thp, key, NULL);
  if (tep == NULL)
  {
    return 0;                             /* no such key in table */
  }

  if (type_code != NULL)
  {
    *type_code = tep->type_code;
  }
  if (N_elements != NULL)
  {
    *N_elements = tep->N_elements;
  }
  return 1;                               /* key is in table */
    }
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableQueryValueInfo)
                           (int *retval, const int *handle,
                            CCTK_INT *type_code, CCTK_INT *N_elements,
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableQueryValueInfo)
                           (int *retval, const int *handle,
                            CCTK_INT *type_code, CCTK_INT *N_elements,
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableQueryValueInfo (*handle, type_code, N_elements, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

/******************************************************************************/

/*@@
  @routine      Util_TableDeleteKey
  @desc
                This function deletes a key (and the corresponding value)
                from a table.

                Note that this invalidates any iterators for this table.
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to the key (a C-style null-terminated string)
  @endvar

  @returntype   int
  @returndesc
                0 for ok (key existed before this call,
                          and has now been deleted)<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid<BR>
                UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character<BR>
                UTIL_ERROR_TABLE_NO_SUCH_KEY    no such key in table
  @endreturndesc
  @@*/
int Util_TableDeleteKey(int handle, const char *key)
{
  struct table_header *const thp = get_table_header_ptr(handle);
  if (thp == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

  if (is_bad_key(key))
  {
    return UTIL_ERROR_TABLE_BAD_KEY;
  }

  return delete_table_entry_by_key(thp, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableDeleteKey)
                           (int *retval, const int *handle, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableDeleteKey)
                           (int *retval, const int *handle, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableDeleteKey (*handle, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

/******************************************************************************/

/*@@
  @routine      Util_TableCreateFromString
  @desc
                This function creates a new table (with the case-insensitive
                flag set), and sets values in it based on a string argument.
                The string is interpreted with "parameter-file" semantics.
  @enddesc

  @comment
                The "Implementation Restriction" of Util_TableSetFromString()
                applies here as well.
  @endcomment

  @var          string
  @vtype        const char *
  @vdesc        C-style null-terminated string specifying table contents;
                string has parameter-file semantics
  @endvar

  @returntype   int
  @returndesc
                a handle to the newly-created table,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_NO_MEMORY    unable to allocate memory<BR>
                UTIL_ERROR_BAD_KEY      invalid input: key contains
                                        invalid character<BR>
                UTIL_ERROR_BAD_INPUT    invalid input: can't parse input
                                        string<BR>
                and any error codes returned by
                Util_TableCreate() or Util_TableSetFromString()
  @endreturndesc
  @@*/
int Util_TableCreateFromString(const char string[])
{
  const int handle = Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE);
  if (handle < 0)
  {
    return handle;                                    /* error creating table */
  }

    {
  const int status = Util_TableSetFromString(handle, string);
  if (status < 0)
  {
    Util_TableDestroy(handle);
    return status;                           /* error setting values in table */
  }

  return handle;
    }
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableCreateFromString)
                           (int *retval, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableCreateFromString)
                           (int *retval, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (string)
  *retval = Util_TableCreateFromString (string);
  free (string);
}
#endif	/* !UTIL_TABLE_TEST */

/******************************************************************************/

/*@@
  @routine      Util_TableSetFromString
  @desc
                This function does a sequence of Util_TableSet*() calls
                to set table entries based on a parameter-file--like
                string argument.  For example,
                   Util_TableSetFromString(handle, "order=3 dx=0.1")
                is equivalent to
                   Util_TableSetInt(handle, 3, "order");
                   Util_TableSetReal(handle, 0.1, "dx");
  @enddesc
  @history
  @hdate        Thu 23 May 2002
  @hauthor      Thomas Radke
  @hdesc        Completed for setting string and array values
  @endhistory

  @comment
                Implementation Restriction:<BR>
                The present implementation only recognises integer, real,
                and character-string values (not complex), and integer
                and real arrays.<P>
                In more detail, the strings recognized are defined by the
                following BNF:<BR>
                <BLOCKQUOTE>
                   string -> assign*<BR>
                   assign -> whitespace*<BR>
                   assign -> whitespace* key whitespace* =
                                             whitespace* value delimiter<BR>
                   key    -> any string not containing '/' or '=' or
                             whitespace<BR>
                   value  -> array | int_value | real_value | string_value<BR>
                   array  -> { int_value* } | { real_value* }<BR>
                   int_value    -> anything recognized as a valid integer
                                   by strdol(3) in base 10<BR>
                   real_value   -> anything not recognized as a valid integer
                                   by strtol(3) but recognized as valid by
                                   strdod(3)<BR>
                   string_value -> a C-style string enclosed in "double quotes"
                                   (C-style character escape codes are allowed
                                   ie. '\a', '\b', '\f', '\n', '\r', '\t',
                                       '\v', '\\', '\'', '\"', '\?')<BR>
                   string_value -> A string enclosed in 'single quotes'
                                   (C-style character escape codes are *not*
                                    allowed, ie. every character within the
                                    string is interpreted literally)<BR>
                   delimiter -> end-of-string | whitespace<BR>
                   whitespace --> ' ' | '\t' | '\n' | '\r' | '\f' | '\v'<BR>
                </BLOCKQUOTE>
                where * denotes 0 or more repetitions and | denotes logical or.
                <P>
                Notice also that the keys allowed by this function are
                somewhat more restricted than those allowed by the other
                Util_TableSet*() functions, in that this function disallows
                keys containing '=' and/or whitespace.
  @endcomment

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @var          string
  @vtype        const char *
  @vdesc        C-style null-terminated string which is parsed as
                described above to determine the keys and values to be
                set in the table.
  @endvar

  @returntype   int
  @returndesc
                the number of successful Util_TableSet*() calls made, or<BR>
                -ve for error, including<BR>
                UTIL_ERROR_NO_MEMORY    unable to allocate memory<BR>
                UTIL_ERROR_BAD_KEY      invalid input: key contains
                                        invalid character<BR>
                UTIL_ERROR_BAD_INPUT    invalid input: can't parse input
                                        string<BR>
                UTIL_ERROR_TABLE_NO_MIXED_TYPE_ARRAY
                                        invalid input: different array elements
                                        differ in their datatypes<BR>
                and any error codes returned by the Util_TableSet*() functions
                Note that in the event of an error return, assignments
                lexicographically earlier in the input string than where
                the error was detected will already have been made in the
                table.  Unfortunately, there is no easy way to find out
                where the error was detected. :(
  @endreturndesc
  @@*/
int Util_TableSetFromString(int handle, const char string[])
{
#define WHITESPACE " \t\n\r\f\v"
  struct scalar_value scalar;

  #ifdef UTIL_TABLE_DEBUG
  printf("Util_TableSetFromString(handle=%d, \"%s\")\n", handle, string);
  #endif

  /* make a copy of the string so we can write null characters into it */
  /* to partition it into substrings */
    {
  char *const buffer = Util_Strdup(string);
  if (buffer == NULL)
  {
    return UTIL_ERROR_NO_MEMORY;
  }

    {
  int Set_count = 0, status = 0;
  char *p = buffer;

  while (*p != '\0' && status >= 0)
  {
    /*
     * each pass through this loop processes a single key=value
     * assignment starting at p, creating a table entry for it
     */

    /* skip leading whitespaces */
    p += strspn(p, WHITESPACE);

    #ifdef UTIL_TABLE_DEBUG2
    printf("   skipped over delimiters to p-buffer=%d\n", (int) (p-buffer));
    #endif

    if (*p == '\0')
    {
      break;              /* end of string -- nothing more to do */
    }

      {
    const char *const key = p;                /* key -> "key = value..." */
    char *q = p + strcspn (p, WHITESPACE "=");/* q   -> " = value..." */
    p = q + strspn (q, WHITESPACE);           /* p   -> "= value..." */
    if (*p != '=')
    {
      status = UTIL_ERROR_BAD_INPUT;          /* no '=" in "key=value" string */
      break;
    }

    *q = '\0';                                /* key -> "key" */
    ++p;                                      /* p   -> " value..." */
    p += strspn (p, WHITESPACE);              /* p   -> "value..." */
    if (*p == '\0')
    {
      status = UTIL_ERROR_BAD_INPUT;          /* no value supplied */
      break;
    }

      {
    char *value = p;                          /* value -> "value..." */

    /* split "value..." into "value" and "..." */

    /* check the type of value which is either
     *   - a string enclosed in single or double quotes
     *   - an array of scalars enclosed in round brackets
     *   - a scalar (integer or real)
     */
    if (*value == '\'' || *value == '"' || *value == '{')
    {
      /*
       * this block handles string values and arrays
       */
      q = ((*p == '{') ? "}" : p);            /* q points to delimiter char */

      /* advance to the end of the string or array value */
      do
      {
        /* skip escape character in double-quoted string */
        if (*q == '\"' && *p == '\\' && p[1] != '\0')
        {
          p++;
        }
        p++;
      } while (*p && *p != *q);

      if (*p != *q)
      {
        status = UTIL_ERROR_BAD_INPUT;   /* no closing delimiter found */
        break;                           /* in string or array value */
      }

      /* expand character escape codes in double-quoted string */
      if (*p == '\"')
      {
        while (p > q)
        {
          if (*q == '\\')
          {
            #define CHARACTER_ESCAPE_CODES  "abfnrtv\\\'\"?"
            const char *offset = strchr (CHARACTER_ESCAPE_CODES, q[1]);
            const char character_escape_codes[] =
            {'\a', '\b', '\f', '\n', '\r', '\t', '\v', '\\', '\'', '\"', '\?'};

            if (offset)
            {
              memmove (q, q + 1, p - q);
              q[0] = character_escape_codes[offset - CHARACTER_ESCAPE_CODES];
              q[p - q - 1] = '\0';
            }
            else
            {
              break;                       /* invalid escape code found */
            }
          }
          q++;
        }

        if (p != q)
        {
          status = UTIL_ERROR_BAD_INPUT;   /* invalid escape code found */
          break;
        }
      }
      q = value;            /* q points to the opening delimiter char */
      value++;              /* value skips the delimiter char */
    }
    else
    {
      /*
       * this block handles numbers
       */
      p += strcspn (value, WHITESPACE);
      q = value;
    }

    if (*p != '\0')         /* if we're already at the end of the buffer */
                            /* we don't want to advance further */
    {
      *p++ = '\0';          /* value -> "value", p -> "..." */
    }

    /* set the key/value pair in the table, according to its type */
    if (*q != '{')
    {
      /*
       * this block handles numbers and string values
       */
      if (*q == '\'' || *q == '"')
      {
        status = Util_TableSetString(handle, value, key);
      }
      else
      {
        convert_string_to_number (value, &scalar);
        if (scalar.datatype == CCTK_VARIABLE_INT)
        {
          status = Util_TableSetInt(handle, scalar.value.int_scalar, key);
        }
        else if (scalar.datatype == CCTK_VARIABLE_REAL)
        {
          status = Util_TableSetReal(handle, scalar.value.real_scalar, key);
        }
        else
        {
          status = UTIL_ERROR_BAD_INPUT;   /* can't parse scalar value */
        }
      }

      #ifdef UTIL_TABLE_DEBUG2
      if (status >= 0)
      {
        printf("   ==> storing key='%s', value='%s'\n", key, value);
        printf("   after key=value, advanced p to p-buffer=%d\n",
               (int) (p-buffer));
        printf("   ==> '%s'\n", p);
      }
      #endif
    }
    else
    {
      /*
       * this block handles array values
       */
      int nvals = 0;
      int arraysize = 0;
      int datatype = CCTK_VARIABLE_INT;
      int datatypesize = 0;
      char *array = NULL;


      while (*value)
      {
        /*
         * each pass through this loop processes a single key=value
         * assignment in the array string starting at <value>,
         * extracting the scalar value using convert_string_to_number(),
         * and pushing it to the resulting generic array buffer
         */

        /* skip leading whitespaces */
        value += strspn (value, WHITESPACE);

        /* split "value..." into "value" and "..." */
        q = value + strcspn (value, WHITESPACE);
        if (*q != '\0')         /* if we're already at the end of the list */
                                /* we don't want to advance further */
        {
          *q++ = '\0';          /* value -> "value", q -> "..." */
        }

        convert_string_to_number (value, &scalar);
        if (scalar.datatype == -1)
        {
          datatype = scalar.datatype;
          status = UTIL_ERROR_BAD_INPUT;   /* can't parse array value */
          break;
        }
        if (nvals == 0)
        {
          datatype = scalar.datatype;
          datatypesize = CCTK_VarTypeSize (datatype);
        }
        else if (datatype != scalar.datatype)
        {
          /* all array values must have the same datatype */
          datatype = -1;
          status = UTIL_ERROR_TABLE_NO_MIXED_TYPE_ARRAY;
          break;
        }

        if (nvals >= arraysize)
        {
          if (arraysize == 0)
          {
            /* let's start with an array size of 20 elements */
            arraysize = 20;
            array = malloc (arraysize * datatypesize);
          }
          else
          {
            /* double the array size once it's filled up */
            arraysize *= 2;
            array = realloc (array, arraysize * datatypesize);
          }
          if (array == NULL)
          {
            status = UTIL_ERROR_NO_MEMORY;
            break;
          }
        }

        /* push the new scalar into the array buffer */
        memcpy (array + nvals*datatypesize, &scalar.value, datatypesize);

        #ifdef UTIL_TABLE_DEBUG2
        printf("   ==> storing key='%s', array value='%s'\n", key, value);
        #endif

        nvals++;
        value = q;
      }

      if (datatype == CCTK_VARIABLE_INT || datatype == CCTK_VARIABLE_REAL)
      {
        status = Util_TableSetGenericArray(handle, datatype, nvals, array, key);
      }

      if (array)
      {
        free (array);
      }
    }

    ++Set_count;
      }
      }
  }

  #ifdef UTIL_TABLE_DEBUG2
  printf("   returning with code %d\n", status >= 0 ? Set_count : status);
  #endif

  free(buffer);
  return (status >= 0 ? Set_count : status);
    }
    }
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetFromString)
                           (int *retval, const int *handle, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetFromString)
                           (int *retval, const int *handle, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (string)
  *retval = Util_TableSetFromString (*handle, string);
  free (string);
}
#endif	/* !UTIL_TABLE_TEST */

/******************************************************************************/

/*@@
  @routine      Util_TableSetString
  @desc
                This function sets the value associated with a specified
                key to be (a copy of) a specified character string.

                Note that this invalidates any iterators for this table.
  @enddesc

  @comment
                This function stores the value as array of strlen(string)
                CCTK_CHARs; the stored value does *not* include a terminating
                null character.  (This is convenient for Fortran.)

                The implementation assumes (as is presently the case)
                that a string is in fact an array of CCTK_CHAR, i.e.
                that CCTK_CHAR is the same type as (or at least
                compatible with) char.
  @endcomment

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @var          string
  @vtype        const char *
  @vdesc        pointer to the (C-style null-terminated) string
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to the key (a C-style null-terminated string)
  @endvar

  @returntype   int
  @returndesc
                Same as all the other  Util_TableSet*  functions, namely<BR>
                1 for key was already in table before this call
                  (old value was replaced)
                  (it doesn't matter what the old value's type_code and
                   N_elements were, i.e. these do *not* have to match the
                   new value),<BR>
                0 for key was not in table before this call,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid<BR>
                UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character<BR>
                UTIL_ERROR_NO_MEMORY            unable to allocate memory
  @endreturndesc
  @@*/
int Util_TableSetString(int handle,
                        const char *string,
                        const char *key)
{
  return internal_set(handle,
                      CCTK_VARIABLE_CHAR, strlen(string), (const void *) string,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetString)
                           (int *retval, const int *handle, TWO_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetString)
                           (int *retval, const int *handle, TWO_FORTSTRING_ARG)
{
  TWO_FORTSTRING_CREATE (string, key)
  *retval = Util_TableSetString (*handle, string, key);
  free (string); free (key);
}
#endif	/* !UTIL_TABLE_TEST */

/******************************************************************************/

/*@@
  @routine      Util_TableGetString
  @desc
                This function gets a copy of the character-string value
                associated with a specified key, and stores it (or at least
                as much of it as will fit) in a specified character string.
  @enddesc

  @comment
                This function assumes that the value stored in the table
                is an array of CCTK_CHARs, which does *not* include a
                terminating null character.

                The implementation assumes (as is presently the case)
                that a string is in fact an array of CCTK_CHAR, i.e.
                that CCTK_CHAR is the same type as (or at least
                compatible with) char.
  @endcomment

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @var          buffer_length
  @vtype        int (must be >= 1 if buffer != NULL)
  @vdesc        size of  buffer[]
  @endvar

  @var          buffer
  @vtype        char[]
  @vdesc        a buffer into which this function should store
                (at most  buffer_length-1  characters of) the value,
                terminated by a null character as usual for C strings,
                or NULL pointer to skip storing this
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to the key (a C-style null-terminated string)
  @endvar

  @returntype   int
  @returndesc
                the string length of the value (as per strlen()),<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid<BR>
                UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character<BR>
                UTIL_ERROR_BAD_INPUT            buffer != NULL
                                                and buffer_length <= 0<BR>
                UTIL_ERROR_TABLE_NO_SUCH_KEY    no such key in table<BR>
                UTIL_ERROR_TABLE_WRONG_DATA_TYPE    value has data type
                                                    other than CCTK_CHAR<BR>
                UTIL_ERROR_TABLE_STRING_TRUNCATED   buffer != NULL and
                                                    value was truncated
                                                    to fit in buffer[]
  @endreturndesc

  @comment
                If the error code UTIL_ERROR_TABLE_STRING_TRUNCATED is
                returned, then the first buffer_length-1 characters of
                the string are returned in the user's buffer (assuming
                buffer is non-NULL), followed by a null character to
                properly terminate the string in the buffer.  If any
                other error code is returned, the user's value buffer
                (pointed to by buffer if this is non-NULL) is unchanged.
  @endcomment
  @@*/
int Util_TableGetString(int handle,
                        int buffer_length, char buffer[],
                        const char *key)
{
  /* string_length = actual length of string, not counting terminating '\0' */
  const int string_length = internal_get(handle,
                                         CCTK_VARIABLE_CHAR,
                                         buffer_length-1, (void *) buffer,
                                         key);
  if (string_length < 0)
  {
    return string_length;                 /* error return from internal_get() */
  }

  /* explicitly add the terminating null character */
  if (buffer != NULL)
  {
    assert(buffer_length >= 1);   /* this should never fail: */
                                  /* internal_get() should return an error */
                                  /* if buffer != NULL and buffer_length <= 0 */
      {
    const int null_posn = min(string_length, buffer_length-1);
    buffer[null_posn] = '\0';
      }
  }

  return ((buffer != NULL) && (string_length > buffer_length-1))
         ? UTIL_ERROR_TABLE_STRING_TRUNCATED
         : string_length;
}
/*** FIXME: no fortran wrapper yet ***/

/******************************************************************************/

/*@@
  @routine      Util_TableSetGeneric
  @desc
                This function sets the value associated with a specified
                key to be a specified value (treated as a 1-element array),
                whose datatype is specified by a CCTK_VARIABLE_* type code.

                Note that this invalidates any iterators for this table.
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @var          type_code
  @vtype        int
  @vdesc        one of the CCTK_VARIABLE_* constants from "cctk_Types.h",
                describing the actual data type of *value_ptr
  @endvar

  @var          value_ptr
  @vtype        const void *
  @vdesc        a pointer to the value to be associated with the specified key
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to the key (a C-style null-terminated string)
  @endvar

  @returntype   int
  @returndesc
                1 for key was already in table before this call
                  (old value was replaced)
                  (it doesn't matter what the old value's type_code and
                   N_elements were, i.e. these do *not* have to match the
                   new value),<BR>
                0 for key was not in table before this call,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid<BR>
                UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character<BR>
                UTIL_ERROR_NO_MEMORY            unable to allocate memory
  @endreturndesc
  @@*/
int Util_TableSetGeneric(int handle,
                         int type_code, const void *value_ptr,
                         const char *key)
{
  return Util_TableSetGenericArray(handle, type_code, 1, value_ptr, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetGeneric)
                           (int *retval, const int *handle,
                            const int *type_code, const CCTK_POINTER *value,
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetGeneric)
                           (int *retval, const int *handle,
                            const int *type_code, const CCTK_POINTER *value,
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetGeneric (*handle, *type_code, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

/******************************************************************************/

/*@@
  @routine      Util_TableSetGenericArray
  @desc
                This function sets the value associated with a specified
                key to be (a copy of) a specified array, whose datatype is
                specified by a CCTK_VARIABLE_* type code.

                Note that this invalidates any iterators for this table.
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @var          type_code
  @vtype        int
  @vdesc        one of the CCTK_VARIABLE_* constants from "cctk_Types.h",
                describing the actual data type of array[]
  @endvar

  @var          N_elements
  @vtype        int (must be >= 0)
  @vdesc        number of elements in  array[]
  @endvar

  @var          array
  @vtype        const void *
  @vdesc        a pointer to the array (a copy of) which
                is to be associated with the specified key
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to the key (a C-style null-terminated string)
  @endvar

  @returntype   int
  @returndesc
                1 for key was already in table before this call
                  (old value was replaced)
                  (it doesn't matter what the old value's type_code and
                   N_elements were, i.e. these do *not* have to match the
                   new value),<BR>
                0 for key was not in table before this call,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid<BR>
                UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character<BR>
                UTIL_ERROR_BAD_INPUT            N_elements < 0<BR>
                UTIL_ERROR_NO_MEMORY            unable to allocate memory
  @endreturndesc
  @@*/
int Util_TableSetGenericArray(int handle,
                              int type_code, int N_elements, const void *array,
                              const char *key)
{
  return internal_set(handle,
                      type_code, N_elements, array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetGenericArray)
                           (int *retval, const int *handle,
                            const int *type_code, const int *N_elements,
                            const CCTK_POINTER array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetGenericArray)
                           (int *retval, const int *handle,
                            const int *type_code, const int *N_elements,
                            const CCTK_POINTER array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetGenericArray (*handle, *type_code, *N_elements,
                                       array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

/******************************************************************************/

/*@@
  @routine      Util_TableGetGeneric
  @desc
                This function gets the value of the scalar (1-element array)
                value, or more generally the first array element of the value,
                associated with a specified key.  The value may be of any
                supported datatype; the caller specifies the expected type
                by a CCTK_VARIABLE_* type code.
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @var          type_code
  @vtype        int
  @vdesc        one of the CCTK_VARIABLE_* constants from "cctk_Types.h",
                describing the expected data type of the table entry.
  @endvar

  @var          value_ptr
  @vtype        void *
  @vdesc        pointer to where this function should store
                a copy of the value associated with the specified key,
                or NULL pointer to skip storing this
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to the key (a C-style null-terminated string)
  @endvar

  @returntype   int
  @returndesc
                the number of elements in the value,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid<BR>
                UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character<BR>
                UTIL_ERROR_TABLE_NO_SUCH_KEY    no such key in table<BR>
                UTIL_ERROR_TABLE_WRONG_DATA_TYPE value has wrong data type<BR>
                UTIL_ERROR_TABLE_VALUE_IS_EMPTY value is an empty
                                                (0-element) array
  @endreturndesc

  @comment
                Note that it is *not* an error for the value to actually
                be an array with > 1 elements elements; in this case only
                the first element is stored.

                The rationale for this design is that the caller may
                know or suspect that the value is a large array, but
                may only want the first array element; in this case
                this design avoids the caller having to allocate a
                large buffer unnecessarily.

                In contrast, it *is* an error for the value to actually
                be an empty (0-length) array, because then there is no
                ``first array element'' to get.
  @endcomment
  @@*/
int Util_TableGetGeneric(int handle,
                         int type_code, void *value_ptr,
                         const char *key)
{
  const int status
        = Util_TableGetGenericArray(handle, type_code, 1, value_ptr, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetGeneric)
                           (int *retval, const int *handle,
                            const int *type_code, CCTK_POINTER *value,
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetGeneric)
                           (int *retval, const int *handle,
                            const int *type_code, CCTK_POINTER *value,
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetGeneric (*handle, *type_code, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

/******************************************************************************/

/*@@
  @routine      Util_TableGetGenericArray
  @desc
                This is a family of functions, one for each Cactus data type,
                to get a copy of the value associated with a specified key
                (or at least as much of the value as will fit into the
                caller's array).
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @var          type_code
  @vtype        int
  @vdesc        one of the CCTK_VARIABLE_* constants from "cctk_Types.h",
                describing the expected data type of the table entry.
  @endvar

  @var          N_elements
  @vtype        int (must be >= 0)
  @vdesc        number of elements in  array[]
  @endvar

  @var          array
  @vtype        void *
  @vdesc        a pointer to an array into which this function should store
                (at most  N_elements  elements of) a copy of the value
                associated with the specified key,
                or NULL pointer to skip storing this
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to the key (a C-style null-terminated string)
  @endvar

  @returntype   int
  @returndesc
                the number of elements in the value,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid<BR>
                UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character<BR>
                UTIL_ERROR_BAD_INPUT            array != NULL and
                                                N_elements < 0<BR>
                UTIL_ERROR_TABLE_NO_SUCH_KEY    no such key in table<BR>
                UTIL_ERROR_TABLE_WRONG_DATA_TYPE value has wrong data type
  @endreturndesc

  @comment
                Note that it is *not* an error for the value to have
                > N_elements elements; in this case only N_elements are
                stored.  The caller can detect this by comparing the
                return value with N_elements.

                The rationale for this design is that the caller may
                know or suspect that the value is a large array, but
                may only want the first few array elements; in this
                case this design avoids the caller having to allocate
                a large buffer unnecessarily.

                It is also *not* an error for the value to have < N_elements
                elements; again the caller can detect this by comparing the
                return value with N_elements.

                Note also that if any error code is returned, the
                caller's value buffer (pointed to by  value_buffer)
                is unchanged.
  @endcomment
  @@*/
int Util_TableGetGenericArray(int handle,
                              int type_code, int N_elements, void *array,
                              const char *key)
{
  return internal_get(handle,
                      type_code, N_elements, array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetGenericArray)
                           (int *retval, const int *handle,
                            const int *type_code, const int *N_elements,
                            CCTK_POINTER array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetGenericArray)
                           (int *retval, const int *handle,
                            const int *type_code, const int *N_elements,
                            CCTK_POINTER array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetGenericArray (*handle, *type_code, *N_elements,
                                       array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

/******************************************************************************/

/*@@
  @routine      Util_TableSet*
  @desc
                This is a family of functions, one for each Cactus data type,
                to set the value associated with a specified key to be a
                specified value (treated as a 1-element array).

                Note that this invalidates any iterators for this table.
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @var          value
  @vtype        one of
                   CCTK_POINTER, CCTK_FPOINTER,
                   CCTK_CHAR,
                   CCTK_INT, CCTK_INT1, CCTK_INT2, CCTK_INT4, CCTK_INT8,
                   CCTK_REAL, CCTK_REAL4, CCTK_REAL8, CCTK_REAL16,
                   CCTK_COMPLEX, CCTK_COMPLEX8, CCTK_COMPLEX16, CCTK_COMPLEX32
                (not all of these may be supported on any given system)
  @vdesc        the value to be associated with the specified key
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to the key (a C-style null-terminated string)
  @endvar

  @returntype   int
  @returndesc
                1 for key was already in table before this call
                  (old value was replaced)
                  (it doesn't matter what the old value's type_code and
                   N_elements were, i.e. these do *not* have to match the
                   new value),<BR>
                0 for key was not in table before this call,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid<BR>
                UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character<BR>
                UTIL_ERROR_NO_MEMORY            unable to allocate memory
  @endreturndesc
  @@*/

/**************************************/

/*
 * pointers
 */

int Util_TableSetPointer(int handle, CCTK_POINTER value, const char *key)
{
  return Util_TableSetPointerArray(handle, 1, &value, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetPointer)
                           (int *retval, const int *handle,
                            const CCTK_POINTER *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetPointer)
                           (int *retval, const int *handle, const CCTK_POINTER *value,
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetPointer (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

int Util_TableSetFPointer(int handle, CCTK_FPOINTER value, const char *key)
{
  return Util_TableSetFPointerArray(handle, 1, &value, key);
}

/*
 * ... the following function (an alias for the previous one) is for
 *     backwards compatability only, and is deprecated as of 4.0beta13
 */
int Util_TableSetFnPointer(int handle, CCTK_FPOINTER value, const char *key)
{
  return Util_TableSetFPointerArray(handle, 1, &value, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetFPointer)
                           (int *retval, const int *handle,
                            const CCTK_FPOINTER *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetFPointer)
                           (int *retval, const int *handle,
                            const CCTK_FPOINTER *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetFPointer (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifndef UTIL_TABLE_TEST
/*
 * ... the following function (an alias for the previous one) is for
 *     backwards compatability only, and is deprecated as of 4.0beta13
 */
void CCTK_FCALL CCTK_FNAME (Util_TableSetFnPointer)
                           (int *retval, const int *handle,
                            const CCTK_FPOINTER *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetFnPointer)
                           (int *retval, const int *handle,
                            const CCTK_FPOINTER *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetFPointer (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

/**************************************/

/*
 * a single character
 */

int Util_TableSetChar(int handle, CCTK_CHAR value, const char *key)
{
  return Util_TableSetCharArray(handle, 1, &value, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetChar)
                           (int *retval, const int *handle,
                            const CCTK_CHAR *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetChar)
                           (int *retval, const int *handle,
                            const CCTK_CHAR *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetChar (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

/**************************************/

/*
 * integers
 */

int Util_TableSetInt(int handle, CCTK_INT value, const char *key)
{
  return Util_TableSetIntArray(handle, 1, &value, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt)
                           (int *retval, const int *handle,
                            const CCTK_INT *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt)
                           (int *retval, const int *handle,
                            const CCTK_INT *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetInt (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifdef CCTK_INT1
int Util_TableSetInt1(int handle, CCTK_INT1 value, const char *key)
{
  return Util_TableSetInt1Array(handle, 1, &value, key);
}

void CCTK_FCALL CCTK_FNAME (Util_TableSetInt1)
                           (int *retval, const int *handle,
                            const CCTK_INT1 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt1)
                           (int *retval, const int *handle,
                            const CCTK_INT1 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetInt1 (*handle, *value, key);
  free (key);
}
#endif

#ifdef CCTK_INT2
int Util_TableSetInt2(int handle, CCTK_INT2 value, const char *key)
{
  return Util_TableSetInt2Array(handle, 1, &value, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt2)
                           (int *retval, const int *handle,
                            const CCTK_INT2 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt2)
                           (int *retval, const int *handle,
                            const CCTK_INT2 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetInt2 (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_INT4
int Util_TableSetInt4(int handle, CCTK_INT4 value, const char *key)
{
  return Util_TableSetInt4Array(handle, 1, &value, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt4)
                           (int *retval, const int *handle,
                            const CCTK_INT4 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt4)
                           (int *retval, const int *handle,
                            const CCTK_INT4 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetInt4 (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_INT8
int Util_TableSetInt8(int handle, CCTK_INT8 value, const char *key)
{
  return Util_TableSetInt8Array(handle, 1, &value, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt8)
                           (int *retval, const int *handle,
                            const CCTK_INT8 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt8)
                           (int *retval, const int *handle,
                            const CCTK_INT8 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetInt8 (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

/**************************************/

/*
 * real numbers
 */

int Util_TableSetReal(int handle, CCTK_REAL value, const char *key)
{
  return Util_TableSetRealArray(handle, 1, &value, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetReal)
                           (int *retval, const int *handle,
                            const CCTK_REAL *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetReal)
                           (int *retval, const int *handle,
                            const CCTK_REAL *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetReal (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifdef CCTK_REAL4
int Util_TableSetReal4(int handle, CCTK_REAL4 value, const char *key)
{
  return Util_TableSetReal4Array(handle, 1, &value, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetReal4)
                           (int *retval, const int *handle,
                            const CCTK_REAL4 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetReal4)
                           (int *retval, const int *handle,
                            const CCTK_REAL4 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetReal4 (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL8
int Util_TableSetReal8(int handle, CCTK_REAL8 value, const char *key)
{
  return Util_TableSetReal8Array(handle, 1, &value, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetReal8)
                           (int *retval, const int *handle,
                            const CCTK_REAL8 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetReal8)
                           (int *retval, const int *handle,
                            const CCTK_REAL8 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetReal8 (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL16
int Util_TableSetReal16(int handle, CCTK_REAL16 value, const char *key)
{
  return Util_TableSetReal16Array(handle, 1, &value, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetReal16)
                           (int *retval, const int *handle,
                            const CCTK_REAL16 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetReal16)
                           (int *retval, const int *handle,
                            const CCTK_REAL16 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetReal16 (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

/**************************************/

/*
 * complex numbers
 */

int Util_TableSetComplex(int handle, CCTK_COMPLEX value, const char *key)
{
  return Util_TableSetComplexArray(handle, 1, &value, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplex)
                           (int *retval, const int *handle,
                            const CCTK_COMPLEX *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplex)
                           (int *retval, const int *handle,
                            const CCTK_COMPLEX *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetComplex (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifdef CCTK_REAL4
int Util_TableSetComplex8(int handle, CCTK_COMPLEX8 value, const char *key)
{
  return Util_TableSetComplex8Array(handle, 1, &value, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplex8)
                           (int *retval, const int *handle,
                            const CCTK_COMPLEX8 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplex8)
                           (int *retval, const int *handle,
                            const CCTK_COMPLEX8 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetComplex8 (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL8
int Util_TableSetComplex16(int handle, CCTK_COMPLEX16 value, const char *key)
{
  return Util_TableSetComplex16Array(handle, 1, &value, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplex16)
                           (int *retval, const int *handle,
                            const CCTK_COMPLEX16 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplex16)
                           (int *retval, const int *handle,
                            const CCTK_COMPLEX16 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetComplex16 (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL16
int Util_TableSetComplex32(int handle, CCTK_COMPLEX32 value, const char *key)
{
  return Util_TableSetComplex32Array(handle, 1, &value, key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplex32)
                           (int *retval, const int *handle,
                            const CCTK_COMPLEX32 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplex32)
                           (int *retval, const int *handle,
                            const CCTK_COMPLEX32 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetComplex32 (*handle, *value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

/******************************************************************************/

/*@@
  @routine      Util_TableSet*Array
  @desc
                This is a family of functions, one for each Cactus data type,
                to set the value associated with the specified key to be
                (a copy of) a specified array.

                Note that this invalidates any iterators for this table.
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @var          N_elements
  @vtype        int (must be >= 0)
  @vdesc        number of elements in  array[]
  @endvar

  @var          array
  @vtype        const T[], where T is one of
                   CCTK_POINTER, CCTK_FPOINTER,
                   CCTK_CHAR,
                   CCTK_INT, CCTK_INT1, CCTK_INT2, CCTK_INT4, CCTK_INT8,
                   CCTK_REAL, CCTK_REAL4, CCTK_REAL8, CCTK_REAL16,
                   CCTK_COMPLEX, CCTK_COMPLEX8, CCTK_COMPLEX16, CCTK_COMPLEX32
                (not all of these may be supported on any given system)
  @vdesc        a pointer to the array (a copy of) which
                is to be associated with the specified key
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to the key (a C-style null-terminated string)
  @endvar

  @returntype   int
  @returndesc
                1 for key was already in table before this call
                  (old value was replaced)
                  (it doesn't matter what the old value's type_code and
                   N_elements were, i.e. these do *not* have to match the
                   new value),<BR>
                0 for key was not in table before this call,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid<BR>
                UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character<BR>
                UTIL_ERROR_BAD_INPUT            N_elements < 0<BR>
                UTIL_ERROR_NO_MEMORY            unable to allocate memory
  @endreturndesc
  @@*/

/**************************************/

/*
 * arrays of pointers
 */

int Util_TableSetPointerArray(int handle,
                              int N_elements, const CCTK_POINTER array[],
                              const char *key)
{
  return internal_set(handle,
                      CCTK_VARIABLE_POINTER, N_elements, (const void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetPointerArray)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_POINTER array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetPointerArray)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_POINTER array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetPointerArray (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

int Util_TableSetFPointerArray(int handle,
                               int N_elements, const CCTK_FPOINTER array[],
                               const char *key)
{
  return
    internal_set(handle,
                 CCTK_VARIABLE_FPOINTER, N_elements, (const void *) array,
                 key);
}

/*
 * ... the following function (an alias for the previous one) is for
 *     backwards compatability only, and is deprecated as of 4.0beta13
 */
int Util_TableSetFnPointerArray(int handle,
                                int N_elements, const CCTK_FPOINTER array[],
                                const char *key)
{
  return
    internal_set(handle,
                 CCTK_VARIABLE_FPOINTER, N_elements, (const void *) array,
                 key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetFPointerArray)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_FPOINTER array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetFPointerArray)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_FPOINTER array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetFPointerArray (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifndef UTIL_TABLE_TEST
/*
 * ... the following function (an alias for the previous one) is for
 *     backwards compatability only, and is deprecated as of 4.0beta13
 */
void CCTK_FCALL CCTK_FNAME (Util_TableSetFnPointerArray)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_FPOINTER array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetFnPointerArray)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_FPOINTER array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetFPointerArray (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

/**************************************/

/*
 * arrays of characters (i.e. character strings)
 */

int Util_TableSetCharArray(int handle,
                           int N_elements, const CCTK_CHAR array[],
                           const char *key)
{
  return internal_set(handle,
                      CCTK_VARIABLE_CHAR, N_elements, (const void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetCharArray)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_CHAR array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetCharArray)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_CHAR array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetCharArray (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

/**************************************/

/*
 * arrays of integers
 */

int Util_TableSetIntArray(int handle,
                          int N_elements, const CCTK_INT array[],
                          const char *key)
{
  return internal_set(handle,
                      CCTK_VARIABLE_INT, N_elements, (const void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetIntArray)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_INT array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetIntArray)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_INT array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetIntArray (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifdef CCTK_INT1
int Util_TableSetInt1Array(int handle,
                           int N_elements, const CCTK_INT1 array[],
                           const char *key)
{
  return internal_set(handle,
                      CCTK_VARIABLE_INT1, N_elements, (const void *) array,
                      key);
}

void CCTK_FCALL CCTK_FNAME (Util_TableSetInt1Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_INT1 array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt1Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_INT1 array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetInt1Array (*handle, *N_elements, array, key);
  free (key);
}
#endif

#ifdef CCTK_INT2
int Util_TableSetInt2Array(int handle,
                           int N_elements, const CCTK_INT2 array[],
                           const char *key)
{
  return internal_set(handle,
                      CCTK_VARIABLE_INT2, N_elements, (const void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt2Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_INT2 array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt2Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_INT2 array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetInt2Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_INT4
int Util_TableSetInt4Array(int handle,
                           int N_elements, const CCTK_INT4 array[],
                           const char *key)
{
  return internal_set(handle,
                      CCTK_VARIABLE_INT4, N_elements, (const void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt4Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_INT4 array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt4Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_INT4 array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetInt4Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_INT8
int Util_TableSetInt8Array(int handle,
                           int N_elements, const CCTK_INT8 array[],
                           const char *key)
{
  return internal_set(handle,
                      CCTK_VARIABLE_INT8, N_elements, (const void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt8Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_INT8 array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetInt8Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_INT8 array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetInt8Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

/**************************************/

/*
 * arrays of real numbers
 */

int Util_TableSetRealArray(int handle,
                           int N_elements, const CCTK_REAL array[],
                           const char *key)
{
  return
    internal_set(handle,
                    CCTK_VARIABLE_REAL, N_elements, (const void *) array,
                    key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetRealArray)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_REAL array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetRealArray)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_REAL array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetRealArray (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifdef CCTK_REAL4
int Util_TableSetReal4Array(int handle,
                            int N_elements, const CCTK_REAL4 array[],
                            const char *key)
{
  return internal_set(handle,
                      CCTK_VARIABLE_REAL4, N_elements, (const void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetReal4Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_REAL4 array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetReal4Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_REAL4 array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetReal4Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL8
int Util_TableSetReal8Array(int handle,
                            int N_elements, const CCTK_REAL8 array[],
                            const char *key)
{
  return internal_set(handle,
                      CCTK_VARIABLE_REAL8, N_elements, (const void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetReal8Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_REAL8 array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetReal8Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_REAL8 array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetReal8Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL16
int Util_TableSetReal16Array(int handle,
                             int N_elements, const CCTK_REAL16 array[],
                             const char *key)
{
  return internal_set(handle,
                      CCTK_VARIABLE_REAL16, N_elements, (const void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetReal16Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_REAL16 array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetReal16Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_REAL16 array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetReal16Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

/**************************************/

/*
 * arrays of complex numbers
 */

int Util_TableSetComplexArray(int handle,
                              int N_elements, const CCTK_COMPLEX array[],
                              const char *key)
{
  return internal_set(handle,
                      CCTK_VARIABLE_COMPLEX, N_elements, (const void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplexArray)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_COMPLEX array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplexArray)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_COMPLEX array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetComplexArray (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifdef CCTK_REAL4
int Util_TableSetComplex8Array(int handle,
                               int N_elements, const CCTK_COMPLEX8 array[],
                               const char *key)
{
  return internal_set(handle,
                      CCTK_VARIABLE_COMPLEX8, N_elements, (const void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplex8Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_COMPLEX8 array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplex8Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_COMPLEX8 array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetComplex8Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL8
int Util_TableSetComplex16Array(int handle,
                                int N_elements, const CCTK_COMPLEX16 array[],
                                const char *key)
{
  return internal_set(handle,
                      CCTK_VARIABLE_COMPLEX16, N_elements, (const void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplex16Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_COMPLEX16 array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplex16Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_COMPLEX16 array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetComplex16Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL16
int Util_TableSetComplex32Array(int handle,
                                int N_elements, const CCTK_COMPLEX32 array[],
                                const char *key)
{
  return internal_set(handle,
                      CCTK_VARIABLE_COMPLEX32, N_elements, (const void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplex32Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_COMPLEX32 array[], ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableSetComplex32Array)
                           (int *retval, const int *handle,
                            const int *N_elements,
                            const CCTK_COMPLEX32 array[], ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableSetComplex32Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

/******************************************************************************/

/*@@
  @routine      Util_TableGet*
  @desc
                This is a family of functions, one for each Cactus data type,
                to get a copy of the scalar (1-element array) value, or more
                generally the first array element of the value, associated
                with a specified key.
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @var          value
  @vtype        T *, where T is one of
                   CCTK_POINTER, CCTK_FPOINTER,
                   CCTK_CHAR,
                   CCTK_INT, CCTK_INT1, CCTK_INT2, CCTK_INT4, CCTK_INT8,
                   CCTK_REAL, CCTK_REAL4, CCTK_REAL8, CCTK_REAL16,
                   CCTK_COMPLEX, CCTK_COMPLEX8, CCTK_COMPLEX16, CCTK_COMPLEX32
                (not all of these may be supported on any given system)
  @vdesc        pointer to where this function should store
                a copy of the value associated with the specified key,
                or NULL pointer to skip storing this
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to the key (a C-style null-terminated string)
  @endvar

  @returntype   int
  @returndesc
                the number of elements in the value,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid<BR>
                UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character<BR>
                UTIL_ERROR_TABLE_NO_SUCH_KEY    no such key in table<BR>
                UTIL_ERROR_TABLE_WRONG_DATA_TYPE value has wrong data type<BR>
                UTIL_ERROR_TABLE_VALUE_IS_EMPTY value is an empty
                                                (0-element) array
  @endreturndesc

  @comment
                Note that it is *not* an error for the value to actually
                be an array with > 1 elements elements; in this case only
                the first element is stored.

                The rationale for this design is that the caller may
                know or suspect that the value is a large array, but
                may only want the first array element; in this case
                this design avoids the caller having to allocate a
                large buffer unnecessarily.

                In contrast, it *is* an error for the value to actually
                be an empty (0-length) array, because then there is no
                ``first array element'' to get.
  @endcomment
  @@*/

/**************************************/

/* pointers */
int Util_TableGetPointer(int handle, CCTK_POINTER *value, const char *key)
{
  const int status = Util_TableGetPointerArray(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetPointer)
                           (int *retval, const int *handle,
                            CCTK_POINTER *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetPointer)
                           (int *retval, const int *handle,
                            CCTK_POINTER *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetPointer (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

int Util_TableGetFPointer(int handle, CCTK_FPOINTER *value, const char *key)
{
  const int status = Util_TableGetFPointerArray(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

/*
 * ... the following function (an alias for the previous one) is for
 *     backwards compatability only, and is deprecated as of 4.0beta13
 */
int Util_TableGetFnPointer(int handle, CCTK_FPOINTER *value, const char *key)
{
  const int status = Util_TableGetFPointerArray(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetFPointer)
                           (int *retval, const int *handle,
                            CCTK_FPOINTER *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetFPointer)
                           (int *retval, const int *handle,
                            CCTK_FPOINTER *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetFPointer (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifndef UTIL_TABLE_TEST
/*
 * ... the following function (an alias for the previous one) is for
 *     backwards compatability only, and is deprecated as of 4.0beta13
 */
void CCTK_FCALL CCTK_FNAME (Util_TableGetFnPointer)
                           (int *retval, const int *handle,
                            CCTK_FPOINTER *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetFnPointer)
                           (int *retval, const int *handle,
                            CCTK_FPOINTER *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetFPointer (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

/**************************************/

/* a single character */
int Util_TableGetChar(int handle, CCTK_CHAR *value, const char *key)
{
  const int status = Util_TableGetCharArray(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetChar)
                           (int *retval, const int *handle,
                            CCTK_CHAR *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetChar)
                           (int *retval, const int *handle,
                            CCTK_CHAR *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetChar (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

/**************************************/

/* integers */
int Util_TableGetInt(int handle, CCTK_INT *value, const char *key)
{
  const int status = Util_TableGetIntArray(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt)
                           (int *retval, const int *handle,
                            CCTK_INT *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt)
                           (int *retval, const int *handle,
                            CCTK_INT *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetInt (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifdef CCTK_INT1
int Util_TableGetInt1(int handle, CCTK_INT1 *value, const char *key)
{
  const int status = Util_TableGetInt1Array(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

void CCTK_FCALL CCTK_FNAME (Util_TableGetInt1)
                           (int *retval, const int *handle,
                            CCTK_INT1 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt1)
                           (int *retval, const int *handle,
                            CCTK_INT1 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetInt1 (*handle, value, key);
  free (key);
}
#endif

#ifdef CCTK_INT2
int Util_TableGetInt2(int handle, CCTK_INT2 *value, const char *key)
{
  const int status = Util_TableGetInt2Array(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt2)
                           (int *retval, const int *handle,
                            CCTK_INT2 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt2)
                           (int *retval, const int *handle,
                            CCTK_INT2 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetInt2 (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_INT4
int Util_TableGetInt4(int handle, CCTK_INT4 *value, const char *key)
{
  const int status = Util_TableGetInt4Array(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt4)
                           (int *retval, const int *handle,
                            CCTK_INT4 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt4)
                           (int *retval, const int *handle,
                            CCTK_INT4 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetInt4 (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_INT8
int Util_TableGetInt8(int handle, CCTK_INT8 *value, const char *key)
{
  const int status = Util_TableGetInt8Array(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt8)
                           (int *retval, const int *handle,
                            CCTK_INT8 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt8)
                           (int *retval, const int *handle,
                            CCTK_INT8 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetInt8 (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

/**************************************/

/* real numbers */
int Util_TableGetReal(int handle, CCTK_REAL *value, const char *key)
{
  const int status = Util_TableGetRealArray(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetReal)
                           (int *retval, const int *handle,
                            CCTK_REAL *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetReal)
                           (int *retval, const int *handle,
                            CCTK_REAL *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetReal (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifdef CCTK_REAL4
int Util_TableGetReal4(int handle, CCTK_REAL4 *value, const char *key)
{
  const int status = Util_TableGetReal4Array(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetReal4)
                           (int *retval, const int *handle,
                            CCTK_REAL4 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetReal4)
                           (int *retval, const int *handle,
                            CCTK_REAL4 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetReal4 (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL8
int Util_TableGetReal8(int handle, CCTK_REAL8 *value, const char *key)
{
  const int status = Util_TableGetReal8Array(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetReal8)
                           (int *retval, const int *handle,
                            CCTK_REAL8 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetReal8)
                           (int *retval, const int *handle,
                            CCTK_REAL8 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetReal8 (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL16
int Util_TableGetReal16(int handle, CCTK_REAL16 *value, const char *key)
{
  const int status = Util_TableGetReal16Array(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetReal16)
                           (int *retval, const int *handle,
                            CCTK_REAL16 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetReal16)
                           (int *retval, const int *handle,
                            CCTK_REAL16 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetReal16 (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

/**************************************/

/* complex numbers */
int Util_TableGetComplex(int handle, CCTK_COMPLEX *value, const char *key)
{
  const int status = Util_TableGetComplexArray(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplex)
                           (int *retval, const int *handle,
                            CCTK_COMPLEX *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplex)
                           (int *retval, const int *handle,
                            CCTK_COMPLEX *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetComplex (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifdef CCTK_REAL4
int Util_TableGetComplex8(int handle, CCTK_COMPLEX8 *value, const char *key)
{
  const int status = Util_TableGetComplex8Array(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplex8)
                           (int *retval, const int *handle,
                            CCTK_COMPLEX8 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplex8)
                           (int *retval, const int *handle,
                            CCTK_COMPLEX8 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetComplex8 (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL8
int Util_TableGetComplex16(int handle, CCTK_COMPLEX16 *value, const char *key)
{
  const int status = Util_TableGetComplex16Array(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplex16)
                           (int *retval, const int *handle,
                            CCTK_COMPLEX16 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplex16)
                           (int *retval, const int *handle,
                            CCTK_COMPLEX16 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetComplex16 (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL16
int Util_TableGetComplex32(int handle, CCTK_COMPLEX32 *value, const char *key)
{
  const int status = Util_TableGetComplex32Array(handle, 1, value, key);
  return (status == 0)
         ? UTIL_ERROR_TABLE_VALUE_IS_EMPTY
         : status;
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplex32)
                           (int *retval, const int *handle,
                            CCTK_COMPLEX32 *value, ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplex32)
                           (int *retval, const int *handle,
                            CCTK_COMPLEX32 *value, ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetComplex32 (*handle, value, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

/******************************************************************************/

/*@@
  @routine      Util_TableGet*Array
  @desc
                This is a family of functions, one for each Cactus data type,
                to get a copy of the value associated with a specified key
                (or at least as much of the value as will fit into the
                caller's array).
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @var          N_elements
  @vtype        int (must be >= 0)
  @vdesc        number of elements in  array[]
  @endvar

  @var          array
  @vtype        T[], where T is one of
                   CCTK_POINTER, CCTK_FPOINTER,
                   CCTK_CHAR,
                   CCTK_INT, CCTK_INT1, CCTK_INT2, CCTK_INT4, CCTK_INT8,
                   CCTK_REAL, CCTK_REAL4, CCTK_REAL8, CCTK_REAL16,
                   CCTK_COMPLEX, CCTK_COMPLEX8, CCTK_COMPLEX16, CCTK_COMPLEX32
                (not all of these may be supported on any given system)
  @vdesc        an array into which this function should store
                (at most  N_elements  elements of) a copy of the value
                associated with the specified key,
                or NULL pointer to skip storing this
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to the key (a C-style null-terminated string)
  @endvar

  @comment
                Note that it is *not* an error for the value to have
                > N_elements elements; in this case only N_elements are
                stored.  The caller can detect this by comparing the
                return value with N_elements.

                The rationale for this design is that the caller may
                know or suspect that the value is a large array, but
                may only want the first few array elements; in this
                case this design avoids the caller having to allocate
                a large buffer unnecessarily.

                It is also *not* an error for the value to have < N_elements
                elements; again the caller can detect this by comparing the
                return value with N_elements.

                Note also that if any error code is returned, the
                caller's value buffer (pointed to by  value_buffer)
                is unchanged.
  @endcomment

  @returntype   int
  @returndesc
                the number of elements in the value,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid<BR>
                UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character<BR>
                UTIL_ERROR_BAD_INPUT            array != NULL and N_elements < 0<BR>
                UTIL_ERROR_TABLE_NO_SUCH_KEY    no such key in table<BR>
                UTIL_ERROR_TABLE_WRONG_DATA_TYPE value has wrong data type
  @endreturndesc
  @@*/

/**************************************/

/* arrays of pointers */
int Util_TableGetPointerArray(int handle,
                              int N_elements, CCTK_POINTER array[],
                              const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_POINTER, N_elements, (void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetPointerArray)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_POINTER array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetPointerArray)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_POINTER array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetPointerArray (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

int Util_TableGetFPointerArray(int handle,
                               int N_elements, CCTK_FPOINTER array[],
                               const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_FPOINTER, N_elements, (void *) array,
                      key);
}

/*
 * ... the following function (an alias for the previous one) is for
 *     backwards compatability only, and is deprecated as of 4.0beta13
 */
int Util_TableGetFnPointerArray(int handle,
                                int N_elements, CCTK_FPOINTER array[],
                                const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_FPOINTER, N_elements, (void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetFPointerArray)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_FPOINTER array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetFPointerArray)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_FPOINTER array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetFPointerArray (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifndef UTIL_TABLE_TEST
/*
 * ... the following function (an alias for the previous one) is for
 *     backwards compatability only, and is deprecated as of 4.0beta13
 */
void CCTK_FCALL CCTK_FNAME (Util_TableGetFnPointerArray)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_FPOINTER array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetFnPointerArray)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_FPOINTER array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetFPointerArray (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

/**************************************/

/* arrays of characters (i.e. character strings) */
int Util_TableGetCharArray(int handle,
                           int N_elements, CCTK_CHAR array[],
                           const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_CHAR, N_elements, (void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetCharArray)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_CHAR array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetCharArray)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_CHAR array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetCharArray (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

/**************************************/

/* integers */
int Util_TableGetIntArray(int handle,
                          int N_elements, CCTK_INT array[],
                          const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_INT, N_elements, (void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetIntArray)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_INT array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetIntArray)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_INT array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetIntArray (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifdef CCTK_INT1
int Util_TableGetInt1Array(int handle,
                           int N_elements, CCTK_INT1 array[],
                           const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_INT1, N_elements, (void *) array,
                      key);
}

void CCTK_FCALL CCTK_FNAME (Util_TableGetInt1Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_INT1 array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt1Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_INT1 array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetInt1Array (*handle, *N_elements, array, key);
  free (key);
}
#endif

#ifdef CCTK_INT2
int Util_TableGetInt2Array(int handle,
                           int N_elements, CCTK_INT2 array[],
                           const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_INT2, N_elements, (void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt2Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_INT2 array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt2Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_INT2 array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetInt2Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_INT4
int Util_TableGetInt4Array(int handle,
                           int N_elements, CCTK_INT4 array[],
                           const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_INT4, N_elements, (void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt4Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_INT4 array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt4Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_INT4 array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetInt4Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_INT8
int Util_TableGetInt8Array(int handle,
                           int N_elements, CCTK_INT8 array[],
                           const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_INT8, N_elements, (void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt8Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_INT8 array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetInt8Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_INT8 array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetInt8Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

/**************************************/

/* real numbers */
int Util_TableGetRealArray(int handle,
                           int N_elements, CCTK_REAL array[],
                           const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_REAL, N_elements, (void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetRealArray)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_REAL array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetRealArray)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_REAL array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetRealArray (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifdef CCTK_REAL4
int Util_TableGetReal4Array(int handle,
                            int N_elements, CCTK_REAL4 array[],
                            const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_REAL4, N_elements, (void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetReal4Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_REAL4 array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetReal4Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_REAL4 array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetReal4Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL8
int Util_TableGetReal8Array(int handle,
                            int N_elements, CCTK_REAL8 array[],
                            const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_REAL8, N_elements, (void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetReal8Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_REAL8 array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetReal8Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_REAL8 array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetReal8Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL16
int Util_TableGetReal16Array(int handle,
                             int N_elements, CCTK_REAL16 array[],
                             const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_REAL16, N_elements, (void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetReal16Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_REAL16 array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetReal16Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_REAL16 array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetReal16Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

/**************************************/

/* complex numbers */
int Util_TableGetComplexArray(int handle,
                              int N_elements, CCTK_COMPLEX array[],
                              const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_COMPLEX, N_elements, (void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplexArray)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_COMPLEX array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplexArray)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_COMPLEX array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetComplexArray (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */

#ifdef CCTK_REAL4
int Util_TableGetComplex8Array(int handle,
                               int N_elements, CCTK_COMPLEX8 array[],
                               const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_COMPLEX8, N_elements, (void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplex8Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_COMPLEX8 array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplex8Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_COMPLEX8 array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetComplex8Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL8
int Util_TableGetComplex16Array(int handle,
                                int N_elements, CCTK_COMPLEX16 array[],
                                const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_COMPLEX16, N_elements, (void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplex16Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_COMPLEX16 array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplex16Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_COMPLEX16 array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetComplex16Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

#ifdef CCTK_REAL16
int Util_TableGetComplex32Array(int handle,
                                int N_elements, CCTK_COMPLEX32 array[],
                                const char *key)
{
  return internal_get(handle,
                      CCTK_VARIABLE_COMPLEX16, N_elements, (void *) array,
                      key);
}

#ifndef UTIL_TABLE_TEST
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplex32Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_COMPLEX32 array[],
                            ONE_FORTSTRING_ARG);
void CCTK_FCALL CCTK_FNAME (Util_TableGetComplex32Array)
                           (int *retval, const int *handle,
                            const int *N_elements, CCTK_COMPLEX32 array[],
                            ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE (key)
  *retval = Util_TableGetComplex32Array (*handle, *N_elements, array, key);
  free (key);
}
#endif	/* !UTIL_TABLE_TEST */
#endif

/******************************************************************************/
/***** Table Iterator API *****************************************************/
/******************************************************************************/

/*@@
  @routine      Util_TableItCreate
  @desc
                This function creates a new table iterator.  The iterator
                points to the starting entry in the table's traversal order.
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @returntype   int
  @returndesc
                a handle to the newly-created iterator,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           table handle is invalid<BR>
                UTIL_ERROR_NO_MEMORY            unable to allocate memory
  @endreturndesc
  @@*/
int Util_TableItCreate(int handle)
{
  const struct table_header *const thp = get_table_header_ptr(handle);
  if (thp == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

  #ifdef UTIL_TABLE_DEBUG
  printf("Util_TableItCreate(handle=%d)\n", handle);
  #endif

  if (N_iterators == N_ip_array)
  {
    /* grow  iterator_array  to get some room to create the new table */
    #ifdef UTIL_TABLE_DEBUG
    printf("   growing ip_array[] from old size %d\n",
           N_ip_array);
    #endif
    if (grow_pointer_array(&N_ip_array, &ip_array) < 0)
    {
      return UTIL_ERROR_NO_MEMORY;                        /* can't grow array */
    }
    #ifdef UTIL_TABLE_DEBUG
    printf("      to new size %d\n",
           N_ip_array);
    #endif
  }

  /* we should now have space to create the new iterator */
  assert(N_iterators < N_ip_array);

  /* find an unused iterator handle */
  #ifdef UTIL_TABLE_DEBUG
  printf("   searching for an unused iterator handle\n");
  printf("   (N_iterators=%d N_ip_array=%d\n", N_iterators, N_ip_array);
  #endif
    {
  int ihandle;
    for (ihandle = 0 ; ihandle < N_ip_array ; ++ihandle)
    {
      #ifdef UTIL_TABLE_DEBUG2
      printf("      checking ihandle=%d\n", ihandle);
      #endif
      if (ip_array[ihandle] == NULL)
      {
        /* we've found an unused ihandle ==> create the iterator */
        struct iterator *const ip = (struct iterator *)
                                    malloc(sizeof(struct iterator));
        if (ip == NULL)
        {
          return UTIL_ERROR_NO_MEMORY;         /* can't allocate new iterator */
        }

        #ifdef UTIL_TABLE_DEBUG2
        printf("   using ihandle=%d\n", ihandle);
        #endif

        ip->thp = thp;
        ip->tep = thp->head;    /* iterator initially -> start of table */

        ++N_iterators;
        ip_array[ihandle] = (void *) ip;

        return ihandle;                                      /* NORMAL RETURN */
      }
    }

  /* we should never get to here! */
  assert(false);
  abort();                                      /* internal error (core dump) */
  /* prevent compiler warning 'function should return a value' */
  return(0);
    }
}

/******************************************************************************/

/*@@
  @routine      Util_TableItClone
  @desc
                This function clones (makes an exact copy of) a table
                iterator.  That is, it creates a new iterator which points
                to the same table entry as an existing iterator.
  @enddesc

  @var          ihandle
  @vtype        int
  @vdesc        handle to the iterator to be cloned
  @endvar

  @returntype   int
  @returndesc
                a handle to the newly-created iterator,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           iterator handle is invalid<BR>
                UTIL_ERROR_NO_MEMORY            unable to allocate memory
  @endreturndesc
  @@*/
int Util_TableItClone(int ihandle)
{
  struct iterator *const ip = get_iterator_ptr(ihandle);
  if (ip == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

    {
  const int clone_ihandle = Util_TableItCreate(ip->thp->handle);
  if (clone_ihandle < 0)
  {
    return clone_ihandle;                 /* error in creating clone iterator */
  }

    {
  struct iterator *const clone_ip = get_iterator_ptr(clone_ihandle);
  clone_ip->tep = ip->tep;
  return clone_ihandle;
    }
    }
}

/******************************************************************************/

/*@@
  @routine      Util_TableItDestroy
  @desc
                This function destroys a table iterator.
  @enddesc

  @var          ihandle
  @vtype        int
  @vdesc        handle to the iterator
  @endvar

  @returntype   int
  @returndesc
                0 for ok,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           iterator handle is invalid
  @endreturndesc
  @@*/
int Util_TableItDestroy(int ihandle)
{
  struct iterator *const ip = get_iterator_ptr(ihandle);
  if (ip == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

  #ifdef UTIL_TABLE_DEBUG
  printf("Util_TableItDestroy(ihandle=%d)\n", ihandle);
  #endif

  --N_iterators;
  ip_array[ihandle] = NULL;
  free(ip);

  return 0;                               /* ok */
}

/******************************************************************************/

/*@@
  @routine      Util_TableItQueryIsNull
  @desc
                This function queries whether a table iterator is in the
                "null-pointer" state, i.e. whether it does *not* point
                to some table entry.

                Bad things (garbage results, core dumps) may happen if
                you call this function on a table iterator which has been
                invalidated by a change in the table's contents.
  @enddesc

  @var          ihandle
  @vtype        int
  @vdesc        handle to the iterator
  @endvar

  @returntype   int
  @returndesc
                1 for iterator is in "null-pointer" state,<BR>
                0 for iterator points to some table entry,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           iterator handle is invalid
  @endreturndesc
  @@*/
int Util_TableItQueryIsNull(int ihandle)
{
  const struct iterator *const ip = get_iterator_ptr(ihandle);
  if (ip == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

  return (ip->tep == NULL)
         ? 1                              /* iterator in "null-pointer" state */
         : 0;                             /* iterator -> some table entry */
}

/******************************************************************************/

/*@@
  @routine      Util_TableItQueryIsNonNull
  @desc
                This function queries whether a table iterator is *not* in
                the "null-pointer" state, i.e. whether it points to some
                table entry.

                Bad things (garbage results, core dumps) may happen if
                you call this function on an iterator which has been
                invalidated by a change in the table's contents.
  @enddesc

  @var          ihandle
  @vtype        int
  @vdesc        handle to the iterator
  @endvar

  @returntype   int
  @returndesc
                1 for iterator points to some table entry,<BR>
                0 for iterator is in "null-pointer" state,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           iterator handle is invalid
  @endreturndesc
  @@*/
int Util_TableItQueryIsNonNull(int ihandle)
{
  const struct iterator *const ip = get_iterator_ptr(ihandle);
  if (ip == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

  return (ip->tep == NULL)
         ? 0                              /* iterator in "null-pointer" state */
         : 1;                             /* iterator -> some table entry */
}

/******************************************************************************/

/*@@
  @routine      Util_TableItQueryTableHandle
  @desc
                This function queries which table a table iterator points
                into.

                Note that this is always well-defined, even if the iterator
                is in the "null-pointer" state, and even if the iterator
                has been invalidated by a change in the table's contents.
  @enddesc

  @var          ihandle
  @vtype        int
  @vdesc        handle to the iterator
  @endvar

  @returntype   int
  @returndesc
                table handle,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           iterator handle is invalid
  @endreturndesc
  @@*/
int Util_TableItQueryTableHandle(int ihandle)
{
  const struct iterator *const ip = get_iterator_ptr(ihandle);
  if (ip == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

  return ip->thp->handle;
}

/******************************************************************************/

/*@@
  @routine      Util_TableItQueryKeyValueInfo
  @desc
                This function queries the key and the type and number of
                elements of the value corresponding to that key, of the
                table entry to which an iterator points.  This is in fact
                the main purpose of iterators.

                Bad things (garbage results, core dumps) may happen if
                you call this function on an iterator which has been
                invalidated by a change in the table's contents.
  @enddesc

  @var          ihandle
  @vtype        int
  @vdesc        handle to the iterator
  @endvar

  @var          key_buffer_length,
  @vtype        int (must be >= 1 if key_buffer != NULL)
  @vdesc        length of  key_buffer[]  buffer
  @endvar

  @var          key_buffer,
  @vtype        char []
  @vdesc        a buffer into which this function should store
                (at most  key_buffer_length-1  characters of) the key,
                terminated by a null character as usual for C strings,
                or NULL pointer to skip storing this
  @endvar

  @var          type_code
  @vtype        CCTK_INT *
  @vdesc        pointer to where this function should store
                the value's type code
                (one of the CCTK_VARIABLE_* constants from "cctk_Types.h"),
                or NULL pointer to skip storing this
  @endvar

  @var          N_elements
  @vtype        CCTK_INT *
  @vdesc        pointer to where this function should store
                the number of array elements in the value,
                or NULL pointer to skip storing this
  @endvar

  @returntype   int
  @returndesc
                the string length of the key (as per strlen()),<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           iterator handle is invalid<BR>
                UTIL_ERROR_TABLE_ITERATOR_IS_NULL  iterator is in
                                                   "null-pointer" state<BR>
                UTIL_ERROR_TABLE_STRING_TRUNCATED  key_buffer != NULL and
                                                   key was truncated
                                                   to fit in key_buffer[]
  @endreturndesc

  @comment
                If the error code UTIL_ERROR_TABLE_STRING_TRUNCATED is
                returned, then the first key_buffer_length-1 characters of
                the string are returned in the user's key buffer (assuming
                key_buffer is non-NULL), followed by a null character to
                properly terminate the string in the buffer.  If any
                other error code is returned, the user's key buffer
                (pointed to by key_buffer if this is non-NULL) is unchanged.
  @endcomment
  @@*/
int Util_TableItQueryKeyValueInfo(int ihandle,
                                  int key_buffer_length, char key_buffer[],
                                  CCTK_INT *type_code, CCTK_INT *N_elements)
{
  const struct iterator *const ip = get_iterator_ptr(ihandle);
  if (ip == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

    {
  const struct table_entry *const tep = ip->tep;
  if (tep == NULL)
  {
    return UTIL_ERROR_TABLE_ITERATOR_IS_NULL;
  }

    {
  const int actual_key_length = strlen(tep->key);

  /* store the fixed-length output arguments first, so the caller */
  /* will have them even if we hit an error trying to copy the key */
  if (type_code != NULL)
  {
    *type_code = tep->type_code;
  }
  if (N_elements != NULL)
  {
    *N_elements = tep->N_elements;
  }

  if (key_buffer != NULL)
  {
    const int N_key_copy = min(key_buffer_length-1, actual_key_length);
    if (N_key_copy < 0)     /* can only happen if key_buffer_length <= 0 */
    {
      /*
       * We have to bail out now, before trying the memcpy(), because
       * memcpy() takes a size_t (= unsigned) value for its count of how
       * many chars to copy, and converting our -ve N_key_copy to size_t
       * would give a huge +ve count :( :(
       */
      return UTIL_ERROR_TABLE_STRING_TRUNCATED;
    }
    memcpy(key_buffer, tep->key, N_key_copy);
    key_buffer[N_key_copy] = '\0';
    if (N_key_copy < actual_key_length)
    {
      return UTIL_ERROR_TABLE_STRING_TRUNCATED;
    }
  }

  return actual_key_length;         /* ok */
    }
    }
}

/******************************************************************************/

/*@@
  @routine      Util_TableItAdvance
  @desc
                This function advances a table iterator to the next entry
                in the table's traversal order.

                Bad things (garbage results, core dumps) may happen if
                you call this function on an iterator which has been
                invalidated by a change in the table's contents.
  @enddesc

  @var          ihandle
  @vtype        int
  @vdesc        handle to the iterator
  @endvar

  @returntype   int
  @returndesc
                same as that of Util_TableItQueryNonNull(ihandle)
                after advancing the iterator, i.e.<BR>
                1 for ok and iterator now points to some table element,<BR>
                0 for advance-past-last-entry
                  (sets iterator to "null-pointer" state),<BR>
                0 if iterator was already in "null-pointer" state)
                  (in this case this call is a no-op),<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           iterator handle is invalid
  @endreturndesc
  @@*/
int Util_TableItAdvance(int ihandle)
{
  struct iterator *const ip = get_iterator_ptr(ihandle);
  if (ip == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

  if (ip->tep == NULL)
  {
    return 0;         /* iterator was already in "null-pointer" state */
  }

  ip->tep = ip->tep->next;

  return (ip->tep == NULL)
         ? 0              /* advance past last entry */
                          /* ==> iterator now in "null-pointer" state */
         : 1;             /* ok */
}

/******************************************************************************/

/*@@
  @routine      Util_TableItResetToStart
  @desc
                This function resets a table iterator to point to the
                starting entry in the table's traversal order.

                Note that it is always ok to call this function, even
                if the iterator has been invalidated by a change in the
                table's contents.
  @enddesc

  @var          ihandle
  @vtype        int
  @vdesc        handle to the iterator
  @endvar

  @returntype   int
  @returndesc
                same as that of Util_TableItQueryNonNull(ihandle)
                after resetting the iterator, i.e.<BR>
                1 for ok and iterator now points to some table element,<BR>
                0 for ok and iterator is now in "null-pointer" state
                  (means table is empty)<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           iterator handle is invalid
  @endreturndesc
  @@*/
int Util_TableItResetToStart(int ihandle)
{
  struct iterator *const ip = get_iterator_ptr(ihandle);
  if (ip == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

  ip->tep = ip->thp->head;
  return (ip->tep == NULL)
         ? 0              /* ok, iterator is now in "null-pointer" state */
                          /*     (table must be empty) */
         : 1;             /* ok, iterator points to some table element */
}

/******************************************************************************/

/*@@
  @routine      Util_TableItSetToNull
  @desc
                This function sets a table iterator to the "null-pointer"
                state.

                Note that it is always ok to call this function, even
                if the iterator has been invalidated by a change in the
                table's contents.
  @enddesc

  @var          ihandle
  @vtype        int
  @vdesc        handle to the iterator
  @endvar

  @returntype   int
  @returndesc
                0 for ok,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           iterator handle is invalid
  @endreturndesc
  @@*/
int Util_TableItSetToNull(int ihandle)
{
  struct iterator *const ip = get_iterator_ptr(ihandle);
  if (ip == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

  ip->tep = NULL;
  return 0;                               /* ok */
}

/******************************************************************************/

/*@@
  @routine      Util_TableItSetToKey
  @desc
                This function sets a table iterator to point to a
                specified table entry.  It has the same effect as
                Util_TableItResetToStart() followed by repeatedly
                calling Util_TableItAdvance() until the iterator
                points to the desired table entry.

                Note that it is always ok to call this function, even
                if the iterator has been invalidated by a change in the
                table's contents.
  @enddesc

  @var          ihandle
  @vtype        int
  @vdesc        handle to the iterator
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to the key (a C-style null-terminated string)
  @endvar

  @returntype   int
  @returndesc
                0 for ok,<BR>
                UTIL_ERROR_BAD_HANDLE           iterator handle is invalid<BR>
                UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character<BR>
                UTIL_ERROR_TABLE_NO_SUCH_KEY    no such key in table
  @endreturndesc
  @@*/
int Util_TableItSetToKey(int ihandle, const char *key)
{
  struct iterator *const ip = get_iterator_ptr(ihandle);
  if (ip == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

  if (is_bad_key(key))
  {
    return UTIL_ERROR_TABLE_BAD_KEY;
  }

  ip->tep = find_table_entry(ip->thp, key, NULL);
  if (ip->tep == NULL)
  {
    return UTIL_ERROR_TABLE_NO_SUCH_KEY;
  }

  return 0;
}

/******************************************************************************/
/***** Internal Support Functions *********************************************/
/******************************************************************************/

/*@@
  @routine      internal_set
  @desc
                This is the internal function implementing all the
                        Util_TableSet*()
                        Util_TableSet*Array()
                functions except Util_TableSetString().  It sets the
                value associated with a specified key, to be a copy
                of a specified array.

                Note that this invalidates any iterators for this table.
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @var          N_elements
  @vtype        int (must be >= 0)
  @vdesc        number of elements in  array[]
  @endvar

  @var          array
  @vtype        const T[], where T is one of
                   CCTK_POINTER, CCTK_FPOINTER,
                   CCTK_CHAR,
                   CCTK_INT, CCTK_INT1, CCTK_INT2, CCTK_INT4, CCTK_INT8,
                   CCTK_REAL, CCTK_REAL4, CCTK_REAL8, CCTK_REAL16,
                   CCTK_COMPLEX, CCTK_COMPLEX8, CCTK_COMPLEX16, CCTK_COMPLEX32
                (not all of these may be supported on any given system)
  @vdesc        the array (a copy of) which is to be
                associated with the specified key
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to the key (a C-style null-terminated string)
  @endvar

  @returntype   int
  @returndesc
                1 for key was already in table before this call
                  (old value was replaced)
                  (it doesn't matter what the old value's type_code and
                   N_elements were, i.e. these do *not* have to match the
                   new value),<BR>
                0 for key was not in table before this call,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid<BR>
                UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character<BR>
                UTIL_ERROR_BAD_INPUT            N_elements < 0<BR>
                UTIL_ERROR_NO_MEMORY            unable to allocate memory
  @endreturndesc
  @@*/
static
  int internal_set(int handle,
                   int type_code, int N_elements, const void *value,
                   const char *key)
{
  struct table_header *const thp = get_table_header_ptr(handle);
  if (thp == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

  if (is_bad_key(key))
  {
    return UTIL_ERROR_TABLE_BAD_KEY;
  }
  if (N_elements < 0)
  {
    return UTIL_ERROR_BAD_INPUT;
  }

  #ifdef UTIL_TABLE_DEBUG
  printf("internal_set(handle=%d, type_code=%d, N_elements=%d, key=\"%s\")\n",
         handle, type_code, N_elements, key);
  #endif

  /* if key is already in table, delete it */
  /* ... this is a harmless no-op if it's not already in the table */
    {
  int return_value;
  switch (delete_table_entry_by_key(thp, key))
  {
    case 0:
      return_value = 1;       /* key was already in table before this call */
                              /* (we've just deleted it, and we're about */
                              /*  to set the replacement in the table) */
      break;
    case UTIL_ERROR_TABLE_NO_SUCH_KEY:
      return_value = 0;       /* key was not in table before this call */
      break;
    default:
      /* unexpected return code from  delete_table_entry_by_key() */
      /* (this should never happen!) */
      assert(false);
      abort();                                  /* internal error (core dump) */
  }

    {
  const int status = insert_table_entry(thp,
                                        key,
                                        type_code, N_elements, value);
  if (status < 0)
  {
    return status;                        /* error inserting entry into table */
  }

  return return_value;
    }
    }
}

/******************************************************************************/

/*@@
  @routine      internal_get
  @desc
                This is the internal function implementing all the
                        Util_TableGet*()
                        Util_TableGet*Array()
                functions except for Util_TableGetString().  It copies
                up to N_elements of the value associated with a specified
                key, into a user-supplied buffer.
  @enddesc

  @var          handle
  @vtype        int
  @vdesc        handle to the table
  @endvar

  @var          N_value_buffer
  @vtype        int (must be >= 0)
  @vdesc        number of elements in  array[]
  @endvar

  @var          value_buffer
  @vtype        T[], where T is one of
                   CCTK_POINTER, CCTK_FPOINTER,
                   CCTK_CHAR,
                   CCTK_INT, CCTK_INT1, CCTK_INT2, CCTK_INT4, CCTK_INT8,
                   CCTK_REAL, CCTK_REAL4, CCTK_REAL8, CCTK_REAL16,
                   CCTK_COMPLEX, CCTK_COMPLEX8, CCTK_COMPLEX16, CCTK_COMPLEX32
                (not all of these may be supported on any given system)
  @vdesc        an array into which this function should store
                (at most  N_elements  elements of) a copy of the value
                associated with the specified key,
                or NULL pointer to skip storing this
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to the key (a C-style null-terminated string)
  @endvar

  @returntype   int
  @returndesc
                number of elements in the value,<BR>
                -ve for error, including<BR>
                UTIL_ERROR_BAD_HANDLE           handle is invalid<BR>
                UTIL_ERROR_TABLE_BAD_KEY        key contains '/' character<BR>
                UTIL_ERROR_BAD_INPUT            N_value_buffer < 0<BR>
                UTIL_ERROR_BAD_INPUT            value_buffer != NULL
                                                and N_value_buffer < 0<BR>
                UTIL_ERROR_TABLE_NO_SUCH_KEY    no such key in table<BR>
                UTIL_ERROR_TABLE_WRONG_DATA_TYPE value has wrong data type
  @endreturndesc

  @comment
                Note that it is *not* an error for the value to have
                > N_value_buffer elements; in this case only N_value_buffer
                are stored.  The caller can detect this by comparing the
                return value with N_value_buffer.

                Note also that if any error code is returned, the
                caller's value buffer (pointed to by  value_buffer)
                is unchanged.
  @endcomment
  @@*/
static
  int internal_get(int handle,
                   int type_code, int N_value_buffer, void *value_buffer,
                   const char *key)
{
  const struct table_header *const thp = get_table_header_ptr(handle);
  if (thp == NULL)
  {
    return UTIL_ERROR_BAD_HANDLE;
  }

  if (is_bad_key(key))
  {
    return UTIL_ERROR_TABLE_BAD_KEY;
  }

  #ifdef UTIL_TABLE_DEBUG
  printf(
     "internal_get(handle=%d, type_code=%d, N_value_buffer=%d, key=\"%s\")\n",
         handle, type_code, N_value_buffer, key);
  #endif

    {
  const struct table_entry *const tep = find_table_entry(thp, key, NULL);
  if (tep == NULL)
  {
    return UTIL_ERROR_TABLE_NO_SUCH_KEY;              /* no such key in table */
  }

  if (tep->type_code != type_code)
  {
    return UTIL_ERROR_TABLE_WRONG_DATA_TYPE;     /* value has wrong data type */
  }

  if (value_buffer != NULL)
  {
    if (N_value_buffer < 0)
    {
      return UTIL_ERROR_BAD_INPUT;
    }
      {
    const int N_copy = min(N_value_buffer, tep->N_elements);
    const size_t sizeof_N_copy_elements = N_copy * CCTK_VarTypeSize(type_code);
    #ifdef UTIL_TABLE_DEBUG
    printf(
       "   copying N_copy=%d elements (sizeof_N_copy_elements=%d bytes)\n",
           N_copy, (int) sizeof_N_copy_elements);
    #endif
    memcpy(value_buffer, tep->value, sizeof_N_copy_elements);
      }
  }

  return tep->N_elements;
    }
}

/******************************************************************************/

/*
 * This function gets a pointer to a table's header, given the table handle.
 *
 * Arguments:
 * handle = The table handle.
 *
 * Results:
 * If the handle is invalid (i.e. there is no such table), this function
 *    returns NULL.
 * If the handle is valid, this function returns a pointer to the table header.
 */
static
  struct table_header *get_table_header_ptr(int handle)
{
  return ((handle >= 0) && (handle < N_thp_array))
         ? (struct table_header *) thp_array[handle]      /* valid handle */
         : NULL;                                          /* invalid handle */
}

/******************************************************************************/

/*
 * check if key is syntactically "bad" (eg contains '/' character)
 * returns true for bad key, false for ok
 */
static
  bool is_bad_key(const char *key)
{
  assert(key != NULL);

  if (strchr(key, '/') != NULL)
  {
    return true;
  }

  return false;                           /* ok */
}

/******************************************************************************/

/*
 * This function finds the (first) table entry with a given key.
 * Optionally, it also finds the table entry *before* that one in
 * the linked list.
 *
 * Arguments:
 * thp -> The table header.
 * key -> The key to search for.
 * prev_tep_ptr = If this is non-NULL, then this function sets
 *                *prev_tep_ptr to -> the table entry *before* the one
 *                with the given key, or NULL if the table entry with
 *                the given key is the starting one in the table.
 *                Thus if  prev_tep_ptr  is non-NULL, then after this
 *                function returns, the returned result is
 *                (*prev_tep_ptr == NULL) ? thp->head : (*prev_tep)->next
 *
 * Results:
 * The function returns a pointer to the table entry, or NULL if the
 * key isn't found in the table.
 */
static
  struct table_entry *find_table_entry(const struct table_header *thp,
                                       const char *key,
                                       struct table_entry **prev_tep_ptr)
{
  assert(thp != NULL);
  assert(key != NULL);

    {
  const bool case_insensitive_flag
          = thp->flags & UTIL_TABLE_FLAGS_CASE_INSENSITIVE;
  struct table_entry *prev_tep = NULL;
  struct table_entry *tep = thp->head;
  for ( ; tep != NULL ; prev_tep = tep, tep = tep->next)
  {
    if (  case_insensitive_flag
          ?  (Util_StrCmpi(key, tep->key) == 0)
          :  (     strcmp (key, tep->key) == 0)  )
    {
      if (prev_tep_ptr != NULL)
      {
        *prev_tep_ptr = prev_tep;
      }
      return tep;                         /* key found in table */
    }
  }

  return NULL;                            /* key not found in table */
    }
}

/******************************************************************************/

/*@@
  @routine      insert_table_entry
  @desc
                This is an internal function used in implementing
                Util_TableClone() and internal_set().  It allocates
                a new table entry and sets the fields in it to be
                copies of the arguments.
  @enddesc

  @var          thp
  @vtype        struct table_header *
  @vdesc        pointer to the table header
  @endvar

  @var          key
  @vtype        const char *
  @vdesc        pointer to a (C-style null-terminated) string, a copy
                of which is to be the new table entry's key
  @endvar

  @var          type_code
  @vtype        int
  @vdesc        the value to be the new table entry's type code
                (one of the CCTK_VARIABLE_* constants from "cctk_Types.h"),
  @endvar

  @var          N_elements
  @vtype        int (must be >= 0)
  @vdesc        number of elements in  array[]
  @endvar

  @var          array
  @vtype        const T[], where T is one of
                   CCTK_POINTER, CCTK_FPOINTER,
                   CCTK_CHAR,
                   CCTK_INT, CCTK_INT1, CCTK_INT2, CCTK_INT4, CCTK_INT8,
                   CCTK_REAL, CCTK_REAL4, CCTK_REAL8, CCTK_REAL16,
                   CCTK_COMPLEX, CCTK_COMPLEX8, CCTK_COMPLEX16, CCTK_COMPLEX32
                (not all of these may be supported on any given system)
  @vdesc        (pointer to) an array, a copy of which is to be
                the new table entry's value
  @endvar

  @returntype   int
  @returndesc
                0 for ok,<BR>
                UTIL_ERROR_NO_MEMORY            unable to allocate memory
  @endreturndesc
  @@*/
static
  int insert_table_entry(struct table_header *thp,
                         const char *key,
                         int type_code, int N_elements, const void *value)
{
  struct table_entry *tep = (struct table_entry *)
                            malloc(sizeof(struct table_entry));
  if (tep == NULL)
  {
    return UTIL_ERROR_NO_MEMORY;            /* can't allocate new table entry */
  }

  #ifdef UTIL_TABLE_DEBUG
  printf("insert_table_entry(type_code=%d, N_elements=%d, key=\"%s\")...\n",
         type_code, N_elements, key);
  #endif

  tep->key = Util_Strdup(key);
  if (tep->key == NULL)
  {
    free(tep);
    return UTIL_ERROR_NO_MEMORY;         /* can't allocate memory to copy key */
  }

  tep->type_code = type_code;
  tep->N_elements = N_elements;

    {
  const size_t sizeof_value = N_elements * CCTK_VarTypeSize(type_code);
  #ifdef UTIL_TABLE_DEBUG2
  printf("   allocating new buffer of size sizeof_value=%d bytes\n",
         (int) sizeof_value);
  #endif
    {
  void *const buffer = malloc(sizeof_value);
  if (buffer == NULL)
  {
    free(tep->key);
    free(tep);
    return UTIL_ERROR_NO_MEMORY;   /* can't allocate memory for copy of value */
  }
  #ifdef UTIL_TABLE_DEBUG
  printf("   copying sizeof_value=%d bytes into buffer\n", (int) sizeof_value);
  #endif
  memcpy(buffer, value, sizeof_value);
  tep->value = buffer;

  /* insert the table entry into the table's linked list */
  /* (we could insert it anywhere; for simplicity we insert it at the head) */
  tep->next = thp->head;
  thp->head = tep;

  return 0;
    }
    }
}

/******************************************************************************/

/*
 * This function deletes an entry (specified by its key) from a table,
 * freeing its table entry and the pointed-to key and value.
 *
 * Results:
 * The return value is the same as for Util_TableDeleteKey(), i.e.
 *      0 for ok (key existed before this call, and has now been deleted)
 *      -ve for error, including
 *      UTIL_ERROR_TABLE_NO_SUCH_KEY    no such key in table
 */
static
  int delete_table_entry_by_key(struct table_header *thp, const char *key)
{
  struct table_entry *prev_tep;
  struct table_entry *const tep = find_table_entry(thp, key, &prev_tep);
  if (tep == NULL)
  {
    return UTIL_ERROR_TABLE_NO_SUCH_KEY;
  }

  delete_table_entry_by_ptr(thp, prev_tep);
  return 0;                           /* ok: key existed before this call, */
                                      /* and has now been deleted */
}

/******************************************************************************/

/*
 * This function deletes an entry from a table, freeing its table entry
 * and the pointed-to key and value.  The entry to be deleted is specified
 * by a pointer to the *previous* table entry, or NULL to delete the
 * starting entry in the list.
 */
static
  void delete_table_entry_by_ptr(struct table_header *thp,
                                 struct table_entry *prev_tep)
{
/* this is the entry we want to delete */
struct table_entry *const tep = (prev_tep == NULL) ? thp->head
                                                   : prev_tep->next;
assert(tep != NULL);

/* unlink it from the list */
if (prev_tep == NULL)
{
  thp->head = tep->next;
}
else
{
  prev_tep->next = tep->next;
}

assert(tep->key != NULL);
free(tep->key);

assert(tep->value != NULL);
free(tep->value);

free(tep);
}

/******************************************************************************/

/*
 * This function gets a pointer to an iterator, given the iterator handle.
 *
 * Arguments:
 * ihandle = The iterator handle.
 *
 * Results:
 * If the handle is valid, this function returns a pointer to the iterator.
 * If the handle is invalid (i.e. there is no such table), this function
 *    returns NULL.
 */
static
  struct iterator *get_iterator_ptr(int ihandle)
{
  return ((ihandle >= 0) && (ihandle < N_ip_array))
         ? (struct iterator *) ip_array[ihandle]          /* valid handle */
         : NULL;                                          /* invalid handle */
}

/******************************************************************************/

/*
 * This function grows an malloc-allocated array of  void *  pointers
 * via realloc(), initializing the new space to NULL pointers.
 *
 * Arguments:
 * *pN = (in out) array size
 * *pvp_array = (in out) Pointer to growable array of  void *  pointers.
 *
 * Results:
 * This function returns
 *      0 for ok,
 *      -ve for error, including
 *      UTIL_ERROR_NO_MEMORY            can't allocate memory to grow table
 */
static
  int grow_pointer_array(int *pN, void ***pvp_array)
{
const int N = *pN;
  void **vp_array = *pvp_array;
  const int new_N = GROW(N);
  void **new_vp_array = realloc(vp_array, new_N*sizeof(void *));
  if (new_vp_array == NULL)
  {
    return UTIL_ERROR_NO_MEMORY;                          /* can't grow array */
  }

  /* initialize the new space to NULL pointers */
    {
  int i;
  for (i = N ; i < new_N ; ++i)
  {
    new_vp_array[i] = NULL;
  }
    }

  *pvp_array = new_vp_array;
  *pN = new_N;
  return 0;                               /* ok */
}

/******************************************************************************/

/*
 * This function converts the given string into a scalar value of type
 * CCTK_INT or CCTK_REAL.
 *
 * Arguments:
 * string = (in) null-terminated string to be converted into a number
 * scalar = (out) structure defining the type and value of the number
 */
static
  void convert_string_to_number(const char *string, struct scalar_value *scalar)
{
  long int int_scalar;
  double real_scalar;
  char *endptr;


  scalar->datatype = -1;

  if (*string)
  {
    int_scalar = strtol (string, &endptr, 10);
    if (*endptr == 0 && int_scalar != LONG_MIN && int_scalar != LONG_MAX)
    {
      scalar->datatype = CCTK_VARIABLE_INT;
      scalar->value.int_scalar = int_scalar;
    }
    else
    {
      real_scalar = strtod (string, &endptr);
      if (*endptr == 0 && real_scalar != +HUGE_VAL && real_scalar != -HUGE_VAL)
      {
        scalar->datatype = CCTK_VARIABLE_REAL;
        scalar->value.real_scalar = real_scalar;
      }
    }
  }
}

/******************************************************************************/
/***** Table and Iterator Dump Routines ***************************************/
/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
* This function prints out all the tables and their data structures.
*/
static
void print_all_tables(void)
{
  int handle;

  printf("N_tables=%d N_thp_array=%d\n", N_tables, N_thp_array);
  for (handle = 0 ; handle < N_thp_array ; ++handle)
  {
    print_table(handle);
  }
}
#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function prints out a table.
 */
static
  void print_table(int handle)
{
  printf("thp_array[%d]: ", handle);
    {
  const struct table_header *const thp = get_table_header_ptr(handle);
  if (thp == NULL)
  {
    printf("NULL\n");
  }
  else
  {
    printf("flags=0x%x handle=%d\n", thp->flags, thp->handle);
      {
    const struct table_entry *tep = thp->head;
    for ( ; tep != NULL ; tep = tep->next)
    {
      printf("    [tep=%p]\n", (const void *) tep);
      printf("\tkey=\"%s\"\n", tep->key);
      printf("\ttype_code=%d N_elements=%d\n", tep->type_code, tep->N_elements);
        {
      int i;
      switch  (tep->type_code)
      {
        case CCTK_VARIABLE_INT:
          printf("\t[int]");
            {
          const CCTK_INT *const value_ptr_int = (const CCTK_INT *) tep->value;
          for (i = 0 ; i < tep->N_elements ; ++i)
          {
            printf("\t%d", (int) value_ptr_int[i]);
          }
            }
          break;
        case CCTK_VARIABLE_REAL:
          printf("\t[real]");
            {
          const CCTK_REAL *const value_ptr_real
                  = (const CCTK_REAL *) tep->value;
          for (i = 0 ; i < tep->N_elements ; ++i)
          {
            printf("\t%g", (double) value_ptr_real[i]);
          }
            }
          break;
        case CCTK_VARIABLE_COMPLEX:
          printf("\t[complex]");
            {
          const CCTK_COMPLEX *const value_ptr_complex
                  = (const CCTK_COMPLEX *) tep->value;
          for (i = 0 ; i < tep->N_elements ; ++i)
          {
            printf("\t(%g,%g)",
                     (double) value_ptr_complex[i].Re,
                     (double) value_ptr_complex[i].Im);
          }
            }
          break;
        default:
          printf("\t[sorry, don't know how to print this type!]");
          break;
      }
      printf("\n");
        }
    }
      }
  }
    }
}
#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function prints out all the iterators and their data structures.
 */
static
  void print_all_iterators(void)
{
  int ihandle;

  printf("N_iterators=%d N_ip_array=%d\n", N_iterators, N_ip_array);
  for (ihandle = 0 ; ihandle < N_ip_array ; ++ihandle)
  {
    const struct iterator *const ip = get_iterator_ptr(ihandle);
    printf("ip_array[%d]: ", ihandle);
    if (ip == NULL)
    {
      printf("NULL\n");
    }
    else
    {
      printf("thp=%p tep=%p\n", (const void *) ip->thp, (const void *) ip->tep);
    }
  }
}
#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/
/***** Standalone Test Driver *************************************************/
/******************************************************************************/

#ifdef UTIL_TABLE_TEST

/*
 * low-level macros to test set/get of scalars of various types
 */

#define CHECK_SET_GET_INT(handle, type,                                 \
                          key_already_exists, case_insensitive,         \
                          set_fn, get_fn)                               \
    {                                                                   \
  type x = 42;                                                          \
  assert( set_fn(handle, x, "int_x") == key_already_exists );           \
  x = 1;                                                                \
  assert( get_fn(handle, &x, "int_x") == 1 );                           \
  assert( x == 42 );                                                    \
  if (case_insensitive)                                                 \
  {                                                                     \
    x = 2;                                                              \
    assert( get_fn(handle, &x, "Int_X") == 1 );                         \
    assert( x == 42 );                                                  \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    assert( get_fn(handle, &x, "Int_X")                                 \
            == UTIL_ERROR_TABLE_NO_SUCH_KEY );                          \
  }                                                                     \
    }                                                           /* end macro */

/**************************************/

#define CHECK_SET_GET_GENERIC_INT(handle, type_code, type,              \
                                  key_already_exists, case_insensitive) \
    {                                                                   \
  type gx = 42;                                                         \
  assert( Util_TableSetGeneric(handle,                                  \
                               type_code, (const void *) &gx,           \
                               "gint_x") == key_already_exists );       \
  gx = 1;                                                               \
  assert( Util_TableGetGeneric(handle,                                  \
                               type_code, (void *) &gx,                 \
                               "gint_x") == 1 );                        \
  assert( gx == 42 );                                                   \
  if (case_insensitive)                                                 \
  {                                                                     \
    gx = 2;                                                             \
    assert( Util_TableGetGeneric(handle,                                \
                                 type_code, (void *) &gx,               \
                                 "GInt_X") == 1 );                      \
    assert( gx == 42 );                                                 \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    assert( Util_TableGetGeneric(handle,                                \
                                 type_code, (void *) &gx,               \
                                 "GInt_X")                              \
            == UTIL_ERROR_TABLE_NO_SUCH_KEY );                          \
  }                                                                     \
    }                                                           /* end macro */

/**************************************/

#define CHECK_SET_GET_REAL(handle, type,                                \
                           key_already_exists, case_insensitive,        \
                           set_fn, get_fn)                              \
    {                                                                   \
  type y = 42.25;                                                       \
  assert( set_fn(handle, y, "REAL_y") == key_already_exists );          \
  y = 1.25;                                                             \
  assert( get_fn(handle, &y, "REAL_y") == 1 );                          \
  assert( y == 42.25 );                                                 \
  if (case_insensitive)                                                 \
  {                                                                     \
    y = 1.5;                                                            \
    assert( get_fn(handle, &y, "real_y") == 1 );                        \
    assert( y == 42.25 );                                               \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    assert( get_fn(handle, &y, "real_y")                                \
            == UTIL_ERROR_TABLE_NO_SUCH_KEY );                          \
  }                                                                     \
    }                                                           /* end macro */

/**************************************/

#define CHECK_SET_GET_COMPLEX(handle, type,                             \
                              key_already_exists, case_insensitive,     \
                              set_fn, get_fn)                           \
    {                                                                   \
  static type z = { 42.25, 105.5 };                                     \
  assert( set_fn(handle, z, "COMPlex_Z") == key_already_exists );       \
  z.Re = 1.25;            z.Im = -2.78;                                 \
  assert( get_fn(handle, &z, "COMPlex_Z") == 1 );                       \
  assert( z.Re == 42.25 );                                              \
  assert( z.Im == 105.5 );                                              \
  if (case_insensitive)                                                 \
  {                                                                     \
    z.Re = 1.5;             z.Im = -2.83;                               \
    assert( get_fn(handle, &z, "COMPLEX_Z") == 1 );                     \
    assert( z.Re == 42.25 );                                            \
    assert( z.Im == 105.5 );                                            \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    assert( get_fn(handle, &z, "COMPLEX_Z")                             \
            == UTIL_ERROR_TABLE_NO_SUCH_KEY );                          \
  }                                                                     \
    }                                                           /* end macro */

/******************************************************************************/

/*
 * low-level macros to test set/get of arrays of various types
 */

#define CHECK_SET_GET_INT_ARRAY(handle, type, key_already_exists,       \
                                set_fn, get_fn)                         \
    {                                                                   \
  static type xx[5] = { 41, 42, 48, 45, 47 };                           \
  assert( set_fn(handle, 3, xx, "xx") == key_already_exists );          \
  xx[0] = 14;  xx[1] = 15;  xx[2] = 16;  xx[3] = 17;  xx[4] = 19;       \
  /* try to get 4 values, but only 3 were stored ==> only get 3 */      \
  assert( get_fn(handle, 4, xx, "xx") == 3 );                           \
  assert( xx[0] == 41 );                                                \
  assert( xx[1] == 42 );                                                \
  assert( xx[2] == 48 );                                                \
  assert( xx[3] == 17 );                                                \
  assert( xx[4] == 19 );                                                \
    }                                                           /* end macro */

/**************************************/

#define CHECK_SET_GET_REAL_ARRAY(handle, type, key_already_exists,      \
                                 set_fn, get_fn)                        \
    {                                                                   \
  static type yy[5] = { 41.25, 42.5, 48.0, 45.75, 47.125 };             \
  assert( set_fn(handle, 4, yy, "yy") == key_already_exists );          \
  yy[0] = 14.0;  yy[1] = 15.5;  yy[2] = 16.0;                           \
  yy[3] = 17.5;  yy[4] = 19.5;                                          \
  /* only get 3 of 4 stored values */                                   \
  assert( get_fn(handle, 3, yy, "yy") == 4 );                           \
  assert( yy[0] == 41.25 );                                             \
  assert( yy[1] == 42.5 );                                              \
  assert( yy[2] == 48.0 );                                              \
  assert( yy[3] == 17.5 );                                              \
  assert( yy[4] == 19.5 );                                              \
    }                                                           /* end macro */

/**************************************/

#define CHECK_SET_GET_GENERIC_REAL_ARRAY(handle, type_code, type,       \
                                         key_already_exists)            \
    {                                                                   \
  static type gyy[5] = { 41.25, 42.5, 48.0, 45.75, 47.125 };            \
  assert( Util_TableSetGenericArray(handle,                             \
                                    type_code, 4, (const void *) gyy,   \
                                    "gyy") == key_already_exists );     \
  gyy[0] = 14.0;  gyy[1] = 15.5;  gyy[2] = 16.0;                        \
  gyy[3] = 17.5;  gyy[4] = 19.5;                                        \
  /* only get 3 of 4 stored values */                                   \
  assert( Util_TableGetGenericArray(handle,                             \
                                    type_code, 3, gyy,                  \
                                    "gyy") == 4 );                      \
  assert( gyy[0] == 41.25 );                                            \
  assert( gyy[1] == 42.5 );                                             \
  assert( gyy[2] == 48.0 );                                             \
  assert( gyy[3] == 17.5 );                                             \
  assert( gyy[4] == 19.5 );                                             \
    }                                                           /* end macro */

/**************************************/

#define CHECK_SET_GET_COMPLEX_ARRAY(handle, type, key_already_exists,   \
                                    set_fn, get_fn)                     \
    {                                                                   \
  static type zz[5]                                                     \
          = { {3.5,1.25}, {9.5,4.5}, {0.5,8.0}, {5.0,5.5}, {4.5,7.25} };\
  assert( set_fn(handle, 4, zz, "zz") == key_already_exists );          \
  zz[0].Re = 10.25;       zz[0].Im = 11.75;                             \
  zz[1].Re = -2.5;        zz[1].Im = 3.5;                               \
  zz[2].Re = 14.0;        zz[2].Im = -8.5;                              \
  zz[3].Re = 0.25;        zz[3].Im = 8.875;                             \
  zz[4].Re = -0.25;       zz[4].Im = -0.75;                             \
  /* only get 3 of 4 stored values */                                   \
  assert( get_fn(handle, 3, zz, "zz") == 4 );                           \
  assert( zz[0].Re == 3.5 );      assert( zz[0].Im == 1.25 );           \
  assert( zz[1].Re == 9.5 );      assert( zz[1].Im == 4.5 );            \
  assert( zz[2].Re == 0.5 );      assert( zz[2].Im == 8.0 );            \
  assert( zz[3].Re == 0.25 );     assert( zz[3].Im == 8.875 );          \
  assert( zz[4].Re == -0.25 );    assert( zz[4].Im == -0.75 );          \
    }                                                           /* end macro */

#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This program is a standalone test driver for the key/value table system.
 */
int main(void)
{
  test_nonexistent_tables();
  test_table_create_destroy();

    {
  const int handle = Util_TableCreate(UTIL_TABLE_FLAGS_DEFAULT);
  assert( handle == 0 );
  assert( Util_TableSetInt(handle, 42, "foo/") == UTIL_ERROR_TABLE_BAD_KEY );

    {
  const int HANDLE = Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE);

  #ifdef UTIL_TABLE_DEBUG
  printf("--- printing handle=%d table (should be empty)\n", handle);
  print_table(handle);
  printf("--- about to test set/get on handle=%d table\n", handle);
  #endif
  test_set_get(handle, false);
  test_set_get(HANDLE, true);

  test_iterators(handle);
  test_delete_table_entry(handle, false);
  test_iterators(HANDLE);
  test_delete_table_entry(HANDLE, true);

  test_set_get_array(handle);
  test_set_get_array(HANDLE);

  test_set_get_string(handle, false);
  test_set_get_pointers(handle);
  test_set_get_fpointers(handle);

    {
  const int HANDLE2 = test_set_create_from_string();
  test_clone(HANDLE2);
  test_set_get_string(HANDLE2, true);

  printf("all ok!\n" );
  return 0;
    }
    }
    }
}
#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function tests that various operations on nonexistent tables
 * and iterators give error returns.
 */
static
  void test_nonexistent_tables(void)
{
  assert( Util_TableDestroy(42) == UTIL_ERROR_BAD_HANDLE );
  assert( Util_TableQueryFlags(-42) == UTIL_ERROR_BAD_HANDLE );
  assert( Util_TableQueryNKeys(0) == UTIL_ERROR_BAD_HANDLE );
  assert( Util_TableDeleteKey(-1, "pickle") == UTIL_ERROR_BAD_HANDLE );
  assert( Util_TableSetInt(-1, 42, "fourty-two") == UTIL_ERROR_BAD_HANDLE );
  assert( Util_TableGetReal(-1, NULL, "something wierd")
          == UTIL_ERROR_BAD_HANDLE );

  assert( Util_TableItCreate(42) == UTIL_ERROR_BAD_HANDLE );
  assert( Util_TableItDestroy(42) == UTIL_ERROR_BAD_HANDLE );
  assert( Util_TableItQueryIsNull(42) == UTIL_ERROR_BAD_HANDLE );
  assert( Util_TableItQueryIsNonNull(42) == UTIL_ERROR_BAD_HANDLE );
  assert( Util_TableItQueryTableHandle(42) == UTIL_ERROR_BAD_HANDLE );
  assert( Util_TableItQueryKeyValueInfo(42,
                                        0, NULL,
                                        NULL, NULL) == UTIL_ERROR_BAD_HANDLE );
  assert( Util_TableItAdvance(42) == UTIL_ERROR_BAD_HANDLE );
  assert( Util_TableItResetToStart(42) == UTIL_ERROR_BAD_HANDLE );
  assert( Util_TableItSetToNull(42) == UTIL_ERROR_BAD_HANDLE );
}
#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function tests creation and destruction of tables.
 * It also tests
 * - querying flags words
 * - querying NKeys and MaxKeyLength for empty tables
 * - deleting keys from empty tables
 *
 * It assumes that no tables exist when the function is called,
 * and it eventually destroys all the tables it creates.
 *
 * Bugs:
 * Parts of this test are tied to the present implementation -- it
 * uses local variables of the implementation, and it assumes a specific
 * strategy for allocating handles.
 */
static
  void test_table_create_destroy(void)
{
  assert( N_tables == 0 );

  assert( Util_TableCreate(UTIL_TABLE_FLAGS_DEFAULT) == 0 );
  assert( N_tables == 1 );
  assert( Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE) == 1 );
  assert( N_tables == 2 );
  assert( Util_TableCreate(UTIL_TABLE_FLAGS_DEFAULT) == 2 );
  assert( N_tables == 3 );
  assert( Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE) == 3 );
  assert( N_tables == 4 );
  assert( get_table_header_ptr(0) != NULL );
  assert( get_table_header_ptr(1) != NULL );
  assert( get_table_header_ptr(2) != NULL );
  assert( get_table_header_ptr(3) != NULL );
  assert( Util_TableQueryFlags(0) == UTIL_TABLE_FLAGS_DEFAULT );
  assert( Util_TableQueryFlags(1) == UTIL_TABLE_FLAGS_CASE_INSENSITIVE );
  assert( Util_TableQueryFlags(2) == UTIL_TABLE_FLAGS_DEFAULT );
  assert( Util_TableQueryFlags(3) == UTIL_TABLE_FLAGS_CASE_INSENSITIVE );

  assert( Util_TableDeleteKey(3, "pickle") == UTIL_ERROR_TABLE_NO_SUCH_KEY );
  assert( Util_TableDeleteKey(3, "Pickle") == UTIL_ERROR_TABLE_NO_SUCH_KEY );
  assert( Util_TableDeleteKey(3, "PICKLE") == UTIL_ERROR_TABLE_NO_SUCH_KEY );

  assert( Util_TableDestroy(2) == 0 );
  assert( N_tables == 3 );
  assert( get_table_header_ptr(0) != NULL );
  assert( get_table_header_ptr(1) != NULL );
  assert( get_table_header_ptr(2) == NULL );
  assert( get_table_header_ptr(3) != NULL );

  assert( Util_TableCreate(0x43) == 2 );
  assert( N_tables == 4 );
  assert( get_table_header_ptr(0) != NULL );
  assert( get_table_header_ptr(1) != NULL );
  assert( get_table_header_ptr(2) != NULL );
  assert( Util_TableQueryFlags(2) == 0x43);
  assert( get_table_header_ptr(3) != NULL );

  assert( Util_TableDestroy(1) == 0 );
  assert( N_tables == 3 );
  assert( get_table_header_ptr(0) != NULL );
  assert( Util_TableQueryNKeys(0) == 0 );
  assert( get_table_header_ptr(1) == NULL );
  assert( Util_TableQueryMaxKeyLength(1) == UTIL_ERROR_BAD_HANDLE );
  assert( get_table_header_ptr(2) != NULL );
  assert( Util_TableQueryMaxKeyLength(2) == 0 );
  assert( get_table_header_ptr(3) != NULL );
  assert( Util_TableDeleteKey(3, "pickle") == UTIL_ERROR_TABLE_NO_SUCH_KEY );

  assert( Util_TableDestroy(1) == UTIL_ERROR_BAD_HANDLE );
  assert( N_tables == 3 );
  assert( get_table_header_ptr(0) != NULL );
  assert( get_table_header_ptr(1) == NULL );
  assert( get_table_header_ptr(2) != NULL );
  assert( get_table_header_ptr(3) != NULL );

  assert( Util_TableDestroy(0) == 0 );
  assert( Util_TableDestroy(2) == 0 );
  assert( Util_TableDestroy(3) == 0 );

  assert( N_tables == 0 );
}
#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function tests set/get of various-typed scalars.
 * It also tests querying NKeys, MaxKeyLength, and some keys.
 *
 * It assumes the table is empty when this function is called;
 * it leaves entries in the table.
 */
static
  void test_set_get(int handle, bool case_insensitive)
{
  /*
   * Note we put a test of a type that's guaranteed to be defined...
   * - at the *beginning* of each group of tests, so we can properly
   *   assert whether or not the key was already in table beforehand.
   * - at the *end* of each group of tests, so the final table contents
   *   are known independently of which types are and aren't defined.
   */

  /* integers */
  CHECK_SET_GET_INT(handle, CCTK_INT, 0, case_insensitive,
                    Util_TableSetInt, Util_TableGetInt);
  #ifdef CCTK_INT1
  CHECK_SET_GET_INT(handle, CCTK_INT1, 1, case_insensitive,
                    Util_TableSetInt1, Util_TableGetInt1);
  #endif
  #ifdef CCTK_INT2
  CHECK_SET_GET_INT(handle, CCTK_INT2, 1, case_insensitive,
                    Util_TableSetInt2, Util_TableGetInt2);
  #endif
  #ifdef CCTK_INT4
  CHECK_SET_GET_INT(handle, CCTK_INT4, 1, case_insensitive,
                    Util_TableSetInt4, Util_TableGetInt4);
  #endif
  #ifdef CCTK_INT8
  CHECK_SET_GET_INT(handle, CCTK_INT8, 1, case_insensitive,
                    Util_TableSetInt8, Util_TableGetInt8);
  #endif
  CHECK_SET_GET_INT(handle, CCTK_INT, 1, case_insensitive,
                    Util_TableSetInt, Util_TableGetInt);
  assert( Util_TableQueryNKeys(handle) == 1 );
  assert( Util_TableQueryMaxKeyLength(handle) == (int)strlen("int_x") );

  /* generic scalars which are actually integers */
  CHECK_SET_GET_GENERIC_INT(handle, CCTK_VARIABLE_INT, CCTK_INT,
                            0, case_insensitive);
  #ifdef CCTK_INT1
  CHECK_SET_GET_GENERIC_INT(handle, CCTK_VARIABLE_INT1, CCTK_INT1,
                            1, case_insensitive);
  #endif
  #ifdef CCTK_INT2
  CHECK_SET_GET_GENERIC_INT(handle, CCTK_VARIABLE_INT2, CCTK_INT2,
                            1, case_insensitive);
  #endif
  #ifdef CCTK_INT4
  CHECK_SET_GET_GENERIC_INT(handle, CCTK_VARIABLE_INT4, CCTK_INT4,
                            1, case_insensitive);
  #endif
  #ifdef CCTK_INT8
  CHECK_SET_GET_GENERIC_INT(handle, CCTK_VARIABLE_INT8, CCTK_INT8,
                            1, case_insensitive);
  #endif
  CHECK_SET_GET_GENERIC_INT(handle, CCTK_VARIABLE_INT, CCTK_INT,
                            1, case_insensitive);
  assert( Util_TableQueryNKeys(handle) == 2 );
  assert( Util_TableQueryMaxKeyLength(handle) == (int)strlen("gint_x") );
  assert( Util_TableDeleteKey(handle, "gint_x") == 0 );
  assert( Util_TableQueryNKeys(handle) == 1 );

  /* complex numbers */
  CHECK_SET_GET_COMPLEX(handle, CCTK_COMPLEX, 0, case_insensitive,
                        Util_TableSetComplex, Util_TableGetComplex);
  #ifdef CCTK_REAL4
  CHECK_SET_GET_COMPLEX(handle, CCTK_COMPLEX8, 1, case_insensitive,
                        Util_TableSetComplex8, Util_TableGetComplex8);
  #endif
  #ifdef CCTK_REAL8
  CHECK_SET_GET_COMPLEX(handle, CCTK_COMPLEX16, 1, case_insensitive,
                        Util_TableSetComplex16, Util_TableGetComplex16);
  #endif
  #ifdef CCTK_REAL16
  CHECK_SET_GET_COMPLEX(handle, CCTK_COMPLEX32, 1, case_insensitive,
                        Util_TableSetComplex32, Util_TableGetComplex32);
  #endif
  CHECK_SET_GET_COMPLEX(handle, CCTK_COMPLEX, 1, case_insensitive,
                        Util_TableSetComplex, Util_TableGetComplex);
  assert( Util_TableQueryNKeys(handle) == 2 );
  assert( Util_TableQueryMaxKeyLength(handle) == (int)strlen("COMPlex_Z") );

  /* reals */
  CHECK_SET_GET_REAL(handle, CCTK_REAL, 0, case_insensitive,
                     Util_TableSetReal, Util_TableGetReal);
  #ifdef CCTK_REAL4
  CHECK_SET_GET_REAL(handle, CCTK_REAL4, 1, case_insensitive,
                     Util_TableSetReal4, Util_TableGetReal4);
  #endif
  #ifdef CCTK_REAL8
  CHECK_SET_GET_REAL(handle, CCTK_REAL8, 1, case_insensitive,
                     Util_TableSetReal8, Util_TableGetReal8);
  #endif
  #ifdef CCTK_REAL16
  CHECK_SET_GET_REAL(handle, CCTK_REAL16, 1, case_insensitive,
                     Util_TableSetReal16, Util_TableGetReal16);
  #endif
  CHECK_SET_GET_REAL(handle, CCTK_REAL, 1, case_insensitive,
                     Util_TableSetReal, Util_TableGetReal);
  assert( Util_TableQueryNKeys(handle) == 3 );
  assert( Util_TableQueryMaxKeyLength(handle) == (int)strlen("COMPlex_Z") );

    {
  CCTK_INT type_code, N_elements;
  assert( Util_TableQueryValueInfo(handle, &type_code, &N_elements, "COMPlex_Z")
          == 1 );
  assert( type_code == CCTK_VARIABLE_COMPLEX );
  assert( N_elements == 1 );

  assert( Util_TableQueryValueInfo(handle, &type_code, &N_elements, "pickle")
          == 0 );

  assert( Util_TableQueryValueInfo(handle, NULL, NULL, "int_x") == 1 );
  assert( Util_TableQueryValueInfo(handle, NULL, NULL, "Int_x")
          == (case_insensitive ? 1 : 0) );
  assert( Util_TableQueryValueInfo(handle, NULL, NULL, "real_y")
          == (case_insensitive ? 1 : 0) );
  assert( Util_TableQueryValueInfo(handle, NULL, NULL, "COMPLEX_Z")
          == (case_insensitive ? 1 : 0) );
    }
}
#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function tests set/get of various-typed arrays.
 *
 * It assumes the table is empty when this function is called;
 * it leaves entries in the table.
 */
static
  void test_set_get_array(int handle)
{
  /* the comments of  test_set_get()  about test ordering, also apply here */

  /* integers */
  CHECK_SET_GET_INT_ARRAY(handle, CCTK_CHAR, 0,
                          Util_TableSetCharArray, Util_TableGetCharArray);
  CHECK_SET_GET_INT_ARRAY(handle, CCTK_INT, 1,
                          Util_TableSetIntArray, Util_TableGetIntArray);
  #ifdef CCTK_INT1
  CHECK_SET_GET_INT_ARRAY(handle, CCTK_INT1, 1,
                          Util_TableSetInt1Array, Util_TableGetInt1Array);
  #endif
  #ifdef CCTK_INT2
  CHECK_SET_GET_INT_ARRAY(handle, CCTK_INT2, 1,
                          Util_TableSetInt2Array, Util_TableGetInt2Array);
  #endif
  #ifdef CCTK_INT4
  CHECK_SET_GET_INT_ARRAY(handle, CCTK_INT4, 1,
                          Util_TableSetInt4Array, Util_TableGetInt4Array);
  #endif
  #ifdef CCTK_INT8
  CHECK_SET_GET_INT_ARRAY(handle, CCTK_INT8, 1,
                          Util_TableSetInt8Array, Util_TableGetInt8Array);
  #endif
  CHECK_SET_GET_INT_ARRAY(handle, CCTK_INT, 1,
                          Util_TableSetIntArray, Util_TableGetIntArray);

  /* reals */
  CHECK_SET_GET_REAL_ARRAY(handle, CCTK_REAL, 0,
                           Util_TableSetRealArray, Util_TableGetRealArray);
  #ifdef CCTK_REAL4
  CHECK_SET_GET_REAL_ARRAY(handle, CCTK_REAL4, 1,
                           Util_TableSetReal4Array, Util_TableGetReal4Array);
  #endif
  #ifdef CCTK_REAL8
  CHECK_SET_GET_REAL_ARRAY(handle, CCTK_REAL8, 1,
                           Util_TableSetReal8Array, Util_TableGetReal8Array);
  #endif
  #ifdef CCTK_REAL16
  CHECK_SET_GET_REAL_ARRAY(handle, CCTK_REAL16, 1,
                           Util_TableSetReal16Array, Util_TableGetReal16Array);
  #endif
  CHECK_SET_GET_REAL_ARRAY(handle, CCTK_REAL, 1,
                           Util_TableSetRealArray, Util_TableGetRealArray);

  /* generic arrays which are actually reals */
  CHECK_SET_GET_GENERIC_REAL_ARRAY(handle, CCTK_VARIABLE_REAL, CCTK_REAL, 0);
  #ifdef CCTK_REAL4
  CHECK_SET_GET_GENERIC_REAL_ARRAY(handle, CCTK_VARIABLE_REAL4, CCTK_REAL4, 1);
  #endif
  #ifdef CCTK_REAL8
  CHECK_SET_GET_GENERIC_REAL_ARRAY(handle, CCTK_VARIABLE_REAL8, CCTK_REAL8, 1);
  #endif
  #ifdef CCTK_REAL16
  CHECK_SET_GET_GENERIC_REAL_ARRAY(handle,
                                   CCTK_VARIABLE_REAL16, CCTK_REAL16, 1);
  #endif
  CHECK_SET_GET_GENERIC_REAL_ARRAY(handle, CCTK_VARIABLE_REAL, CCTK_REAL, 1);

  /* complex numbers */
  CHECK_SET_GET_COMPLEX_ARRAY(handle, CCTK_COMPLEX, 0,
                              Util_TableSetComplexArray,
                              Util_TableGetComplexArray);
  #ifdef CCTK_REAL4
  CHECK_SET_GET_COMPLEX_ARRAY(handle, CCTK_COMPLEX8, 1,
                              Util_TableSetComplex8Array,
                              Util_TableGetComplex8Array);
  #endif
  #ifdef CCTK_REAL8
  CHECK_SET_GET_COMPLEX_ARRAY(handle, CCTK_COMPLEX16, 1,
                              Util_TableSetComplex16Array,
                              Util_TableGetComplex16Array);
  #endif
  #ifdef CCTK_REAL16
  CHECK_SET_GET_COMPLEX_ARRAY(handle, CCTK_COMPLEX32, 1,
                              Util_TableSetComplex32Array,
                              Util_TableGetComplex32Array);
  #endif
  CHECK_SET_GET_COMPLEX_ARRAY(handle, CCTK_COMPLEX, 1,
                              Util_TableSetComplexArray,
                              Util_TableGetComplexArray);
}
#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function tests iterating through a table and resetting an iterator.
 * It assumes the initial table contents are those generated by
 * test_get_set() , namely 3 keys "REAL_y", "COMPlex_Z", "int_x".
 *
 * Bugs:
 * This test is tied to the present implementation -- it assumes a
 * specific ordering of table elements returned by an iterator.
 */
static
  void test_iterators(int handle)
{
  const int ihandle = Util_TableItCreate(handle);
  assert( ihandle >= 0 );
  assert( Util_TableItQueryTableHandle(ihandle) == handle );
  assert( Util_TableItQueryIsNonNull(ihandle) == 1);
  assert( Util_TableItQueryIsNull(ihandle) == 0);

  /* set up the key buffer */
    {
  const int max_key_length = Util_TableQueryMaxKeyLength(handle);
  assert( max_key_length == (int)strlen("COMPlex_Z") );
    {
  const int N_key_buffer = max_key_length + 1;
  char *const key_buffer = malloc(N_key_buffer);
  assert( key_buffer != NULL );

  /* walk the table to verify iterator traversal */
    {
  CCTK_INT type_code, N_elements;

  /* REAL_y */
  assert( Util_TableItQueryKeyValueInfo(ihandle,
                                        N_key_buffer, key_buffer,
                                        &type_code, &N_elements)
          == (int)strlen("REAL_y") );
  assert( strcmp(key_buffer, "REAL_y") == 0 );
  assert( type_code == CCTK_VARIABLE_REAL );
  assert( N_elements == 1 );

  assert( Util_TableItAdvance(ihandle) == 1 );

  /* COMPlex_Z */
  type_code = 123456;
  N_elements = 54321;
  assert( Util_TableItQueryKeyValueInfo(ihandle,
                                        N_key_buffer, key_buffer,
                                        &type_code, &N_elements)
          == (int)strlen("COMPlex_Z") );
  assert( strcmp(key_buffer, "COMPlex_Z") == 0 );
  assert( type_code == CCTK_VARIABLE_COMPLEX );
  assert( N_elements == 1 );

  assert( Util_TableItAdvance(ihandle) == 1 );

  /* clone the iterator and check the clone */
    {
  const int clone_ihandle = Util_TableItClone(ihandle);
  type_code = 123456;
  N_elements = 54321;
  assert( Util_TableItQueryKeyValueInfo(clone_ihandle,
                                        N_key_buffer, key_buffer,
                                        &type_code, &N_elements)
          == (int)strlen("int_x") );
  assert( strcmp(key_buffer, "int_x") == 0 );
  assert( type_code == CCTK_VARIABLE_INT );
  assert( N_elements == 1 );

  /* int_x */
  type_code = 123456;
  N_elements = 54321;
  assert( Util_TableItQueryKeyValueInfo(ihandle,
                                        N_key_buffer, key_buffer,
                                        &type_code, &N_elements)
          == (int)strlen("int_x") );
  assert( strcmp(key_buffer, "int_x") == 0 );
  assert( type_code == CCTK_VARIABLE_INT );
  assert( N_elements == 1 );

  /* advance past last table entry ==> "null-pointer" state */
  assert( Util_TableItAdvance(ihandle) == 0 );
  assert( Util_TableItQueryIsNull(ihandle) == 1);
  assert( Util_TableItQueryIsNonNull(ihandle) == 0);

  /* advance again ==> stays in "null-pointer" state */
  assert( Util_TableItAdvance(ihandle) == 0 );
  assert( Util_TableItQueryIsNull(ihandle) == 1);
  assert( Util_TableItQueryIsNonNull(ihandle) == 0);
  assert( Util_TableItQueryKeyValueInfo(ihandle,
                                        0, NULL,
                                        NULL, NULL)
          == UTIL_ERROR_TABLE_ITERATOR_IS_NULL );

  /* test reset to starting point */
  assert( Util_TableItResetToStart(ihandle) == 1 );
  assert( Util_TableItQueryIsNonNull(ihandle) == 1 );
  assert( Util_TableItQueryIsNull(ihandle) == 0 );

  /* COMPlex_Z */
  type_code = 123456;
  N_elements = 54321;
  assert( Util_TableItAdvance(ihandle) == 1 );
  assert( Util_TableItQueryKeyValueInfo(ihandle,
                                        N_key_buffer, key_buffer,
                                        &type_code, &N_elements)
          == (int)strlen("COMPlex_Z") );
  assert( strcmp(key_buffer, "COMPlex_Z") == 0 );
  assert( type_code == CCTK_VARIABLE_COMPLEX );
  assert( N_elements == 1 );

  /* test reset to "null-pointer" state */
  assert( Util_TableItSetToNull(ihandle) == 0 );
  assert( Util_TableItQueryIsNull(ihandle) == 1);
  assert( Util_TableItQueryIsNonNull(ihandle) == 0);

  /* test set to key "REAL_y" */
  assert( Util_TableItSetToKey(ihandle, "REAL_y") == 0 );
  assert( Util_TableItQueryIsNonNull(ihandle) == 1);
  assert( Util_TableItQueryIsNull(ihandle) == 0);
  assert( Util_TableItQueryKeyValueInfo(ihandle,
                                        N_key_buffer, key_buffer,
                                        &type_code, &N_elements)
          == (int)strlen("REAL_y") );
  assert( strcmp(key_buffer, "REAL_y") == 0 );
  assert( type_code == CCTK_VARIABLE_REAL );
  assert( N_elements == 1 );

  assert( Util_TableItDestroy(ihandle) == 0 );
  assert( Util_TableItDestroy(clone_ihandle) == 0 );

  free(key_buffer);
    }
    }
    }
    }
}

/******************************************************************************/

/*
 * This function tests deleting table entries.
 * It assumes the initial table contents are those generated by
 * test_get_set() , namely 3 keys {"REAL_y", "COMPlex_Z", "int_x"}.
 *
 * Bugs:
 * This test is tied to the present implementation -- it assumes a
 * specific ordering of table elements returned by an iterator.
 */
static
  void test_delete_table_entry(int handle, bool case_insensitive)
{
  /* set up the key buffer */
  const int max_key_length = Util_TableQueryMaxKeyLength(handle);
  assert( max_key_length == (int)strlen("COMPlex_Z") );
    {
  const int N_key_buffer = max_key_length + 1;
  char *const key_buffer = malloc(N_key_buffer);
  assert( key_buffer != NULL );

  /*
   * delete the starting table entry "REAL_y"
   * (this is a special case in the implementation)
   */

  assert( Util_TableQueryNKeys(handle) == 3 );
  assert( Util_TableDeleteKey(handle,
                              case_insensitive ? "rEAL_y" : "REAL_y")
          == 0 );
  assert( Util_TableQueryNKeys(handle) == 2 );

  /* walk the table again to verify remaining keys {"COMPlex_Z", "int_x"} */
  assert( Util_TableQueryNKeys(handle) == 2 );
    {
  int ihandle = Util_TableItCreate(handle);
  assert( ihandle >= 0 );
  assert( Util_TableItQueryTableHandle(ihandle) == handle );
  assert( Util_TableItQueryIsNonNull(ihandle) == 1);
  assert( Util_TableItQueryIsNull(ihandle) == 0);

  /* COMPlex_Z */
    {
  CCTK_INT type_code = 123456;
  CCTK_INT N_elements = 54321;
  assert( Util_TableItQueryKeyValueInfo(ihandle,
                                        N_key_buffer, key_buffer,
                                        &type_code, &N_elements)
          == (int)strlen("COMPlex_Z") );
  assert( strcmp(key_buffer, "COMPlex_Z") == 0 );
  assert( type_code == CCTK_VARIABLE_COMPLEX );
  assert( N_elements == 1 );

  /* int_x */
  type_code = 123456;
  N_elements = 54321;
  assert( Util_TableItAdvance(ihandle) == 1 );
  assert( Util_TableItQueryKeyValueInfo(ihandle,
                                        N_key_buffer, key_buffer,
                                        &type_code, &N_elements)
          == (int)strlen("int_x") );
  assert( strcmp(key_buffer, "int_x") == 0 );
  assert( type_code == CCTK_VARIABLE_INT );
  assert( N_elements == 1 );

  /* advance past last table entry ==> "null-pointer" state */
  assert( Util_TableItAdvance(ihandle) == 0 );
  assert( Util_TableItQueryIsNull(ihandle) == 1);
  assert( Util_TableItQueryIsNonNull(ihandle) == 0);

  /* delete the last key "int_x" */
  assert( Util_TableDeleteKey(handle,
                              case_insensitive ? "INT_X" : "int_x")
          == 0 );

  /* walk the table again to verify remaining key {"COMPlex_Z"} */
  assert( Util_TableQueryNKeys(handle) == 1 );
    {
  int ihandle2 = Util_TableItCreate(handle);
  assert( ihandle2 >= 0 );
  assert( Util_TableItQueryTableHandle(ihandle2) == handle );
  assert( Util_TableItQueryIsNonNull(ihandle2) == 1);
  assert( Util_TableItQueryIsNull(ihandle2) == 0);

  /* COMPlex_Z */
  type_code = 123456;
  N_elements = 54321;
  assert( Util_TableItQueryKeyValueInfo(ihandle2,
                                        N_key_buffer, key_buffer,
                                        &type_code, &N_elements)
          == (int)strlen("COMPlex_Z") );
  assert( strcmp(key_buffer, "COMPlex_Z") == 0 );
  assert( type_code == CCTK_VARIABLE_COMPLEX );
  assert( N_elements == 1 );

  /* advance past last table entry ==> "null-pointer" state */
  assert( Util_TableItAdvance(ihandle2) == 0 );
  assert( Util_TableItQueryIsNull(ihandle2) == 1);
  assert( Util_TableItQueryIsNonNull(ihandle2) == 0);

  /* delete the last key "COMPlex_Z" */
  assert( Util_TableQueryNKeys(handle) == 1 );
  assert( Util_TableDeleteKey(handle,
                              case_insensitive ? "INT_X" : "int_x")
          == UTIL_ERROR_TABLE_NO_SUCH_KEY );
  assert( Util_TableQueryNKeys(handle) == 1 );
  assert( Util_TableDeleteKey(handle,
                              case_insensitive ? "compLEX_z" : "COMPlex_Z")
          == 0 );
  assert( Util_TableQueryNKeys(handle) == 0 );

    {
  /* check that table is indeed now empty */
  int ihandle3 = Util_TableItCreate(handle);
  assert( ihandle3 >= 0 );
  assert( Util_TableItQueryIsNull(ihandle3) == 1);
  assert( Util_TableItQueryIsNonNull(ihandle3) == 0);

  /* clean up our iterators */
  assert( Util_TableItDestroy(ihandle2) == 0 );
  assert( Util_TableItDestroy(42) == UTIL_ERROR_BAD_HANDLE );
  assert( Util_TableItDestroy(ihandle3) == 0 );
  assert( Util_TableItDestroy(ihandle) == 0 );
  free(key_buffer);
    }
    }
    }
    }
    }
}
#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function tests
 *      Util_TableSetFromString()
 *      Util_TableCreateFromString()
 * It returns the handle of one of the newly-created tables.
 *
 * Bugs:
 * This test is tied to the present implementation -- it assumes a
 * specific ordering of table elements returned by an iterator.
 */
static
  int test_set_create_from_string(void)
{
  /*
   * Test an empty string
   */
  const int handle = Util_TableCreateFromString("");
  assert( Util_TableQueryNKeys(handle) == 0 );

  /*
   * Test some error cases
   */
  assert( Util_TableSetFromString(handle, "foo" ) == UTIL_ERROR_BAD_INPUT );
  assert( Util_TableSetFromString(handle, "foo/" ) == UTIL_ERROR_BAD_INPUT );

  assert( Util_TableCreateFromString("foo" ) == UTIL_ERROR_BAD_INPUT );
  assert( Util_TableCreateFromString("foo/" ) == UTIL_ERROR_BAD_INPUT );
  assert( Util_TableCreateFromString("foo=" ) == UTIL_ERROR_BAD_INPUT );
  assert( Util_TableCreateFromString("foo/=12" ) == UTIL_ERROR_TABLE_BAD_KEY );
  assert( Util_TableCreateFromString("foo=12;" ) == UTIL_ERROR_BAD_INPUT );
  assert( Util_TableCreateFromString("foo=12,0" ) == UTIL_ERROR_BAD_INPUT );
  assert( Util_TableCreateFromString("foo='" ) == UTIL_ERROR_BAD_INPUT );
  assert( Util_TableCreateFromString("foo=\"" ) == UTIL_ERROR_BAD_INPUT );
  assert( Util_TableCreateFromString("foo=\"'" ) == UTIL_ERROR_BAD_INPUT );
  assert( Util_TableCreateFromString("foo={" ) == UTIL_ERROR_BAD_INPUT );
  assert( Util_TableCreateFromString("foo=}" ) == UTIL_ERROR_BAD_INPUT );
  assert( Util_TableCreateFromString("foo={0" ) == UTIL_ERROR_BAD_INPUT );
  assert( Util_TableCreateFromString("foo=0}" ) == UTIL_ERROR_BAD_INPUT );
  assert( Util_TableCreateFromString("foo={bar}" ) == UTIL_ERROR_BAD_INPUT );
  assert( Util_TableCreateFromString(" foo = { \"\r\t\n\v\" } " ) ==
          UTIL_ERROR_BAD_INPUT );
  assert( Util_TableCreateFromString("foo={0 1.0}" ) ==
          UTIL_ERROR_TABLE_NO_MIXED_TYPE_ARRAY );

  /*
   * Test some "good" strings with single values
   */
    {
  CCTK_INT  int_value = 0;
  CCTK_REAL real_value = 0.0;
  CCTK_CHAR string_value[10] = "";
  CCTK_INT  int_array[4] = {0, 0, 0, 0};
  CCTK_REAL real_array[4] = {0.0, 0.0, 0.0, 0.0};
  CCTK_INT  type, nelems;

  assert( Util_TableSetFromString(handle, "foo=12" ) == 1 );
  assert( Util_TableSetFromString(handle, " foo=12" ) == 1 );
  assert( Util_TableSetFromString(handle, "foo =12" ) == 1 );
  assert( Util_TableSetFromString(handle, "foo= 12" ) == 1 );
  assert( Util_TableSetFromString(handle, "foo=12 " ) == 1 );
  assert( Util_TableSetFromString(handle, " foo = 12 " ) == 1 );
  assert( Util_TableSetFromString(handle, " foo = .0 " ) == 1 );
  assert( Util_TableSetFromString(handle, " foo = 12. " ) == 1 );

  assert( Util_TableSetFromString(handle, " foo = +12 " ) == 1 );
  assert( Util_TableGetInt (handle, &int_value, "foo") == 1 );
  assert( int_value == 12 );

  assert( Util_TableSetFromString(handle, " foo = 012 " ) == 1 );
  assert( Util_TableGetInt (handle, &int_value, "foo") == 1 );
  assert( int_value == 12 );

  assert( Util_TableSetFromString(handle, " foo = -12.0 " ) == 1 );
  assert( Util_TableGetReal (handle, &real_value, "foo") == 1 );
  assert( real_value == -12.0 );

  assert( Util_TableSetFromString(handle, " foo = '\\nbar\\r' " ) == 1 );
  assert( Util_TableGetCharArray (handle, sizeof (string_value), string_value,
                                  "foo") == (int) strlen ("\\nbar\\r") );
  string_value[strlen ("\\nbar\\r")] = 0;
  assert( strcmp ((char *) string_value, "\\nbar\\r") == 0 );

  assert( Util_TableSetFromString(handle, " foo = \"\tbar\v\" " ) == 1 );
  assert( Util_TableGetCharArray (handle, sizeof (string_value), string_value,
                                  "foo") == (int) strlen ("\tbar\v") );
  string_value[strlen ("\tbar\v")] = 0;
  assert( strcmp ((char *) string_value, "\tbar\v") == 0 );

  assert( Util_TableSetFromString(handle, " foo = {} " ) == 1 );
  assert( Util_TableQueryValueInfo (handle, &type, &nelems, "foo") == 1 );
  assert( type == CCTK_VARIABLE_INT && nelems == 0 );

  assert( Util_TableSetFromString(handle, " foo = {\t-1 +2 -3 +4\t} " ) == 1 );
  assert( Util_TableGetIntArray (handle, 5, int_array, "foo") == 4 );
  assert( int_array[0] == -1 && int_array[1] == 2 &&
          int_array[2] == -3 && int_array[3] == 4 );

  assert( Util_TableSetFromString(handle,
                                  " foo = {\t-1.1\t+2.2\t-3.3\t+4.4\t} ") == 1);
  assert( Util_TableGetRealArray (handle, 100, real_array, "foo") == 4 );
  assert( real_array[0] == -1.1 && real_array[1] == 2.2 &&
          real_array[2] == -3.3 && real_array[3] == 4.4 );

  assert( Util_TableDeleteKey (handle, "foo") == 0 );
  assert( Util_TableQueryNKeys (handle) == 0 );
    }

  /*
   * Test some "good" strings with multiple values of different types
   */
    {
  CCTK_CHAR string_value[10] = "";
  CCTK_INT  int_array[3] = {0, 0, 0};
  CCTK_REAL real_array[3] = {0.0, 0.0, 0.0};

  const int handle2 = Util_TableCreate(UTIL_TABLE_FLAGS_DEFAULT);
  assert( Util_TableSetFromString(handle2, "ij = 42\t"
                                           "real1 = 3.5\r"
                                           "string = 'string'\v"
                                           "int_array = {0 1 2}\n"
                                           "real_array = {0.0 1.1 2.2}") == 5);
  assert( Util_TableQueryNKeys(handle2) == 5 );
  assert( Util_TableSetFromString(handle2, "real_e=2.75") == 1);
  assert( Util_TableQueryNKeys(handle2) == 6 );
  assert( Util_TableGetIntArray (handle2, 5, int_array, "int_array") == 3 );
  assert( Util_TableGetRealArray (handle2, 100, real_array, "real_array") == 3);
  assert( Util_TableGetCharArray (handle2, sizeof (string_value), string_value,
                                  "string") == (int) strlen ("string") );
  string_value[strlen ("string")] = 0;

  assert( int_array[0] == 0 && int_array[1] == 1 && int_array[2] == 2 );
  assert( real_array[0] == 0.0 && real_array[1] == 1.1 && real_array[2] == 2.2);
  assert( strcmp ((char *) string_value, "string") == 0 );

  assert( Util_TableDeleteKey (handle2, "real_array") == 0 );
  assert( Util_TableDeleteKey (handle2, "int_array") == 0 );
  assert( Util_TableDeleteKey (handle2, "string") == 0 );
  check_table_contents(handle2, true);

  assert( Util_TableDestroy(handle2) == 0 );
    }
    {
  const int handle3
          = Util_TableCreateFromString("  ij=42 real1=3.5 real_e=2.75  ");
  assert( handle3 >= 0 );

  assert( Util_TableQueryFlags(handle3) == UTIL_TABLE_FLAGS_CASE_INSENSITIVE );
  assert( Util_TableQueryNKeys(handle3) == 3 );
  check_table_contents(handle3, true);

  return handle3;
    }
}
#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function tests  Util_Table{Set,Get}String()
 */
static
  void test_set_get_string(int handle, bool case_insensitive)
{
  assert( Util_TableSetString(handle, "Germany", "AEI") == 0 );
  assert( Util_TableSetString(handle, "Golm", "AEI") == 1 );

    {
  CCTK_INT type_code, N_elements;
  assert( Util_TableQueryValueInfo(handle,
                                   &type_code, &N_elements,
                                   case_insensitive ? "aei" : "AEI") == 1 );
  assert( type_code == CCTK_VARIABLE_CHAR );
  assert( N_elements == (int)strlen("Golm") );

    {
  const int N_buffer = N_elements+1;
  char *const buffer = (char *) malloc(N_buffer);
  assert( buffer != NULL );
  assert( Util_TableGetCharArray(handle,
                                 N_buffer, (CCTK_CHAR *) buffer,
                                 "AEI") == (int)strlen("Golm") );
  assert( Util_TableGetString(handle,
                              0, NULL,
                              "AEI") == (int)strlen("Golm") );
  assert( Util_TableGetString(handle,
                              N_buffer, buffer,
                              case_insensitive ? "aEI" : "AEI")
          == (int)strlen("Golm") );
  assert( strcmp(buffer, "Golm") == 0 );

  /* check getting string longer than buffer */
  assert( Util_TableSetString(handle, "Max-Planck", "famous") == 0 );
  assert( Util_TableGetString(handle,
                              N_buffer, buffer,
                              case_insensitive ? "FAMouS" : "famous")
          == UTIL_ERROR_TABLE_STRING_TRUNCATED );
  assert( strcmp(buffer, "Max-") == 0 );
    }
    }
}
#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function tests
 *	Util_Table{Set,Get}Pointer()
 *	Util_Table{Set,Get}PointerArray()
 */
static
  void test_set_get_pointers(int handle)
{
  CCTK_INT i, j;
  assert( Util_TableSetPointer(handle, (CCTK_POINTER) &i, "i_ptr") == 0);
  assert( Util_TableSetPointer(handle, (CCTK_POINTER) &j, "j_ptr") == 0);

    {
  CCTK_POINTER iptr, jptr;
  assert( Util_TableGetPointer(handle, &iptr, "i_ptr") == 1 );
  assert( (CCTK_INT*) iptr == &i );
  assert( Util_TableGetPointer(handle, &jptr, "j_ptr") == 1 );
  assert( (CCTK_INT*) jptr == &j );

    {
  CCTK_POINTER ijptr[2];
  ijptr[0] = &j;
  ijptr[1] = &i;
  assert( Util_TableSetPointerArray(handle, 2, ijptr, "ijptr") == 0 );

    {
  CCTK_POINTER ijptr_copy[5];
  assert( Util_TableGetPointerArray(handle, 5, ijptr_copy, "ijptr") == 2 );
  assert( ijptr_copy[0] = ijptr[0] );
  assert( ijptr_copy[1] = ijptr[1] );
    }
    }
    }
}
#endif	/* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function tests
 *	Util_Table{Set,Get}FPointer()
 *	Util_Table{Set,Get}FPointerArray()
 */
static
  void test_set_get_fpointers(int handle)
{
  CCTK_FPOINTER dptr = (CCTK_FPOINTER) & test_set_get_pointers;
  CCTK_FPOINTER fptr = (CCTK_FPOINTER) & test_set_get_fpointers;
  assert( Util_TableSetFPointer(handle, dptr, "dptr") == 0);
  assert( Util_TableSetFPointer(handle, fptr, "fptr") == 0);

    {
  CCTK_FPOINTER dptr_copy, fptr_copy;
  assert( Util_TableGetFPointer(handle, &dptr_copy, "dptr") == 1 );
  assert( dptr_copy == dptr );
  assert( Util_TableGetFPointer(handle, &fptr_copy, "fptr") == 1 );
  assert( fptr_copy == fptr );

    {
  CCTK_FPOINTER fdptrs[2];
  fdptrs[0] = fptr;
  fdptrs[1] = dptr;
  assert( Util_TableSetFPointerArray(handle, 2, fdptrs, "fdptrs") == 0 );
  
    {
  CCTK_FPOINTER fdptrs_copy[5];
  assert( Util_TableGetFPointerArray(handle, 2, fdptrs_copy, "fdptrs") == 2 );
  assert( fdptrs_copy[0] = fdptrs[0] );
  assert( fdptrs_copy[1] = fdptrs[1] );
    }
    }
    }
}
#endif	/* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function tests cloning a table.  We assume that on entry the
 * table contains the 3 keys
 *      real_e = 2.75
 *      real1 = 3.5
 *      ij = 42
 * in that order.
 */
void test_clone(int handle)
{
  check_table_contents(handle, true);     /* entry assumption */
    {
  const int clone_handle = Util_TableClone(handle);

  /* make sure we didn't modify the table that we cloned */
  check_table_contents(handle, true);

  /* check the clone */
  check_table_contents(clone_handle, false);

  /*
   * check that the tables are now distinct by
   * changing each of them and checking that the other is unchanged
   */
  assert( Util_TableSetInt(handle, 105, "universal number") == 0 );
  check_table_contents(clone_handle, false);
  assert( Util_TableDeleteKey(handle, "universal number") == 0 );

  assert( Util_TableSetInt(clone_handle, 105, "universal number #2") == 0 );
  check_table_contents(handle, true);
  assert( Util_TableDeleteKey(clone_handle, "universal number #2") == 0 );

  assert( Util_TableDestroy(clone_handle) == 0 );
    }
}
#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function does a sequence of assert() calls to verify that
 * a table contains the 3 keys
 *      real_e = 2.75
 *      real1 = 3.5
 *      ij = 42
 * in a specified order.
 *
 * Arguments:
 * order_up_flag = true --> check for the order {real_e,real1,ij}
 *                 false --> check for the order {ij,real1,real_e}
 *
 * Bugs:
 * Having to test for a specific order is a kludge.
 */
static
  void check_table_contents(int handle, bool order_up_flag)
{
  assert( Util_TableQueryNKeys(handle) == 3 );

  /* set up the key buffer */
    {
  const int max_key_length = Util_TableQueryMaxKeyLength(handle);
  assert( max_key_length == (int)strlen("real_e") );
    {
  const int N_key_buffer = max_key_length + 1;
  char *const key_buffer = malloc(N_key_buffer);
  assert( key_buffer != NULL );

  /* walk through the table to verify contents in the right order */
    {
  const int ihandle = Util_TableItCreate(handle);
  if (order_up_flag)
  {
    check_table_contents_real_e(handle,ihandle);
    assert( Util_TableItAdvance(ihandle) == 1 );
    check_table_contents_real1(handle,ihandle);
    assert( Util_TableItAdvance(ihandle) == 1 );
    check_table_contents_ij(handle, ihandle);
  }
  else
  {
    check_table_contents_ij(handle, ihandle);
    assert( Util_TableItAdvance(ihandle) == 1 );
    check_table_contents_real1(handle,ihandle);
    assert( Util_TableItAdvance(ihandle) == 1 );
    check_table_contents_real_e(handle,ihandle);
  }
  assert( Util_TableItAdvance(ihandle) == 0 );
  assert(Util_TableItDestroy(ihandle) == 0 );
    }
    }
    }
}
#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function does a sequence of assert() calls to verify that
 * the longest key in a table has the length of "real_e", and that
 * a table iterator points to the key  ij = 42 .
 */
static
  void check_table_contents_ij(int handle, int ihandle)
{
  /* set up the key buffer */
  const int max_key_length = Util_TableQueryMaxKeyLength(handle);
  assert( max_key_length == (int)strlen("real_e") );
    {
  const int N_key_buffer = max_key_length + 1;
  char *const key_buffer = malloc(N_key_buffer);
  assert( key_buffer != NULL );

  /* check ij = 42 */
    {
  int type_code = 123456;
  int N_elements = 54321;
  assert( Util_TableItQueryKeyValueInfo(ihandle,
                                        N_key_buffer, key_buffer,
                                        &type_code, &N_elements)
          == (int)strlen("ij") );
  assert( strcmp(key_buffer, "ij") == 0 );
  assert( type_code == CCTK_VARIABLE_INT );
  assert( N_elements == 1 );
    {
  CCTK_INT value_int;
  assert( Util_TableGetInt(handle, &value_int, "ij") == 1 );
  assert( value_int == 42 );
    }
    }
    }
}
#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function does a sequence of assert() calls to verify that
 * the longest key in a table has the length of "real_e", and that
 * a table iterator points to the key  real1 = 3.5 .  (N.b. 3.5 is
 * assumed to be exactly representable as a floating point number.)
 */
static
  void check_table_contents_real1(int handle, int ihandle)
{
  /* set up the key buffer */
  const int max_key_length = Util_TableQueryMaxKeyLength(handle);
  assert( max_key_length == (int)strlen("real_e") );
    {
  const int N_key_buffer = max_key_length + 1;
  char *const key_buffer = malloc(N_key_buffer);
  assert( key_buffer != NULL );

  /* check real1 = 3.5 */
    {
  int type_code = 123456;
  int N_elements = 54321;
  assert( Util_TableItQueryKeyValueInfo(ihandle,
                                        N_key_buffer, key_buffer,
                                        &type_code, &N_elements)
          == (int)strlen("real1") );
  assert( strcmp(key_buffer, "real1") == 0 );
  assert( type_code == CCTK_VARIABLE_REAL );
  assert( N_elements == 1 );
    {
  CCTK_REAL value_real;
  assert( Util_TableGetReal(handle, &value_real, key_buffer) == 1 );
  assert( value_real == 3.5 );
    }
    }
    }
}
#endif  /* UTIL_TABLE_TEST */

/******************************************************************************/

#ifdef UTIL_TABLE_TEST
/*
 * This function does a sequence of assert() calls to verify that
 * the longest key in a table has the length of "real_e", and that
 * a table iterator points to the key  real_e = 2.75 .  (N.b. 2.75 is
 * assumed to be exactly representable as a floating point number.)
 */
static
  void check_table_contents_real_e(int handle, int ihandle)
{
  /* set up the key buffer */
  const int max_key_length = Util_TableQueryMaxKeyLength(handle);
  assert( max_key_length == (int)strlen("real_e") );
    {
  const int N_key_buffer = max_key_length + 1;
  char *const key_buffer = malloc(N_key_buffer);
  assert( key_buffer != NULL );

  /* check real_e = 2.75 */
    {
  int type_code = 123456;
  int N_elements = 54321;
  assert( Util_TableItQueryKeyValueInfo(ihandle,
                                        N_key_buffer, key_buffer,
                                        &type_code, &N_elements)
          == (int)strlen("real_e") );
  assert( strcmp(key_buffer, "real_e") == 0 );
  assert( type_code == CCTK_VARIABLE_REAL );
  assert( N_elements == 1 );
    {
  CCTK_REAL value_real;
  assert( Util_TableGetReal(handle, &value_real, key_buffer) == 1 );
  assert( value_real == 2.75 );

  value_real = 314159.271828;
  assert( Util_TableGetGeneric(handle,
                               CCTK_VARIABLE_REAL, (void *) &value_real,
                               key_buffer)
          == 1 );
  assert( value_real == 2.75 );

  value_real = 314159.271828;
  assert( Util_TableGetGenericArray(handle,
                                    CCTK_VARIABLE_REAL, 1, (void *) &value_real,
                                    key_buffer)
          == 1 );
  assert( value_real == 2.75 );
    }
    }
    }
}
#endif  /* UTIL_TABLE_TEST */
