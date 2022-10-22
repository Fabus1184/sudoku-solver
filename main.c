#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#define USAGE \
"Usage: ./main {file | -}\n" \
"Solve the sudoku specified in file or from stdin.\n" \
"\n"          \
"Options:\n"              \
"   -h, --help       display this message\n"          \
"\n"              \
"The first 91 ascii numbers of file or stdin are interpreted as a sudoku,\n"\
"any other characters are ignored.\n"

uint8_t sudoku[9 * 9];

uint32_t djb2(void) {
    uint32_t hash = 5381;
    for (uint8_t i = 0; i < (9 * 9); ++i) {
        hash = ((hash << 5U) + hash) + sudoku[i];
    }
    return hash;
}

uint8_t row_col(uint8_t row, uint8_t col) {
    return sudoku[(row * 9) + col];
}

uint8_t group_field(uint8_t group, uint8_t field) {
    uint8_t row = (uint8_t) ((3 * (group / 3)) + (field / 3));
    uint8_t col = (uint8_t) ((3 * (group % 3)) + (field % 3));
    return row_col(row, col);
}

uint16_t possible(uint8_t cell) {
    uint16_t result = 0xFFFF;
    uint8_t row = (uint8_t) (cell / 9);
    for (uint8_t i = 0; i < 9; ++i) {
        uint8_t n = row_col(row, i);
        if (n != 0) {
            result &= ~(1U << (n - 1U));
        }
    }

    uint8_t col = (uint8_t) (cell % 9);
    for (uint8_t i = 0; i < 9; ++i) {
        uint8_t n = row_col(i, col);
        if (n != 0) {
            result &= ~(1U << (n - 1U));
        }
    }

    uint8_t group = (uint8_t) ((3 * (row / 3)) + (col / 3));
    for (uint8_t i = 0; i < 9; ++i) {
        uint8_t n = group_field(group, i);
        if (n != 0) {
            result &= ~(1U << (n - 1U));
        }
    }

    return result;
}

uint8_t iterations = 0;
uint8_t solutions = 0;

uint8_t solve(uint8_t cell, uint8_t single) {
    ++iterations;

    if (cell >= (9 * 9)) {
        ++solutions;
        return single;
    }

    if (sudoku[cell] != 0) {
        return solve((uint8_t) (cell + 1), single);
    }

    uint16_t n = possible(cell);
    if (0 == n) {
        return 0;
    }

    uint8_t tmp = sudoku[cell];
    for (uint8_t i = 0; i < 9; ++i) {
        if (1 == ((n >> i) & 1U)) {
            sudoku[cell] = (uint8_t) (i + 1);
            uint8_t result = solve((uint8_t) (cell + 1), single);
            if (0 != result) {
                return result;
            }
        }
    }
    sudoku[cell] = tmp;
    return 0;
}

#define UNPACK9(x) *(x), *((x) + 1), *((x) + 2), *((x) + 3), *((x) + 4), *((x) + 5), *((x) + 6), *((x) + 7), *((x) + 8)

void print(void) {
    printf("╔═════════╦═════════╦═════════╗\n");
    for (uint8_t row = 0; row < 9; ++row) {
        if ((3 == row) || (6 == row)) {
            printf("╠═════════╬═════════╬═════════╣\n");
        }
        printf("║ %hhd  %hhd  %hhd ║ %hhd  %hhd  %hhd ║ %hhd  %hhd  %hhd ║\n", UNPACK9(sudoku + (9 * row)));
    }
    printf("╚═════════╩═════════╩═════════╝\n");
}

int64_t micros(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (1000 * 1000 * ts.tv_sec) + (ts.tv_nsec / 1000);
}

bool parse(FILE *file) {
    int8_t n = 0;
    int32_t c;
    while ((EOF != (c = fgetc(file))) && (n < (9 * 9))) {
        if ((c >= '0') && (c <= '9')) {
            sudoku[n++] = (uint8_t) (c - '0');
        }
    }
    fclose(file);
    return n == (9 * 9);
}

int main(int argc, char **argv) {
    if (((argc != 1) && (argc != 2))
        || ((2 == argc) && (0 == strcmp(argv[1], "-h")))) {
        printf("%s", USAGE);
        return 1;
    }

    FILE *file;
    if (1 == argc) {
        file = stdin;
    } else {
        file = fopen(argv[1], "r");
    }

    if (NULL == file) {
        printf("Error: Could not open file '%s'\n", argv[1]);
        return 1;
    }

    if (!parse(file)) {
        if (1 == argc) {
            printf("Error: Could not parse stdin\n");
        } else {
            printf("Error: Could not parse file '%s'\n", argv[1]);
        }
        return 1;
    }

    int64_t start = micros();
    solve(0, 1);
    int64_t end = micros();

    printf("Finished: %'d iterations, %d solutions, took %.3fms, djb2 hash: 0x%X\n", iterations, solutions,
           ((float_t) (uint64_t) (end - start)) / 1000.f, djb2());
    print();
}
