/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GLib Team and others 1997-2000. See the AUTHORS
 * file for a list of people on the GLib Team. See the ChangeLog
 * files for a list of changes. These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

#ifndef __EH_HASH_H__
#define __EH_HASH_H__

#include "list.h"

typedef struct _ExecHelpHashTable ExecHelpHashTable;

typedef int (*ExecHelpHRFunc) (void * key, void * value, void * user_data);

typedef struct _ExecHelpHashTableIter ExecHelpHashTableIter;

struct _ExecHelpHashTableIter
{
 /*< private >*/
 void * dummy1;
 void * dummy2;
 void * dummy3;
 int dummy4;
 int dummy5;
 void * dummy6;
};

ExecHelpHashTable* exechelp_hash_table_new (ExecHelpHashFunc hash_func, ExecHelpEqualFunc key_equal_func);
ExecHelpHashTable* exechelp_hash_table_new_full (ExecHelpHashFunc hash_func, ExecHelpEqualFunc key_equal_func, ExecHelpDestroyNotify key_destroy_func, ExecHelpDestroyNotify value_destroy_func);
void exechelp_hash_table_destroy (ExecHelpHashTable *hash_table);
int exechelp_hash_table_insert (ExecHelpHashTable *hash_table, void * key, void * value);
int exechelp_hash_table_replace (ExecHelpHashTable *hash_table, void * key, void * value);
int exechelp_hash_table_add (ExecHelpHashTable *hash_table, void * key);
int exechelp_hash_table_remove (ExecHelpHashTable *hash_table, const void * key);
void exechelp_hash_table_remove_all (ExecHelpHashTable *hash_table);
int exechelp_hash_table_steal (ExecHelpHashTable *hash_table, const void * key);
void exechelp_hash_table_steal_all (ExecHelpHashTable *hash_table);
void * exechelp_hash_table_lookup (ExecHelpHashTable *hash_table, const void * key);
int exechelp_hash_table_contains (ExecHelpHashTable *hash_table, const void * key);
int exechelp_hash_table_lookup_extended (ExecHelpHashTable *hash_table, const void * lookup_key,
 void * *orig_key, void * *value);
 void exechelp_hash_table_foreach (ExecHelpHashTable *hash_table, ExecHelpHFunc func, void * user_data);
void * exechelp_hash_table_find (ExecHelpHashTable *hash_table, ExecHelpHRFunc predicate, void * user_data);
unsigned int exechelp_hash_table_foreach_remove (ExecHelpHashTable *hash_table, ExecHelpHRFunc func, void * user_data);
unsigned int exechelp_hash_table_foreach_steal (ExecHelpHashTable *hash_table, ExecHelpHRFunc func, void * user_data);
unsigned int exechelp_hash_table_size (ExecHelpHashTable *hash_table);
ExecHelpList * exechelp_hash_table_get_keys (ExecHelpHashTable *hash_table);
ExecHelpList * exechelp_hash_table_get_values (ExecHelpHashTable *hash_table);
void * * exechelp_hash_table_get_keys_as_array (ExecHelpHashTable *hash_table, unsigned int *length);
void exechelp_hash_table_iter_init (ExecHelpHashTableIter *iter, ExecHelpHashTable *hash_table);
int exechelp_hash_table_iter_next (ExecHelpHashTableIter *iter, void * *key, void * *value);
ExecHelpHashTable* exechelp_hash_table_iter_get_hash_table (ExecHelpHashTableIter *iter);
void exechelp_hash_table_iter_remove (ExecHelpHashTableIter *iter);
void exechelp_hash_table_iter_replace (ExecHelpHashTableIter *iter, void * value);
void exechelp_hash_table_iter_steal (ExecHelpHashTableIter *iter);

ExecHelpHashTable* exechelp_hash_table_ref (ExecHelpHashTable *hash_table);
void exechelp_hash_table_unref (ExecHelpHashTable *hash_table);

/* Hash Functions
 */
int exechelp_str_equal (const void * v1, const void * v2);
unsigned int exechelp_str_hash (const void * v);
int exechelp_int_equal (const void * v1, const void * v2);
unsigned int exechelp_int_hash (const void * v);
int exechelp_int64_equal (const void * v1, const void * v2);
unsigned int exechelp_int64_hash (const void * v);
int exechelp_double_equal (const void * v1, const void * v2);
unsigned int exechelp_double_hash (const void * v);
unsigned int exechelp_direct_hash (const void * v);
int exechelp_direct_equal (const void * v1, const void * v2);

#endif /* __EH_HASH_H__ */
