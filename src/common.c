/* 
    2015 (c) Steve Dodier-Lazaro <sidnioulz@gmail.com>
    This file is part of ExecHelper.

    ExecHelper is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ExecHelper is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ExecHelper.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _GNU_SOURCE

#include "common.h"
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


/***** in the server-side, to generate the list *****/
//TODO generic app-group-list, initialised from /etc/firejai/*profiles in firejail and passed to helper
//TODO apt plugin to auto-feed app-group-list on queries
//TODO use app-group-list for associated apps


char *exechelp_resolve_path(const char *file)
{
  if (!file || *file == '\0')
    return NULL;

  /* Don't search when it contains a slash.  */
  if (strchr(file, '/') != NULL)
  {
    char *real = realpath(file, NULL);
    return real;
  }
  else
  {
    size_t pathlen;
    size_t alloclen = 0;
    char *path = getenv("PATH");
    if (path == NULL)
    {
      pathlen = confstr(_CS_PATH, (char *) NULL, 0);
      alloclen = pathlen + 1;
    }
    else
      pathlen = strlen(path);

    size_t len = strlen(file) + 1;
    alloclen += pathlen + len + 1;

    char *name;
    char *path_malloc = NULL;
    path_malloc = name = malloc(alloclen);
    if (name == NULL)
      return NULL;

    if (path == NULL)
    {
      /* There is no `PATH' in the environment.
         The default search path is the current directory
         followed by the path `confstr' returns for `_CS_PATH'.  */
      path = name + pathlen + len + 1;
      path[0] = ':';
      (void) confstr(_CS_PATH, path + 1, pathlen);
    }

    /* Copy the file name at the top.  */
    name = (char *) memcpy(name + pathlen + 1, file, len);
    /* And add the slash.  */
    *--name = '/';

    char *return_value = NULL;
    short succeeded = 0;
    short got_eacces = 0;
    char *p = path;
    do
    {
      char *startp;

      path = p;
      p = strchrnul(path, ':');

      if (p == path)
        /* Two adjacent colons, or a colon at the beginning or the end
           of `PATH' means to search the current directory.  */
        startp = name + 1;
      else
        startp = (char *) memcpy(name - (p - path), path, p - path);
      
      /* Try to execute this name.  If it works, execve will not return. */
      errno = 0;
      if (access(startp, X_OK))
      {
        switch (errno)
        {
          case EACCES:
            /* Record the we got a `Permission denied' error.  If we end
               up finding no executable we can use, we want to diagnose
               that we did find one but were denied access.  */
            got_eacces = 1;
          case ENOENT:
          case ENAMETOOLONG:
          case ENOTDIR:
          case ELOOP:
            /* Those errors indicate the file is missing or not executable
               by us, in which case we want to just try the next path
               directory.  */
          case EROFS:
          case ETXTBSY:
            /* Some strange filesystems like AFS return even
               stranger error numbers.  They cannot reasonably mean
               anything else so ignore those, too.  */
            break;

          default:
            /* Some other error means we found an executable file, but
               something went wrong executing it; return the error to our
               caller.  */
            return NULL;
        }
      }
      else
      {
        succeeded = 1;
        return_value = strdup(startp);
      }
    }
    while (*p++ != '\0' && !succeeded);

    /* At least one failure was due to permissions, so report that
       error.  */
    if (got_eacces)
      errno = EACCES;

    free (path_malloc);

    return return_value;
  }
}

#define SELF_PATH_MAX 4096
char *exechelp_get_self_name()
{
  char buf[SELF_PATH_MAX];
  size_t read = readlink("/proc/self/exe", buf, SELF_PATH_MAX);

  if (read == -1)
    return strdup(EXECHELP_NULL_BINARY_PATH);
  else
    return strndup(buf, read);
}

char *exechelp_read_list_from_file(const char *file_path)
{
  static ExecHelpHashTable *cache = NULL;
  static ExecHelpHashTable *mtime = NULL;

  if (!cache || !mtime)
  {
    if (cache)
    {
      exechelp_hash_table_destroy(cache);
      cache = NULL;
    }
    if (mtime)
    {
      exechelp_hash_table_destroy(mtime);
      mtime = NULL;
    }
    cache = exechelp_hash_table_new_full(exechelp_str_hash, exechelp_str_equal, NULL, free);
    mtime = exechelp_hash_table_new_full(exechelp_str_hash, exechelp_str_equal, NULL, NULL);

    if (!cache || !mtime)
      return NULL;
  }

  struct stat sb;
  time_t last_access = 0;
  time_t cached_access = 0;
  int must_refresh = 1;

  if(stat(file_path, &sb) == 0)
    last_access = sb.st_mtim.tv_sec;
  if (exechelp_hash_table_contains(mtime, file_path))
    cached_access = EH_POINTER_TO_ULONG(exechelp_hash_table_lookup(mtime, file_path));
  must_refresh = (last_access > cached_access);
    
  if (must_refresh)
  {
    FILE *f = fopen(file_path, "rb");
    if(f)
    {
      fseek(f, 0, SEEK_END);
      long fsize = ftell(f);
      rewind(f);
      
      char *new_list = malloc(sizeof(char) * (fsize + 1));
      if(new_list)
      {
        int read = fread(new_list, 1, fsize, f);
        new_list[read] = 0;

        fclose(f);

        exechelp_hash_table_remove(cache, file_path);
        exechelp_hash_table_remove(mtime, file_path);

        exechelp_hash_table_insert(cache, (void *)file_path, new_list);
        exechelp_hash_table_insert(mtime, (void *)file_path, EH_ULONG_TO_POINTER(last_access));

        return new_list;
      }

      fclose(f);
    }
  }

  char *list = exechelp_hash_table_lookup(cache, file_path);
  return list;
}

