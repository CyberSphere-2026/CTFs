#include <stdio.h>

void setup()
{
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
}
typedef struct status
{
    char take[0x40];
    int solde;
} status;
void main()
{
    status user;
    setup();
    printf("the rich gets reacher , lets break that model !\n");
    printf("what's your take ?\n");
    fgets(user.take, 0x100, stdin);
    if (user.solde == 0x67676767)
    {
        printf("you are now RICH !\n");
        system("cat flag.txt");
    }
    else
    {
        printf("%x doesnt make you rich you know\n", user.solde);
    }
}