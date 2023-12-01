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

    #pragma omp parallel for shared(matInput, vetInput, righeMatInput, colMatInput) private(i, j)
    for(j = 0; j < colMatInput; ++j){
        for(i = 0; i < righeMatInput; ++i){
            matInput[i][j] = matInput[i][j] * vetInput[j];
        }
    }

    printf("matrice risultato:\n");
    for(i = 0; i < righeMatInput; i++){
        for(j = 0; j < omp_get_max_threads(); j++){
            printf("%d ", matInput[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    int* vet_output = (int*) calloc(righeMatInput, sizeof(int));
    int sumtot;
    #pragma omp parallel for reduction(+:sumtot)
    for(i = 0; i < righeMatInput; ++i)
    {
        sumtot = 0;
        for(j = 0; j < colMatInput; ++j)
        {
            sumtot += matInput[i][j];
        }
        vet_output[i] = sumtot;
    }
        
    
    printf("risultato:\n");
    for(i = 0; i < righeMatInput; ++i){
        printf("%d\n", vet_output[i]);
    }
    
    return EXIT_SUCCESS;
}
