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

#include <stddef.h>
#include <stdlib.h>
#include "slist.h"

ExecHelpSList*
exechelp_slist_alloc (void)
{
  return malloc (sizeof (ExecHelpSList));
}

void
exechelp_slist_free (ExecHelpSList *list)
{
  if (list) {
    exechelp_slist_free (list->next);
    free (list);
  }
}

void
exechelp_slist_free_1 (ExecHelpSList *list)
{
  free (list);
}


void
exechelp_slist_free_full (ExecHelpSList *list,
                   ExecHelpDestroyNotify free_func)
{
  exechelp_slist_foreach (list, (ExecHelpFunc) free_func, NULL);
  exechelp_slist_free (list);
}

ExecHelpSList*
exechelp_slist_append (ExecHelpSList *list,
                void * data)
{
  ExecHelpSList *new_list;
  ExecHelpSList *last;

  new_list = malloc (sizeof (ExecHelpSList));
  new_list->data = data;
  new_list->next = NULL;

  if (list)
    {
      last = exechelp_slist_last (list);

      last->next = new_list;

      return list;
    }
  else
    return new_list;
}

ExecHelpSList*
exechelp_slist_prepend (ExecHelpSList *list,
                 void * data)
{
  ExecHelpSList *new_list;

  new_list = malloc (sizeof (ExecHelpSList));
  new_list->data = data;
  new_list->next = list;

  return new_list;
}

ExecHelpSList*
exechelp_slist_insert (ExecHelpSList *list,
                void * data,
                int position)
{
  ExecHelpSList *prev_list;
  ExecHelpSList *tmp_list;
  ExecHelpSList *new_list;

  if (position < 0)
    return exechelp_slist_append (list, data);
  else if (position == 0)
    return exechelp_slist_prepend (list, data);

  new_list = malloc (sizeof (ExecHelpSList));
  new_list->data = data;

  if (!list)
    {
      new_list->next = NULL;
      return new_list;
    }

  prev_list = NULL;
  tmp_list = list;

  while ((position-- > 0) && tmp_list)
    {
      prev_list = tmp_list;
      tmp_list = tmp_list->next;
    }

  new_list->next = prev_list->next;
  prev_list->next = new_list;

  return list;
}

ExecHelpSList*
exechelp_slist_insert_before (ExecHelpSList *slist,
                       ExecHelpSList *sibling,
                       void * data)
{
  if (!slist)
    {
      slist = malloc (sizeof (ExecHelpSList));
      slist->data = data;
      slist->next = NULL;
      return slist;
    }
  else
    {
      ExecHelpSList *node, *last = NULL;

      for (node = slist; node; last = node, node = last->next)
        if (node == sibling)
          break;
      if (!last)
        {
          node = malloc (sizeof (ExecHelpSList));
          node->data = data;
          node->next = slist;

          return node;
        }
      else
        {
          node = malloc (sizeof (ExecHelpSList));
          node->data = data;
          node->next = last->next;
          last->next = node;

          return slist;
        }
    }
}

ExecHelpSList *
exechelp_slist_concat (ExecHelpSList *list1, ExecHelpSList *list2)
{
  if (list2)
    {
      if (list1)
        exechelp_slist_last (list1)->next = list2;
      else
        list1 = list2;
    }

  return list1;
}

ExecHelpSList*
exechelp_slist_remove (ExecHelpSList *list,
                const void * data)
{
  ExecHelpSList *tmp, *prev = NULL;

  tmp = list;
  while (tmp)
    {
      if (tmp->data == data)
        {
          if (prev)
            prev->next = tmp->next;
          else
            list = tmp->next;

          exechelp_slist_free_1 (tmp);
          break;
        }
      prev = tmp;
      tmp = prev->next;
    }

  return list;
}

ExecHelpSList*
exechelp_slist_remove_all (ExecHelpSList *list,
                    const void * data)
{
  ExecHelpSList *tmp, *prev = NULL;

  tmp = list;
  while (tmp)
    {
      if (tmp->data == data)
        {
          ExecHelpSList *next = tmp->next;

          if (prev)
            prev->next = next;
          else
            list = next;

          exechelp_slist_free_1 (tmp);
          tmp = next;
        }
      else
        {
          prev = tmp;
          tmp = prev->next;
        }
    }

  return list;
}

