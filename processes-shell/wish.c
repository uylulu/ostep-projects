#include "logger.h"
#include "path_manager.h"
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// pareses the input line into space seperated arguments
// return value: on success, returns the number of arguments read, and the args
// on failure, returns -1; The caller has the
// responsibility of freeing the array args.
int read_arguments(char *line, char *args[], int max_size) {
    char *temp = line;
    char *token;
    int counter = 0;
    while ((token = strsep(&temp, " "))) {
        if (*token == '\0')
            continue;
        ++counter;
        if (counter > max_size) {
            return -1;
        }
        args[counter - 1] = token;
    }
    args[counter] = '\0';

    return counter;
}

int execute_cmd(const char *cmd, char *const argv[]) {
    pid_t pid = fork();
    if (pid == -1) {
        print_err_message();
        return -1;
    } else if (pid == 0) {
        execv(cmd, argv);
        _exit(1);
    } else {
        int status;
        int rc = wait(&status);
        if (rc == -1) {
            return -1;
        }
        return 0;
    }
}

void execute_shell(FILE *stream) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread = 0;

    char *args[ARG_MAX + 1];

    while (print_shell() && (nread = getline(&line, &len, stream)) != -1) {
        if (nread > 0 && line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
            --nread;
        }

        int num_args = read_arguments(line, args, ARG_MAX);
        if (num_args == -1) {
            print_err_message();
            continue;
        }
        if (num_args == 0)
            continue;

        if (!strcmp(args[0], "exit")) {
            if (num_args != 1) {
                print_err_message();
                continue;
            }
            break;
        } else if (!strcmp(args[0], "cd")) {
            if (num_args > 2 || chdir(args[1]) == -1) {
                print_err_message();
                continue;
            }
        } else if (!strcmp(args[0], "path")) {
            char **p = NULL;
            if (num_args >= 2) {
                p = &args[1];
            }
            if (update_path(p, num_args - 1)) {
                print_err_message();
                continue;
            }
        } else {
            char *cmd = find_path(args[0]);
            if (cmd == NULL) {
                print_err_message();
                continue;
            }
            args[0] = cmd;
            if (execute_cmd(cmd, args) == -1) {
                print_err_message();
                continue;
            }
        }
    }
    free(line);
    return;
}

int main(int argc, char **argv) {
    update_path((char *[]){"/bin"}, 1);
    FILE *stream = NULL;

    if (argc == 1) {
        stream = stdin;
    } else if (argc == 2) {
        stream = fopen(argv[1], "r");
    }

    if (stream == NULL) {
        printf("Error opening input stream");
        exit(1);
    }

    execute_shell(stream);
    return EXIT_SUCCESS;
}
