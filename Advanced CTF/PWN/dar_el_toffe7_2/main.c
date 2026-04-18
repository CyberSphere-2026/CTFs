#include <linux/audit.h>
#include <linux/filter.h>
#include <linux/seccomp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

#define MAX_NOTES 8
#define MAX_FREES 6
#define MIN_SIZE 0x80
#define MAX_SIZE 0x500
#define TIMEOUT 90

struct note {
  size_t size;
  char *data;
};

static struct note notes[MAX_NOTES];
static int free_count = 0;
static int null_used = 0;
static int edit_used = 0;

static void setup_seccomp(void) {
  struct sock_filter filter[] = {
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, nr)),

      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_openat, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_read, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_close, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_exit, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_exit_group, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_brk, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_mmap, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_mprotect, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_fstat, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_lseek, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_getrandom, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_newfstatat, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_futex, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_prlimit64, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_clock_nanosleep, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),

      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL),
  };
  struct sock_fprog prog = {
      .len = sizeof(filter) / sizeof(filter[0]),
      .filter = filter,
  };
  prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
  prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog);
}

static void init(void) {
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  alarm(TIMEOUT);
}

static int read_int(void) {
  char buf[32] = {0};
  if (read(STDIN_FILENO, buf, sizeof(buf) - 1) <= 0)
    _exit(1);
  return atoi(buf);
}

static void read_n(char *buf, size_t n) {
  size_t i = 0;
  while (i < n) {
    ssize_t r = read(STDIN_FILENO, buf + i, n - i);
    if (r <= 0)
      _exit(1);
    i += r;
  }
}

static void do_add(void) {
  int idx = -1;
  for (int i = 0; i < MAX_NOTES; i++) {
    if (!notes[i].data) {
      idx = i;
      break;
    }
  }
  if (idx < 0) {
    puts("Full");
    return;
  }

  printf("Size: ");
  size_t sz = (size_t)read_int();
  if (sz < MIN_SIZE || sz > MAX_SIZE) {
    puts("Bad");
    return;
  }

  char *p = malloc(sz);
  if (!p) {
    puts("Fail");
    return;
  }
  memset(p, 0, sz);

  printf("Data: ");
  read_n(p, sz);

  notes[idx].size = sz;
  notes[idx].data = p;
  printf("Idx: %d\n", idx);
}

static void do_del(void) {
  if (free_count >= MAX_FREES) {
    puts("Limit");
    return;
  }
  printf("Idx: ");
  int idx = read_int();
  if (idx < 0 || idx >= MAX_NOTES || !notes[idx].data) {
    puts("Bad");
    return;
  }

  memset(notes[idx].data, 0, notes[idx].size);
  free(notes[idx].data);
  notes[idx].data = NULL;
  notes[idx].size = 0;
  free_count++;
  puts("OK");
}

static void do_null(void) {
  if (null_used) {
    puts("Used");
    return;
  }
  printf("Idx: ");
  int idx = read_int();
  if (idx < 0 || idx >= MAX_NOTES || !notes[idx].data) {
    puts("Bad");
    return;
  }

  notes[idx].data[notes[idx].size] = '\0';
  null_used = 1;
  puts("OK");
}

static void do_edit(void) {
  if (edit_used) {
    puts("Used");
    return;
  }
  printf("Idx: ");
  int idx = read_int();
  if (idx < 0 || idx >= MAX_NOTES || !notes[idx].data) {
    puts("Bad");
    return;
  }

  char buf[0x10];
  printf("Data: ");
  read_n(buf, 0x10);
  memcpy(notes[idx].data, buf, 0x10);
  edit_used = 1;
  puts("OK");
}

static void menu(void) {
  puts("1) Add  2) Del  3) Null  4) Edit  5) Quit");
  printf("> ");
}

int main(void) {
  init();
  setup_seccomp();

  puts("== Wide Awake v3 ==");

  while (1) {
    menu();
    switch (read_int()) {
    case 1:
      do_add();
      break;
    case 2:
      do_del();
      break;
    case 3:
      do_null();
      break;
    case 4:
      do_edit();
      break;
    case 5:
      puts("Bye");
      _exit(0);
    default:
      puts("?");
      break;
    }
  }
}
