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

#include <stddef.h>
#include <stdlib.h>
#include "list.h"

/**
 * SECTION:linked_lists_double
 * @title: Doubly-Linked Lists
 * @short_description: linked lists that can be iterated over in both directions
 *
 * The #ExecHelpList structure and its associated functions provide a standard
 * doubly-linked list data structure.
 *
 * Each element in the list contains a piece of data, together with
 * pointers which link to the previous and next elements in the list.
 * Using these pointers it is possible to move through the list in both
 * directions (unlike the singly-linked [ExecHelpSList][glib-Singly-Linked-Lists],
 * which only allows movement through the list in the forward direction).
 *
 * The double linked list does not keep track of the number of items 
 * and does not keep track of both the start and end of the list. If
 * you want fast access to both the start and the end of the list, 
 * and/or the number of items in the list, use a
 * [GQueue][glib-Double-ended-Queues] instead.
 *
 * The data contained in each element can be either integer values, by
 * using one of the [Type Conversion Macros][glib-Type-Conversion-Macros],
 * or simply pointers to any type of data.
 *
 * List elements are allocated from the [slice allocator][glib-Memory-Slices],
 * which is more efficient than allocating elements individually.
 *
 * Note that most of the #ExecHelpList functions expect to be passed a pointer
 * to the first element in the list. The functions which insert
 * elements return the new start of the list, which may have changed.
 *
 * There is no function to create a #ExecHelpList. %NULL is considered to be
 * a valid, empty list so you simply set a #ExecHelpList* to %NULL to initialize
 * it.
 *
 * To add elements, use exechelp_list_append(), exechelp_list_prepend(),
 * exechelp_list_insert() and exechelp_list_insert_sorted().
 *
 * To visit all elements in the list, use a loop over the list:
 * |[<!-- language="C" -->
 * ExecHelpList *l;
 * for (l = list; l != NULL; l = l->next)
 *   {
 *     // do something with l->data
 *   }
 * ]|
 *
 * To call a function for each element in the list, use exechelp_list_foreach().
 *
 * To loop over the list and modify it (e.g. remove a certain element)
 * a while loop is more appropriate, for example:
 * |[<!-- language="C" -->
 * ExecHelpList *l = list;
 * while (l != NULL)
 *   {
 *     ExecHelpList *next = l->next;
 *     if (should_be_removed (l))
 *       {
 *         // possibly free l->data
 *         list = exechelp_list_delete_link (list, l);
 *       }
 *     l = next;
 *   }
 * ]|
 *
 * To remove elements, use exechelp_list_remove().
 *
 * To navigate in a list, use exechelp_list_first(), exechelp_list_last(),
 * exechelp_list_next(), exechelp_list_previous().
 *
 * To find elements in the list use exechelp_list_nth(), exechelp_list_nth_data(),
 * exechelp_list_find() and exechelp_list_find_custom().
 *
 * To find the index of an element use exechelp_list_position() and
 * exechelp_list_index().
 *
 * To free the entire list, use exechelp_list_free() or exechelp_list_free_full().
 */

/**
 * ExecHelpList:
 * @data: holds the element's data, which can be a pointer to any kind
 *        of data, or any integer value using the 
 *        [Type Conversion Macros][glib-Type-Conversion-Macros]
 * @next: contains the link to the next element in the list
 * @prev: contains the link to the previous element in the list
 *
 * The #ExecHelpList struct is used for each element in a doubly-linked list.
 **/

/**
 * exechelp_list_previous:
 * @list: an element in a #ExecHelpList
 *
 * A convenience macro to get the previous element in a #ExecHelpList.
 * Note that it is considered perfectly acceptable to access
 * @list->previous directly.
 *
 * Returns: the previous element, or %NULL if there are no previous
 *          elements
 **/

/**
 * exechelp_list_next:
 * @list: an element in a #ExecHelpList
 *
 * A convenience macro to get the next element in a #ExecHelpList.
 * Note that it is considered perfectly acceptable to access
 * @list->next directly.
 *
 * Returns: the next element, or %NULL if there are no more elements
 **/