int exechelp_str_has_prefix(const char *str, const char *prefix)
{
  if (!str || !prefix)
    return 0;

  size_t i = 0;
  for(;str[i] && prefix[i]; ++i)
  {
    if (str[i] != prefix[i])
      return 0;
  }

  return !prefix[i];
}

int exechelp_str_has_prefix_on_sep(const char *str, const char *prefix, const char sep)
{
  if (!str || !prefix)
    return 0;

  size_t i = 0;
  for (;str[i] && prefix[i] && prefix[i]!=sep; ++i)
  {
    if (str[i] != prefix[i] && prefix[i]!=sep)
    {
      return 0;
    }
  }

  return (prefix[i] == '\0' || prefix[i] == sep);
}

ExecHelpBinaryAssociations *exechelp_get_binary_associations()
{
  static ExecHelpBinaryAssociations *assoc = NULL;

  if(!assoc)
  {
    //TODO initialise by reading app profiles
    
    assoc = malloc (sizeof(ExecHelpBinaryAssociations));
    assoc->assoc = NULL;
    assoc->index = exechelp_hash_table_new(exechelp_str_hash, exechelp_str_equal);

    ExecHelpSList *firefox = NULL;
    firefox = exechelp_slist_prepend(firefox, "/usr/lib/firefox/firefox");
    exechelp_hash_table_insert(assoc->index,  "/usr/lib/firefox/firefox", "/usr/bin/firefox");
    firefox = exechelp_slist_prepend(firefox, "/usr/lib/firefox/plugin-container");
    exechelp_hash_table_insert(assoc->index,  "/usr/lib/firefox/plugin-container", "/usr/bin/firefox");
    firefox = exechelp_slist_prepend(firefox, "/usr/lib/firefox/webapprt-stub");
    exechelp_hash_table_insert(assoc->index,  "/usr/lib/firefox/webapprt-stub", "/usr/bin/firefox");
    firefox = exechelp_slist_prepend(firefox, "/usr/bin/firefox");
    exechelp_hash_table_insert(assoc->index,  "/usr/bin/firefox", "/usr/bin/firefox");
    assoc->assoc = exechelp_slist_prepend(assoc->assoc, firefox);

    ExecHelpSList *vlc = NULL;
    vlc = exechelp_slist_prepend(vlc, "/usr/bin/cvlc");
    exechelp_hash_table_insert(assoc->index,  "/usr/bin/cvlc", "/usr/bin/vlc");
    vlc = exechelp_slist_prepend(vlc, "/usr/bin/vlc-wrapper");
    exechelp_hash_table_insert(assoc->index,  "/usr/bin/vlc-wrapper", "/usr/bin/vlc");
    vlc = exechelp_slist_prepend(vlc, "/usr/lib/vlc/vlc-cache-gen");
    exechelp_hash_table_insert(assoc->index,  "/usr/bin/vlc-cache-gen", "/usr/bin/vlc");
//    vlc = exechelp_slist_prepend(vlc, "/home/steve/Development/ExecHelper/exec-helper-test");
//    exechelp_hash_table_insert(assoc->index,  "/home/steve/Development/ExecHelper/exec-helper-test", "/usr/bin/vlc");
    vlc = exechelp_slist_prepend(vlc, "/usr/bin/vlc");
    exechelp_hash_table_insert(assoc->index,  "/usr/bin/vlc", "/usr/bin/vlc");
    assoc->assoc = exechelp_slist_prepend(assoc->assoc, vlc);

    ExecHelpSList *thunar = NULL;
    thunar = exechelp_slist_prepend(thunar, "/usr/bin/thunar-settings");
    exechelp_hash_table_insert(assoc->index,  "/usr/bin/thunar-settings", "/usr/bin/thunar");
    thunar = exechelp_slist_prepend(thunar, "/usr/bin/thunar-volman");
    exechelp_hash_table_insert(assoc->index,  "/usr/bin/volman", "/usr/bin/thunar");
    thunar = exechelp_slist_prepend(thunar, "/usr/bin/thunar-volman-settings");
    exechelp_hash_table_insert(assoc->index,  "/usr/bin/thunar-volman-settings", "/usr/bin/thunar");
    thunar = exechelp_slist_prepend(thunar, "/usr/bin/thunar");
    exechelp_hash_table_insert(assoc->index,  "/usr/bin/thunar", "/usr/bin/thunar");
    assoc->assoc = exechelp_slist_prepend(assoc->assoc, thunar);
  }

  return assoc;
}

