/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

/*
 * MT safe
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>  /* memset */

#include "hash.h"
#include "common.h"

/**
 * SECTION:hash_tables
 * @title: Hash Tables
 * @short_description: associations between keys and values so that
 *     given a key the value can be found quickly
 *
 * A #ExecHelpHashTable provides associations between keys and values which is
 * optimized so that given a key, the associated value can be found
 * very quickly.
 *
 * Note that neither keys nor values are copied when inserted into the
 * #ExecHelpHashTable, so they must exist for the lifetime of the #ExecHelpHashTable.
 * This means that the use of static strings is OK, but temporary
 * strings (i.e. those created in buffers and those returned by GTK+
 * widgets) should be copied with exechelp_strdup() before being inserted.
 *
 * If keys or values are dynamically allocated, you must be careful to
 * ensure that they are freed when they are removed from the
 * #ExecHelpHashTable, and also when they are overwritten by new insertions
 * into the #ExecHelpHashTable. It is also not advisable to mix static strings
 * and dynamically-allocated strings in a #ExecHelpHashTable, because it then
 * becomes difficult to determine whether the string should be freed.
 *
 * To create a #ExecHelpHashTable, use exechelp_hash_table_new().
 *
 * To insert a key and value into a #ExecHelpHashTable, use
 * exechelp_hash_table_insert().
 *
 * To lookup a value corresponding to a given key, use
 * exechelp_hash_table_lookup() and exechelp_hash_table_lookup_extended().
 *
 * exechelp_hash_table_lookup_extended() can also be used to simply
 * check if a key is present in the hash table.
 *
 * To remove a key and value, use exechelp_hash_table_remove().
 *
 * To call a function for each key and value pair use
 * exechelp_hash_table_foreach() or use a iterator to iterate over the
 * key/value pairs in the hash table, see #ExecHelpHashTableIter.
 *
 * To destroy a #ExecHelpHashTable use exechelp_hash_table_destroy().
 *
 * A common use-case for hash tables is to store information about a
 * set of keys, without associating any particular value with each
 * key. ExecHelpHashTable optimizes one way of doing so: If you store only
 * key-value pairs where key == value, then ExecHelpHashTable does not
 * allocate memory to store the values, which can be a considerable
 * space saving, if your set is large. The functions
 * exechelp_hash_table_add() and exechelp_hash_table_contains() are designed to be
 * used when using #ExecHelpHashTable this way.
 */

/**
 * ExecHelpHashTable:
 *
 * The #ExecHelpHashTable struct is an opaque data structure to represent a
 * [Hash Table][glib-Hash-Tables]. It should only be accessed via the
 * following functions.
 */

/**
 * ExecHelpHashFunc:
 * @key: a key
 *
 * Specifies the type of the hash function which is passed to
 * exechelp_hash_table_new() when a #ExecHelpHashTable is created.
 *
 * The function is passed a key and should return a #unsigned int hash value.
 * The functions exechelp_direct_hash(), exechelp_int_hash() and exechelp_str_hash() provide
 * hash functions which can be used when the key is a #void *, #int*,
 * and #char* respectively.
 *
 * exechelp_direct_hash() is also the appropriate hash function for keys
 * of the form `int_TO_POINTER (n)` (or similar macros).
 *
 * <!-- FIXME: Need more here. --> A good hash functions should produce
 * hash values that are evenly distributed over a fairly large range.
 * The modulus is taken with the hash table size (a prime number) to
 * find the 'bucket' to place each key into. The function should also
 * be very fast, since it is called for each key lookup.
 *
 * Note that the hash functions provided by GLib have these qualities,
 * but are not particularly robust against manufactured keys that
 * cause hash collisions. Therefore, you should consider choosing
 * a more secure hash function when using a ExecHelpHashTable with keys
 * that originate in untrusted data (such as HTTP requests).
 * Using exechelp_str_hash() in that situation might make your application
 * vulerable to
 * [Algorithmic Complexity Attacks](https://lwn.net/Articles/474912/).
 *
 * The key to choosing a good hash is unpredictability.  Even
 * cryptographic hashes are very easy to find collisions for when the
 * remainder is taken modulo a somewhat predictable prime number.  There
 * must be an element of randomness that an attacker is unable to guess.
 *
 * Returns: the hash value corresponding to the key
 */

/**
 * ExecHelpHFunc:
 * @key: a key
 * @value: the value corresponding to the key
 * @user_data: user data passed to exechelp_hash_table_foreach()
 *
 * Specifies the type of the function passed to exechelp_hash_table_foreach().
 * It is called with each key/value pair, together with the @user_data
 * parameter which is passed to exechelp_hash_table_foreach().
 */

/**
 * ExecHelpHRFunc:
 * @key: a key
 * @value: the value associated with the key
 * @user_data: user data passed to exechelp_hash_table_remove()
 *
 * Specifies the type of the function passed to
 * exechelp_hash_table_foreach_remove(). It is called with each key/value
 * pair, together with the @user_data parameter passed to
 * exechelp_hash_table_foreach_remove(). It should return %1 if the
 * key/value pair should be removed from the #ExecHelpHashTable.
 *
 * Returns: %1 if the key/value pair should be removed from the
 *     #ExecHelpHashTable
 */

/**
 * ExecHelpEqualFunc:
 * @a: a value
 * @b: a value to compare with
 *
 * Specifies the type of a function used to test two values for
 * equality. The function should return %1 if both values are equal
 * and %0 otherwise.
 *
 * Returns: %1 if @a = @b; %0 otherwise
 */

/**
 * ExecHelpHashTableIter:
 *
 * A ExecHelpHashTableIter structure represents an iterator that can be used
 * to iterate over the elements of a #ExecHelpHashTable. ExecHelpHashTableIter
 * structures are typically allocated on the stack and then initialized
 * with exechelp_hash_table_iter_init().
 */


#define HASH_TABLE_MIN_SHIFT 3  /* 1 << 3 == 8 buckets */

#define UNUSED_HASH_VALUE 0
#define TOMBSTONE_HASH_VALUE 1
#define HASH_IS_UNUSED(h_) ((h_) == UNUSED_HASH_VALUE)
#define HASH_IS_TOMBSTONE(h_) ((h_) == TOMBSTONE_HASH_VALUE)
#define HASH_IS_REAL(h_) ((h_) >= 2)

struct _ExecHelpHashTable
{
  int                     size;
  int                     mod;
  unsigned int            mask;
  int                     nnodes;
  int                     noccupied;  /* nnodes + tombstones */

  void *                 *keys;
  unsigned int           *hashes;
  void *                 *values;

  ExecHelpHashFunc        hash_func;
  ExecHelpEqualFunc       key_equal_func;
  int                     ref_count;
#ifndef EH_DISABLE_ASSERT
  /*
   * Tracks the structure of the hash table, not its contents: is only
   * incremented when a node is added or removed (is not incremented
   * when the key or data of a node is modified).
   */
  int                     version;
#endif
  ExecHelpDestroyNotify   key_destroy_func;
  ExecHelpDestroyNotify   value_destroy_func;
};

typedef struct
{
  ExecHelpHashTable  *hash_table;
  void *              dummy1;
  void *              dummy2;
  int                 position;
  int                 dummy3;
  int                 version;
} EHRealIter;

CASSERT(sizeof (ExecHelpHashTableIter) == sizeof (EHRealIter), hash_c);

/* Each table size has an associated prime modulo (the first prime
 * lower than the table size) used to find the initial bucket. Probing
 * then works modulo 2^n. The prime modulo is necessary to get a
 * good distribution with poor hash functions.
 */