/**
 * exechelp_list_alloc:
 *
 * Allocates space for one #ExecHelpList element. It is called by
 * exechelp_list_append(), exechelp_list_prepend(), exechelp_list_insert() and
 * exechelp_list_insert_sorted() and so is rarely used on its own.
 *
 * Returns: a pointer to the newly-allocated #ExecHelpList element
 **/
ExecHelpList *
exechelp_list_alloc (void)
{
  return malloc(sizeof(ExecHelpList) * 1);
}

/**
 * exechelp_list_free: 
 * @list: a #ExecHelpList
 *
 * Frees all of the memory used by a #ExecHelpList.
 * The freed elements are returned to the slice allocator.
 *
 * If list elements contain dynamically-allocated memory, you should
 * either use exechelp_list_free_full() or free them manually first.
 */
void
exechelp_list_free (ExecHelpList *list)
{
  if (list) {
    exechelp_list_free (list->next); 
    free (list);
  }
}

/**
 * exechelp_list_free_1:
 * @list: a #ExecHelpList element
 *
 * Frees one #ExecHelpList element, but does not update links from the next and
 * previous elements in the list, so you should not call this function on an
 * element that is currently part of a list.
 *
 * It is usually used after exechelp_list_remove_link().
 */
/**
 * exechelp_list_free1:
 *
 * Another name for exechelp_list_free_1().
 **/
void
exechelp_list_free_1 (ExecHelpList *list)
{
  free (list);
}

/**
 * exechelp_list_free_full:
 * @list: a pointer to a #ExecHelpList
 * @free_func: the function to be called to free each element's data
 *
 * Convenience method, which frees all the memory used by a #ExecHelpList,
 * and calls @free_func on every element's data.
 *
 * Since: 2.28
 */
void
exechelp_list_free_full (ExecHelpList          *list,
                  ExecHelpDestroyNotify  free_func)
{
  exechelp_list_foreach (list, (ExecHelpFunc) free_func, NULL);
  exechelp_list_free (list);
}

/**
 * exechelp_list_append:
 * @list: a pointer to a #ExecHelpList
 * @data: the data for the new element
 *
 * Adds a new element on to the end of the list.
 *
 * Note that the return value is the new start of the list,
 * if @list was empty; make sure you store the new value.
 *
 * exechelp_list_append() has to traverse the entire list to find the end,
 * which is inefficient when adding multiple elements. A common idiom
 * to avoid the inefficiency is to use exechelp_list_prepend() and reverse
 * the list with exechelp_list_reverse() when all elements have been added.
 *
 * |[<!-- language="C" -->
 * // Notice that these are initialized to the empty list.
 * ExecHelpList *strinexechelp_list = NULL, *number_list = NULL;
 *
 * // This is a list of strings.
 * strinexechelp_list = exechelp_list_append (strinexechelp_list, "first");
 * strinexechelp_list = exechelp_list_append (strinexechelp_list, "second");
 * 
 * // This is a list of integers.
 * number_list = exechelp_list_append (number_list, int_TO_POINTER (27));
 * number_list = exechelp_list_append (number_list, int_TO_POINTER (14));
 * ]|
 *
 * Returns: either @list or the new start of the #ExecHelpList if @list was %NULL
 */
ExecHelpList *
exechelp_list_append (ExecHelpList    *list,
               void *  data)
{
  ExecHelpList *new_list;
  ExecHelpList *last;
  
  new_list = malloc(sizeof (ExecHelpList));
  new_list->data = data;
  new_list->next = NULL;
  
  if (list)
    {
      last = exechelp_list_last (list);
      /* exechelp_assert (last != NULL); */
      last->next = new_list;
      new_list->prev = last;

      return list;
    }
  else
    {
      new_list->prev = NULL;
      return new_list;
    }
} 

