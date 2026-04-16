#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>


void win(){
    char flag[100];
    int fd = open("flag.txt", 0);
    read(fd, flag, 100);
    write(1, flag, 100);
}

void setup(){
    setvbuf(stdin, 0, _IONBF, 0);
    setvbuf(stdout, 0, _IONBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);
}

void chall() {
    int a, b;
    int sum;

    puts("System requires two verification codes.");
    puts("Both values must be positive integers.");

    do {
        printf("Enter code A: ");
        scanf("%d", &a);
    } while(a<0);

    do {
        printf("Enter code B: ");
        scanf("%d", &b);
    } while(b<0);

    sum = a + b;

    printf("Verification result: %d\n", sum);

    if (sum < 0) {
        win();
    } else {
        puts("Access denied.");
    }
}

int main() {
    setup();
    chall();
    return 0;
}
