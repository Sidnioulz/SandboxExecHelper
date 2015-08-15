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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
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

#ifndef __EH_LIST_H__
#define __EH_LIST_H__

#include "slist.h"


typedef struct _ExecHelpList ExecHelpList;

struct _ExecHelpList
{
  void * data;
  ExecHelpList *next;
  ExecHelpList *prev;
};

/* Doubly linked lists
 */
ExecHelpList*   exechelp_list_alloc                   (void);
void     exechelp_list_free                    (ExecHelpList            *list);
void     exechelp_list_free_1                  (ExecHelpList            *list);
#define  exechelp_list_free1                   exechelp_list_free_1
void     exechelp_list_free_full               (ExecHelpList            *list,
					 ExecHelpDestroyNotify    free_func);
ExecHelpList*   exechelp_list_append                  (ExecHelpList            *list,
					 void *          data);
ExecHelpList*   exechelp_list_prepend                 (ExecHelpList            *list,
					 void *          data);
ExecHelpList*   exechelp_list_insert                  (ExecHelpList            *list,
					 void *          data,
					 int              position);
ExecHelpList*   exechelp_list_insert_sorted           (ExecHelpList            *list,
					 void *          data,
					 ExecHelpCompareFunc      func);
ExecHelpList*   exechelp_list_insert_sorted_with_data (ExecHelpList            *list,
					 void *          data,
					 ExecHelpCompareDataFunc  func,
					 void *          user_data);
ExecHelpList*   exechelp_list_insert_before           (ExecHelpList            *list,
					 ExecHelpList            *sibling,
					 void *          data);
ExecHelpList*   exechelp_list_concat                  (ExecHelpList            *list1,
					 ExecHelpList            *list2);
ExecHelpList*   exechelp_list_remove                  (ExecHelpList            *list,
					 const void *     data);
ExecHelpList*   exechelp_list_remove_all              (ExecHelpList            *list,
					 const void *     data);
ExecHelpList*   exechelp_list_remove_link             (ExecHelpList            *list,
					 ExecHelpList            *llink);
ExecHelpList*   exechelp_list_delete_link             (ExecHelpList            *list,
					 ExecHelpList            *link_);
ExecHelpList*   exechelp_list_reverse                 (ExecHelpList            *list);
ExecHelpList*   exechelp_list_copy                    (ExecHelpList            *list);

ExecHelpList*   exechelp_list_copy_deep               (ExecHelpList            *list,
					 ExecHelpCopyFunc         func,
					 void *          user_data);

ExecHelpList*   exechelp_list_nth                     (ExecHelpList            *list,
					 unsigned int             n);
ExecHelpList*   exechelp_list_nth_prev                (ExecHelpList            *list,
					 unsigned int             n);
ExecHelpList*   exechelp_list_find                    (ExecHelpList            *list,
					 const void *     data);
ExecHelpList*   exechelp_list_find_custom             (ExecHelpList            *list,
					 const void *     data,
					 ExecHelpCompareFunc      func);
int     exechelp_list_position                (ExecHelpList            *list,
					 ExecHelpList            *llink);
int     exechelp_list_index                   (ExecHelpList            *list,
					 const void *     data);
ExecHelpList*   exechelp_list_last                    (ExecHelpList            *list);
ExecHelpList*   exechelp_list_first                   (ExecHelpList            *list);
unsigned int    exechelp_list_length                  (ExecHelpList            *list);
void     exechelp_list_foreach                 (ExecHelpList            *list,
					 ExecHelpFunc             func,
					 void *          user_data);
ExecHelpList*   exechelp_list_sort                    (ExecHelpList            *list,
					 ExecHelpCompareFunc      compare_func);
ExecHelpList*   exechelp_list_sort_with_data          (ExecHelpList            *list,
					 ExecHelpCompareDataFunc  compare_func,
					 void *          user_data) ;
void * exechelp_list_nth_data                (ExecHelpList            *list,
					 unsigned int             n);


#define exechelp_list_previous(list)	        ((list) ? (((ExecHelpList *)(list))->prev) : NULL)
#define exechelp_list_next(list)	        ((list) ? (((ExecHelpList *)(list))->next) : NULL)

#endif /* __EH_LIST_H__ */
