#include "test_utils.h"
int main()
{
    void *p = malloc(100);
    free(p);
    printf("Test passé\n");
    return 0;
}