/**
 * exechelp_list_prepend:
 * @list: a pointer to a #ExecHelpList, this must point to the top of the list
 * @data: the data for the new element
 *
 * Prepends a new element on to the start of the list.
 *
 * Note that the return value is the new start of the list,
 * which will have changed, so make sure you store the new value. 
 *
 * |[<!-- language="C" -->
 * // Notice that it is initialized to the empty list.
 * ExecHelpList *list = NULL;
 *
 * list = exechelp_list_prepend (list, "last");
 * list = exechelp_list_prepend (list, "first");
 * ]|
 *
 * Do not use this function to prepend a new element to a different
 * element than the start of the list. Use exechelp_list_insert_before() instead.
 *
 * Returns: a pointer to the newly prepended element, which is the new 
 *     start of the #ExecHelpList
 */
ExecHelpList *
exechelp_list_prepend (ExecHelpList    *list,
                void *  data)
{
  ExecHelpList *new_list;
  
  new_list = malloc(sizeof (ExecHelpList));
  new_list->data = data;
  new_list->next = list;
  
  if (list)
    {
      new_list->prev = list->prev;
      if (list->prev)
        list->prev->next = new_list;
      list->prev = new_list;
    }
  else
    new_list->prev = NULL;
  
  return new_list;
}

/**
 * exechelp_list_insert:
 * @list: a pointer to a #ExecHelpList, this must point to the top of the list
 * @data: the data for the new element
 * @position: the position to insert the element. If this is 
 *     negative, or is larger than the number of elements in the 
 *     list, the new element is added on to the end of the list.
 * 
 * Inserts a new element into the list at the given position.
 *
 * Returns: the (possibly changed) start of the #ExecHelpList
 */
ExecHelpList *
exechelp_list_insert (ExecHelpList    *list,
               void *  data,
               int      position)
{
  ExecHelpList *new_list;
  ExecHelpList *tmp_list;

  if (position < 0)
    return exechelp_list_append (list, data);
  else if (position == 0)
    return exechelp_list_prepend (list, data);

  tmp_list = exechelp_list_nth (list, position);
  if (!tmp_list)
    return exechelp_list_append (list, data);

  new_list = malloc(sizeof (ExecHelpList));
  new_list->data = data;
  new_list->prev = tmp_list->prev;
  tmp_list->prev->next = new_list;
  new_list->next = tmp_list;
  tmp_list->prev = new_list;

  return list;
}

/**
 * exechelp_list_insert_before:
 * @list: a pointer to a #ExecHelpList, this must point to the top of the list
 * @sibling: the list element before which the new element 
 *     is inserted or %NULL to insert at the end of the list
 * @data: the data for the new element
 *
 * Inserts a new element into the list before the given position.
 *
 * Returns: the (possibly changed) start of the #ExecHelpList
 */
ExecHelpList *
exechelp_list_insert_before (ExecHelpList    *list,
                      ExecHelpList    *sibling,
                      void *  data)
{
  if (!list)
    {
      list = exechelp_list_alloc ();
      list->data = data;
      return list;
    }
  else if (sibling)
    {
      ExecHelpList *node;

      node = malloc(sizeof (ExecHelpList));
      node->data = data;
      node->prev = sibling->prev;
      node->next = sibling;
      sibling->prev = node;
      if (node->prev)
        {
          node->prev->next = node;
          return list;
        }
      else
        {
          return node;
        }
    }
  else
    {
      ExecHelpList *last;

      last = list;
      while (last->next)
        last = last->next;

      last->next = malloc(sizeof (ExecHelpList));
      last->next->data = data;
      last->next->prev = last;
      last->next->next = NULL;

      return list;
    }
}

/**
 * exechelp_list_concat:
 * @list1: a #ExecHelpList, this must point to the top of the list
 * @list2: the #ExecHelpList to add to the end of the first #ExecHelpList,
 *     this must point  to the top of the list
 *
 * Adds the second #ExecHelpList onto the end of the first #ExecHelpList.
 * Note that the elements of the second #ExecHelpList are not copied.
 * They are used directly.
 *
 * This function is for example used to move an element in the list.
 * The following example moves an element to the top of the list:
 * |[<!-- language="C" -->
 * list = exechelp_list_remove_link (list, llink);
 * list = exechelp_list_concat (llink, list);
 * ]|
 *
 * Returns: the start of the new #ExecHelpList, which equals @list1 if not %NULL 
 */