static const int prime_mod [] =
{
  1,          /* For 1 << 0 */
  2,
  3,
  7,
  13,
  31,
  61,
  127,
  251,
  509,
  1021,
  2039,
  4093,
  8191,
  16381,
  32749,
  65521,      /* For 1 << 16 */
  131071,
  262139,
  524287,
  1048573,
  2097143,
  4194301,
  8388593,
  16777213,
  33554393,
  67108859,
  134217689,
  268435399,
  536870909,
  1073741789,
  2147483647  /* For 1 << 31 */
};

static void
exechelp_hash_table_set_shift (ExecHelpHashTable *hash_table, int shift)
{
  int i;
  unsigned int mask = 0;

  hash_table->size = 1 << shift;
  hash_table->mod  = prime_mod [shift];

  for (i = 0; i < shift; i++)
    {
      mask <<= 1;
      mask |= 1;
    }

  hash_table->mask = mask;
}

static int
exechelp_hash_table_find_closest_shift (int n)
{
  int i;

  for (i = 0; n; i++)
    n >>= 1;

  return i;
}

static void
exechelp_hash_table_set_shift_from_size (ExecHelpHashTable *hash_table, int size)
{
  int shift;

  shift = exechelp_hash_table_find_closest_shift (size);
  shift = (shift > HASH_TABLE_MIN_SHIFT) ? shift : HASH_TABLE_MIN_SHIFT;

  exechelp_hash_table_set_shift (hash_table, shift);
}

/*
 * exechelp_hash_table_lookup_node:
 * @hash_table: our #ExecHelpHashTable
 * @key: the key to lookup against
 * @hash_return: key hash return location
 *
 * Performs a lookup in the hash table, preserving extra information
 * usually needed for insertion.
 *
 * This function first computes the hash value of the key using the
 * user's hash function.
 *
 * If an entry in the table matching @key is found then this function
 * returns the index of that entry in the table, and if not, the
 * index of an unused node (empty or tombstone) where the key can be
 * inserted.
 *
 * The computed hash value is returned in the variable pointed to
 * by @hash_return. This is to save insertions from having to compute
 * the hash record again for the new record.
 *
 * Returns: index of the described node
 */
static inline unsigned int
exechelp_hash_table_lookup_node (ExecHelpHashTable    *hash_table,
                          const void *  key,
                          unsigned int         *hash_return)
{
  unsigned int node_index;
  unsigned int node_hash;
  unsigned int hash_value;
  unsigned int first_tombstone = 0;
  int have_tombstone = 0;
  unsigned int step = 0;

  /* If this happens, then the application is probably doing too much work
   * from a destroy notifier. The alternative would be to crash any second
   * (as keys, etc. will be NULL).
   * Applications need to either use exechelp_hash_table_destroy, or ensure the hash
   * table is empty prior to removing the last reference using exechelp_hash_table_unref(). */
  assert (hash_table->ref_count > 0);

  hash_value = hash_table->hash_func (key);
  if (!HASH_IS_REAL (hash_value))
    hash_value = 2;

  *hash_return = hash_value;

  node_index = hash_value % hash_table->mod;
  node_hash = hash_table->hashes[node_index];

  while (!HASH_IS_UNUSED (node_hash))
    {
      /* We first check if our full hash values
       * are equal so we can avoid calling the full-blown
       * key equality function in most cases.
       */
      if (node_hash == hash_value)
        {
          void * node_key = hash_table->keys[node_index];

          if (hash_table->key_equal_func)
            {
              if (hash_table->key_equal_func (node_key, key))
                return node_index;
            }
          else if (node_key == key)
            {
              return node_index;
            }
        }
      else if (HASH_IS_TOMBSTONE (node_hash) && !have_tombstone)
        {
          first_tombstone = node_index;
          have_tombstone = 1;
        }

      step++;
      node_index += step;
      node_index &= hash_table->mask;
      node_hash = hash_table->hashes[node_index];
    }

  if (have_tombstone)
    return first_tombstone;

  return node_index;
}

/*
 * exechelp_hash_table_remove_node:
 * @hash_table: our #ExecHelpHashTable
 * @node: pointer to node to remove
 * @notify: %1 if the destroy notify handlers are to be called
 *
 * Removes a node from the hash table and updates the node count.
 * The node is replaced by a tombstone. No table resize is performed.
 *
 * If @notify is %1 then the destroy notify functions are called
 * for the key and value of the hash node.
 */
static void
exechelp_hash_table_remove_node (ExecHelpHashTable   *hash_table,
                          int          i,
                          int      notify)
{
  void * key;
  void * value;

  key = hash_table->keys[i];
  value = hash_table->values[i];

  /* Erect tombstone */
  hash_table->hashes[i] = TOMBSTONE_HASH_VALUE;

  /* Be GC friendly */
  hash_table->keys[i] = NULL;
  hash_table->values[i] = NULL;

  hash_table->nnodes--;

  if (notify && hash_table->key_destroy_func)
    hash_table->key_destroy_func (key);

  if (notify && hash_table->value_destroy_func)
    hash_table->value_destroy_func (value);

}

/*
 * exechelp_hash_table_remove_all_nodes:
 * @hash_table: our #ExecHelpHashTable
 * @notify: %1 if the destroy notify handlers are to be called
 *
 * Removes all nodes from the table.  Since this may be a precursor to
 * freeing the table entirely, no resize is performed.
 *
 * If @notify is %1 then the destroy notify functions are called
 * for the key and value of the hash node.
 */
static void
exechelp_hash_table_remove_all_nodes (ExecHelpHashTable *hash_table,
                               int    notify,
                               int    destruction)
{
  int i;
  void * key;
  void * value;
  int old_size;
  void * *old_keys;
  void * *old_values;
  unsigned int    *old_hashes;

  /* If the hash table is already empty, there is nothing to be done. */
  if (hash_table->nnodes == 0)
    return;

  hash_table->nnodes = 0;
  hash_table->noccupied = 0;

  if (!notify ||
      (hash_table->key_destroy_func == NULL &&
       hash_table->value_destroy_func == NULL))
    {
      if (!destruction)
        {
          memset (hash_table->hashes, 0, hash_table->size * sizeof (unsigned int));
          memset (hash_table->keys, 0, hash_table->size * sizeof (void *));
          memset (hash_table->values, 0, hash_table->size * sizeof (void *));
        }

      return;
    }

  /* Keep the old storage space around to iterate over it. */
  old_size = hash_table->size;
  old_keys   = hash_table->keys;
  old_values = hash_table->values;
  old_hashes = hash_table->hashes;

  /* Now create a new storage space; If the table is destroyed we can use the
   * shortcut of not creating a new storage. This saves the allocation at the
   * cost of not allowing any recursive access.
   * However, the application doesn't own any reference anymore, so access
   * is not allowed. If accesses are done, then either an assert or crash
   * *will* happen. */
  exechelp_hash_table_set_shift (hash_table, HASH_TABLE_MIN_SHIFT);
  if (!destruction)
    {
      hash_table->keys   = exechelp_malloc0 (sizeof (void *) * hash_table->size);
      hash_table->values = hash_table->keys;
      hash_table->hashes = exechelp_malloc0 (sizeof (unsigned int) * hash_table->size);
    }
  else
    {
      hash_table->keys   = NULL;
      hash_table->values = NULL;
      hash_table->hashes = NULL;
    }

  for (i = 0; i < old_size; i++)
    {
      if (HASH_IS_REAL (old_hashes[i]))
        {
          key = old_keys[i];
          value = old_values[i];

          old_hashes[i] = UNUSED_HASH_VALUE;
          old_keys[i] = NULL;
          old_values[i] = NULL;

          if (hash_table->key_destroy_func == NULL)
            hash_table->key_destroy_func (key);

          if (hash_table->value_destroy_func != NULL)
            hash_table->value_destroy_func (value);
        }
    }

  /* Destroy old storage space. */
  if (old_keys != old_values)
    free (old_values);

  free (old_keys);
  free (old_hashes);
}