static inline ExecHelpSList*
_exechelp_slist_remove_link (ExecHelpSList *list,
                      ExecHelpSList *link)
{
  ExecHelpSList *tmp;
  ExecHelpSList *prev;

  prev = NULL;
  tmp = list;

  while (tmp)
    {
      if (tmp == link)
        {
          if (prev)
            prev->next = tmp->next;
          if (list == tmp)
            list = list->next;

          tmp->next = NULL;
          break;
        }

      prev = tmp;
      tmp = tmp->next;
    }

  return list;
}

ExecHelpSList*
exechelp_slist_remove_link (ExecHelpSList *list,
                     ExecHelpSList *link_)
{
  return _exechelp_slist_remove_link (list, link_);
}

ExecHelpSList*
exechelp_slist_delete_link (ExecHelpSList *list,
                     ExecHelpSList *link_)
{
  list = _exechelp_slist_remove_link (list, link_);
  free (link_);

  return list;
}

ExecHelpSList*
exechelp_slist_copy (ExecHelpSList *list)
{
  return exechelp_slist_copy_deep (list, NULL, NULL);
}

ExecHelpSList*
exechelp_slist_copy_deep (ExecHelpSList *list, ExecHelpCopyFunc func, void * user_data)
{
  ExecHelpSList *new_list = NULL;

  if (list)
    {
      ExecHelpSList *last;

      new_list = malloc (sizeof (ExecHelpSList));
      if (func)
        new_list->data = func (list->data, user_data);
      else
        new_list->data = list->data;
      last = new_list;
      list = list->next;
      while (list)
        {
          last->next = malloc (sizeof (ExecHelpSList));
          last = last->next;
          if (func)
            last->data = func (list->data, user_data);
          else
            last->data = list->data;
          list = list->next;
        }
      last->next = NULL;
    }

  return new_list;
}

ExecHelpSList*
exechelp_slist_reverse (ExecHelpSList *list)
{
  ExecHelpSList *prev = NULL;

  while (list)
    {
      ExecHelpSList *next = list->next;

      list->next = prev;

      prev = list;
      list = next;
    }

  return prev;
}
ExecHelpSList*
exechelp_slist_nth (ExecHelpSList *list,
             unsigned int n)
{
  while (n-- > 0 && list)
    list = list->next;

  return list;
}
void *
exechelp_slist_nth_data (ExecHelpSList *list,
                  unsigned int n)
{
  while (n-- > 0 && list)
    list = list->next;

  return list ? list->data : NULL;
}
ExecHelpSList*
exechelp_slist_find (ExecHelpSList *list,
              const void * data)
{
  while (list)
    {
      if (list->data == data)
        break;
      list = list->next;
    }

  return list;
}
ExecHelpSList*
exechelp_slist_find_custom (ExecHelpSList *list,
                     const void * data,
                     ExecHelpCompareFunc func)
{
  if (!func)
    return NULL;

  while (list)
    {
      if (! func (list->data, data))
        return list;
      list = list->next;
    }

  return NULL;
}
int
exechelp_slist_position (ExecHelpSList *list,
                  ExecHelpSList *llink)
{
  int i;

  i = 0;
  while (list)
    {
      if (list == llink)
        return i;
      i++;
      list = list->next;
    }

  return -1;
}
int
exechelp_slist_index (ExecHelpSList *list,
               const void * data)
{
  int i;

  i = 0;
  while (list)
    {
      if (list->data == data)
        return i;
      i++;
      list = list->next;
    }

  return -1;
}
ExecHelpSList*
exechelp_slist_last (ExecHelpSList *list)
{
  if (list)
    {
      while (list->next)
        list = list->next;
    }

  return list;
}
unsigned int
exechelp_slist_length (ExecHelpSList *list)
{
  unsigned int length;

  length = 0;
  while (list)
    {
      length++;
      list = list->next;
    }

  return length;
}
void
exechelp_slist_foreach (ExecHelpSList *list,
                 ExecHelpFunc func,
                 void * user_data)
{
  while (list)
    {
      ExecHelpSList *next = list->next;
      (*func) (list->data, user_data);
      list = next;
    }
}

