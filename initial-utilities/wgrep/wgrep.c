#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void find_words_in_stream(const char *template, FILE *fp) {
  char *line = NULL;
  size_t len = 0;
  size_t template_len = strlen(template);
  ssize_t nread;

  while ((nread = getline(&line, &len, fp)) != -1) {
    for (ssize_t j = 0; j < nread - (ssize_t)template_len; ++j) {
      bool equals = true;

      for (ssize_t t = 0; t < (ssize_t)template_len; ++t) {
        equals &= (template[t] == line[j + t]);
      }

      if (equals) {
        printf("%s", line);
        break;
      }
    }
  }

  free(line);
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("wgrep: searchterm [file ...]\n");
    exit(1);
  }

  char *template = argv[1];
  for (int i = 2; i < argc; i++) {
    const char *file_path = argv[i];
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
      printf("wgrep: cannot open file\n");
      exit(1);
    }

    find_words_in_stream(template, fp);

    if (fclose(fp) == EOF) {
      printf("Error closing file\n");
      exit(1);
    }
  }

  if (argc == 2) {
    find_words_in_stream(template, stdin);
  }

  return EXIT_SUCCESS;
}
