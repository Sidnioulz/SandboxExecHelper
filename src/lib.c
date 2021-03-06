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

#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "common.h"
#include "realpath.h"

/**
 * @fn exechelp_is_associated_helper_client
 * @brief Tells whether a specific binary is associated with the currently
 * running program by its sandbox profile
 *
 * @param target: the full path of the binary to be queried
 * @return 1 if target is associated with the running app, 0 otherwise
 */
static int exechelp_is_associated_helper_client(const char *target)
{
  DEBUG("Child process determining whether '%s' is an authorised helper program...", target);
  int found = 0;

  if (!target)
    goto ret;

  char *associations = exechelp_read_list_from_file(EXECHELP_HELPER_BINS_PATH);
  if (!associations)
    goto ret;

  found = (strstr(associations, target) != NULL);

  ret:
  DEBUG(" %s\n", (found? "Yes" : "No"));
  return found;
}

/**
 * @fn exechelp_is_sandbox_managed_app
 * @brief Tells whether a specific binary should be executed in a special
 * environment by the sandbox helper rather than in the current process
 *
 * @param target: the full path of the binary to be queried
 * @return 1 if target is to be managed by the sandbox, 0 otherwise
 */
static int exechelp_is_sandbox_managed_app(const char *target)
{
  DEBUG("Child process determining whether '%s' is a forbidden program within the sandbox...", target);
  int found = 0;

  if (!target)
    goto ret;

  char *managed = exechelp_read_list_from_file(EXECHELP_MANAGED_BINS_PATH);
  if (!managed)
    goto ret;

  found = (strstr(managed, target) != NULL);

  ret:
  DEBUG(" %s\n", (found? "Yes" : "No"));
  return found;
}

/**
 * @fn exechelp_targets_sandbox_managed_file
 * @brief Tries its best to determine whether an execve call is likely to
 * access a resource registered to only be processed in a special sandbox
 * profile. For instance, if /home/user/Secure/foo.pdf must only be opened
 * in a tightened sandbox, execve'ing this file in an image
 *
 * @param target: the full path of the binary to be queried
 * @param argv: the list of arguments forwarded to execve
 * @return a -1 terminated array with the indexes of all the files to be
 * opened by the trusted sandbox helper rather than this local app, or 
 * NULL if it is safe to open all the files in this app's sandbox.
 */
static ExecHelpExecutionPolicy *exechelp_targets_sandbox_managed_file(const char *target, char *const argv[])
{
  DEBUG("Child process determining whether arguments passed to execve('%s') contain forbidden files...", target);
  DEBUG2("%s", "\n");

  char *managed = exechelp_read_list_from_file(EXECHELP_MANAGED_FILES_PATH);
  if (!managed)
  {
    DEBUG2("DEBUG: Could not find a list of sandbox-managed files to check arguments before executing '%s'", target);
    return 0;
  }

  ExecHelpExecutionPolicy *ret = NULL;
  int len = 0, some_forbidden = 0;
  for(;argv[len];++len);
  ret = exechelp_malloc0(sizeof(ExecHelpExecutionPolicy) * (len+1));
  ret[0] = HELPERS; /* Just to make the array nicer to loop through, mark executable as a helper */

  DEBUG2("DEBUG: %d arguments will be examined\n", len);
  for(len=1;argv[len];++len)
  {
    const char *arg = argv[len];
    char *real = exechelp_coreutils_realpath(arg);
    DEBUG2("DEBUG: checking if argument %d ('%s') is to be managed by the sandbox\n", len, arg);

    short is_file = strchr(arg, '/') != NULL;
    if(!is_file)
    {
      struct stat sb;
      if(stat(real, &sb) == 0)
        is_file = 1;
      else
        is_file = (errno == EACCES || errno == ELOOP || errno == EOVERFLOW) ? 1:0;
    }

    if (is_file)
      DEBUG2("DEBUG: \t\t'%s' is believed to be a file, located at '%s'\n", arg, real);
    else
      DEBUG2("DEBUG: \t\t'%s' is believed not to be a file\n", arg);

    if(exechelp_file_list_contains_path(managed, real))
    {
      DEBUG2("DEBUG: \t\t'%s' is forbidden within the sandbox\n", arg);
      ret[len] = SANDBOX_MANAGED;
      some_forbidden = 1;
    }
    else
    {
      DEBUG2("DEBUG: \t\t'%s' is allowed within the sandbox\n", arg);
      ret[len] = UNSPECIFIED;
    }

    ++arg;
  }

  DEBUG2("%s", "Found forbidden files in arguments?");
  DEBUG(" %s\n", some_forbidden? "Yes" : "No");
  return ret;
}

