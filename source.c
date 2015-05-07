#include <stdio.h>
#include <time.h>

#define k 5

typedef struct occurrence {
    int sequence: 20;
    int position: 7;
    struct occurrence *next;
} new_t;

new_t memory[1000000 * k];
new_t *types[1 << (2 * k)];

void
elapsed_time(void)
{
    static clock_t interval;
    if (!interval) {
        interval = clock();
    } else {
        interval = clock() - interval;
        printf("Well done in %.3f seconds.\n", (double)interval / CLOCKS_PER_SEC);
        interval = 0;
    }
}

int
main(int argc, char const *argv[])
{
    elapsed_time();
    for (int i = 0; i < 123456789; i++);
    elapsed_time();
    return 0;
}