ExecHelpSList *exechelp_get_associations_for_main_binary(ExecHelpBinaryAssociations *assoc, const char *mainkey)
{
  if(!assoc || !mainkey)
    return NULL;

  ExecHelpSList *assocs = assoc->assoc;
  while (assocs)
  {
    if (assocs->data && (strcmp(((ExecHelpSList *)(assocs->data))->data, mainkey) == 0))
      break;
    DEBUG2 ("DEBUG: %p becomes %p\n", assocs, assocs->next);
    assocs = assocs->next;
  }
  if (assocs)
    assocs = assocs->data;

  return assocs;
}

int exechelp_is_associated_helper(const char *caller, const char *callee)
{
  int associated = 0;

  if (caller && callee)
  {
    DEBUG2("DEBUG: caller is '%s', callee is '%s'\n", caller, callee);

    ExecHelpBinaryAssociations *assoc = exechelp_get_binary_associations();

    if (exechelp_hash_table_contains(assoc->index, caller))
    {
      DEBUG2("DEBUG: %s", "caller has associations with other apps\n");
      const char *mainkey = exechelp_hash_table_lookup(assoc->index, caller);

      if (mainkey)
      {
        DEBUG2("DEBUG: caller's parent app is %s\n", mainkey);
        ExecHelpSList *assocs = exechelp_get_associations_for_main_binary(assoc, mainkey);

        if (assocs)
          associated = exechelp_slist_find_custom(assocs, callee, (ExecHelpCompareFunc)strcmp) != NULL;
      }
      else
        DEBUG2("DEBUG: %s", "caller's parent app could not be found\n");
    }
    else
    {
      //TODO read the information from the package manager, and insert whatever is found in assoc
      DEBUG2("DEBUG: %s", "caller is not associated with other apps\n");
    }
  }

  DEBUG2 ("DEBUG: callee '%s' is %sin the list of associated apps for caller '%s'\n", callee, (associated? "":"not "), caller);
  return associated;
}

char *exechelp_extract_associations_for_binary(const char *receiving_binary)
{
  if (receiving_binary)
  {
    DEBUG2("DEBUG: extracting the binary associations for '%s'\n", receiving_binary);

    ExecHelpBinaryAssociations *assoc = exechelp_get_binary_associations();

    if (exechelp_hash_table_contains(assoc->index, receiving_binary))
    {
      DEBUG2("DEBUG: %s", "receiving binary has associations with other apps\n");
      const char *mainkey = exechelp_hash_table_lookup(assoc->index, receiving_binary);

      if (mainkey)
      {
        DEBUG2("DEBUG: receiving binary's parent app is %s\n", mainkey);
        ExecHelpSList *assocs = exechelp_get_associations_for_main_binary(assoc, mainkey);

        char *names = malloc(sizeof(char));
        names[0] = '\0';
        ExecHelpSList *iter = assocs;
        while(iter && names)
        {
          char *prev_names = names;
          char *data = iter->data;

          size_t new_len = strlen(names) + strlen(data) + EXECHELP_LIST_SEPARATOR_LEN + 1;
          names = malloc(sizeof(char) * new_len);
          if (names)
            snprintf(names, new_len, "%s%s%s", prev_names, (prev_names[0] != '\0' ? EXECHELP_LIST_SEPARATOR:""), data);

          free (prev_names);
          iter = iter->next;
        }

        return names;
      }
      else
        DEBUG2("DEBUG: %s", "receiving binary's parent app could not be found\n");
    }
    else
      DEBUG2("DEBUG: %s", "receiving binary is not associated with other apps\n");
  }

  return "";
}

int exechelp_file_list_contains_path(const char *managed, const char *real)
{
  const char *iter = managed;
  int matched_a_line = 0;
  
  while(iter && iter[0]!='\0' && !matched_a_line)
  {
    //printf("testing %s vs %s\n", real, iter);
    matched_a_line = exechelp_str_has_prefix_on_sep(real, iter, EXECHELP_FILE_SEPARATOR_CHR);
    iter = strstr(iter, EXECHELP_FILE_SEPARATOR);
    if (iter)
      iter++;
  }

  return matched_a_line;
}

void *exechelp_malloc0(size_t size)
{
  void *mem = malloc(size);
  if (mem)
    memset(mem, 0, size);
  return mem;
}

void *exechelp_memdup (const void *mem, unsigned int byte_size)
{
  void *new_mem;

  if (mem)
  {
    new_mem = malloc(byte_size);
    memcpy(new_mem, mem, byte_size);
  }
  else
    new_mem = NULL;

  return new_mem;
}

