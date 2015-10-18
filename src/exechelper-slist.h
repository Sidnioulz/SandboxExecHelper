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

#ifndef __EH_SLIST_H__
#define __EH_SLIST_H__

typedef struct _ExecHelpSList ExecHelpSList;

struct _ExecHelpSList
{
  void *data;
  ExecHelpSList *next;
};

#define CASSERT(predicate, file) _impl_CASSERT_LINE(predicate,__LINE__,file)

#define _impl_PASTE(a,b) a##b
#define _impl_CASSERT_LINE(predicate, line, file) \
    typedef char _impl_PASTE(assertion_failed_##file##_,line)[2*!!(predicate)-1];

#define EH_INT_TO_POINTER(i) ((void *) (long) (i))
#define EH_ULONG_TO_POINTER(i) ((void *) (i))
#define EH_POINTER_TO_INT(p) ((int) (long) (p))
#define EH_POINTER_TO_UINT(p) ((unsigned int) (unsigned long) (p))
#define EH_POINTER_TO_ULONG(p) ((unsigned long) (p))

typedef int (*ExecHelpCompareFunc) (const void *, const void *);
typedef int (*ExecHelpCompareDataFunc) (const void *, const void *, void *);
typedef void (*ExecHelpDestroyNotify) (void *);
typedef void * (*ExecHelpCopyFunc) (const void *, void *);
typedef void (*ExecHelpFunc) (void *, void *);
typedef int (*ExecHelpEqualFunc) (const void *, const void *);
typedef unsigned int (*ExecHelpHashFunc) (const void *);
typedef void (*ExecHelpHFunc) (void *, void *, void *);


/* Singly linked lists
 */
ExecHelpSList*  exechelp_slist_alloc                   (void);
void     exechelp_slist_free                    (ExecHelpSList           *list);
void     exechelp_slist_free_1                  (ExecHelpSList           *list);
#define	 exechelp_slist_free1		         exechelp_slist_free_1
void     exechelp_slist_free_full               (ExecHelpSList           *list,
                                          ExecHelpDestroyNotify);
ExecHelpSList*  exechelp_slist_append                  (ExecHelpSList           *list,
					  void *          data);
ExecHelpSList*  exechelp_slist_prepend                 (ExecHelpSList           *list,
					  void *          data);
ExecHelpSList*  exechelp_slist_insert                  (ExecHelpSList           *list,
					  void *          data,
					  int              position);
ExecHelpSList*  exechelp_slist_insert_sorted           (ExecHelpSList           *list,
					  void *          data,
                                          ExecHelpCompareFunc);
ExecHelpSList*  exechelp_slist_insert_sorted_with_data (ExecHelpSList           *list,
					  void *          data,
                                          ExecHelpCompareDataFunc,
					  void *          user_data);
ExecHelpSList*  exechelp_slist_insert_before           (ExecHelpSList           *slist,
                                          ExecHelpSList           *sibling,
					  void *          data);
ExecHelpSList*  exechelp_slist_concat                  (ExecHelpSList           *list1,
                                          ExecHelpSList           *list2);
ExecHelpSList*  exechelp_slist_remove                  (ExecHelpSList           *list,
					  const void *     data);
ExecHelpSList*  exechelp_slist_remove_all              (ExecHelpSList           *list,
					  const void *     data);
ExecHelpSList*  exechelp_slist_remove_link             (ExecHelpSList           *list,
                                          ExecHelpSList           *link_);
ExecHelpSList*  exechelp_slist_delete_link             (ExecHelpSList           *list,
                                          ExecHelpSList           *link_);
ExecHelpSList*  exechelp_slist_reverse                 (ExecHelpSList           *list);
ExecHelpSList*  exechelp_slist_copy                    (ExecHelpSList           *list);

ExecHelpSList*  exechelp_slist_copy_deep               (ExecHelpSList            *list,
                                          ExecHelpCopyFunc,
					  void *          user_data);
ExecHelpSList*  exechelp_slist_nth                     (ExecHelpSList           *list,
					  unsigned int             n);
ExecHelpSList*  exechelp_slist_find                    (ExecHelpSList           *list,
					  const void *     data);
ExecHelpSList*  exechelp_slist_find_custom             (ExecHelpSList           *list,
					  const void *     data,
                                          ExecHelpCompareFunc);
int     exechelp_slist_position                (ExecHelpSList           *list,
                                          ExecHelpSList           *llink);
int     exechelp_slist_index                   (ExecHelpSList           *list,
					  const void *     data);
ExecHelpSList*  exechelp_slist_last                    (ExecHelpSList           *list);
unsigned int    exechelp_slist_length                  (ExecHelpSList           *list);
void     exechelp_slist_foreach                 (ExecHelpSList           *list,
                                          ExecHelpFunc,
					  void *          user_data);
ExecHelpSList*  exechelp_slist_sort                    (ExecHelpSList           *list,
                                          ExecHelpCompareFunc);
ExecHelpSList*  exechelp_slist_sort_with_data          (ExecHelpSList           *list,
                                          ExecHelpCompareDataFunc,
					  void *          user_data);
void * exechelp_slist_nth_data                (ExecHelpSList           *list,
					  unsigned int             n);

#define  exechelp_slist_next(slist)	         ((slist) ? (((exechelpSList *)(slist))->next) : NULL)

#endif /* __EH_SLIST_H__ */
