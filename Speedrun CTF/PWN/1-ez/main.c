#include <stdio.h>

void setup()
{
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
}

void win()
{
    system("cat flag.txt");
}
void main()
{
    setup();
    char buf[0x40];
    gets(buf);
}