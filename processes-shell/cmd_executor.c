
#include "logger.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int execute_cmd(const char *cmd, char *const argv[], FILE *output_stream) {
    int fd = -1;
    if (output_stream) {
        fd = fileno(output_stream);
    }

    pid_t pid = fork();
    if (pid == -1) {
        print_err_message();
        return -1;
    } else if (pid == 0) {
        if (fd != -1) {
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
        }
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