static int exechelp_filter_forbidden_exec(const char *target, char *const argv[], char *const envp[],
                                          char **allowed_target, char **allowed_argv[],
                                          char **forbidden_target, char **forbidden_argv[])
{
  if(!target || !argv)
    return 0;
  size_t arg_len = 0;
  while(argv[arg_len++]);

  ExecHelpExecutionPolicy pol = EXECHELP_DEFAULT_POLICY;

  if (exechelp_is_associated_helper_client (target) && (pol & HELPERS))
    goto binary_clear;
  else if (exechelp_is_sandbox_managed_app (target) && (pol & SANDBOX_MANAGED))
    goto binary_clear;
  else if (pol & UNSPECIFIED)
    goto binary_clear;
  else
    goto binary_forbidden;

  binary_clear:
  {
    DEBUG2("DEBUG: Child process can partly or completely execute '%s', now checking parameters...\n", target);

    ExecHelpExecutionPolicy *decisions = exechelp_targets_sandbox_managed_file(target, argv);
    ExecHelpExecutionPolicy *iter = decisions + 1;
    int i = 1, have_forbidden = 0;
    while (*iter)
    {
      have_forbidden |= !(*iter & (HELPERS | UNSPECIFIED));
      ++iter;
      ++i;
    }

    /* Don't do anything fancy yet for mixed forbidden-allowed executions, just
     * delegate to the sandbox but let it know to expect a mixed setup, we can
     * then ask users how they want the execution handled
     */
    if (have_forbidden)
      goto binary_forbidden;

    DEBUG2("DEBUG: Child process is allowed to execute '%s' and to access all of its parameters, proceeding\n", target);
    *allowed_target = strdup(target);
    *allowed_argv = arg_len? malloc(sizeof(char *) * arg_len) : NULL;
    *allowed_argv = memcpy(*allowed_argv, argv, sizeof(char *) * (arg_len+1));

    return 1;
  }

  binary_forbidden:
  {
    DEBUG2("DEBUG: Child process is not allowed to execute '%s', or some parameters are are not allowed; delegating the whole execution\n", target);
    *forbidden_target = strdup(target);
    *forbidden_argv = arg_len? malloc(sizeof(char *) * (arg_len+1)) : NULL;
    *forbidden_argv = memcpy(*forbidden_argv, argv, sizeof(char *) * (arg_len+1));

    return 0;
  }
}

/* int execl(const char *path, const char *arg, ...) will call execve */
/* int execle(const char *path, const char *arg, ...) will call execve */
/* int execlp(const char *file, const char *arg, ...) will call execvp */
/* int execv(const char *path, char *const argv[]) will call execve */
/* int execvp(const char *file, char *const argv[]) will call execvpe */

int execve(const char *path, char *const argv[], char *const envp[])
{
  typeof(execve) *original_execve = dlsym(RTLD_NEXT, "execve");
  DEBUG("Child process is attempting to execute (execve) binary '%s'\n", path);

  char *allowed_exec = NULL, *forbidden_exec = NULL;
  char **allowed_argv = NULL, **forbidden_argv = NULL;
  int ret_value = 0;

  exechelp_filter_forbidden_exec(path, argv, envp,
                                 &allowed_exec, &allowed_argv,
                                 &forbidden_exec, &forbidden_argv);

  /* First getting rid of the denied process/files because we know we will return from
   * this call.
   */
  if (forbidden_exec)
  {
    if (!allowed_exec)
      DEBUG("Child process delegating the execution of '%s' to the sandbox\n", path); 
    else
      DEBUG("Child process must delegate the execution of some parameters of '%s' to the sandbox\n", path); 

    /* Here we execute a fake execve to a specific path, so the sandbox gets notified.
     * We do this rather than deny the system call with a built-in security mechanism
     * because a seccomp + ptrace combination would require that we compile a new
     * execve seccomp policy every time we want to deny a system call, and that we then
     * refind and recompile the original seccomp policy, and reload it. This would be
     * too costly, and ptrace itself cannot prevent the execve call from happening so
     * we rely on the sandboxed process to self-censor instead. Disobeying processes
     * could be detected by duplicating the checking logic in the trusted daemon that
     * monitors the execve calls of the sandboxed process.
     */
    size_t altered_len = strlen(EXECHELP_MONITORED_EXEC_PATH) + (forbidden_exec? strlen(forbidden_exec):0) + 1;
    char *altered_path = malloc(sizeof(char) * altered_len);
    snprintf(altered_path, altered_len, "%s%s", EXECHELP_MONITORED_EXEC_PATH, (forbidden_exec? forbidden_exec:""));

    int ret = (*original_execve)(altered_path, forbidden_argv, envp);
    free(altered_path);

    /* Ideally the sandbox is configured to return EACCES for such paths, but the
     * normal error to be had is ENOENT without a compatible sandbox. We force the
     * error to EACCES for that reason.
     */
    DEBUG("Child process's system call successfully hijacked for sandbox to take over (returned %d)\n", ret);
  }

  /* Then executing the allowed process with the allowed parameters. We might not return.
   */
  if (allowed_exec)
  {
    DEBUG("%s", "Child process is allowed to proceed by the sandbox\n");
    ret_value = (*original_execve)(allowed_exec, allowed_argv, envp);
  }
  else
  {
    errno = EACCES;
    ret_value = -1;
  }

  free(allowed_exec);
  free(forbidden_exec);

  if (allowed_argv)
    free(allowed_argv);

  if (forbidden_argv)
    free(forbidden_argv);

  return ret_value;
}

