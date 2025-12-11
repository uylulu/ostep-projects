#include "cmd_executor.h"
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

char *trim(char *str) {
    int n = strlen(str);

    int begin = -1;
    int end = -1;
    for (int i = 0; i < n; i++) {
        if (str[i] != ' ') {
            begin = i;
            break;
        }
    }
    for (int i = n - 1; i >= 0; i--) {
        if (str[i] != ' ') {
            end = i;
            break;
        }
    }
    if (begin == -1 || end == -1) {
        return NULL;
    }
    for (int i = begin; i <= end; i++) {
        str[i - begin] = str[i];
    }
    str[end - begin + 1] = '\0';
    return str;
}

bool has_space(char *str) {
    char *ptr = str;
    while (*ptr != '\0') {
        if (*ptr == ' ')
            return true;
        ptr++;
    }
    return false;
}

int count_redirections(char *str) {
    int res = 0;
    char *ptr = str;
    while (*ptr != '\0') {
        res += (*ptr == '>');
        ptr++;
    }
    return res;
}

// pareses the input line into space seperated arguments
// return value: on success, returns the number of arguments read, and the args
// on failure, returns -1; The caller has the
// responsibility of freeing the array args.
int read_arguments(char *line, char *args[], int max_size,
                   const char *delimiter) {
    bool all_space = true;
    for (int i = 0; line[i] != '\0'; i++) {
        if (line[i] != ' ') {
            all_space = false;
            break;
        }
    }
    if (all_space) {
        return 0;
    }
    char *temp = line;
    char *token;
    int counter = 0;
    while ((token = strsep(&temp, delimiter))) {
        if (*token == '\0')
            continue;
        ++counter;
        if (counter > max_size) {
            return -1;
        }
        args[counter - 1] = trim(token);
    }
    args[counter] = NULL;
    return counter;
}

int parse_and_run(char *cmd) {
    const int max_args = ARG_MAX + 1;
    char *args[max_args];

    int num_parallel = read_arguments(cmd, args, max_args, "&");
    if (num_parallel == 0) {
        return 0;
    }
    if (num_parallel > 1) {
        pid_t pids[num_parallel];
        for (int i = 0; i < num_parallel; i++) {
            pids[i] = fork();
            if (pids[i] < 0) {
                return -1;
            }
            if (pids[i] == 0) {
                int rt = parse_and_run(args[i]);
                if (rt) {
                    print_err_message();
                    exit(1);
                }
                exit(0);
            }
        }
        for (int i = 0; i < num_parallel; i++) {
            wait(NULL);
        }
        return 0;
    }

    int cnt_redirections = count_redirections(cmd);
    if (cnt_redirections >= 2) {
        return 1;
    }

    int redirecton = read_arguments(cmd, args, max_args, ">");

    FILE *output_stream = NULL;
    if (redirecton > 2) {
        return 1;
    } else if (cnt_redirections == 1) {
        if (redirecton <= 1 || has_space(args[1])) {
            return 1;
        }
        output_stream = fopen(args[1], "w");
        if (output_stream == NULL) {
            return 1;
        }
    }

    char *temp = strdup(args[0]);
    int num_args = read_arguments(temp, args, max_args, " ");
    if (num_args == -1) {
        goto failure;
    }
    if (num_args == 0)
        goto sucess;

    if (!strcmp(args[0], "exit")) {
        if (num_args != 1) {
            goto failure;
        }
        exit(0);
    } else if (!strcmp(args[0], "cd")) {
        if (num_args > 2 || chdir(args[1]) == -1) {
            goto failure;
        }
    } else if (!strcmp(args[0], "path")) {
        char **p = NULL;
        if (num_args >= 2) {
            p = &args[1];
        }
        if (update_path(p, num_args - 1)) {
            goto failure;
        }
    } else {
        if (strlen(args[0]) == 0)
            goto failure;

        char *cmd = find_path(args[0]);
        if (cmd == NULL) {
            goto failure;
        }
        args[0] = cmd;
        int rt = execute_cmd(cmd, args, output_stream);
        if (rt) {
            goto failure;
        }
        goto sucess;
    }

sucess:
    if (output_stream) {
        fclose(output_stream);
    }
    free(temp);
    return 0;
failure:
    if (output_stream) {
        fclose(output_stream);
    }
    free(temp);
    return 1;
}

void execute_shell(FILE *stream) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread = 0;

    while (print_shell() && (nread = getline(&line, &len, stream)) != -1) {
        if (nread > 0 && line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
            --nread;
        }
        int rt = parse_and_run(line);
        if (rt) {
            print_err_message();
        }
    }
    free(line);
    return;
}

int main(int argc, char **argv) {
    update_path((char *[]){"/bin"}, 1);
    if (argc == 1) {
        execute_shell(stdin);
        return EXIT_SUCCESS;
    }
    FILE *stream = NULL;
    for (int i = 1; i < argc; i++) {
        stream = fopen(argv[i], "r");
        if (stream == NULL) {
            print_err_message();
            exit(1);
        }
        execute_shell(stream);
    }
    return EXIT_SUCCESS;
}
