#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void initializeMatrix(int* matrix, int size) {
    int i;
    for (i = 0; i < size * size; ++i) {
        matrix[i] = i;
    }
}

void matrixMultiply(int* A, int* B, int* C, int blockSize) {
    int i, j, k;
    for (i = 0; i < blockSize; ++i) {
        for (j = 0; j < blockSize; ++j) {
            for (k = 0; k < blockSize; ++k) {
                C[i * blockSize + j] += A[i * blockSize + k] * B[k * blockSize + j];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    int rank, nProcessors, matrixSize, blockSize;
    double t0 = 0.0, t1 = 0.0, time = 0.0;
    double timetot = 0.0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcessors);

    if (argc < 2) {
        if (rank == 0) {
            printf("Specify the matrix's size\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    
    if(nProcessors != 1 && nProcessors != 4){
      if (rank == 0) {
            printf("Number of processors must be 1 or 4\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    matrixSize = atoi(argv[1]);

    if(matrixSize < 1){
      if (rank == 0) {
            printf("Matrix size must be greater than 0\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int p = sqrt(nProcessors);

    if (matrixSize % p != 0) {
        if (rank == 0) {
            printf("Matrix size must be a multiple of the square root of the number of processors.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    blockSize = matrixSize / p;

    int* A = (int*)malloc(blockSize * blockSize * sizeof(int));
    int* B = (int*)malloc(blockSize * blockSize * sizeof(int));
    int* C = (int*)calloc(blockSize * blockSize, sizeof(int));

    initializeMatrix(A, blockSize);
    initializeMatrix(B, blockSize);

    int dims[2] = {p, p};
    int periods[2] = {0, 0};
    MPI_Comm gridComm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &gridComm);

    int coords[2];
    MPI_Cart_coords(gridComm, rank, 2, coords);

    MPI_Comm rowComm, colComm;
    MPI_Comm_split(gridComm, coords[0], coords[1], &rowComm);
    MPI_Comm_split(gridComm, coords[1], coords[0], &colComm);

    // Start timing
    t0 = MPI_Wtime();

    int step;
    int root;
    int shiftSrc, shiftDest;
    // Implement Broadcast-Multiply-Rolling Strategy
    for (step = 0; step < p; ++step) {
        // Broadcast A block within each column
        root = (coords[0] + step) % p;
        if (root == coords[1]) {
            MPI_Bcast(A, blockSize * blockSize, MPI_INT, root, colComm);
        }

        // Multiply current blocks of A and B
        matrixMultiply(A, B, C, blockSize);

        // Roll B block to the left
        
        MPI_Cart_shift(gridComm, 1, -1, &shiftSrc, &shiftDest); // Shift along the second dimension

        // Only perform send/receive if the ranks are valid
        if (shiftSrc != MPI_PROC_NULL && shiftDest != MPI_PROC_NULL) {
            MPI_Sendrecv_replace(B, blockSize * blockSize, MPI_INT, shiftDest, 0, shiftSrc, 0, gridComm, MPI_STATUS_IGNORE);
        }
    }

    // Stop timing and print result
    t1 = MPI_Wtime();
    time = t1 - t0;
    MPI_Reduce(&time, &timetot, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        printf("Total time: %f seconds\n\n", timetot);
    }

    // TODO: Gather results at root see if we need it

    free(A);
    free(B);
    free(C);

    MPI_Finalize();
    return 0;
}