/*
 * exechelp_hash_table_resize:
 * @hash_table: our #ExecHelpHashTable
 *
 * Resizes the hash table to the optimal size based on the number of
 * nodes currently held. If you call this function then a resize will
 * occur, even if one does not need to occur.
 * Use exechelp_hash_table_maybe_resize() instead.
 *
 * This function may "resize" the hash table to its current size, with
 * the side effect of cleaning up tombstones and otherwise optimizing
 * the probe sequences.
 */
static void
exechelp_hash_table_resize (ExecHelpHashTable *hash_table)
{
  void * *new_keys;
  void * *new_values;
  unsigned int *new_hashes;
  int old_size;
  int i;

  old_size = hash_table->size;
  exechelp_hash_table_set_shift_from_size (hash_table, hash_table->nnodes * 2);

  new_keys = exechelp_malloc0 (sizeof (void *) * hash_table->size);
  if (hash_table->keys == hash_table->values)
    new_values = new_keys;
  else
    new_values = exechelp_malloc0 (sizeof (void *) * hash_table->size);
  new_hashes = exechelp_malloc0 (sizeof (unsigned int) * hash_table->size);

  for (i = 0; i < old_size; i++)
    {
      unsigned int node_hash = hash_table->hashes[i];
      unsigned int hash_val;
      unsigned int step = 0;

      if (!HASH_IS_REAL (node_hash))
        continue;

      hash_val = node_hash % hash_table->mod;

      while (!HASH_IS_UNUSED (new_hashes[hash_val]))
        {
          step++;
          hash_val += step;
          hash_val &= hash_table->mask;
        }

      new_hashes[hash_val] = hash_table->hashes[i];
      new_keys[hash_val] = hash_table->keys[i];
      new_values[hash_val] = hash_table->values[i];
    }

  if (hash_table->keys != hash_table->values)
    free (hash_table->values);

  free (hash_table->keys);
  free (hash_table->hashes);

  hash_table->keys = new_keys;
  hash_table->values = new_values;
  hash_table->hashes = new_hashes;

  hash_table->noccupied = hash_table->nnodes;
}

/*
 * exechelp_hash_table_maybe_resize:
 * @hash_table: our #ExecHelpHashTable
 *
 * Resizes the hash table, if needed.
 *
 * Essentially, calls exechelp_hash_table_resize() if the table has strayed
 * too far from its ideal size for its number of nodes.
 */
static inline void
exechelp_hash_table_maybe_resize (ExecHelpHashTable *hash_table)
{
  int noccupied = hash_table->noccupied;
  int size = hash_table->size;

  if ((size > hash_table->nnodes * 4 && size > 1 << HASH_TABLE_MIN_SHIFT) ||
      (size <= noccupied + (noccupied / 16)))
    exechelp_hash_table_resize (hash_table);
}

/**
 * exechelp_hash_table_new:
 * @hash_func: a function to create a hash value from a key
 * @key_equal_func: a function to check two keys for equality
 *
 * Creates a new #ExecHelpHashTable with a reference count of 1.
 *
 * Hash values returned by @hash_func are used to determine where keys
 * are stored within the #ExecHelpHashTable data structure. The exechelp_direct_hash(),
 * exechelp_int_hash(), exechelp_int64_hash(), exechelp_double_hash() and exechelp_str_hash()
 * functions are provided for some common types of keys.
 * If @hash_func is %NULL, exechelp_direct_hash() is used.
 *
 * @key_equal_func is used when looking up keys in the #ExecHelpHashTable.
 * The exechelp_direct_equal(), exechelp_int_equal(), exechelp_int64_equal(), exechelp_double_equal()
 * and exechelp_str_equal() functions are provided for the most common types
 * of keys. If @key_equal_func is %NULL, keys are compared directly in
 * a similar fashion to exechelp_direct_equal(), but without the overhead of
 * a function call.
 *
 * Returns: a new #ExecHelpHashTable
 */
ExecHelpHashTable *
exechelp_hash_table_new (ExecHelpHashFunc  hash_func,
                  ExecHelpEqualFunc key_equal_func)
{
  return exechelp_hash_table_new_full (hash_func, key_equal_func, NULL, NULL);
}


/**
 * exechelp_hash_table_new_full:
 * @hash_func: a function to create a hash value from a key
 * @key_equal_func: a function to check two keys for equality
 * @key_destroy_func: (allow-none): a function to free the memory allocated for the key
 *     used when removing the entry from the #ExecHelpHashTable, or %NULL
 *     if you don't want to supply such a function.
 * @value_destroy_func: (allow-none): a function to free the memory allocated for the
 *     value used when removing the entry from the #ExecHelpHashTable, or %NULL
 *     if you don't want to supply such a function.
 *
 * Creates a new #ExecHelpHashTable like exechelp_hash_table_new() with a reference
 * count of 1 and allows to specify functions to free the memory
 * allocated for the key and value that get called when removing the
 * entry from the #ExecHelpHashTable.
 *
 * Since version 2.42 it is permissible for destroy notify functions to
 * recursively remove further items from the hash table. This is only
 * permissible if the application still holds a reference to the hash table.
 * This means that you may need to ensure that the hash table is empty by
 * calling exechelp_hash_table_remove_all before releasing the last reference using
 * exechelp_hash_table_unref().
 *
 * Returns: a new #ExecHelpHashTable
 */
ExecHelpHashTable *
exechelp_hash_table_new_full (ExecHelpHashFunc      hash_func,
                       ExecHelpEqualFunc     key_equal_func,
                       ExecHelpDestroyNotify key_destroy_func,
                       ExecHelpDestroyNotify value_destroy_func)
{
  ExecHelpHashTable *hash_table;

  hash_table = malloc (sizeof(ExecHelpHashTable));
  exechelp_hash_table_set_shift (hash_table, HASH_TABLE_MIN_SHIFT);
  hash_table->nnodes             = 0;
  hash_table->noccupied          = 0;
  hash_table->hash_func          = hash_func ? hash_func : exechelp_direct_hash;
  hash_table->key_equal_func     = key_equal_func;
  hash_table->ref_count          = 1;
#ifndef EH_DISABLE_ASSERT
  hash_table->version            = 0;
#endif
  hash_table->key_destroy_func   = key_destroy_func;
  hash_table->value_destroy_func = value_destroy_func;
  hash_table->keys               = exechelp_malloc0 (sizeof (void *) * hash_table->size);
  hash_table->values             = hash_table->keys;
  hash_table->hashes             = exechelp_malloc0 (sizeof (unsigned int) * hash_table->size);

  return hash_table;
}

/**
 * exechelp_hash_table_iter_init:
 * @iter: an uninitialized #ExecHelpHashTableIter
 * @hash_table: a #ExecHelpHashTable
 *
 * Initializes a key/value pair iterator and associates it with
 * @hash_table. Modifying the hash table after calling this function
 * invalidates the returned iterator.
 * |[<!-- language="C" -->
 * ExecHelpHashTableIter iter;
 * void * key, value;
 *
 * exechelp_hash_table_iter_init (&iter, hash_table);
 * while (exechelp_hash_table_iter_next (&iter, &key, &value))
 *   {
 *     // do something with key and value
 *   }
 * ]|
 *
 * Since: 2.16
 */
void
exechelp_hash_table_iter_init (ExecHelpHashTableIter *iter,
                        ExecHelpHashTable     *hash_table)
{
  EHRealIter *ri = (EHRealIter *) iter;

  if (iter == NULL || hash_table == NULL)
    return;

  ri->hash_table = hash_table;
  ri->position = -1;
#ifndef EH_DISABLE_ASSERT
  ri->version = hash_table->version;
#endif
}