static ExecHelpSList*
exechelp_slist_insert_sorted_real (ExecHelpSList *list,
                            void * data,
                            ExecHelpFunc func,
                            void * user_data)
{
  ExecHelpSList *tmp_list = list;
  ExecHelpSList *prev_list = NULL;
  ExecHelpSList *new_list;
  int cmp;

  if (!func)
      return list;

  if (!list)
    {
      new_list = malloc (sizeof (ExecHelpSList));
      new_list->data = data;
      new_list->next = NULL;
      return new_list;
    }

  cmp = ((ExecHelpCompareDataFunc) func) (data, tmp_list->data, user_data);

  while ((tmp_list->next) && (cmp > 0))
    {
      prev_list = tmp_list;
      tmp_list = tmp_list->next;

      cmp = ((ExecHelpCompareDataFunc) func) (data, tmp_list->data, user_data);
    }

  new_list = malloc (sizeof (ExecHelpSList));
  new_list->data = data;

  if ((!tmp_list->next) && (cmp > 0))
    {
      tmp_list->next = new_list;
      new_list->next = NULL;
      return list;
    }

  if (prev_list)
    {
      prev_list->next = new_list;
      new_list->next = tmp_list;
      return list;
    }
  else
    {
      new_list->next = list;
      return new_list;
    }
}
ExecHelpSList*
exechelp_slist_insert_sorted (ExecHelpSList *list,
                       void * data,
                       ExecHelpCompareFunc func)
{
  return exechelp_slist_insert_sorted_real (list, data, (ExecHelpFunc) func, NULL);
}
ExecHelpSList*
exechelp_slist_insert_sorted_with_data (ExecHelpSList *list,
                                 void * data,
                                 ExecHelpCompareDataFunc func,
                                 void * user_data)
{
  return exechelp_slist_insert_sorted_real (list, data, (ExecHelpFunc) func, user_data);
}

static ExecHelpSList *
exechelp_slist_sort_merge (ExecHelpSList *l1,
                    ExecHelpSList *l2,
                    ExecHelpFunc compare_func,
                    void * user_data)
{
  ExecHelpSList list, *l;
  int cmp;

  l=&list;

  while (l1 && l2)
    {
      cmp = ((ExecHelpCompareDataFunc) compare_func) (l1->data, l2->data, user_data);

      if (cmp <= 0)
        {
          l=l->next=l1;
          l1=l1->next;
        }
      else
        {
          l=l->next=l2;
          l2=l2->next;
        }
    }
  l->next= l1 ? l1 : l2;

  return list.next;
}

static ExecHelpSList *
exechelp_slist_sort_real (ExecHelpSList *list,
                   ExecHelpFunc compare_func,
                   void * user_data)
{
  ExecHelpSList *l1, *l2;

  if (!list)
    return NULL;
  if (!list->next)
    return list;

  l1 = list;
  l2 = list->next;

  while ((l2 = l2->next) != NULL)
    {
      if ((l2 = l2->next) == NULL)
        break;
      l1=l1->next;
    }
  l2 = l1->next;
  l1->next = NULL;

  return exechelp_slist_sort_merge (exechelp_slist_sort_real (list, compare_func, user_data),
                             exechelp_slist_sort_real (l2, compare_func, user_data),
                             compare_func,
                             user_data);
}
ExecHelpSList *
exechelp_slist_sort (ExecHelpSList *list,
              ExecHelpCompareFunc compare_func)
{
  return exechelp_slist_sort_real (list, (ExecHelpFunc) compare_func, NULL);
}
ExecHelpSList *
exechelp_slist_sort_with_data (ExecHelpSList *list,
                        ExecHelpCompareDataFunc compare_func,
                        void * user_data)
{
  return exechelp_slist_sort_real (list, (ExecHelpFunc) compare_func, user_data);
}
