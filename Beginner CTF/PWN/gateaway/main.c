#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LOG_SZ 512
#define PKT_SZ 36
#define FAIL_SZ 24

typedef struct __attribute__((packed)) {
  uint8_t ver;
  uint8_t type;
  uint16_t flags;
  uint32_t src;
  uint32_t dst;
  uint32_t seq;
  uint32_t chk;
  char data[16];
} pkt_t;

pkt_t cur;

void win(void) { system("/bin/sh"); }

void flush_log(void) { puts("[!] flushing"); }

void setup(void) {
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
}

void run(void) {
  struct {
    char buf[LOG_SZ];
    void (*flush)(void);
    int off;
  } log;

  log.flush = flush_log;
  log.off = 0;

  puts("Gateway v2.0\n");

  for (;;) {
    printf("[%d/%d] ", log.off, LOG_SZ);

    if (read(0, &cur, PKT_SZ) != PKT_SZ) {
      break;
    }

    if (cur.ver == 0x43 && cur.type == 0x53 && cur.chk) {
      memcpy(log.buf + log.off, &cur, PKT_SZ);
      log.off += PKT_SZ;
    } else {
      memcpy(log.buf + log.off, &cur, FAIL_SZ);
      log.off += FAIL_SZ;
    }

    if (log.off > LOG_SZ) {
      log.flush();
      memset(log.buf, 0, LOG_SZ);
      log.off = 0;
    }
  }
}

int main(void) {
  setup();
  run();
}
