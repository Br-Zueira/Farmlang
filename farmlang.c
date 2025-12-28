#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "farmlang.h"

int main(int argc, char *argv[]) {
    // inicializa aleatoriedade uma vez
    srand((unsigned)time(NULL));

    if (argc < 2) {
        fprintf(stderr, "Usage: %s file.farm\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("Error opening file");
        return 1;
    }

    // lê todo o arquivo para um buffer
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        perror("Memory error");
        fclose(f);
        return 1;
    }

    fread(buffer, 1, size, f);
    buffer[size] = '\0'; // termina a string

    fclose(f);

    // roda o interpretador FarmLang
    int result = fl_run_source(buffer);

    free(buffer);
    return result;
}
