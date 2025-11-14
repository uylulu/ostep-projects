#include <stdio.h>
#include <stdlib.h>

void uncompress_file(FILE *stream) {
  unsigned char bytes[5] = {0};

  while (fread(bytes, 1, 5, stream) == 5) {
    int count =
        bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[2] << 24);

    for (int i = 0; i < count; ++i) {
      printf("%c", bytes[4]);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("wunzip: file1 [file2 ...]\n");
    exit(1);
  }

  for (int i = 1; i < argc; ++i) {
    FILE *fp = fopen((const char *)argv[i], "r");
    if (fp == NULL) {
      printf("wunzip: cannot read file\n");
      exit(1);
    }

    uncompress_file(fp);

    if (fclose(fp) != 0) {
      printf("Error closing file");
      exit(1);
    }
  }

  return 0;
}
