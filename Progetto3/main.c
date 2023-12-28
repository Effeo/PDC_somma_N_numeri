#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void initializeMatrix(int* matrix, int size) {
    int i;
    for (i = 0; i < size * size; ++i) {
        matrix[i] = i + 1;
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
    double t0, t1, time, timetot;
    int *fullA, *fullB, *A, *B, *C, *finalMatrix;
    int i, j;

    if (argc < 2) {
        printf("Specify the matrix's size\n");
        return EXIT_FAILURE;
    }

    matrixSize = atoi(argv[1]);
    if (matrixSize < 1) {
        printf("Matrix size must be greater than 0\n");
        return EXIT_FAILURE;
    }

    fullA = (int*)malloc(matrixSize * matrixSize * sizeof(int));
    fullB = (int*)malloc(matrixSize * matrixSize * sizeof(int));

    if( fullA == NULL || fullB == NULL){
        printf("Error allocating memory for fullA or fullB\n");
        return EXIT_FAILURE;
    }

    initializeMatrix(fullA, matrixSize);
    initializeMatrix(fullB, matrixSize);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcessors);

    if (nProcessors != 1 || nProcessors != 4) {
        if (rank == 0) {
            printf("Number of processors must be 1 or 4\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    
    int p = sqrt(nProcessors);
    if (matrixSize % p != 0 || nProcessors != p * p) {
        if (rank == 0) {
            printf("Matrix size must be a multiple of the square root of the number of processors and processors must be a perfect square.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    blockSize = matrixSize / p;
    A = (int*)malloc(blockSize * blockSize * sizeof(int));
    B = (int*)malloc(blockSize * blockSize * sizeof(int));
    C = (int*)calloc(blockSize * blockSize, sizeof(int));

    if( A == NULL || B == NULL || C == NULL){
        printf("Error allocating memory for A, B or C\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int dims[2] = {p, p};
    int periods[2] = {0, 0};
    int coords[2];
    int subCoords[2];

    MPI_Comm gridComm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &gridComm);
    MPI_Cart_coords(gridComm, rank, 2, coords);

    subCoords[0] = 0;
    subCoords[1] = 1;
    MPI_Comm rowComm;
    MPI_Cart_sub(gridComm, subCoords, &rowComm);
    
    subCoords[0] = 1;
    subCoords[1] = 0;
    MPI_Comm colComm;
    MPI_Cart_sub(gridComm, subCoords, &colComm);

    MPI_Barrier(MPI_COMM_WORLD);
    t0 = MPI_Wtime();
    
    int startRow = coords[0] * blockSize;
    int startCol = coords[1] * blockSize;
    for (i = 0; i < blockSize; ++i) {
        for (j = 0; j < blockSize; ++j) {
            A[i * blockSize + j] = fullA[(startRow + i) * matrixSize + (startCol + j)];
            B[i * blockSize + j] = fullB[(startRow + i) * matrixSize + (startCol + j)];
        }
    }
    
    int step;
    int* AToBeBroadcasted = (int*)malloc(blockSize * blockSize * sizeof(int));
    if( AToBeBroadcasted == NULL){
        printf("Error allocating memory for AToBeBroadcasted\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int root, destination, source;
    for (step = 0; step < dims[0]; ++step) {
        // Broadcast A
        root = (coords[0] + step) % p;
        if(root == coords[1]){
            AToBeBroadcasted = A;
        }

        MPI_Bcast(AToBeBroadcasted, blockSize * blockSize, MPI_INT, root, rowComm);

        // Multiply A and B
        matrixMultiply(AToBeBroadcasted, B, C, blockSize);

        // Roll B
        destination = (coords[0] - 1 + p) % p;
        source = (coords[0] + 1) % p;
        MPI_Sendrecv_replace(B, blockSize * blockSize, MPI_INT, destination, 0, source, 0, colComm, MPI_STATUS_IGNORE);
    }
    free(AToBeBroadcasted);

    t1 = MPI_Wtime();
    time = t1 - t0;
    MPI_Reduce(&time, &timetot, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        printf("Total time: %f seconds\n\n", timetot);
    }

    MPI_Finalize();
    return 0;
}