/**
 * exechelp_hash_table_iter_next:
 * @iter: an initialized #ExecHelpHashTableIter
 * @key: (allow-none): a location to store the key, or %NULL
 * @value: (allow-none): a location to store the value, or %NULL
 *
 * Advances @iter and retrieves the key and/or value that are now
 * pointed to as a result of this advancement. If %0 is returned,
 * @key and @value are not set, and the iterator becomes invalid.
 *
 * Returns: %0 if the end of the #ExecHelpHashTable has been reached.
 *
 * Since: 2.16
 */
int
exechelp_hash_table_iter_next (ExecHelpHashTableIter *iter,
                        void *       *key,
                        void *       *value)
{
  EHRealIter *ri = (EHRealIter *) iter;
  int position;

  if (iter == NULL || ri->version != ri->hash_table->version || ri->position >= ri->hash_table->size)
    return 0;

  position = ri->position;

  do
    {
      position++;
      if (position >= ri->hash_table->size)
        {
          ri->position = position;
          return 0;
        }
    }
  while (!HASH_IS_REAL (ri->hash_table->hashes[position]));

  if (key != NULL)
    *key = ri->hash_table->keys[position];
  if (value != NULL)
    *value = ri->hash_table->values[position];

  ri->position = position;
  return 1;
}

/**
 * exechelp_hash_table_iter_get_hash_table:
 * @iter: an initialized #ExecHelpHashTableIter
 *
 * Returns the #ExecHelpHashTable associated with @iter.
 *
 * Returns: the #ExecHelpHashTable associated with @iter.
 *
 * Since: 2.16
 */
ExecHelpHashTable *
exechelp_hash_table_iter_get_hash_table (ExecHelpHashTableIter *iter)
{
  if (!iter)
    return NULL;

  return ((EHRealIter *) iter)->hash_table;
}

static void
iter_remove_or_steal (EHRealIter *ri, int notify)
{
  if (!ri || ri->version != ri->hash_table->version || ri->position < 0 || ri->position >= ri->hash_table->size)
    return;

  exechelp_hash_table_remove_node (ri->hash_table, ri->position, notify);

#ifndef EH_DISABLE_ASSERT
  ri->version++;
  ri->hash_table->version++;
#endif
}

/**
 * exechelp_hash_table_iter_remove:
 * @iter: an initialized #ExecHelpHashTableIter
 *
 * Removes the key/value pair currently pointed to by the iterator
 * from its associated #ExecHelpHashTable. Can only be called after
 * exechelp_hash_table_iter_next() returned %1, and cannot be called
 * more than once for the same key/value pair.
 *
 * If the #ExecHelpHashTable was created using exechelp_hash_table_new_full(),
 * the key and value are freed using the supplied destroy functions,
 * otherwise you have to make sure that any dynamically allocated
 * values are freed yourself.
 *
 * It is safe to continue iterating the #ExecHelpHashTable afterward:
 * |[<!-- language="C" -->
 * while (exechelp_hash_table_iter_next (&iter, &key, &value))
 *   {
 *     if (condition)
 *       exechelp_hash_table_iter_remove (&iter);
 *   }
 * ]|
 *
 * Since: 2.16
 */
void
exechelp_hash_table_iter_remove (ExecHelpHashTableIter *iter)
{
  iter_remove_or_steal ((EHRealIter *) iter, 1);
}

/*
 * exechelp_hash_table_insert_node:
 * @hash_table: our #ExecHelpHashTable
 * @node_index: pointer to node to insert/replace
 * @key_hash: key hash
 * @key: (allow-none): key to replace with, or %NULL
 * @value: value to replace with
 * @keep_new_key: whether to replace the key in the node with @key
 * @reusing_key: whether @key was taken out of the existing node
 *
 * Inserts a value at @node_index in the hash table and updates it.
 *
 * If @key has been taken out of the existing node (ie it is not
 * passed in via a exechelp_hash_table_insert/replace) call, then @reusing_key
 * should be %1.
 *
 * Returns: %1 if the key did not exist yet
 */
static int
exechelp_hash_table_insert_node (ExecHelpHashTable *hash_table,
                          unsigned int       node_index,
                          unsigned int       key_hash,
                          void *    new_key,
                          void *    new_value,
                          int    keep_new_key,
                          int    reusing_key)
{
  int already_exists;
  unsigned int old_hash;
  void * key_to_free = NULL;
  void * value_to_free = NULL;

  old_hash = hash_table->hashes[node_index];
  already_exists = HASH_IS_REAL (old_hash);

  /* Proceed in three steps.  First, deal with the key because it is the
   * most complicated.  Then consider if we need to split the table in
   * two (because writing the value will result in the set invariant
   * becoming broken).  Then deal with the value.
   *
   * There are three cases for the key:
   *
   *  - entry already exists in table, reusing key:
   *    free the just-passed-in new_key and use the existing value
   *
   *  - entry already exists in table, not reusing key:
   *    free the entry in the table, use the new key
   *
   *  - entry not already in table:
   *    use the new key, free nothing
   *
   * We update the hash at the same time...
   */
  if (already_exists)
    {
      /* Note: we must record the old value before writing the new key
       * because we might change the value in the event that the two
       * arrays are shared.
       */
      value_to_free = hash_table->values[node_index];

      if (keep_new_key)
        {
          key_to_free = hash_table->keys[node_index];
          hash_table->keys[node_index] = new_key;
        }
      else
        key_to_free = new_key;
    }
  else
    {
      hash_table->hashes[node_index] = key_hash;
      hash_table->keys[node_index] = new_key;
    }

  /* Step two: check if the value that we are about to write to the
   * table is the same as the key in the same position.  If it's not,
   * split the table.
   */
  if ((hash_table->keys == hash_table->values && hash_table->keys[node_index] != new_value))
    hash_table->values = exechelp_memdup (hash_table->keys, sizeof (void *) * hash_table->size);

  /* Step 3: Actually do the write */
  hash_table->values[node_index] = new_value;

  /* Now, the bookkeeping... */
  if (!already_exists)
    {
      hash_table->nnodes++;

      if (HASH_IS_UNUSED (old_hash))
        {
          /* We replaced an empty node, and not a tombstone */
          hash_table->noccupied++;
          exechelp_hash_table_maybe_resize (hash_table);
        }

#ifndef EH_DISABLE_ASSERT
      hash_table->version++;
#endif
    }

  if (already_exists)
    {
      if (hash_table->key_destroy_func && !reusing_key)
        (* hash_table->key_destroy_func) (key_to_free);
      if (hash_table->value_destroy_func)
        (* hash_table->value_destroy_func) (value_to_free);
    }

  return !already_exists;
}

/**
 * exechelp_hash_table_iter_replace:
 * @iter: an initialized #ExecHelpHashTableIter
 * @value: the value to replace with
 *
 * Replaces the value currently pointed to by the iterator
 * from its associated #ExecHelpHashTable. Can only be called after
 * exechelp_hash_table_iter_next() returned %1.
 *
 * If you supplied a @value_destroy_func when creating the
 * #ExecHelpHashTable, the old value is freed using that function.
 *
 * Since: 2.30
 */
void
exechelp_hash_table_iter_replace (ExecHelpHashTableIter *iter,
                           void *        value)
{
  EHRealIter *ri;
  unsigned int node_hash;
  void * key;

  ri = (EHRealIter *) iter;


  if (!ri || ri->version != ri->hash_table->version || ri->position < 0 || ri->position >= ri->hash_table->size)
    return;

  node_hash = ri->hash_table->hashes[ri->position];
  key = ri->hash_table->keys[ri->position];

  exechelp_hash_table_insert_node (ri->hash_table, ri->position, node_hash, key, value, 1, 1);

#ifndef EH_DISABLE_ASSERT
  ri->version++;
  ri->hash_table->version++;
#endif
}

