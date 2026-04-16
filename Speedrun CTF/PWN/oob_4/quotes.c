#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void setup(){
    setvbuf(stdin, 0, _IONBF, 0);
    setvbuf(stdout, 0, _IONBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);
}


typedef struct {
    char secret[64];
    char quotes[10][64];
} data_t;

data_t d;

void init() {
    FILE *f = fopen("flag.txt", "r");
    if (f) fgets(d.secret, 64, f);

    strcpy(d.quotes[0], "i debug therefore i am");
    strcpy(d.quotes[1], "segfaults are just existential hints");
    strcpy(d.quotes[2], "trust issues? try pointers");
    strcpy(d.quotes[3], "assembly is just poetry with trauma");
    strcpy(d.quotes[4], "sleep is a blocking syscall");
    strcpy(d.quotes[5], "reality has no debugger");
    strcpy(d.quotes[6], "if it compiles, it's art");
    strcpy(d.quotes[7], "undefined behavior builds character");
    strcpy(d.quotes[8], "NULL is a lifestyle choice");
    strcpy(d.quotes[9], "CTFs: where sanity goes to free()");
}

void chall() {
    int i;
    puts("Top 10 quotes for enlightened hackers:");
    puts("index [0-10]:");
    scanf("%d", &i);

    if (i >= 10) {
        puts("out of bound !");
        return;
    }

    printf("%s\n", d.quotes[i]);
}


int main() {
    setup();
    init();
    for (int i = 0 ; i<3 ; i++){
        chall();
    }
}
