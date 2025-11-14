#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LIMIT 300

int main(int argc, const char **argv) {
  for (int i = 1; i < argc; i++) {

    const char *file_path = argv[i];
    char buf[MAX_LINE_LIMIT];

    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
      printf("wcat: cannot open file\n");
      exit(1);
    }
    while (1) {
      char *res = fgets(buf, MAX_LINE_LIMIT, file);
      if (res == NULL) {
        break;
      }
      printf("%s", res);
    }
    if (fclose(file)) {
      printf("Error closing file\n");
      exit(1);
    }
  }
  return 0;
}