/**
 * exechelp_hash_table_iter_steal:
 * @iter: an initialized #ExecHelpHashTableIter
 *
 * Removes the key/value pair currently pointed to by the
 * iterator from its associated #ExecHelpHashTable, without calling
 * the key and value destroy functions. Can only be called
 * after exechelp_hash_table_iter_next() returned %1, and cannot
 * be called more than once for the same key/value pair.
 *
 * Since: 2.16
 */
void
exechelp_hash_table_iter_steal (ExecHelpHashTableIter *iter)
{
  iter_remove_or_steal ((EHRealIter *) iter, 0);
}


/**
 * exechelp_hash_table_ref:
 * @hash_table: a valid #ExecHelpHashTable
 *
 * Atomically increments the reference count of @hash_table by one.
 * This function is MT-safe and may be called from any thread.
 *
 * Returns: the passed in #ExecHelpHashTable
 *
 * Since: 2.10
 */
ExecHelpHashTable *
exechelp_hash_table_ref (ExecHelpHashTable *hash_table)
{
  if(!hash_table)
    return NULL;

  __sync_add_and_fetch(&hash_table->ref_count, 1);

  return hash_table;
}

/**
 * exechelp_hash_table_unref:
 * @hash_table: a valid #ExecHelpHashTable
 *
 * Atomically decrements the reference count of @hash_table by one.
 * If the reference count drops to 0, all keys and values will be
 * destroyed, and all memory allocated by the hash table is released.
 * This function is MT-safe and may be called from any thread.
 *
 * Since: 2.10
 */
void
exechelp_hash_table_unref (ExecHelpHashTable *hash_table)
{
  if(!hash_table)
    return;

  if (__sync_sub_and_fetch(&hash_table->ref_count, 1) == 0)
    {
      exechelp_hash_table_remove_all_nodes (hash_table, 1, 1);
      if (hash_table->keys != hash_table->values)
        free (hash_table->values);
      free (hash_table->keys);
      free (hash_table->hashes);
      free (hash_table);
    }
}

/**
 * exechelp_hash_table_destroy:
 * @hash_table: a #ExecHelpHashTable
 *
 * Destroys all keys and values in the #ExecHelpHashTable and decrements its
 * reference count by 1. If keys and/or values are dynamically allocated,
 * you should either free them first or create the #ExecHelpHashTable with destroy
 * notifiers using exechelp_hash_table_new_full(). In the latter case the destroy
 * functions you supplied will be called on all keys and values during the
 * destruction phase.
 */
void
exechelp_hash_table_destroy (ExecHelpHashTable *hash_table)
{
  if(!hash_table)
    return;

  exechelp_hash_table_remove_all (hash_table);
  exechelp_hash_table_unref (hash_table);
}

/**
 * exechelp_hash_table_lookup:
 * @hash_table: a #ExecHelpHashTable
 * @key: the key to look up
 *
 * Looks up a key in a #ExecHelpHashTable. Note that this function cannot
 * distinguish between a key that is not present and one which is present
 * and has the value %NULL. If you need this distinction, use
 * exechelp_hash_table_lookup_extended().
 *
 * Returns: (allow-none): the associated value, or %NULL if the key is not found
 */
void *
exechelp_hash_table_lookup (ExecHelpHashTable    *hash_table,
                     const void *  key)
{
  unsigned int node_index;
  unsigned int node_hash;

  if(!hash_table)
    return NULL;

  node_index = exechelp_hash_table_lookup_node (hash_table, key, &node_hash);

  return HASH_IS_REAL (hash_table->hashes[node_index])
    ? hash_table->values[node_index]
    : NULL;
}

/**
 * exechelp_hash_table_lookup_extended:
 * @hash_table: a #ExecHelpHashTable
 * @lookup_key: the key to look up
 * @oriexechelp_key: (allow-none): return location for the original key, or %NULL
 * @value: (allow-none): return location for the value associated with the key, or %NULL
 *
 * Looks up a key in the #ExecHelpHashTable, returning the original key and the
 * associated value and a #int which is %1 if the key was found. This
 * is useful if you need to free the memory allocated for the original key,
 * for example before calling exechelp_hash_table_remove().
 *
 * You can actually pass %NULL for @lookup_key to test
 * whether the %NULL key exists, provided the hash and equal functions
 * of @hash_table are %NULL-safe.
 *
 * Returns: %1 if the key was found in the #ExecHelpHashTable
 */
int
exechelp_hash_table_lookup_extended (ExecHelpHashTable    *hash_table,
                              const void *  lookup_key,
                              void *      *oriexechelp_key,
                              void *      *value)
{
  unsigned int node_index;
  unsigned int node_hash;

  if(!hash_table)
    return 0;

  node_index = exechelp_hash_table_lookup_node (hash_table, lookup_key, &node_hash);

  if (!HASH_IS_REAL (hash_table->hashes[node_index]))
    return 0;

  if (oriexechelp_key)
    *oriexechelp_key = hash_table->keys[node_index];

  if (value)
    *value = hash_table->values[node_index];

  return 1;
}

/*
 * exechelp_hash_table_insert_internal:
 * @hash_table: our #ExecHelpHashTable
 * @key: the key to insert
 * @value: the value to insert
 * @keep_new_key: if %1 and this key already exists in the table
 *   then call the destroy notify function on the old key.  If %0
 *   then call the destroy notify function on the new key.
 *
 * Implements the common logic for the exechelp_hash_table_insert() and
 * exechelp_hash_table_replace() functions.
 *
 * Do a lookup of @key. If it is found, replace it with the new
 * @value (and perhaps the new @key). If it is not found, create
 * a new node.
 *
 * Returns: %1 if the key did not exist yet
 */
static int
exechelp_hash_table_insert_internal (ExecHelpHashTable *hash_table,
                              void *    key,
                              void *    value,
                              int    keep_new_key)
{
  unsigned int key_hash;
  unsigned int node_index;

  if(!hash_table)
    return 0;

  node_index = exechelp_hash_table_lookup_node (hash_table, key, &key_hash);

  return exechelp_hash_table_insert_node (hash_table, node_index, key_hash, key, value, keep_new_key, 0);
}

/**
 * exechelp_hash_table_insert:
 * @hash_table: a #ExecHelpHashTable
 * @key: a key to insert
 * @value: the value to associate with the key
 *
 * Inserts a new key and value into a #ExecHelpHashTable.
 *
 * If the key already exists in the #ExecHelpHashTable its current
 * value is replaced with the new value. If you supplied a
 * @value_destroy_func when creating the #ExecHelpHashTable, the old
 * value is freed using that function. If you supplied a
 * @key_destroy_func when creating the #ExecHelpHashTable, the passed
 * key is freed using that function.
 *
 * Returns: %1 if the key did not exist yet
 */
int
exechelp_hash_table_insert (ExecHelpHashTable *hash_table,
                     void *    key,
                     void *    value)
{
  return exechelp_hash_table_insert_internal (hash_table, key, value, 0);
}

/**
 * exechelp_hash_table_replace:
 * @hash_table: a #ExecHelpHashTable
 * @key: a key to insert
 * @value: the value to associate with the key
 *
 * Inserts a new key and value into a #ExecHelpHashTable similar to
 * exechelp_hash_table_insert(). The difference is that if the key
 * already exists in the #ExecHelpHashTable, it gets replaced by the
 * new key. If you supplied a @value_destroy_func when creating
 * the #ExecHelpHashTable, the old value is freed using that function.
 * If you supplied a @key_destroy_func when creating the
 * #ExecHelpHashTable, the old key is freed using that function.
 *
 * Returns: %1 of the key did not exist yet
 */
int
exechelp_hash_table_replace (ExecHelpHashTable *hash_table,
                      void *    key,
                      void *    value)
{
  return exechelp_hash_table_insert_internal (hash_table, key, value, 1);
}