ExecHelpList *
exechelp_list_concat (ExecHelpList *list1,
               ExecHelpList *list2)
{
  ExecHelpList *tmp_list;
  
  if (list2)
    {
      tmp_list = exechelp_list_last (list1);
      if (tmp_list)
        tmp_list->next = list2;
      else
        list1 = list2;
      list2->prev = tmp_list;
    }
  
  return list1;
}

static inline ExecHelpList *
_exechelp_list_remove_link (ExecHelpList *list,
                     ExecHelpList *link)
{
  if (link == NULL)
    return list;

  if (link->prev)
    {
      if (link->prev->next == link)
        link->prev->next = link->next;
      //else
        //exechelp_warning ("corrupted double-linked list detected");
    }
  if (link->next)
    {
      if (link->next->prev == link)
        link->next->prev = link->prev;
      //else
        //exechelp_warning ("corrupted double-linked list detected");
    }

  if (link == list)
    list = list->next;

  link->next = NULL;
  link->prev = NULL;

  return list;
}

/**
 * exechelp_list_remove:
 * @list: a #ExecHelpList, this must point to the top of the list
 * @data: the data of the element to remove
 *
 * Removes an element from a #ExecHelpList.
 * If two elements contain the same data, only the first is removed.
 * If none of the elements contain the data, the #ExecHelpList is unchanged.
 *
 * Returns: the (possibly changed) start of the #ExecHelpList
 */
ExecHelpList *
exechelp_list_remove (ExecHelpList         *list,
               const void *  data)
{
  ExecHelpList *tmp;

  tmp = list;
  while (tmp)
    {
      if (tmp->data != data)
        tmp = tmp->next;
      else
        {
          list = _exechelp_list_remove_link (list, tmp);
          free (tmp);

          break;
        }
    }
  return list;
}

/**
 * exechelp_list_remove_all:
 * @list: a #ExecHelpList, this must point to the top of the list
 * @data: data to remove
 *
 * Removes all list nodes with data equal to @data.
 * Returns the new head of the list. Contrast with
 * exechelp_list_remove() which removes only the first node
 * matching the given data.
 *
 * Returns: the (possibly changed) start of the #ExecHelpList
 */
ExecHelpList *
exechelp_list_remove_all (ExecHelpList         *list,
                   const void *  data)
{
  ExecHelpList *tmp = list;

  while (tmp)
    {
      if (tmp->data != data)
        tmp = tmp->next;
      else
        {
          ExecHelpList *next = tmp->next;

          if (tmp->prev)
            tmp->prev->next = next;
          else
            list = next;
          if (next)
            next->prev = tmp->prev;

          free (tmp);
          tmp = next;
        }
    }
  return list;
}

/**
 * exechelp_list_remove_link:
 * @list: a #ExecHelpList, this must point to the top of the list
 * @llink: an element in the #ExecHelpList
 *
 * Removes an element from a #ExecHelpList, without freeing the element.
 * The removed element's prev and next links are set to %NULL, so 
 * that it becomes a self-contained list with one element.
 *
 * This function is for example used to move an element in the list
 * (see the example for exechelp_list_concat()) or to remove an element in
 * the list before freeing its data:
 * |[<!-- language="C" --> 
 * list = exechelp_list_remove_link (list, llink);
 * free_some_data_that_may_access_the_list_again (llink->data);
 * exechelp_list_free (llink);
 * ]|
 *
 * Returns: the (possibly changed) start of the #ExecHelpList
 */
ExecHelpList *
exechelp_list_remove_link (ExecHelpList *list,
                    ExecHelpList *llink)
{
  return _exechelp_list_remove_link (list, llink);
}

/**
 * exechelp_list_delete_link:
 * @list: a #ExecHelpList, this must point to the top of the list
 * @link_: node to delete from @list
 *
 * Removes the node link_ from the list and frees it. 
 * Compare this to exechelp_list_remove_link() which removes the node 
 * without freeing it.
 *
 * Returns: the (possibly changed) start of the #ExecHelpList
 */
