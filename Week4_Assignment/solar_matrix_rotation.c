#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void print_matrix(int **grid, int matrix_size) {
    int **cell = grid;
    for (int row = 0; row < matrix_size; row++) {
        for (int col = 0; col < matrix_size; col++, cell++)
            printf("%4d", **cell);
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

static void smooth(int **grid, int matrix_size) {
    int *prev_row_vals = malloc(matrix_size * sizeof(int));
    int *curr_row_vals = malloc(matrix_size * sizeof(int));
    if (!prev_row_vals || !curr_row_vals) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    int **row_start = grid;
    for (int row = 0; row < matrix_size; row++, row_start += matrix_size) {
        
        
        int **cell = row_start;
        int *snapshot = curr_row_vals;
        for (int col = 0; col < matrix_size; col++, cell++, snapshot++)
            *snapshot = **cell;

        cell = row_start;
        for (int col = 0; col < matrix_size; col++, cell++) {
            int sum = 0, count = 0;
            for (int col_offset = -1; col_offset <= 1; col_offset++) {
                int neighbor_col = col + col_offset;
                if (neighbor_col < 0 || neighbor_col >= matrix_size) continue;

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
}

int main(void) {
    int matrix_size;
    printf("Enter matrix size (2-10): ");
    if (scanf("%d", &matrix_size) != 1 || matrix_size < 2 || matrix_size > 10) {
        printf("Invalid size. Must be between 2 and 10.\n");
        return 1;
    }

    srand((unsigned)time(NULL));

    int **grid = malloc((size_t)matrix_size * matrix_size * sizeof(int *));
    if (!grid) return 1;
    for (int **cell = grid; cell < grid + matrix_size * matrix_size; cell++) {
        *cell = malloc(sizeof(int));
        **cell = rand() % 256;
    }

    printf("\nOriginal Randomly Generated Matrix:\n");
    print_matrix(grid, matrix_size);

    rotate_cw(grid, matrix_size);
    printf("\nMatrix after 90° Clockwise Rotation:\n");
    print_matrix(grid, matrix_size);

    smooth(grid, matrix_size);
    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    print_matrix(grid, matrix_size);

    for (int **cell = grid; cell < grid + matrix_size * matrix_size; cell++) free(*cell);
    free(grid);
    return 0;
}