/**
 * exechelp_hash_table_add:
 * @hash_table: a #ExecHelpHashTable
 * @key: a key to insert
 *
 * This is a convenience function for using a #ExecHelpHashTable as a set.  It
 * is equivalent to calling exechelp_hash_table_replace() with @key as both the
 * key and the value.
 *
 * When a hash table only ever contains keys that have themselves as the
 * corresponding value it is able to be stored more efficiently.  See
 * the discussion in the section description.
 *
 * Returns: %1 if the key did not exist yet
 *
 * Since: 2.32
 */
int
exechelp_hash_table_add (ExecHelpHashTable *hash_table,
                  void *    key)
{
  return exechelp_hash_table_insert_internal (hash_table, key, key, 1);
}

/**
 * exechelp_hash_table_contains:
 * @hash_table: a #ExecHelpHashTable
 * @key: a key to check
 *
 * Checks if @key is in @hash_table.
 *
 * Returns: %1 if @key is in @hash_table, %0 otherwise.
 *
 * Since: 2.32
 **/
int
exechelp_hash_table_contains (ExecHelpHashTable    *hash_table,
                       const void *  key)
{
  unsigned int node_index;
  unsigned int node_hash;

  if(!hash_table)
    return 0;

  node_index = exechelp_hash_table_lookup_node (hash_table, key, &node_hash);

  return HASH_IS_REAL (hash_table->hashes[node_index]);
}

/*
 * exechelp_hash_table_remove_internal:
 * @hash_table: our #ExecHelpHashTable
 * @key: the key to remove
 * @notify: %1 if the destroy notify handlers are to be called
 * Returns: %1 if a node was found and removed, else %0
 *
 * Implements the common logic for the exechelp_hash_table_remove() and
 * exechelp_hash_table_steal() functions.
 *
 * Do a lookup of @key and remove it if it is found, calling the
 * destroy notify handlers only if @notify is %1.
 */
static int
exechelp_hash_table_remove_internal (ExecHelpHashTable    *hash_table,
                              const void *  key,
                              int       notify)
{
  unsigned int node_index;
  unsigned int node_hash;

  if(!hash_table)
    return 0;

  node_index = exechelp_hash_table_lookup_node (hash_table, key, &node_hash);

  if (!HASH_IS_REAL (hash_table->hashes[node_index]))
    return 0;

  exechelp_hash_table_remove_node (hash_table, node_index, notify);
  exechelp_hash_table_maybe_resize (hash_table);

#ifndef EH_DISABLE_ASSERT
  hash_table->version++;
#endif

  return 1;
}

/**
 * exechelp_hash_table_remove:
 * @hash_table: a #ExecHelpHashTable
 * @key: the key to remove
 *
 * Removes a key and its associated value from a #ExecHelpHashTable.
 *
 * If the #ExecHelpHashTable was created using exechelp_hash_table_new_full(), the
 * key and value are freed using the supplied destroy functions, otherwise
 * you have to make sure that any dynamically allocated values are freed
 * yourself.
 *
 * Returns: %1 if the key was found and removed from the #ExecHelpHashTable
 */
int
exechelp_hash_table_remove (ExecHelpHashTable    *hash_table,
                     const void *  key)
{
  return exechelp_hash_table_remove_internal (hash_table, key, 1);
}

/**
 * exechelp_hash_table_steal:
 * @hash_table: a #ExecHelpHashTable
 * @key: the key to remove
 *
 * Removes a key and its associated value from a #ExecHelpHashTable without
 * calling the key and value destroy functions.
 *
 * Returns: %1 if the key was found and removed from the #ExecHelpHashTable
 */
int
exechelp_hash_table_steal (ExecHelpHashTable    *hash_table,
                    const void *  key)
{
  return exechelp_hash_table_remove_internal (hash_table, key, 0);
}

/**
 * exechelp_hash_table_remove_all:
 * @hash_table: a #ExecHelpHashTable
 *
 * Removes all keys and their associated values from a #ExecHelpHashTable.
 *
 * If the #ExecHelpHashTable was created using exechelp_hash_table_new_full(),
 * the keys and values are freed using the supplied destroy functions,
 * otherwise you have to make sure that any dynamically allocated
 * values are freed yourself.
 *
 * Since: 2.12
 */
void
exechelp_hash_table_remove_all (ExecHelpHashTable *hash_table)
{
  if(!hash_table)
    return;

#ifndef EH_DISABLE_ASSERT
  if (hash_table->nnodes != 0)
    hash_table->version++;
#endif

  exechelp_hash_table_remove_all_nodes (hash_table, 1, 0);
  exechelp_hash_table_maybe_resize (hash_table);
}

/**
 * exechelp_hash_table_steal_all:
 * @hash_table: a #ExecHelpHashTable
 *
 * Removes all keys and their associated values from a #ExecHelpHashTable
 * without calling the key and value destroy functions.
 *
 * Since: 2.12
 */
void
exechelp_hash_table_steal_all (ExecHelpHashTable *hash_table)
{
  if(!hash_table)
    return;

#ifndef EH_DISABLE_ASSERT
  if (hash_table->nnodes != 0)
    hash_table->version++;
#endif

  exechelp_hash_table_remove_all_nodes (hash_table, 0, 0);
  exechelp_hash_table_maybe_resize (hash_table);
}

/*
 * exechelp_hash_table_foreach_remove_or_steal:
 * @hash_table: a #ExecHelpHashTable
 * @func: the user's callback function
 * @user_data: data for @func
 * @notify: %1 if the destroy notify handlers are to be called
 *
 * Implements the common logic for exechelp_hash_table_foreach_remove()
 * and exechelp_hash_table_foreach_steal().
 *
 * Iterates over every node in the table, calling @func with the key
 * and value of the node (and @user_data). If @func returns %1 the
 * node is removed from the table.
 *
 * If @notify is 1 then the destroy notify handlers will be called
 * for each removed node.
 */
static unsigned int
exechelp_hash_table_foreach_remove_or_steal (ExecHelpHashTable *hash_table,
                                      ExecHelpHRFunc     func,
                                      void *    user_data,
                                      int    notify)
{
  unsigned int deleted = 0;
  int i;
#ifndef EH_DISABLE_ASSERT
  int version = hash_table->version;
#endif

  for (i = 0; i < hash_table->size; i++)
    {
      unsigned int node_hash = hash_table->hashes[i];
      void * node_key = hash_table->keys[i];
      void * node_value = hash_table->values[i];

      if (HASH_IS_REAL (node_hash) &&
          (* func) (node_key, node_value, user_data))
        {
          exechelp_hash_table_remove_node (hash_table, i, notify);
          deleted++;
        }

#ifndef EH_DISABLE_ASSERT
      if (version != hash_table->version)
        return 0;
#endif
    }

  exechelp_hash_table_maybe_resize (hash_table);

#ifndef EH_DISABLE_ASSERT
  if (deleted > 0)
    hash_table->version++;
#endif

  return deleted;
}

/**
 * exechelp_hash_table_foreach_remove:
 * @hash_table: a #ExecHelpHashTable
 * @func: the function to call for each key/value pair
 * @user_data: user data to pass to the function
 *
 * Calls the given function for each key/value pair in the
 * #ExecHelpHashTable. If the function returns %1, then the key/value
 * pair is removed from the #ExecHelpHashTable. If you supplied key or
 * value destroy functions when creating the #ExecHelpHashTable, they are
 * used to free the memory allocated for the removed keys and values.
 *
 * See #ExecHelpHashTableIter for an alternative way to loop over the
 * key/value pairs in the hash table.
 *
 * Returns: the number of key/value pairs removed
 */
unsigned int
exechelp_hash_table_foreach_remove (ExecHelpHashTable *hash_table,
                             ExecHelpHRFunc     func,
                             void *    user_data)
{
  if(!hash_table || !func)
    return 0;

  return exechelp_hash_table_foreach_remove_or_steal (hash_table, func, user_data, 1);
}

