#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1000  // Dimension of the matrices

void initialize_matrix(double* matrix, int rows, int cols) {
    int i, j;

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            matrix[i*cols + j] = 1.0;
        }
    }
}

void multiply(double* A, double* B, double* C, int rows, int cols) {
    int i, j, k;
    
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            C[i*cols + j] = 0.0;
            for (k = 0; k < cols; k++) {
                C[i*cols + j] += A[i*cols + k] * B[k*cols + j];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    int rank, size;
    int i, j;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int dims[2] = {0, 0};
    MPI_Dims_create(size, 2, dims);

    int periods[2] = {0, 0};
    MPI_Comm grid_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &grid_comm);

    int coords[2];
    MPI_Cart_coords(grid_comm, rank, 2, coords);

    int rows = N / dims[0];
    int cols = N / dims[1];

    double* A = (double*)malloc(rows * cols * sizeof(double));
    double* B = (double*)malloc(rows * cols * sizeof(double));
    double* C = (double*)malloc(rows * cols * sizeof(double));

    if (rank == 0) {
        initialize_matrix(A, rows, cols);
        initialize_matrix(B, rows, cols);
    }

    MPI_Bcast(A, rows*cols, MPI_DOUBLE, 0, grid_comm);
    MPI_Bcast(B, rows*cols, MPI_DOUBLE, 0, grid_comm);

    multiply(A, B, C, rows, cols);

    if (rank == 0) {
        for (i = 0; i < rows; i++) {
            for (j = 0; j < cols; j++) {
                printf("%f ", C[i*cols + j]);
            }
            printf("\n");
        }
    }

    free(A);
    free(B);
    free(C);

    MPI_Finalize();

    return 0;
}