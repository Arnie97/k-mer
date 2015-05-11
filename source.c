#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define fgetc(stream) (offset++, fgetc(stream))
#define MEGABYTE 1048576

#define k0 50  // the value of specified k
#define k1 13  // enhanced storage threshold

#if k0 > k1
    #define k k1
#else
    #define k k0
#endif

typedef struct occurrence {
    unsigned int sequence: 20;
    unsigned int position: 7;
#if k0 > k1
    unsigned int offset;
#endif
    struct occurrence *next;
} new_t;

new_t memory[1000000 * (101 - k0)];
new_t *cursor = memory;
new_t *types[1 << (2 * k)] = {NULL};

FILE *fp;


void
elapsed_time(void)
{
    static clock_t interval;
    if (!interval) {
        interval = clock();
    } else {
        interval = clock() - interval;
        printf("\nWell done in %.3f seconds.\n\n",
            (double)interval / CLOCKS_PER_SEC);
        interval = 0;
    }
}


void
throw(int error_code, char *message)
{
    if (message != NULL) {
        fprintf(stderr, "ERROR: %s\n", message);
    }
    if (error_code) {
        exit(error_code);
    }
}


int
load_input(FILE *fp)
{
    int sequence = 0, position, offset = 0, k_mer_type;
    for (int c = '\n'; c != EOF; c = fgetc(fp)) {
        if (c == '\n') {
            while ((c = fgetc(fp)) != EOF && c != '\n');
            sequence++;
            position = -k;
            k_mer_type = 0;
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
        cursor->sequence = sequence;
        cursor->position = position;
#if k0 > k1
        cursor->offset = offset - k;
#endif
        cursor->next = types[k_mer_type];
        types[k_mer_type] = cursor++;
    }
    return sequence - 1;
}


int
get_line(char *buffer)
{
    int k_mer_type = 0, c;
    static int input_count;

    input_count++;
    for (;;) {
        printf("In [%d]: ", input_count);
        fflush(stdout);
        if (fgets(buffer, k0 + 2, stdin) == NULL) {
        } else if (strlen(buffer) < k0 + 1) {
            throw(0, "Text too short.");
        } else if (buffer[k0] != '\n') {
            throw(0, "Text too long.");
            while ((c = getchar()) != '\n' && c != EOF);  // flushes to line end
        } else {
            for (int i = 0; i < k; i++) {
                k_mer_type >>= 2;
                switch (buffer[i]) {
                case 'A': /* k_mer_type ||= 0 << (2 * k - 2); */ break;
                case 'C': k_mer_type |= 1 << (2 * k - 2); break;
                case 'G': k_mer_type |= 2 << (2 * k - 2); break;
                case 'T': k_mer_type |= 3 << (2 * k - 2); break;
                default:
                    throw(0, "Unexpected character.");
                    goto skip;
                }
            }
            buffer[k0] = '\0';  // removes the trailing newline
            return k_mer_type;
            skip: ;
        }
    }
}


int
query(int k_mer_type, char *buffer)
{
    int k_mer_count = 0;
    new_t *result = types[k_mer_type];
    do {
#if k0 > k1
        char temp[k0 + 1];
        fseek(fp, result->offset, SEEK_SET);
        fgets(temp, k0 + 1, fp);
        if (strcmp(buffer, temp) != 0) {
            continue;
        }
#endif
        printf("Sequence %6u + %02u\n",
            result->sequence, result->position);
        k_mer_count++;
    } while ((result = result->next) != NULL);
    return k_mer_count;
}


int
main(int argc, char const *argv[])
{
    if (argc < 2) {
        throw(1, "No input files.");
    }

    for (int i = 1; i < argc; i++) {
        printf("Opening %s...\n", argv[i]);
        fp = fopen(argv[i], "r");
        if (fp == NULL) {
            throw(2, "Unable to open specified file.");
        }
#if k0 > k1
        puts("Enhanced storage mode enabled.");
#endif

        printf("Loading %s into memory...\n", argv[i]);
        elapsed_time();
        printf("\n%d sequences have been loaded into memory:\n", load_input(fp));
        int k_mer_count = cursor - memory;
        int index_count = 1 << (2 * k);
        printf(
            "%d %d-mers are stored in %d megabytes,\n"
            "while %d indexes in about %d megabytes.\n",
            k_mer_count, k0, sizeof(new_t) * k_mer_count / MEGABYTE,
            index_count,   sizeof(new_t *) * index_count / MEGABYTE
        );
        elapsed_time();

        for (;;) {
            char buffer[k0 + 2];  // newline and null character
            int k_mer_type = get_line(buffer);
            elapsed_time();
            printf("%d results found.\n", query(k_mer_type, buffer));
            elapsed_time();
        }
        fclose(fp);
    }
    return 0;
}
