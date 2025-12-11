#include "logger.h"
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *paths[ARG_MAX + 1];
int path_count = 0;
int max_path_length = 0;

void clear_paths(void) {
    path_count = 0;
    max_path_length = 0;
    for (int i = 0; i < path_count; ++i) {
        free(paths[i]);
    }
}

int update_path(char *args[], int n) {
    if (n > ARG_MAX) {
        print_err_message();
        return -1;
    }
    clear_paths();

    path_count = n;
    max_path_length = 0;

    for (int i = 0; i < n; i++) {
        paths[i] = strdup(args[i]);
        int current_len = strlen(args[i]);
        max_path_length =
            max_path_length > current_len ? max_path_length : current_len;
    }
    return 0;
}

// returns the appropriate path that exists, user have the responsibility to
// free the string after use.
char *find_path(char *cmd) {
    if (!access(cmd, X_OK)) {
        return cmd;
    }
    char *tmp = malloc(sizeof(char) * (PATH_MAX + 1));
    for (int i = 0; i < path_count; i++) {
        int len = snprintf(tmp, sizeof(char) * (PATH_MAX + 1), "%s/%s",
                           paths[i], cmd);
        if (len < 0) {
            return NULL;
        }
        if (!access(tmp, X_OK)) {
            return tmp;
        }
    }
    free(tmp);
    return NULL;
}
