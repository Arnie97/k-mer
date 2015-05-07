#include <stdio.h>
#define k 5

typedef struct occurrence {
    int sequence: 20;
    int position: 7;
    struct occurrence *next;
} new_t;

new_t memory[1000000 * k];
new_t *types[1 << (2 * k)];

int
main(int argc, char const *argv[])
{
    return 0;
}
