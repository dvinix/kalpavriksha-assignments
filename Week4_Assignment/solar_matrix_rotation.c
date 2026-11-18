#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_MATRIX_SIZE 2
#define MAX_MATRIX_SIZE 10
#define MAX_INTENSITY   256

static void print_matrix(int **grid, int matrix_size) {
    int **cell = grid;
    for (int row = 0; row < matrix_size; row++) {
        for (int col = 0; col < matrix_size; col++, cell++) {
            printf("%4d", **cell);
        }
        printf("\n");
    }
}

static void rotate_cw(int **grid, int matrix_size) {
    for (int layer = 0; layer < matrix_size / 2; layer++) {
        for (int offset = layer; offset < matrix_size - layer - 1; offset++) {
            int **top    = grid + layer * matrix_size + offset;
            int **right  = grid + offset * matrix_size + (matrix_size - 1 - layer);
            int **bottom = grid + (matrix_size - 1 - layer) * matrix_size + (matrix_size - 1 - offset);
            int **left   = grid + (matrix_size - 1 - offset) * matrix_size + layer;

            int *saved = *top;
            *top       = *left;
            *left      = *bottom;
            *bottom    = *right;
            *right     = saved;
        }
    }
}

static bool smooth(int **grid, int matrix_size) {
    int *prev_row_vals = malloc((size_t)matrix_size * sizeof(int));
    int *curr_row_vals = malloc((size_t)matrix_size * sizeof(int));
    if (!prev_row_vals || !curr_row_vals) {
        fprintf(stderr, "Out of memory\n");
        free(prev_row_vals);
        free(curr_row_vals);
        return false;
    }

    int **row_start = grid;
    for (int row = 0; row < matrix_size; row++, row_start += matrix_size) {
        int **cell = row_start;
        int *snapshot = curr_row_vals;
        for (int col = 0; col < matrix_size; col++, cell++, snapshot++) {
            *snapshot = **cell;
        }

        cell = row_start;
        for (int col = 0; col < matrix_size; col++, cell++) {
            int sum = 0, count = 0;
            for (int col_offset = -1; col_offset <= 1; col_offset++) {
                int neighbor_col = col + col_offset;
                if (neighbor_col < 0 || neighbor_col >= matrix_size) {
                    continue;
                }

                if (row > 0) {
                    sum += *(prev_row_vals + neighbor_col);
                    count++;
                }
                sum += *(curr_row_vals + neighbor_col);
                count++;
                if (row < matrix_size - 1) {
                    sum += **(row_start + matrix_size + neighbor_col);
                    count++;
                }
            }
            **cell = sum / count;
        }

        int *swap_buf = prev_row_vals;
        prev_row_vals = curr_row_vals;
        curr_row_vals = swap_buf;
    }

    free(prev_row_vals);
    free(curr_row_vals);
    return true;
}

static int read_matrix_size(void) {
    char buffer[64];
    while (1) {
        printf("Enter matrix size (%d-%d): ", MIN_MATRIX_SIZE, MAX_MATRIX_SIZE);
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            return -1;
        }

        char *endptr;
        errno = 0;
        long val = strtol(buffer, &endptr, 10);

        if (endptr == buffer || errno == ERANGE) {
            printf("Invalid size. Must be between %d and %d.\n", MIN_MATRIX_SIZE, MAX_MATRIX_SIZE);
            continue;
        }

        while (*endptr == ' ' || *endptr == '\t' || *endptr == '\r' || *endptr == '\n') {
            endptr++;
        }

        if (*endptr != '\0' || val < MIN_MATRIX_SIZE || val > MAX_MATRIX_SIZE) {
            printf("Invalid size. Must be between %d and %d.\n", MIN_MATRIX_SIZE, MAX_MATRIX_SIZE);
            continue;
        }

        return (int)val;
    }
}

static void free_matrix(int **grid, int matrix_size) {
    if (!grid) {
        return;
    }
    int total_cells = matrix_size * matrix_size;
    for (int **cell = grid; cell < grid + total_cells; cell++) {
        free(*cell);
    }
    free(grid);
}

static int **create_matrix(int matrix_size) {
    int total_cells = matrix_size * matrix_size;
    int **grid = malloc((size_t)total_cells * sizeof(int *));
    if (!grid) {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }

    for (int **cell = grid; cell < grid + total_cells; cell++) {
        *cell = malloc(sizeof(int));
        if (!*cell) {
            fprintf(stderr, "Out of memory\n");
            for (int **cleanup = grid; cleanup < cell; cleanup++) {
                free(*cleanup);
            }
            free(grid);
            return NULL;
        }
        **cell = rand() % MAX_INTENSITY;
    }

    return grid;
}

int main(void) {
    int matrix_size = read_matrix_size();
    if (matrix_size < 0) {
        return 1;
    }

    srand((unsigned)time(NULL));

    int **grid = create_matrix(matrix_size);
    if (!grid) {
        return 1;
    }

    printf("\nOriginal Randomly Generated Matrix:\n");
    print_matrix(grid, matrix_size);

    rotate_cw(grid, matrix_size);
    printf("\nMatrix after 90° Clockwise Rotation:\n");
    print_matrix(grid, matrix_size);

    if (!smooth(grid, matrix_size)) {
        free_matrix(grid, matrix_size);
        return 1;
    }
    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    print_matrix(grid, matrix_size);

    free_matrix(grid, matrix_size);
    return 0;
}