ExecHelpList *
exechelp_list_delete_link (ExecHelpList *list,
                    ExecHelpList *link_)
{
  list = _exechelp_list_remove_link (list, link_);
  free (link_);

  return list;
}

/**
 * exechelp_list_copy:
 * @list: a #ExecHelpList, this must point to the top of the list
 *
 * Copies a #ExecHelpList.
 *
 * Note that this is a "shallow" copy. If the list elements 
 * consist of pointers to data, the pointers are copied but 
 * the actual data is not. See exechelp_list_copy_deep() if you need
 * to copy the data as well.
 *
 * Returns: the start of the new list that holds the same data as @list
 */
ExecHelpList *
exechelp_list_copy (ExecHelpList *list)
{
  return exechelp_list_copy_deep (list, NULL, NULL);
}

/**
 * exechelp_list_copy_deep:
 * @list: a #ExecHelpList, this must point to the top of the list
 * @func: a copy function used to copy every element in the list
 * @user_data: user data passed to the copy function @func, or %NULL
 *
 * Makes a full (deep) copy of a #ExecHelpList.
 *
 * In contrast with exechelp_list_copy(), this function uses @func to make
 * a copy of each list element, in addition to copying the list
 * container itself.
 *
 * @func, as a #ExecHelpCopyFunc, takes two arguments, the data to be copied
 * and a @user_data pointer. It's safe to pass %NULL as user_data,
 * if the copy function takes only one argument.
 *
 * For instance, if @list holds a list of GObjects, you can do:
 * |[<!-- language="C" -->   
 * another_list = exechelp_list_copy_deep (list, (ExecHelpCopyFunc) exechelp_object_ref, NULL);
 * ]|
 *
 * And, to entirely free the new list, you could do:
 * |[<!-- language="C" --> 
 * exechelp_list_free_full (another_list, exechelp_object_unref);
 * ]|
 *
 * Returns: the start of the new list that holds a full copy of @list, 
 *     use exechelp_list_free_full() to free it
 *
 * Since: 2.34
 */
