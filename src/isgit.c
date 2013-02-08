// check if current directory (or any valid parent up to HOME) contains .git
#include <sys/param.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv) {
    char result[PATH_MAX];
    char* home = getenv("HOME");

    int len;
    DIR *dirp;
    struct dirent *dp;

    while (1) {
        if (!realpath(".", result)) {
            perror("realpath");
            exit(1);        // realpath() failed with bad path?!
        }
        if (strcmp(result, home) == 0) {
            // assume no Git repos at HOME or above
            fprintf(stderr, "Hit HOME with no .git found\n");
            exit(1);
        }
        // check current directory for a .git file
        dirp = opendir(".");
        while ((dp = readdir(dirp)) != NULL) {
            if (strcmp(dp->d_name, ".git") == 0) {
                printf(".git at %s\n", result);
                exit(0);
            }
        }
        (void)closedir(dirp);
        // none found in current dir, move to parent
        if (chdir("..") != 0) {
            perror("chdir");
            exit(1);        // chdir() failed for some reason
        }
    } // end of loop, go to top and examine new current dir
    exit(99);   // impossible
}