/**
 * exechelp_hash_table_foreach_steal:
 * @hash_table: a #ExecHelpHashTable
 * @func: the function to call for each key/value pair
 * @user_data: user data to pass to the function
 *
 * Calls the given function for each key/value pair in the
 * #ExecHelpHashTable. If the function returns %1, then the key/value
 * pair is removed from the #ExecHelpHashTable, but no key or value
 * destroy functions are called.
 *
 * See #ExecHelpHashTableIter for an alternative way to loop over the
 * key/value pairs in the hash table.
 *
 * Returns: the number of key/value pairs removed.
 */
unsigned int
exechelp_hash_table_foreach_steal (ExecHelpHashTable *hash_table,
                            ExecHelpHRFunc     func,
                            void *    user_data)
{
  if(!hash_table || !func)
    return 0;
  
  return exechelp_hash_table_foreach_remove_or_steal (hash_table, func, user_data, 0);
}

/**
 * exechelp_hash_table_foreach:
 * @hash_table: a #ExecHelpHashTable
 * @func: the function to call for each key/value pair
 * @user_data: user data to pass to the function
 *
 * Calls the given function for each of the key/value pairs in the
 * #ExecHelpHashTable.  The function is passed the key and value of each
 * pair, and the given @user_data parameter.  The hash table may not
 * be modified while iterating over it (you can't add/remove
 * items). To remove all items matching a predicate, use
 * exechelp_hash_table_foreach_remove().
 *
 * See exechelp_hash_table_find() for performance caveats for linear
 * order searches in contrast to exechelp_hash_table_lookup().
 */
void
exechelp_hash_table_foreach (ExecHelpHashTable *hash_table,
                      ExecHelpHFunc      func,
                      void *    user_data)
{
  int i;
#ifndef EH_DISABLE_ASSERT
  int version;
#endif

  if(!hash_table || !func)
    return;

#ifndef EH_DISABLE_ASSERT
  version = hash_table->version;
#endif

  for (i = 0; i < hash_table->size; i++)
    {
      unsigned int node_hash = hash_table->hashes[i];
      void * node_key = hash_table->keys[i];
      void * node_value = hash_table->values[i];

      if (HASH_IS_REAL (node_hash))
        (* func) (node_key, node_value, user_data);

#ifndef EH_DISABLE_ASSERT
      if (version != hash_table->version)
        return;
#endif
    }
}

/**
 * exechelp_hash_table_find:
 * @hash_table: a #ExecHelpHashTable
 * @predicate: function to test the key/value pairs for a certain property
 * @user_data: user data to pass to the function
 *
 * Calls the given function for key/value pairs in the #ExecHelpHashTable
 * until @predicate returns %1. The function is passed the key
 * and value of each pair, and the given @user_data parameter. The
 * hash table may not be modified while iterating over it (you can't
 * add/remove items).
 *
 * Note, that hash tables are really only optimized for forward
 * lookups, i.e. exechelp_hash_table_lookup(). So code that frequently issues
 * exechelp_hash_table_find() or exechelp_hash_table_foreach() (e.g. in the order of
 * once per every entry in a hash table) should probably be reworked
 * to use additional or different data structures for reverse lookups
 * (keep in mind that an O(n) find/foreach operation issued for all n
 * values in a hash table ends up needing O(n*n) operations).
 *
 * Returns: (allow-none): The value of the first key/value pair is returned,
 *     for which @predicate evaluates to %1. If no pair with the
 *     requested property is found, %NULL is returned.
 *
 * Since: 2.4
 */
void *
exechelp_hash_table_find (ExecHelpHashTable *hash_table,
                   ExecHelpHRFunc     predicate,
                   void *    user_data)
{
  int i;
#ifndef EH_DISABLE_ASSERT
  int version;
#endif
  int match;

  if(!hash_table || !predicate)
    return NULL;

#ifndef EH_DISABLE_ASSERT
  version = hash_table->version;
#endif

  match = 0;

  for (i = 0; i < hash_table->size; i++)
    {
      unsigned int node_hash = hash_table->hashes[i];
      void * node_key = hash_table->keys[i];
      void * node_value = hash_table->values[i];

      if (HASH_IS_REAL (node_hash))
        match = predicate (node_key, node_value, user_data);

#ifndef EH_DISABLE_ASSERT
      if (version != hash_table->version)
        return NULL;
#endif

      if (match)
        return node_value;
    }

  return NULL;
}

/**
 * exechelp_hash_table_size:
 * @hash_table: a #ExecHelpHashTable
 *
 * Returns the number of elements contained in the #ExecHelpHashTable.
 *
 * Returns: the number of key/value pairs in the #ExecHelpHashTable.
 */
unsigned int
exechelp_hash_table_size (ExecHelpHashTable *hash_table)
{
  if(!hash_table)
    return 0;

  return hash_table->nnodes;
}

/**
 * exechelp_hash_table_get_keys:
 * @hash_table: a #ExecHelpHashTable
 *
 * Retrieves every key inside @hash_table. The returned data is valid
 * until changes to the hash release those keys.
 *
 * This iterates over every entry in the hash table to build its return value.
 * To iterate over the entries in a #ExecHelpHashTable more efficiently, use a
 * #ExecHelpHashTableIter.
 *
 * Returns: a #ExecHelpList containing all the keys inside the hash
 *     table. The content of the list is owned by the hash table and
 *     should not be modified or freed. Use exechelp_list_free() when done
 *     using the list.
 *
 * Since: 2.14
 */
ExecHelpList *
exechelp_hash_table_get_keys (ExecHelpHashTable *hash_table)
{
  int i;
  ExecHelpList *retval;

  if(!hash_table)
    return NULL;

  retval = NULL;
  for (i = 0; i < hash_table->size; i++)
    {
      if (HASH_IS_REAL (hash_table->hashes[i]))
        retval = exechelp_list_prepend (retval, hash_table->keys[i]);
    }

  return retval;
}

/**
 * exechelp_hash_table_get_keys_as_array:
 * @hash_table: a #ExecHelpHashTable
 * @length: (out): the length of the returned array
 *
 * Retrieves every key inside @hash_table, as an array.
 *
 * The returned array is %NULL-terminated but may contain %NULL as a
 * key.  Use @length to determine the 1 length if it's possible that
 * %NULL was used as the value for a key.
 *
 * Note: in the common case of a string-keyed #ExecHelpHashTable, the return
 * value of this function can be conveniently cast to (const char **).
 *
 * This iterates over every entry in the hash table to build its return value.
 * To iterate over the entries in a #ExecHelpHashTable more efficiently, use a
 * #ExecHelpHashTableIter.
 *
 * You should always free the return result with free().  In the
 * above-mentioned case of a string-keyed hash table, it may be
 * appropriate to use exechelp_strfreev() if you call exechelp_hash_table_steal_all()
 * first to transfer ownership of the keys.
 *
 * Returns: (array length=length) (transfer container): a
 *   %NULL-terminated array containing each key from the table.
 *
 * Since: 2.40
 **/
void * *
exechelp_hash_table_get_keys_as_array (ExecHelpHashTable *hash_table,
                                unsigned int      *length)
{
  void * *result;
  unsigned int i, j = 0;

  result = malloc (sizeof(void *) * hash_table->nnodes + 1);
  for (i = 0; i < hash_table->size; i++)
    {
      if (HASH_IS_REAL (hash_table->hashes[i]))
        result[j++] = hash_table->keys[i];
    }
  //exechelp_assert_cmpint (j, ==, hash_table->nnodes);
  result[j] = NULL;

  if (length)
    *length = j;

  return result;
}

