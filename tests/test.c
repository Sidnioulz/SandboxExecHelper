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
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    printf ("ExecHelper tests - pid %d\n\n", getpid());
    extern char **environ;
    int ret;

    printf("\n\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("Attempting to execute /usr/bin/ristretto via a file descriptor... \n");
    int execfd = open("/usr/bin/ristretto", O_RDONLY);
    char *fargv[] = {"/home/steve/Downloads", "~/Documents/Administratif/CNI", NULL};
    ret = fexecve(execfd, fargv, environ);

    if (ret)
      printf("Failed: %s\n", strerror(errno));
    else
      printf("Success!\n");

    printf("\n\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("Attempting to execute /usr/bin/vlc with forbidden parameters... \n");
    char *argv[] = {"/usr/bin/vlc", "/tmp/test.mp3", "/tmp/test-managed.mp3", "~/Documents/Administratif/Lol", "../../../../tmp/test-managed.mp3", NULL};
    ret = execvpe("vlc", argv, environ);

    if (ret)
      printf("Failed: %s\n", strerror(errno));
    else
      printf("Success!\n");

    printf("\n\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("Attempting to execute /usr/bin/vlc with only allowed parameters... \n");
    char *argv2[] = {"/usr/bin/vlc", "/tmp/test.mp3", "../../foo.mp3", "~/Documents/AuthorisedFiles/Test.mp3", NULL};
    ret = execvpe("totem", argv2, environ);

    if (ret)
      printf("Failed: %s\n", strerror(errno));
    else
      printf("Success!\n");

    return 0;
}
