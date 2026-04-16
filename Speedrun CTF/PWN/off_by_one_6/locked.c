#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void setup(){
    setvbuf(stdin, 0, _IONBF, 0);
    setvbuf(stdout, 0, _IONBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);
}

struct auth {
    char input[32];
    char password[32];
};

struct auth a;

void win() {
    FILE *f = fopen("flag.txt", "r");
    char flag[128];
    fgets(flag, sizeof(flag), f);
    printf("FLAG: %s\n", flag);
    exit(0);
}

void generate_password() {
    int fd = open("/dev/urandom", O_RDONLY);
    for (int i = 0; i < 31; i++) {
        do {
            read(fd, &a.password[i], 1);
        } while (a.password[i] == 0);
    }

    a.password[31] = 0;
    close(fd);
}

void login() {
    puts("Login:");
    read(0, a.input, 33);

    if (strcmp(a.input, a.password) == 0)
        win();
    else
        puts("nope");
}

int main() {
    setup();
    generate_password();
    login();
}
