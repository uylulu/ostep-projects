#include <stdio.h>
#include <stdlib.h>

void write_bytes(const unsigned char *byte, const int *count) {
  if (*byte == '\0' || count == 0) {
    return;
  }

  unsigned char to_write[5] = {};
  to_write[3] = (*count >> 24) & 0xFF;
  to_write[2] = (*count >> 16) & 0xFF;
  to_write[1] = (*count >> 8) & 0xFF;
  to_write[0] = *count & 0xFF;
  to_write[4] = *byte;
  fwrite(to_write, 1, 5, stdout);

  return;
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("wzip: file1 [file2 ...]\n");
    exit(1);
  }

  unsigned char byte[1] = {0};
  int count = 0;
  unsigned char lst_byte = '\0';
  unsigned char to_write[5] = {0};

  for (int i = 1; i < argc; ++i) {
    FILE *stream = fopen((const char *)argv[1], "r");
    if (stream == NULL) {
      printf("wzip: cannot read file\n");
      exit(1);
    }
    while (fread(&byte, 1, 1, stream) == 1) {
      if (count == 0 || byte[0] != lst_byte) {
        write_bytes(&lst_byte, &count);
        lst_byte = byte[0];
        count = 1;
      } else {
        count += 1;
      }
    }

    if (fclose(stream)) {
      printf("Error closing file");
      exit(1);
    }
  }
  write_bytes(&lst_byte, &count);

  return 0;
}
