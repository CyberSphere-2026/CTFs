#include <stdio.h>

void setup()
{
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
}

typedef struct skibidi
{
    char desc[0x40];
    long password;
} skibidi;
void main()
{
    setup();
    skibidi user;
    printf("gooner check : ");

    gets(user.desc);
    if (strcmp(user.desc, "GOON"))
    {
        printf("nah\n");
        exit(0);
    }
    else
    {
        if (user.password == 0x4e4f4f474e4f4f47)
        {
            printf("it pays off , eventually \n");
            system("cat flag.txt");
        }
        else
            printf("nah , fake :(\n");
    }
}