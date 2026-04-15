#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void setup(void) {
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
  srand(time(NULL));
}

char *flag, *flag_base;
long flag_size;
void read_flag() {
  FILE *flag_file = fopen("./flag", "r");
  if (!flag_file) {
    perror("failed to open flag file");
    exit(1);
  }

  fseek(flag_file, 0, SEEK_END);
  flag_size = ftell(flag_file);
  if (flag_size < 0) {
    fprintf(stderr, "invalid flag file size: %ld\n", flag_size);
    fclose(flag_file);
    exit(1);
  }
  flag = flag_base = (char *)malloc((size_t)(flag_size + 64));
  if (flag == NULL) {
    perror("could not allocate flag buffer");
    exit(-1);
  }
  rewind(flag_file);

  fread(flag, 1, (size_t)flag_size, flag_file);
  fclose(flag_file);
}

void *increment(void *arg) {
  int i = *(int *)arg;
  usleep(500000 * i);
  flag++;
  return NULL;
}

void pay() {
  puts("Evade taxes? (y/n)");
  char c = getchar();
  if (c == 'y') {
    exit(0);
  }
  while ((c = getchar()) != '\n' && c != EOF)
    ;
  printf("> %c\n", *flag);
}

void pay_taxes() {
  puts("The TAX MAN came for MY SOUL");
  puts("I will only pay ONE DIME you effin FEDS");
  pthread_t threads[flag_size];
  for (int i = 0; i < flag_size; i++) {
    if (pthread_create(&threads[i], NULL, increment, &i) != 0) {
      perror("pthread_create failed");
      exit(-1);
    }
    usleep(100);
  }
  pay();
  for (int i = 0; i < flag_size; i++) {
    pthread_join(threads[i], NULL);
  }
}

int main(void) {
  setup();
  read_flag();
  pay_taxes();
  return 0;
}
