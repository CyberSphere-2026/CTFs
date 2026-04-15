#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHELLCODE_SIZE 0x100

void setup() {
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
}

int main(void) {
  setup();
  void *mem = mmap(NULL, SHELLCODE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (mem == MAP_FAILED) {
    perror("mmap");
    return 1;
  }

  puts("Give me your shellcode:");

  ssize_t n = read(0, mem, SHELLCODE_SIZE);
  if (n <= 0) {
    puts("No input.");
    return 1;
  }

  unsigned char *buf = (unsigned char *)mem;
  for (ssize_t i = 0; i < n; i++) {
    if (buf[i] == 0x0f || buf[i] == 0x05) {
      puts("Bad byte detected! Nice try.");
      return 1;
    }
  }

  puts("Running...");
  ((void (*)())mem)();

  return 0;
}
