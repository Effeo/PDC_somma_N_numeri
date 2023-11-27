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

  int* resVec =  (int*) calloc(colMat, sizeof(int));

  #pragma omp parallel for shared(resVec, vettore, dimVet) private(i, j) num_threads(omp_get_num_threads())
  for (i = 0; i < righeMat; ++i)
  {
    int r = 0;
    for(j = 0; j < colMat; j++){
        r += mat[i][j] * vettore[j];
    }
    resVec[i] = r;
  }

  #pragma omp master
  {

    for(i = 0; i < righeMat; i++){
        printf("%d\n", resVec[i]);
    }
  }

  return EXIT_SUCCESS;
}
