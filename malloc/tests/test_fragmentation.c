#include "test_utils.h"

int main()
{
    void *p1 = malloc(15);
    void *p2 = malloc(25);

    printf("Test de fragmentation\n");

    assert(p1 != NULL);
    assert(p2 != NULL);
    printf(" -> p1 et p2 alloués avec succès.\n");

    free(p1);
    printf(" -> p1 libéré.\n");

    void *p3 = malloc(8);
    assert(p3 != NULL);
    printf(" -> p3 alloué avec succès.\n");

    free(p2);
    free(p3);
    printf(" -> p2 et p3 libérés. Test terminé sans crash.\n");

    return 0;
}