ExecHelpList *
exechelp_list_copy_deep (ExecHelpList     *list,
                  ExecHelpCopyFunc  func,
                  void *   user_data)
{
  ExecHelpList *new_list = NULL;

  if (list)
    {
      ExecHelpList *last;

      new_list = malloc(sizeof (ExecHelpList));
      if (func)
        new_list->data = func (list->data, user_data);
      else
        new_list->data = list->data;
      new_list->prev = NULL;
      last = new_list;
      list = list->next;
      while (list)
        {
          last->next = malloc(sizeof (ExecHelpList));
          last->next->prev = last;
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

/**
 * exechelp_list_reverse:
 * @list: a #ExecHelpList, this must point to the top of the list
 *
 * Reverses a #ExecHelpList.
 * It simply switches the next and prev pointers of each element.
 *
 * Returns: the start of the reversed #ExecHelpList
 */
ExecHelpList *
exechelp_list_reverse (ExecHelpList *list)
{
  ExecHelpList *last;
  
  last = NULL;
  while (list)
    {
      last = list;
      list = last->next;
      last->next = last->prev;
      last->prev = list;
    }
  
  return last;
}

/**
 * exechelp_list_nth:
 * @list: a #ExecHelpList, this must point to the top of the list
 * @n: the position of the element, counting from 0
 *
 * Gets the element at the given position in a #ExecHelpList.
 *
 * This iterates over the list until it reaches the @n-th position. If you
 * intend to iterate over every element, it is better to use a for-loop as
 * described in the #ExecHelpList introduction.
 *
 * Returns: the element, or %NULL if the position is off 
 *     the end of the #ExecHelpList
 */
ExecHelpList *
exechelp_list_nth (ExecHelpList *list,
            unsigned int  n)
{
  while ((n-- > 0) && list)
    list = list->next;
  
  return list;
}

/**
 * exechelp_list_nth_prev:
 * @list: a #ExecHelpList
 * @n: the position of the element, counting from 0
 *
 * Gets the element @n places before @list.
 *
 * Returns: the element, or %NULL if the position is 
 *     off the end of the #ExecHelpList
 */
ExecHelpList *
exechelp_list_nth_prev (ExecHelpList *list,
                 unsigned int  n)
{
  while ((n-- > 0) && list)
    list = list->prev;
  
  return list;
}

/**
 * exechelp_list_nth_data:
 * @list: a #ExecHelpList, this must point to the top of the list
 * @n: the position of the element
 *
 * Gets the data of the element at the given position.
 *
 * This iterates over the list until it reaches the @n-th position. If you
 * intend to iterate over every element, it is better to use a for-loop as
 * described in the #ExecHelpList introduction.
 *
 * Returns: the element's data, or %NULL if the position 
 *     is off the end of the #ExecHelpList
 */
void *
exechelp_list_nth_data (ExecHelpList *list,
                 unsigned int  n)
{
  while ((n-- > 0) && list)
    list = list->next;
  
  return list ? list->data : NULL;
}

/**
 * exechelp_list_find:
 * @list: a #ExecHelpList, this must point to the top of the list
 * @data: the element data to find
 *
 * Finds the element in a #ExecHelpList which contains the given data.
 *
 * Returns: the found #ExecHelpList element, or %NULL if it is not found
 */
ExecHelpList *
exechelp_list_find (ExecHelpList         *list,
             const void *  data)
{
  while (list)
    {
      if (list->data == data)
        break;
      list = list->next;
    }
  
  return list;
}

/**
 * exechelp_list_find_custom:
 * @list: a #ExecHelpList, this must point to the top of the list
 * @data: user data passed to the function
 * @func: the function to call for each element. 
 *     It should return 0 when the desired element is found
 *
 * Finds an element in a #ExecHelpList, using a supplied function to 
 * find the desired element. It iterates over the list, calling 
 * the given function which should return 0 when the desired 
 * element is found. The function takes two #const void * arguments, 
 * the #ExecHelpList element's data as the first argument and the 
 * given user data.
 *
 * Returns: the found #ExecHelpList element, or %NULL if it is not found
 */
ExecHelpList *
exechelp_list_find_custom (ExecHelpList         *list,
                    const void *  data,
                    ExecHelpCompareFunc   func)
{
  if (!func)
    return list;

  while (list)
    {
      if (! func (list->data, data))
        return list;
      list = list->next;
    }

  return NULL;
}

/**
 * exechelp_list_position:
 * @list: a #ExecHelpList, this must point to the top of the list
 * @llink: an element in the #ExecHelpList
 *
 * Gets the position of the given element 
 * in the #ExecHelpList (starting from 0).
 *
 * Returns: the position of the element in the #ExecHelpList, 
 *     or -1 if the element is not found
 */
int
exechelp_list_position (ExecHelpList *list,
                 ExecHelpList *llink)
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

/**
 * exechelp_list_index:
 * @list: a #ExecHelpList, this must point to the top of the list
 * @data: the data to find
 *
 * Gets the position of the element containing 
 * the given data (starting from 0).
 *
 * Returns: the index of the element containing the data, 
 *     or -1 if the data is not found
 */
int
exechelp_list_index (ExecHelpList         *list,
              const void *  data)
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

/**
 * exechelp_list_last:
 * @list: any #ExecHelpList element
 *
 * Gets the last element in a #ExecHelpList.
 *
 * Returns: the last element in the #ExecHelpList,
 *     or %NULL if the #ExecHelpList has no elements
 */
ExecHelpList *
exechelp_list_last (ExecHelpList *list)
{
  if (list)
    {
      while (list->next)
        list = list->next;
    }
  
  return list;
}

/**
 * exechelp_list_first:
 * @list: any #ExecHelpList element
 *
 * Gets the first element in a #ExecHelpList.
 *
 * Returns: the first element in the #ExecHelpList, 
 *     or %NULL if the #ExecHelpList has no elements
 */
ExecHelpList *
exechelp_list_first (ExecHelpList *list)
{
  if (list)
    {
      while (list->prev)
        list = list->prev;
    }
  
  return list;
}

/**
 * exechelp_list_length:
 * @list: a #ExecHelpList, this must point to the top of the list
 *
 * Gets the number of elements in a #ExecHelpList.
 *
 * This function iterates over the whole list to count its elements.
 * Use a #GQueue instead of a ExecHelpList if you regularly need the number
 * of items. To check whether the list is non-empty, it is faster to check
 * @list against %NULL.
 *
 * Returns: the number of elements in the #ExecHelpList
 */
unsigned int
exechelp_list_length (ExecHelpList *list)
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

/**
 * exechelp_list_foreach:
 * @list: a #ExecHelpList, this must point to the top of the list
 * @func: the function to call with each element's data
 * @user_data: user data to pass to the function
 *
 * Calls a function for each element of a #ExecHelpList.
 */
/**
 * ExecHelpFunc:
 * @data: the element's data
 * @user_data: user data passed to exechelp_list_foreach() or exechelp_slist_foreach()
 *
 * Specifies the type of functions passed to exechelp_list_foreach() and
 * exechelp_slist_foreach().
 */
void
exechelp_list_foreach (ExecHelpList    *list,
                ExecHelpFunc     func,
                void *  user_data)
{
  while (list)
    {
      ExecHelpList *next = list->next;
      (*func) (list->data, user_data);
      list = next;
    }
}

static ExecHelpList*
exechelp_list_insert_sorted_real (ExecHelpList    *list,
                           void *  data,
                           ExecHelpFunc     func,
                           void *  user_data)
{
  ExecHelpList *tmp_list = list;
  ExecHelpList *new_list;
  int cmp;

  if (!func)
    return list;
  
  if (!list) 
    {
      new_list = malloc(sizeof (ExecHelpList));
      new_list->data = data;
      return new_list;
    }
  
  cmp = ((ExecHelpCompareDataFunc) func) (data, tmp_list->data, user_data);

  while ((tmp_list->next) && (cmp > 0))
    {
      tmp_list = tmp_list->next;

      cmp = ((ExecHelpCompareDataFunc) func) (data, tmp_list->data, user_data);
    }

  new_list = malloc(sizeof (ExecHelpList));
  new_list->data = data;

  if ((!tmp_list->next) && (cmp > 0))
    {
      tmp_list->next = new_list;
      new_list->prev = tmp_list;
      return list;
    }
   
  if (tmp_list->prev)
    {
      tmp_list->prev->next = new_list;
      new_list->prev = tmp_list->prev;
    }
  new_list->next = tmp_list;
  tmp_list->prev = new_list;
 
  if (tmp_list == list)
    return new_list;
  else
    return list;
}

/**
 * exechelp_list_insert_sorted:
 * @list: a pointer to a #ExecHelpList, this must point to the top of the
 *     already sorted list
 * @data: the data for the new element
 * @func: the function to compare elements in the list. It should 
 *     return a number > 0 if the first parameter comes after the 
 *     second parameter in the sort order.
 *
 * Inserts a new element into the list, using the given comparison 
 * function to determine its position.
 *
 * If you are adding many new elements to a list, and the number of
 * new elements is much larger than the length of the list, use
 * exechelp_list_prepend() to add the new items and sort the list afterwards
 * with exechelp_list_sort().
 *
 * Returns: the (possibly changed) start of the #ExecHelpList
 */
ExecHelpList *
exechelp_list_insert_sorted (ExecHelpList        *list,
                      void *      data,
                      ExecHelpCompareFunc  func)
{
  return exechelp_list_insert_sorted_real (list, data, (ExecHelpFunc) func, NULL);
}

/**
 * exechelp_list_insert_sorted_with_data:
 * @list: a pointer to a #ExecHelpList, this must point to the top of the
 *     already sorted list
 * @data: the data for the new element
 * @func: the function to compare elements in the list. It should
 *     return a number > 0 if the first parameter  comes after the
 *     second parameter in the sort order.
 * @user_data: user data to pass to comparison function
 *
 * Inserts a new element into the list, using the given comparison 
 * function to determine its position.
 *
 * If you are adding many new elements to a list, and the number of
 * new elements is much larger than the length of the list, use
 * exechelp_list_prepend() to add the new items and sort the list afterwards
 * with exechelp_list_sort().
 *
 * Returns: the (possibly changed) start of the #ExecHelpList
 *
 * Since: 2.10
 */
ExecHelpList *
exechelp_list_insert_sorted_with_data (ExecHelpList            *list,
                                void *          data,
                                ExecHelpCompareDataFunc  func,
                                void *          user_data)
{
  return exechelp_list_insert_sorted_real (list, data, (ExecHelpFunc) func, user_data);
}

static ExecHelpList *
exechelp_list_sort_merge (ExecHelpList     *l1, 
                   ExecHelpList     *l2,
                   ExecHelpFunc     compare_func,
                   void *  user_data)
{
  ExecHelpList list, *l, *lprev;
  int cmp;

  l = &list; 
  lprev = NULL;

  while (l1 && l2)
    {
      cmp = ((ExecHelpCompareDataFunc) compare_func) (l1->data, l2->data, user_data);

      if (cmp <= 0)
        {
          l->next = l1;
          l1 = l1->next;
        } 
      else 
        {
          l->next = l2;
          l2 = l2->next;
        }
      l = l->next;
      l->prev = lprev; 
      lprev = l;
    }
  l->next = l1 ? l1 : l2;
  l->next->prev = l;

  return list.next;
}

static ExecHelpList * 
exechelp_list_sort_real (ExecHelpList    *list,
                  ExecHelpFunc     compare_func,
                  void *  user_data)
{
  ExecHelpList *l1, *l2;
  
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
      l1 = l1->next;
    }
  l2 = l1->next; 
  l1->next = NULL; 

  return exechelp_list_sort_merge (exechelp_list_sort_real (list, compare_func, user_data),
                            exechelp_list_sort_real (l2, compare_func, user_data),
                            compare_func,
                            user_data);
}

/**
 * exechelp_list_sort:
 * @list: a #ExecHelpList, this must point to the top of the list
 * @compare_func: the comparison function used to sort the #ExecHelpList.
 *     This function is passed the data from 2 elements of the #ExecHelpList 
 *     and should return 0 if they are equal, a negative value if the 
 *     first element comes before the second, or a positive value if 
 *     the first element comes after the second.
 *
 * Sorts a #ExecHelpList using the given comparison function. The algorithm 
 * used is a stable sort.
 *
 * Returns: the (possibly changed) start of the #ExecHelpList
 */
/**
 * ExecHelpCompareFunc:
 * @a: a value
 * @b: a value to compare with
 *
 * Specifies the type of a comparison function used to compare two
 * values.  The function should return a negative integer if the first
 * value comes before the second, 0 if they are equal, or a positive
 * integer if the first value comes after the second.
 *
 * Returns: negative value if @a < @b; zero if @a = @b; positive
 *          value if @a > @b
 */
ExecHelpList *
exechelp_list_sort (ExecHelpList        *list,
             ExecHelpCompareFunc  compare_func)
{
  return exechelp_list_sort_real (list, (ExecHelpFunc) compare_func, NULL);
}

/**
 * exechelp_list_sort_with_data:
 * @list: a #ExecHelpList, this must point to the top of the list
 * @compare_func: comparison function
 * @user_data: user data to pass to comparison function
 *
 * Like exechelp_list_sort(), but the comparison function accepts 
 * a user data argument.
 *
 * Returns: the (possibly changed) start of the #ExecHelpList
 */
/**
 * ExecHelpCompareDataFunc:
 * @a: a value
 * @b: a value to compare with
 * @user_data: user data
 *
 * Specifies the type of a comparison function used to compare two
 * values.  The function should return a negative integer if the first
 * value comes before the second, 0 if they are equal, or a positive
 * integer if the first value comes after the second.
 *
 * Returns: negative value if @a < @b; zero if @a = @b; positive
 *          value if @a > @b
 */
ExecHelpList *
exechelp_list_sort_with_data (ExecHelpList            *list,
                       ExecHelpCompareDataFunc  compare_func,
                       void *          user_data)
{
  return exechelp_list_sort_real (list, (ExecHelpFunc) compare_func, user_data);
}
