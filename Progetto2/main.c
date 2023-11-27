#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

int main(int argc, char *argv[])
{
    int dimVet = atoi(argv[1]);
    int righeMat = atoi(argv[2]);
    int colMat = atoi(argv[3]);
    int i, j;

    int** mat = (int**) malloc(righeMat * sizeof(int*));
    int* vettore = (int*) malloc(dimVet * sizeof(int));

    for (i = 0; i < righeMat; i++){
        mat[i] = (int*) malloc(colMat * sizeof(int));
    }

    for(i = 0; i < righeMat; i++){
        for(j = 0; j < colMat; j++){
            mat[i][j] = atoi(argv[4 + i * colMat + j]);
        }
    }

    for(i = 0; i < dimVet; i++){
        vettore[i] = atoi(argv[4 + righeMat * colMat + i]);
    }

    int** resultMat = (int**) malloc(righeMat * sizeof(int*));
    for (i = 0; i < colMat; i++){
        resultMat[i] = (int*) malloc(omp_get_num_threads() * sizeof(int));
    }
    printf("num_threads: %d\n", omp_get_num_threads());
    
    #pragma omp parallel for shared(resultMat, mat, vettore, colMat, righeMat) private(i, j) num_threads(omp_get_num_threads())
    for(j = 0; j < colMat; ++j){
         printf("num_threads 2: %d\n", omp_get_num_threads());
        for(i = 0; i < righeMat; ++i){
            resultMat[i][omp_get_thread_num()] = mat[i][j] * vettore[j];
        }
    }
    printf("num_threads 2: %d\n", omp_get_num_threads());

    printf("matrice risultato:\n");
    for(i = 0; i < righeMat; i++){
        for(j = 0; j < omp_get_num_threads(); j++){
            printf("(%d, %d): %d ", i, j, resultMat[i][j]);
        }
        printf("\n");
    }

    int* resVec =  (int*) calloc(righeMat, sizeof(int));
    
    #pragma omp parallel for
    for(i = 0; i < righeMat; i++){
        for(j = 0; j < omp_get_num_threads(); j++){
            resVec[i] += resultMat[i][j];
        }
    }
    
    /*
    printf("vettore risultato:\n");
    for(i = 0; i < righeMat; i++){
        printf("%d\n", resVec[i]);
    }
    */
    return EXIT_SUCCESS;
}
