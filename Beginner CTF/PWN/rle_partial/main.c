#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

const char *YELLOW = "\033[1;33m";
const char *GREEN = "\033[1;32m";
const char *RED   = "\033[1;31m";
const char *RESET = "\033[0m";

char MESSAGE[64] = {0};

void secret(){
    int fd = open("message.txt" , 0);
    read(fd , MESSAGE , 64);
    close(fd);
}

void setup() {
    setvbuf(stdin, 0, _IONBF, 0);
    setvbuf(stdout, 0, _IONBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);
    secret();
    alarm(180);
}

void integrity_verification(char *secret){
    printf("\n[+] Running integrity verification...\n");
    char pswd[16];
    printf("Verification key: ");
    read(0, pswd, 16 );
    if(memcmp(pswd, secret, 16) == 0){
        printf("%sCONGRATS%s\n%s\n" , GREEN , RESET , MESSAGE);
        printf("%sIntegrity confirmed. Launching secure environment...%s\n" , GREEN , RESET);
        execve("/bin/sh" , 0 , 0);
    } else {
        printf("%sVerification failed.%s\n" , RED , RESET);
    }
}


void segfault_handler(int sig) {
    (void)sig;
    printf("\n%s[!] ALERT: UNAUTHORIZED MEMORY ACCESS DETECTED [!]\nTransmission terminated.%s\n" , RED , RESET);
    exit(1);
}



int rle_compress(unsigned char *in, unsigned char *out, int out_len) {
    int i = 0;
    int j = 0;
    while (in[i] != 0) {
        unsigned char c = in[i];
        unsigned char count = 1;
        while (in[i + count] == c && count < 255)
            count++;
        out[j++] = count;
        out[j++] = c;
        i += count;
    }
    return j;
}


void print_txid() {
    srand(time(0));
    time_t t = time(0);
    struct tm *tm_info = localtime(&t);
    int txid = rand() % 10000;
    printf("\nTXID %04d | %04d-%02d-%02d %02d:%02d | STATUS: QUEUED\n",
           txid,
           tm_info->tm_year + 1900,
           tm_info->tm_mon + 1,
           tm_info->tm_mday,
           tm_info->tm_hour,
           tm_info->tm_min
    );
}

void server() {
    char compressed[256] = {0};
    char message[256] = {0};

    printf("%s=== DNA SEQUENCER HUB ACTIVE ===%s\n" , GREEN , RESET);
    puts("Operator: Dr.11");
    puts("Session uplink ready for DNA transmission.\n");
    printf("Enter DNA sequence > ");
    int n = read(0, message, sizeof(message) - 1);
    if (n <= 0) {
        printf("%sNo data received. Terminating session.%s\n", RED , RESET);
        exit(1);
    }
    int res = rle_compress(message, compressed, sizeof(compressed));
    puts("Compressing sequence for network upload...");
    printf("Compressed data : %s %s %s\n" , YELLOW , compressed , GREEN);
    print_txid();
    puts("Transmission queued. Await further instructions.\n");
}

int main() {
    setup();
    signal(SIGSEGV, segfault_handler);
    server();
    return 0;
}