int execvpe(const char *file, char *const argv[], char *const envp[])
{
  typeof(execvpe) *original_execvpe = dlsym(RTLD_NEXT, "execvpe");
  DEBUG("Child process is attempting to execute (execvpe) binary name '%s'\n", file);

  char *path = exechelp_resolve_path(file);
  if (!path)
  {
    DEBUG2("DEBUG: '%s' could not be resolved to any path, we expect execvpe to return ENOENT or a similar error.\n", file);
    errno = ENOENT;
    return -1;
  }

  char *allowed_exec = NULL, *forbidden_exec = NULL;
  char **allowed_argv = NULL, **forbidden_argv = NULL;
  int ret_value = 0;

  exechelp_filter_forbidden_exec(path, argv, envp,
                                 &allowed_exec, &allowed_argv,
                                 &forbidden_exec, &forbidden_argv);

  /* First getting rid of the denied process/files because we know we will return from
   * this call.
   */
  if (forbidden_exec)
  {
    if (!allowed_exec)
      DEBUG("Child process delegating the execution of '%s' to the sandbox\n", path); 
    else
      DEBUG("Child process must delegate the execution of some parameters of '%s' to the sandbox\n", path); 

    /* Here we execute a fake execvpe to a specific path, so the sandbox gets notified.
     * We do this rather than deny the system call with a built-in security mechanism
     * because a seccomp + ptrace combination would require that we compile a new
     * execvpe seccomp policy every time we want to deny a system call, and that we then
     * refind and recompile the original seccomp policy, and reload it. This would be
     * too costly, and ptrace itself cannot prevent the execvpe call from happening so
     * we rely on the sandboxed process to self-censor instead. Disobeying processes
     * could be detected by duplicating the checking logic in the trusted daemon that
     * monitors the execvpe calls of the sandboxed process.
     */
    size_t altered_len = strlen(EXECHELP_MONITORED_EXEC_PATH) + (forbidden_exec? strlen(forbidden_exec):0) + 1;
    char *altered_path = malloc(sizeof(char) * altered_len);
    snprintf(altered_path, altered_len, "%s%s", EXECHELP_MONITORED_EXEC_PATH, (forbidden_exec? forbidden_exec:""));

    int ret = (*original_execvpe)(altered_path, forbidden_argv, envp);
    free(altered_path);

    /* Ideally the sandbox is configured to return EACCES for such paths, but the
     * normal error to be had is ENOENT without a compatible sandbox. We force the
     * error to EACCES for that reason.
     */
    DEBUG("Child process's system call successfully hijacked for sandbox to take over (returned %d)\n", ret);
  }

  /* Then executing the allowed process with the allowed parameters. We might not return.
   */
  if (allowed_exec)
  {
    DEBUG("%s", "Child process is allowed to proceed by the sandbox\n");
    
    /* We still execute with 'file' in the rare situation where path returns a ENOEXEC 
     * file that also fails running in a shell, in which case execvpe would try another
     * entry in the path. We should improve exechelp_resolve_path to better determine
     * the executability of a path rather than merely checking for permission. However
     * we want to use allowed_exec if it has been modified compared to the original
     * path given to the filter function.
     */
    if (strcmp(path, allowed_exec) == 0)
      ret_value = (*original_execvpe)(file, allowed_argv, envp);
    else
      ret_value = (*original_execvpe)(allowed_exec, allowed_argv, envp);
  }
  else
  {
    errno = EACCES;
    ret_value = -1;
  }

  free(allowed_exec);
  free(forbidden_exec);

  if (allowed_argv)
    free(allowed_argv);

  if (forbidden_argv)
    free(forbidden_argv);

  free(path);
  return ret_value;
}

