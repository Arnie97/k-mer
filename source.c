#include <stdio.h>
#include <stdlib.h>
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

void
throw(int error_code, char *message)
{
    if (message != NULL) {
        fprintf(stderr, "ERROR: %s\n", message);
    }
    exit(error_code);
}

int
load_input(FILE *fp)
{
    int sequence = 0, position, k_mer_type;
    for (char c = '\n'; c != EOF; c = fgetc(fp)) {
        if (c == '\n') {
            while ((c = fgetc(fp)) != EOF && c != '\n');
            sequence++;
            position = -5;
            k_mer_type = 0;
            puts("========");
            continue;
        }

        k_mer_type >>= 2;
        switch (c) {
        case 'A': /* k_mer_type ||= 0 << (2 * k - 2); */ break;
        case 'C': k_mer_type |= 1 << (2 * k - 2); break;
        case 'G': k_mer_type |= 2 << (2 * k - 2); break;
        case 'T': k_mer_type |= 3 << (2 * k - 2); break;
        default:
            throw(3, "Unexpected character.");
        }

        if (++position < 0) {
            continue;
        }
        printf("%d\n", k_mer_type);
    }
    return 0;
}

int
main(int argc, char const *argv[])
{
    if (argc < 2) {
        throw(1, "No input files.");
    }

    for (int i = 1; i < argc; i++) {
        printf("Opening %s...\n", argv[i]);
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            throw(2, "Unable to open specified file.");
        }

        printf("Start loading %s...\n", argv[i]);
        elapsed_time();
        load_input(fp);
        elapsed_time();

        fclose(fp);
    }
    return 0;
}
