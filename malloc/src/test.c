#include <stdio.h>
#include <stdlib.h>
int main()
{
    void *p = malloc(100);
    free(p);
    printf("Test passé\n");
    return 0;
}
