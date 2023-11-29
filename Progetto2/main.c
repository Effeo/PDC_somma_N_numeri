#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

int main(int argc, char *argv[])
{
    int dimVetInput = atoi(argv[1]);
    int righeMatInput = atoi(argv[2]);
    int colMatInput = atoi(argv[3]);
    int i, j;

    int** matInput = (int**) malloc(righeMatInput * sizeof(int*));
    int* vetInput = (int*) malloc(dimVetInput * sizeof(int));

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

    int** matProdotto = (int**) malloc(righeMatInput * sizeof(int*));
    for (i = 0; i < righeMatInput; i++){
        matProdotto[i] = (int*) malloc(omp_get_max_threads() * sizeof(int));
    }

    int k;
    int tempProdotto = 0;
    #pragma omp parallel for shared(matProdotto, matInput, vetInput, righeMatInput, colMatInput) private(i, j, k, tempProdotto)
    for(j = 0; j < colMatInput; ++j){
        for(i = 0; i < righeMatInput; ++i){
            for(k = 0; k < colMatInput; ++k){
                tempProdotto += matInput[i][k] * vetInput[k];
            }
            
            matProdotto[j][omp_get_thread_num()] = tempProdotto;
        }
    }

    /*printf("matrice risultato:\n");
    for(i = 0; i < righeMatInput; i++){
        for(j = 0; j < omp_get_max_threads(); j++){
            printf("%d ", matProdotto[i][j]);
        }
        printf("\n");
    }
    printf("\n");*/

    int* vetSomme =  (int*) calloc(righeMatInput, sizeof(int)); 
    #pragma omp parallel for
    for(i = 0; i < righeMatInput; i++){
        for(j = 0; j < omp_get_num_threads(); j++){
            printf("Mat[%d, %d] thread %d: %d\n", i, j, omp_get_thread_num(), matProdotto[i][j]);
            vetSomme[i] += matProdotto[i][j];
            printf("VetSomme[%d] thread %d: %d\n", i, omp_get_thread_num(), vetSomme[i]);
        }
        printf("VetSommeFinale[%d] thread %d: %d\n", i, omp_get_thread_num(), vetSomme[i]);
    }
    
    
    /*printf("risultato:\n");
    for(i = 0; i < righeMatInput; i++){
        printf("%d\n", vetSomme[i]);
    }*/
    
    return EXIT_SUCCESS;
}
