#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <utmp.h>
#include <sys/utsname.h>

void printUsers()
{
    struct utmp *n;
    setutent();
    n = getutent();
    printf("LoggedIn Users are : ");
    while (n)
    {
        if (n->ut_type == USER_PROCESS)
        {
            printf("%s ", n->ut_user);
        }
        n = getutent();
    }
    printf("\n");
}

int main() {
    printUsers();
}