#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include "uthash.h"

#define INPUT_BUF 1024

static bool double_mul_overflows(double a, double b) {
    if (b == 0.0) return false;
    return fabs(a) > DBL_MAX / fabs(b);
}

int int_rows;
int int_cols;

struct SavedMatrix {
    UT_hash_handle hh;
    int id;
    int nrows;
    int ncols;
    double *data;
};
struct SavedMatrix *matrix_store = NULL;

/* ── Allocation helpers ─────────────────────────────────────────────────── */

static void *xmalloc(size_t n) {
    void *p = malloc(n);
    if (!p) { fprintf(stderr, "Out of memory\n"); exit(1); }
    return p;
}

static void *xrealloc(void *p, size_t n) {
    void *q = realloc(p, n);
    if (!q) { fprintf(stderr, "Out of memory\n"); exit(1); }
    return q;
}

static char *xstrdup(const char *s) {
    char *p = strdup(s);
    if (!p) { fprintf(stderr, "Out of memory\n"); exit(1); }
    return p;
}

/* ── Hash table helpers ─────────────────────────────────────────────────── */

static void store_matrix(struct SavedMatrix *s) {
    HASH_ADD_INT(matrix_store, id, s);
}

static struct SavedMatrix *find_matrix(int id) {
    struct SavedMatrix *s;
    HASH_FIND_INT(matrix_store, &id, s);
    return s;
}

static void delete_all_matrices(void) {
    struct SavedMatrix *cur, *tmp;
    HASH_ITER(hh, matrix_store, cur, tmp) {
        HASH_DEL(matrix_store, cur);
        free(cur->data);
        free(cur);
    }
}

/* ── Matrix display ─────────────────────────────────────────────────────── */

static void printMatrix(double *m) {
    for (int i = 0; i < int_rows; i++) {
        for (int j = 0; j < int_cols; j++)
            printf("%g\t", m[i * int_cols + j]);
        printf("\n");
    }
}

/* ── Core operations ────────────────────────────────────────────────────── */

static void setPointValue(double value, int row, int col, int rows, int cols, double *matrix) {
    if (row < 0 || row >= rows || col < 0 || col >= cols) {
        printf("Error: position (%d,%d) out of bounds for %dx%d matrix\n", row, col, rows, cols);
        return;
    }
    matrix[row * cols + col] = value;
}

static void fillValues(double value, int rows, int cols, double *matrix) {
    for (int i = 0; i < rows * cols; i++)
        matrix[i] = value;
}

static void resizeMatrix(int rowDim, int colDim, int preRowDim, int preColDim, double **matrix) {
    size_t oldSize = (size_t)preRowDim * preColDim;
    size_t newSize = (size_t)rowDim * colDim;
    *matrix = xrealloc(*matrix, newSize * sizeof(double));
    for (size_t i = oldSize; i < newSize; i++)
        (*matrix)[i] = 0.0;
    int_rows = rowDim;
    int_cols = colDim;
}

static void newRandomMatrix(int rowDim, int colDim, double **matrix) {
    *matrix = xrealloc(*matrix, (size_t)rowDim * colDim * sizeof(double));
    for (int i = 0; i < rowDim * colDim; i++)
        (*matrix)[i] = rand() % 100;
    int_rows = rowDim;
    int_cols = colDim;
}

static void multiplyMatrixWithInteger(double value, int rows, int cols, double *matrix) {
    for (int i = 0; i < rows * cols; i++) {
        if (double_mul_overflows(value, matrix[i]))
            printf("Warning: multiplication would overflow at element [%d]\n", i);
        matrix[i] = value * matrix[i];
        if (isinf(matrix[i]) || isnan(matrix[i]))
            printf("Warning: overflow detected at element [%d]\n", i);
    }
}

static void transposeMatrix(double **matrix) {
    int new_rows = int_cols;
    int new_cols = int_rows;
    double *result = xmalloc((size_t)new_rows * new_cols * sizeof(double));
    for (int i = 0; i < int_rows; i++)
        for (int j = 0; j < int_cols; j++)
            result[j * new_cols + i] = (*matrix)[i * int_cols + j];
    free(*matrix);
    *matrix = result;
    int_rows = new_rows;
    int_cols = new_cols;
}

static void generateIdentityMatrix(double **matrix) {
    if (int_rows != int_cols) {
        printf("Cannot generate identity: matrix is not square (%dx%d)\n", int_rows, int_cols);
        return;
    }
    for (int i = 0; i < int_rows; i++)
        for (int j = 0; j < int_cols; j++)
            (*matrix)[i * int_cols + j] = (i == j) ? 1.0 : 0.0;
}

