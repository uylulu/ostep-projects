#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void print_err_message(void) {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
    return;
}

bool print_shell(void) {
    char cwd[PATH_MAX + 1];
    if (!getcwd(cwd, sizeof(cwd))) {
        print_err_message();
        return false;
    }
    // printf("wish>%s$ ", cwd);
    return true;
}
