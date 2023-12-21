#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void initializeMatrix(int* matrix, int size) {
    int i;
    for (i = 0; i < size * size; ++i) {
        matrix[i] = 1;
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
    double startTime, endTime;
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
    if (rank == 0) {
        startTime = MPI_Wtime();
    }

    int step;
    int root;
    int leftRank, rightRank;
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
        MPI_Cart_shift(rowComm, 0, 1, &leftRank, &rightRank);
        MPI_Sendrecv_replace(B, blockSize * blockSize, MPI_INT, leftRank, 0, rightRank, 0, rowComm, MPI_STATUS_IGNORE);
    }

    // Stop timing and print result
    if (rank == 0) {
        endTime = MPI_Wtime();
        printf("Total time: %f seconds\n", endTime - startTime);
    }

    // TODO: Gather results at root see if we need it

    free(A);
    free(B);
    free(C);

    MPI_Finalize();
    return 0;
}
