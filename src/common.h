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

#include <stdio.h>
#include "hash.h"
#include "slist.h"

/* Debug macros */
#ifndef DEBUGLVL
#define DEBUGLVL 1
#endif

#define DEBUG(fmt, ...) \
    do { if (DEBUGLVL) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#define DEBUG2(fmt, ...) \
    do { if (DEBUGLVL>1) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

/* Constants */
#define EXECHELP_NULL_BINARY_PATH         "/dev/null"
#define EXECHELP_MONITORED_EXEC_PATH      "/firejail/denied/"

#define EXECHELP_HELPER_BINS_PATH         "/etc/firejail/self/helper-bins.list"
#define EXECHELP_MANAGED_BINS_PATH        "/etc/firejail/self/managed-bins.list"
#define EXECHELP_MANAGED_FILES_PATH       "/etc/firejail/self/managed-files.list"
#define EXECHELP_FILE_SEPARATOR           "\n"
#define EXECHELP_FILE_SEPARATOR_CHR       '\n'
#define EXECHELP_LIST_SEPARATOR           ":"
#define EXECHELP_LIST_SEPARATOR_LEN       1

#define EXECHELP_ENV_ASSOCIATIONS         "FIREJAIL_ASSOCIATIONS"
#define EXECHELP_ENV_SANDBOX_MANAGED      "FIREJAIL_SANDBOX_MANAGED"
#define EXECHELP_ENV_SANDBOX_FILES        "FIREJAIL_SANDBOX_FILES"

extern char **environ;

/* General utilities */
char *exechelp_resolve_path(const char *target);
char *exechelp_get_self_name();
char *exechelp_read_list_from_file(const char *file_path);
int exechelp_str_has_prefix(const char *str, const char *prefix);
int exechelp_str_has_prefix_on_sep(const char *str, const char *prefix, const char sep);
int exechelp_file_list_contains_path(const char *managed, const char *real);

/* Execution policy */
typedef enum _ExecHelpExecutionPolicy {
  NOTHING = 0,
  HELPERS = 1,
  UNSPECIFIED = 1 << 1,
  SANDBOX_MANAGED = 1 << 2,
  SANDBOX_ITSELF = 1 << 3
} ExecHelpExecutionPolicy;
#define EXECHELP_DEFAULT_POLICY           HELPERS | UNSPECIFIED

/* Binary association structure */
typedef struct _ExecHelpBinaryAssociations {
  ExecHelpSList     *assoc;
  ExecHelpHashTable *index;
} ExecHelpBinaryAssociations;

ExecHelpBinaryAssociations *exechelp_get_binary_associations();
ExecHelpSList *exechelp_get_associations_for_main_binary(ExecHelpBinaryAssociations *assoc, const char *mainkey);
int exechelp_is_associated_helper(const char *caller, const char *callee);
char *exechelp_extract_associations_for_binary(const char *receiving_binary);

/* Memory functions */
void *exechelp_malloc0(size_t size);
void *exechelp_memdup (const void *mem, unsigned int byte_size);