/**
 * exechelp_hash_table_get_values:
 * @hash_table: a #ExecHelpHashTable
 *
 * Retrieves every value inside @hash_table. The returned data
 * is valid until @hash_table is modified.
 *
 * This iterates over every entry in the hash table to build its return value.
 * To iterate over the entries in a #ExecHelpHashTable more efficiently, use a
 * #ExecHelpHashTableIter.
 *
 * Returns: a #ExecHelpList containing all the values inside the hash
 *     table. The content of the list is owned by the hash table and
 *     should not be modified or freed. Use exechelp_list_free() when done
 *     using the list.
 *
 * Since: 2.14
 */
ExecHelpList *
exechelp_hash_table_get_values (ExecHelpHashTable *hash_table)
{
  int i;
  ExecHelpList *retval;

  if(hash_table == NULL)
    return NULL;

  retval = NULL;
  for (i = 0; i < hash_table->size; i++)
    {
      if (HASH_IS_REAL (hash_table->hashes[i]))
        retval = exechelp_list_prepend (retval, hash_table->values[i]);
    }

  return retval;
}

/* Hash functions.
 */

/**
 * exechelp_str_equal:
 * @v1: a key
 * @v2: a key to compare with @v1
 *
 * Compares two strings for byte-by-byte equality and returns %1
 * if they are equal. It can be passed to exechelp_hash_table_new() as the
 * @key_equal_func parameter, when using non-%NULL strings as keys in a
 * #ExecHelpHashTable.
 *
 * Note that this function is primarily meant as a hash table comparison
 * function. For a general-purpose, %NULL-safe string comparison function,
 * see exechelp_strcmp0().
 *
 * Returns: %1 if the two keys match
 */
int
exechelp_str_equal (const void * v1,
             const void * v2)
{
  const char *string1 = v1;
  const char *string2 = v2;

  return strcmp (string1, string2) == 0;
}

/**
 * exechelp_str_hash:
 * @v: a string key
 *
 * Converts a string to a hash value.
 *
 * This function implements the widely used "djb" hash apparently
 * posted by Daniel Bernstein to comp.lang.c some time ago.  The 32
 * bit unsigned hash value starts at 5381 and for each byte 'c' in
 * the string, is updated: `hash = hash * 33 + c`. This function
 * uses the signed value of each byte.
 *
 * It can be passed to exechelp_hash_table_new() as the @hash_func parameter,
 * when using non-%NULL strings as keys in a #ExecHelpHashTable.
 *
 * Returns: a hash value corresponding to the key
 */
unsigned int
exechelp_str_hash (const void * v)
{
  const signed char *p;
  uint32_t h = 5381;

  for (p = v; *p != '\0'; p++)
    h = (h << 5) + h + *p;

  return h;
}

/**
 * exechelp_direct_hash:
 * @v: (allow-none): a #void * key
 *
 * Converts a void * to a hash value.
 * It can be passed to exechelp_hash_table_new() as the @hash_func parameter,
 * when using opaque pointers compared by pointer value as keys in a
 * #ExecHelpHashTable.
 *
 * This hash function is also appropriate for keys that are integers
 * stored in pointers, such as `int_TO_POINTER (n)`.
 *
 * Returns: a hash value corresponding to the key.
 */
unsigned int
exechelp_direct_hash (const void * v)
{
  return EH_POINTER_TO_UINT (v);
}

/**
 * exechelp_direct_equal:
 * @v1: (allow-none): a key
 * @v2: (allow-none): a key to compare with @v1
 *
 * Compares two #void * arguments and returns %1 if they are equal.
 * It can be passed to exechelp_hash_table_new() as the @key_equal_func
 * parameter, when using opaque pointers compared by pointer value as
 * keys in a #ExecHelpHashTable.
 *
 * This equality function is also appropriate for keys that are integers
 * stored in pointers, such as `int_TO_POINTER (n)`.
 *
 * Returns: %1 if the two keys match.
 */
int
exechelp_direct_equal (const void * v1,
                const void * v2)
{
  return v1 == v2;
}

/**
 * exechelp_int_equal:
 * @v1: a pointer to a #int key
 * @v2: a pointer to a #int key to compare with @v1
 *
 * Compares the two #int values being pointed to and returns
 * %1 if they are equal.
 * It can be passed to exechelp_hash_table_new() as the @key_equal_func
 * parameter, when using non-%NULL pointers to integers as keys in a
 * #ExecHelpHashTable.
 *
 * Note that this function acts on pointers to #int, not on #int
 * directly: if your hash table's keys are of the form
 * `int_TO_POINTER (n)`, use exechelp_direct_equal() instead.
 *
 * Returns: %1 if the two keys match.
 */
int
exechelp_int_equal (const void * v1,
             const void * v2)
{
  return *((const int*) v1) == *((const int*) v2);
}

/**
 * exechelp_int_hash:
 * @v: a pointer to a #int key
 *
 * Converts a pointer to a #int to a hash value.
 * It can be passed to exechelp_hash_table_new() as the @hash_func parameter,
 * when using non-%NULL pointers to integer values as keys in a #ExecHelpHashTable.
 *
 * Note that this function acts on pointers to #int, not on #int
 * directly: if your hash table's keys are of the form
 * `int_TO_POINTER (n)`, use exechelp_direct_hash() instead.
 *
 * Returns: a hash value corresponding to the key.
 */
unsigned int
exechelp_int_hash (const void * v)
{
  return *(const int*) v;
}

/**
 * exechelp_int64_equal:
 * @v1: a pointer to a #int64 key
 * @v2: a pointer to a #int64 key to compare with @v1
 *
 * Compares the two #int64 values being pointed to and returns
 * %1 if they are equal.
 * It can be passed to exechelp_hash_table_new() as the @key_equal_func
 * parameter, when using non-%NULL pointers to 64-bit integers as keys in a
 * #ExecHelpHashTable.
 *
 * Returns: %1 if the two keys match.
 *
 * Since: 2.22
 */
int
exechelp_int64_equal (const void * v1,
               const void * v2)
{
  return *((const int64_t*) v1) == *((const int64_t*) v2);
}

/**
 * exechelp_int64_hash:
 * @v: a pointer to a #int64 key
 *
 * Converts a pointer to a #int64 to a hash value.
 *
 * It can be passed to exechelp_hash_table_new() as the @hash_func parameter,
 * when using non-%NULL pointers to 64-bit integer values as keys in a
 * #ExecHelpHashTable.
 *
 * Returns: a hash value corresponding to the key.
 *
 * Since: 2.22
 */
unsigned int
exechelp_int64_hash (const void * v)
{
  return (unsigned int) *(const int64_t*) v;
}

/**
 * exechelp_double_equal:
 * @v1: a pointer to a #double key
 * @v2: a pointer to a #double key to compare with @v1
 *
 * Compares the two #double values being pointed to and returns
 * %1 if they are equal.
 * It can be passed to exechelp_hash_table_new() as the @key_equal_func
 * parameter, when using non-%NULL pointers to doubles as keys in a
 * #ExecHelpHashTable.
 *
 * Returns: %1 if the two keys match.
 *
 * Since: 2.22
 */
int
exechelp_double_equal (const void * v1,
                const void * v2)
{
  return *((const double*) v1) == *((const double*) v2);
}

/**
 * exechelp_double_hash:
 * @v: a pointer to a #double key
 *
 * Converts a pointer to a #double to a hash value.
 * It can be passed to exechelp_hash_table_new() as the @hash_func parameter,
 * It can be passed to exechelp_hash_table_new() as the @hash_func parameter,
 * when using non-%NULL pointers to doubles as keys in a #ExecHelpHashTable.
 *
 * Returns: a hash value corresponding to the key.
 *
 * Since: 2.22
 */
unsigned int
exechelp_double_hash (const void * v)
{
  return (unsigned int) *(const double*) v;
}
