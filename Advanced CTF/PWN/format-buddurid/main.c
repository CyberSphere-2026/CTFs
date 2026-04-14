#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
void setup()
{
    setbuf(stdin, 0);
    setbuf(stdout, 0);
}

int main()
{
    setup();
    char buf[0x100] = {0};
    printf("too late for Mahba but whatever %p\n", ((unsigned long)(&buf) & 0xffff));

    fgets(buf, sizeof(buf), stdin);

    printf(buf);
    return 0;
}