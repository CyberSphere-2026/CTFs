#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char flag[128];

void setup() {
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
}

void read_flag() {
  FILE *f = fopen("./flag", "r");
  if (!f) {
    perror("");
    exit(-1);
  }
  fgets(flag, sizeof(flag), f);
  fclose(f);
}

void vuln() {
  puts("If you have the password I will give you the flag");
  int i = 0;
  unsigned long correct_count = 0;
  char c;
  do {
    printf("> ");
    c = getchar();
    if (c == flag[i++]) {
      correct_count++;
      usleep(200000);
    }
    while ((c = getchar()) != '\n' && c != EOF)
      ;
  } while (c != 0);
  if (correct_count == strlen(flag)) {
    system("cat flag");
  }
}

int main(void) {
  setup();
  read_flag();
  vuln();
  return 0;
}