static void exportAsEquations(double *matrix, int rows, int cols) {
    int coef_cols = (cols > 1) ? cols - 1 : cols;
    bool use_rhs = (cols > 1);
    for (int i = 0; i < rows; i++) {
        bool first_term = true;
        for (int j = 0; j < coef_cols; j++) {
            double coef = matrix[i * cols + j];
            if (coef == 0.0) continue;
            if (first_term) {
                printf("%g*x%d", coef, j + 1);
                first_term = false;
            } else if (coef < 0) {
                printf(" - %g*x%d", -coef, j + 1);
            } else {
                printf(" + %g*x%d", coef, j + 1);
            }
        }
        if (first_term) printf("0");
        if (use_rhs)
            printf(" = %g\n", matrix[i * cols + cols - 1]);
        else
            printf(" = 0\n");
    }
}

static void addMatrixToMatrix(int matrixId, double **matrix) {
    struct SavedMatrix *s = find_matrix(matrixId);
    if (!s) { printf("Matrix with id %d not found\n", matrixId); return; }
    if (s->nrows != int_rows || s->ncols != int_cols) {
        printf("Dimensions don't match (%dx%d vs %dx%d): cannot add\n",
               int_rows, int_cols, s->nrows, s->ncols);
        return;
    }
    for (int i = 0; i < int_rows * int_cols; i++) {
        (*matrix)[i] += s->data[i];
        if (isinf((*matrix)[i]) || isnan((*matrix)[i]))
            printf("Warning: overflow detected during add at element [%d]\n", i);
    }
}

static void subtractMatrixFromMatrix(int matrixId, double **matrix) {
    struct SavedMatrix *s = find_matrix(matrixId);
    if (!s) { printf("Matrix with id %d not found\n", matrixId); return; }
    if (s->nrows != int_rows || s->ncols != int_cols) {
        printf("Dimensions don't match (%dx%d vs %dx%d): cannot subtract\n",
               int_rows, int_cols, s->nrows, s->ncols);
        return;
    }
    for (int i = 0; i < int_rows * int_cols; i++) {
        (*matrix)[i] -= s->data[i];
        if (isinf((*matrix)[i]) || isnan((*matrix)[i]))
            printf("Warning: overflow detected during subtract at element [%d]\n", i);
    }
}

/* A (saved, m×n) × B (current, n×k) → result (m×k) */
static void multiplyMatrixWithMatrix(int matrixId, double **matrix) {
    struct SavedMatrix *s = find_matrix(matrixId);
    if (!s) { printf("Matrix with id %d not found\n", matrixId); return; }

    int m = s->nrows, n = s->ncols;
    int k = int_cols;

    if (n != int_rows) {
        printf("Dimensions incompatible: saved is %dx%d, current is %dx%d\n",
               m, n, int_rows, k);
        return;
    }

    double *result = xmalloc((size_t)m * k * sizeof(double));

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < k; j++) {
            double dot = 0.0;
            for (int p = 0; p < n; p++) {
                double a = s->data[i * n + p];
                double b = (*matrix)[p * k + j];
                if (double_mul_overflows(a, b))
                    printf("Warning: dot product multiplication would overflow at [%d,%d]\n", i, j);
                dot += a * b;
                if (isinf(dot) || isnan(dot))
                    printf("Warning: dot product overflow detected at [%d,%d]\n", i, j);
            }
            result[i * k + j] = dot;
        }
    }

    free(*matrix);
    *matrix = result;
    int_rows = m;
    int_cols = k;
}

/* ── Tokenizer ──────────────────────────────────────────────────────────── */

static char **tokenize(const char *in, int *out_count) {
    char *temp = xstrdup(in);
    char *token = strtok(temp, " \t\n\r");
    char **list = NULL;
    int count = 0;
    while (token) {
        list = xrealloc(list, (size_t)(count + 1) * sizeof(char *));
        list[count++] = xstrdup(token);
        token = strtok(NULL, " \t\n\r");
    }
    free(temp);
    list = xrealloc(list, (size_t)(count + 1) * sizeof(char *));
    list[count] = NULL;
    if (out_count) *out_count = count;
    return list;
}

static void free_tokens(char **list) {
    if (!list) return;
    for (int i = 0; list[i]; i++)
        free(list[i]);
    free(list);
}

/* ── Command handlers ───────────────────────────────────────────────────── */

