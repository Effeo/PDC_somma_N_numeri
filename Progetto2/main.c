#include <stdio.h>
#include <stdlib.h>

#include <omp.h>
#include <math.h>

int strategy1(int righeMat, int colMat)
{
  int i, j;

  int **mat = (int **)malloc(righeMat * sizeof(int *));
  int *vettore = (int *)malloc(colMat * sizeof(int));
  double t0, t1, time;

  for (i = 0; i < righeMat; i++)
  {
    mat[i] = (int *)malloc(colMat * sizeof(int));
  }

  for (i = 0; i < righeMat; i++)
  {
    for (j = 0; j < colMat; j++)
    {
      mat[i][j] = 1;
    }
  }

  for (i = 0; i < colMat; i++)
  {
    vettore[i] = 1;
  }

  int *resVec = (int *)calloc(colMat, sizeof(int));

  t0 = omp_get_wtime();
  #pragma omp parallel for shared(resVec, vettore) private(i, j) num_threads(omp_get_num_threads())
  for (i = 0; i < righeMat; ++i)
  {
    int r = 0;
    for (j = 0; j < colMat; j++)
    {
      r += mat[i][j] * vettore[j];
    }
    resVec[i] = r;
  }

  t1 = omp_get_wtime();
  time = t1 - t0;

  /*printf("risultato:\n");
  for (i = 0; i < righeMat; i++)
  {
    printf("%d\n", resVec[i]);
  }*/

  printf("tempo: %f secondi\n\n", time);
  return EXIT_SUCCESS;
}

int strategy2(int righeMat, int colMat)
{
  int i, j;
  int **matInput = (int **)malloc(righeMat * sizeof(int *));
  int *vetInput = (int *)malloc(colMat * sizeof(int));
  double t0, t1, time;

  // inizializzazione matrice e vettore
  for (i = 0; i < righeMat; i++)
  {
    matInput[i] = (int *)malloc(colMat * sizeof(int));
  }

  for (i = 0; i < righeMat; i++)
  {
    for (j = 0; j < colMat; j++)
    {
      matInput[i][j] = 1;
    }
  }

  for (i = 0; i < colMat; i++)
  {
    vetInput[i] = 1;
  }

  #pragma omp parallel for shared(matInput, vetInput, righeMat, colMat) private(i, j)
  for (j = 0; j < colMat; ++j)
  {
    for (i = 0; i < righeMat; ++i)
    {
      matInput[i][j] = matInput[i][j] * vetInput[j];
    }
  }

  int *vet_output = (int *)calloc(righeMat, sizeof(int));
  int sumtot;
  t0 = omp_get_wtime();
  #pragma omp parallel for reduction(+ : sumtot)
  for (i = 0; i < righeMat; ++i)
  {
    sumtot = 0;
    for (j = 0; j < colMat; ++j)
    {
      sumtot += matInput[i][j];
    }
    vet_output[i] = sumtot;
  }

  t1 = omp_get_wtime();
  time = t1 - t0;

  /*printf("risultato:\n");
  for (i = 0; i < righeMat; ++i)
  {
    printf("%d\n", vet_output[i]);
  }*/

  printf("tempo: %f secondi\n\n", time);
  return EXIT_SUCCESS;
}

int strategy3(int righeMat, int colMat)
{
  int i, j;
  double t0, t1, time;
  int **matInput = (int **)malloc(righeMat * sizeof(int *));
  int *vetInput = (int *)malloc(colMat * sizeof(int));

  // inizializzazione matrice e vettore
  for (i = 0; i < righeMat; i++)
  {
    matInput[i] = (int *)malloc(colMat * sizeof(int));
  }

  for (i = 0; i < righeMat; i++)
  {
    for (j = 0; j < colMat; j++)
    {
      matInput[i][j] = 1;
    }
  }

  for (i = 0; i < colMat; i++)
  {
    vetInput[i] = 1;
  }

  int numThreads = omp_get_max_threads();
  int blockRows = sqrt(numThreads);
  int blockCols = numThreads / blockRows;
  int rowBlockSize = righeMat / blockRows;
  int colBlockSize = colMat / blockCols;
  int rowRemainder = righeMat % blockRows;
  int colRemainder = colMat % blockCols;

  int *vet_output = (int *)calloc(righeMat, sizeof(int));

  t0 = omp_get_wtime();
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

  t1 = omp_get_wtime();
  time = t1 - t0;
  
  /*printf("risultato:\n");
  for (i = 0; i < righeMat; ++i)
  {    
    printf("%d\n", vet_output[i]);
  }*/
  
  printf("tempo: %f secondi\n\n", time);
  return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
  if (argc <= 3)
  {
    printf("Invalid arguments. Args: <strategy index [1, 3]> <row size> <col size>\n");
    return EXIT_FAILURE;
  }

  int strategyIndex = atoi(argv[1]);

  if (strategyIndex < 1 || strategyIndex > 3)
  {
    printf("Invalid strategy index. Allowed values range from 1 to 3 (inclusive). Aborting.\n");
    return EXIT_FAILURE;
  }

  int righeMat = atoi(argv[2]);
  int colMat = atoi(argv[3]);
  
  if(colMat <= 0 || righeMat <= 0) 
  {
    printf("Invalid dimensions. Aborting.\n");
    return EXIT_FAILURE;
  }

  printf("righe matrice: %d\n", righeMat);
  printf("colonne matrice: %d\n", colMat);
  printf("numero di threads: %d\n", omp_get_max_threads());
  if (strategyIndex == 1)
  {
    printf("strategia 1\n");
    return strategy1( righeMat, colMat); 
  }
  else if (strategyIndex == 2)
  {
    printf("strategia 2\n");
    return strategy2( righeMat, colMat);
  }
  else if (strategyIndex == 3)
  {
    printf("strategia 3\n");
    return strategy3( righeMat, colMat); 
  }

  return EXIT_SUCCESS;
}