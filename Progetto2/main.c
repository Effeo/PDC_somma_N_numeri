#include <stdio.h>
#include <stdlib.h>

#include <omp.h>
#include <math.h>

int main(int argc, char *argv[])
{
    int dimVetInput = atoi(argv[1]);
    int righeMatInput = atoi(argv[2]);
    int colMatInput = atoi(argv[3]);
    int i, j;

    int** matInput = (int**) malloc(righeMatInput * sizeof(int*));
    int* vetInput = (int*) malloc(dimVetInput * sizeof(int));

    // inizializzazione matrice e vettore
    for (i = 0; i < righeMatInput; i++){
        matInput[i] = (int*) malloc(colMatInput * sizeof(int));
    }

    for(i = 0; i < righeMatInput; i++){
        for(j = 0; j < colMatInput; j++){
            matInput[i][j] = atoi(argv[4 + i * colMatInput + j]);
        }
    }

    for(i = 0; i < dimVetInput; i++){
        vetInput[i] = atoi(argv[4 + righeMatInput * colMatInput + i]);
    }

    int numThreads = omp_get_max_threads();
    int blockRows = sqrt(numThreads);
    int blockCols = numThreads / blockRows;
    int rowBlockSize = righeMatInput / blockRows;
    int colBlockSize = colMatInput / blockCols;
    int rowRemainder = righeMatInput % blockRows;
    int colRemainder = colMatInput % blockCols;

    int* vet_output = (int*) calloc(righeMatInput, sizeof(int));

    #pragma omp parallel private(i, j)
    {
        int threadNum = omp_get_thread_num();
        int blockRow = threadNum / blockCols;
        int blockCol = threadNum % blockCols;
        int startRow = blockRow * rowBlockSize;
        int endRow = (blockRow == blockRows - 1) ? startRow + rowBlockSize + rowRemainder : startRow + rowBlockSize;
        int startCol = blockCol * colBlockSize;
        int endCol = (blockCol == blockCols - 1) ? startCol + colBlockSize + colRemainder : startCol + colBlockSize;

        for (i = startRow; i < endRow; ++i)
        {
            for (j = startCol; j < endCol; ++j)
            {
                vet_output[i] += matInput[i][j] * vetInput[j];
            }
        }
    }

    printf("risultato:\n");
    for(i = 0; i < righeMatInput; ++i){
        printf("%d\n", vet_output[i]);
    }

    free(vet_output);
    
    return EXIT_SUCCESS;
}