int fexecve(int fd, char *const argv[], char *const envp[])
{
  if(fd<0)
  {
    errno = EINVAL;
    return -1;
  }

  size_t len = strlen("/proc/self/fd/") + 48;
  char *fdpath = malloc(sizeof(char) * len);
  snprintf(fdpath, len, "/proc/self/fd/%d", fd);

  char *path = exechelp_coreutils_areadlink_with_size(fdpath, 2048);
  free(fdpath);
  if(!path)
  {
    errno = EINVAL;
    return -1;
  }

  typeof(execve) *original_execve = dlsym(RTLD_NEXT, "execve");
  typeof(fexecve) *original_fexecve = dlsym(RTLD_NEXT, "fexecve");
  DEBUG("Child process is attempting to execute (fexecve) file descriptor '%s' (%d)\n", path, fd);

  char *allowed_exec = NULL, *forbidden_exec = NULL;
  char **allowed_argv = NULL, **forbidden_argv = NULL;
  int ret_value = 0;

  exechelp_filter_forbidden_exec(path, argv, envp,
                                 &allowed_exec, &allowed_argv,
                                 &forbidden_exec, &forbidden_argv);

  /* First getting rid of the denied process/files because we know we will return from
   * this call.
   */
  if (forbidden_exec)
  {
    if (!allowed_exec)
      DEBUG("Child process delegating the execution of '%s' to the sandbox\n", path); 
    else
      DEBUG("Child process must delegate the execution of some parameters of '%s' to the sandbox\n", path); 

    /* Here we execute a fake execve to a specific path, so the sandbox gets notified.
     * We do this rather than deny the system call with a built-in security mechanism
     * because a seccomp + ptrace combination would require that we compile a new
     * execve seccomp policy every time we want to deny a system call, and that we then
     * refind and recompile the original seccomp policy, and reload it. This would be
     * too costly, and ptrace itself cannot prevent the execve call from happening so
     * we rely on the sandboxed process to self-censor instead. Disobeying processes
     * could be detected by duplicating the checking logic in the trusted daemon that
     * monitors the execve calls of the sandboxed process.
     */
    size_t altered_len = strlen(EXECHELP_MONITORED_EXEC_PATH) + (forbidden_exec? strlen(forbidden_exec):0) + 1;
    char *altered_path = malloc(sizeof(char) * altered_len);
    snprintf(altered_path, altered_len, "%s%s", EXECHELP_MONITORED_EXEC_PATH, (forbidden_exec? forbidden_exec:""));

    int ret = (*original_execve)(altered_path, forbidden_argv, envp);
    free(altered_path);

    /* Ideally the sandbox is configured to return EACCES for such paths, but the
     * normal error to be had is ENOENT without a compatible sandbox. We force the
     * error to EACCES for that reason.
     */
    DEBUG("Child process's system call successfully hijacked for sandbox to take over (returned %d)\n", ret);
  }

  /* Then executing the allowed process with the allowed parameters. We might not return.
   */
  if (allowed_exec)
  {
    DEBUG("%s", "Child process is allowed to proceed by the sandbox\n");
        
    /* We still execute with 'file' in the rare situation where the file descriptor has
     * changed since we read the fd info, because we care most about semantic equivalence
     * with unpreloaded programs. However we want to use allowed_exec if it has been
     * modified compared to the original path given to the filter function.
     */
    if (strcmp(path, allowed_exec) == 0)
      ret_value = (*original_fexecve)(fd, allowed_argv, envp);
    else
      ret_value = (*original_execve)(allowed_exec, allowed_argv, envp);
  }
  else
  {
    errno = EACCES;
    ret_value = -1;
  }

  free(allowed_exec);
  free(forbidden_exec);

  if (allowed_argv)
    free(allowed_argv);

  if (forbidden_argv)
    free(forbidden_argv);

  free(path);
  return ret_value;
}