static void cmd_set(char **t, double **mp) {
    setPointValue(atof(t[3]), atoi(t[1]), atoi(t[2]), int_rows, int_cols, *mp);
}
static void cmd_fill(char **t, double **mp) {
    fillValues(atof(t[1]), int_rows, int_cols, *mp);
}
static void cmd_resize(char **t, double **mp) {
    resizeMatrix(atoi(t[1]), atoi(t[2]), int_rows, int_cols, mp);
}
static void cmd_newrand(char **t, double **mp) {
    newRandomMatrix(atoi(t[1]), atoi(t[2]), mp);
}
static void cmd_mult(char **t, double **mp) {
    multiplyMatrixWithInteger(atof(t[1]), int_rows, int_cols, *mp);
}
static void cmd_save(char **t, double **mp) {
    int id = atoi(t[1]);
    struct SavedMatrix *existing = find_matrix(id);
    if (existing) {
        free(existing->data);
        existing->data  = xmalloc((size_t)int_rows * int_cols * sizeof(double));
        existing->nrows = int_rows;
        existing->ncols = int_cols;
        memcpy(existing->data, *mp, (size_t)int_rows * int_cols * sizeof(double));
        return;
    }
    struct SavedMatrix *s = xmalloc(sizeof(struct SavedMatrix));
    s->data  = xmalloc((size_t)int_rows * int_cols * sizeof(double));
    s->id    = id;
    s->nrows = int_rows;
    s->ncols = int_cols;
    memcpy(s->data, *mp, (size_t)int_rows * int_cols * sizeof(double));
    store_matrix(s);
}
static void cmd_load(char **t, double **mp) {
    int id = atoi(t[1]);
    struct SavedMatrix *s = find_matrix(id);
    if (!s) { printf("Matrix with id %d not found\n", id); return; }
    *mp = xrealloc(*mp, (size_t)s->nrows * s->ncols * sizeof(double));
    memcpy(*mp, s->data, (size_t)s->nrows * s->ncols * sizeof(double));
    int_rows = s->nrows;
    int_cols = s->ncols;
}
static void cmd_mm(char **t, double **mp) { multiplyMatrixWithMatrix(atoi(t[1]), mp); }
static void cmd_ma(char **t, double **mp) { addMatrixToMatrix(atoi(t[1]), mp); }
static void cmd_ms(char **t, double **mp) { subtractMatrixFromMatrix(atoi(t[1]), mp); }

static void cmd_tr(double **mp) { transposeMatrix(mp); }
static void cmd_id(double **mp) { generateIdentityMatrix(mp); }
static void cmd_eq(double **mp) { exportAsEquations(*mp, int_rows, int_cols); }

/* ── Dispatch ───────────────────────────────────────────────────────────── */

typedef struct { const char *name; int args; void (*fn)(char **, double **); } ArgCmd;
typedef struct { const char *name;            void (*fn)(double **);         } NoArgCmd;

static const ArgCmd arg_commands[] = {
    {"set",     3, cmd_set},
    {"fill",    1, cmd_fill},
    {"resize",  2, cmd_resize},
    {"newrand", 2, cmd_newrand},
    {"mult",    1, cmd_mult},
    {"save",    1, cmd_save},
    {"load",    1, cmd_load},
    {"mm",      1, cmd_mm},
    {"ma",      1, cmd_ma},
    {"ms",      1, cmd_ms},
    {NULL,      0, NULL}
};

static const NoArgCmd noarg_commands[] = {
    {"tr", cmd_tr},
    {"id", cmd_id},
    {"eq", cmd_eq},
    {NULL, NULL}
};

/* Returns 1 if the REPL should exit, 0 otherwise. */
static int dispatch(char **tokens, int count, double **mp) {
    if (strcmp(tokens[0], "exit") == 0) return 1;

    for (const ArgCmd *c = arg_commands; c->name; c++) {
        if (strcmp(tokens[0], c->name) == 0) {
            if (count - 1 < c->args) {
                printf("Error: '%s' requires %d argument(s)\n", c->name, c->args);
                return 0;
            }
            c->fn(tokens, mp);
            return 0;
        }
    }

    for (const NoArgCmd *c = noarg_commands; c->name; c++) {
        if (strcmp(tokens[0], c->name) == 0) {
            c->fn(mp);
            return 0;
        }
    }

    printf("Unrecognized command: %s\n", tokens[0]);
    return 0;
}

/* ── Entry point ────────────────────────────────────────────────────────── */

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <rows> <cols>\n", argv[0]);
        return 1;
    }
    int_rows = atoi(argv[1]);
    int_cols = atoi(argv[2]);
    if (int_rows <= 0 || int_cols <= 0) {
        fprintf(stderr, "Error: dimensions must be positive integers\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    double *matrix = calloc((size_t)int_rows * int_cols, sizeof(double));
    if (!matrix) { fprintf(stderr, "Out of memory\n"); return 1; }

    printf("Matrix calculator: %dx%d\n", int_rows, int_cols);
    printf("Commands: set <r> <c> <v>, fill <v>, resize <r> <c>, newrand <r> <c>,\n");
    printf("  mult <v>, save <id>, load <id>, mm <id>, tr, id, ma <id>, ms <id>, eq, exit\n");

    char in[INPUT_BUF];
    while (1) {
        printMatrix(matrix);
        if (!fgets(in, sizeof(in), stdin))
            break;
        int count = 0;
        char **tokens = tokenize(in, &count);
        if (count == 0) { free_tokens(tokens); continue; }
        int should_exit = dispatch(tokens, count, &matrix);
        free_tokens(tokens);
        if (should_exit) break;
    }

    free(matrix);
    delete_all_matrices();
    return 0;
}
