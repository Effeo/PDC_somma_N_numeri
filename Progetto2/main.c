#include <stdio.h>
#include <stdlib.h>

#include <omp.h>
#include <math.h>

int strategy1(int dimVet, int righeMat, int colMat, char *numbers[])
{
  int i, j;

  int **mat = (int **)malloc(righeMat * sizeof(int *));
  int *vettore = (int *)malloc(dimVet * sizeof(int));

  for (i = 0; i < righeMat; i++)
  {
    mat[i] = (int *)malloc(colMat * sizeof(int));
  }

  for (i = 0; i < righeMat; i++)
  {
    for (j = 0; j < colMat; j++)
    {
      mat[i][j] = atoi(numbers[i * colMat + j]);
    }
  }

  for (i = 0; i < dimVet; i++)
  {
    vettore[i] = atoi(numbers[righeMat * colMat + i]);
  }

  int *resVec = (int *)calloc(colMat, sizeof(int));

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

  #pragma omp master
  {

    for (i = 0; i < righeMat; i++)
    {
      printf("%d\n", resVec[i]);
    }
  }

  return EXIT_SUCCESS;
}

int strategy2(int dimVet, int righeMat, int colMat, char *numbers[])
{
  int i, j;

  int **matInput = (int **)malloc(righeMat * sizeof(int *));
  int *vetInput = (int *)malloc(dimVet * sizeof(int));

  // inizializzazione matrice e vettore
  for (i = 0; i < righeMat; i++)
  {
    matInput[i] = (int *)malloc(colMat * sizeof(int));
  }

  for (i = 0; i < righeMat; i++)
  {
    for (j = 0; j < colMat; j++)
    {
      matInput[i][j] = atoi(numbers[i * colMat + j]);
    }
  }

  for (i = 0; i < dimVet; i++)
  {
    vetInput[i] = atoi(numbers[righeMat * colMat + i]);
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

  printf("risultato:\n");
  for (i = 0; i < righeMat; ++i)
  {
    printf("%d\n", vet_output[i]);
  }
  return EXIT_SUCCESS;
}

int strategy3(int dimVet, int righeMat, int colMat, char *numbers[])
{
  int i, j;

  int **matInput = (int **)malloc(righeMat * sizeof(int *));
  int *vetInput = (int *)malloc(dimVet * sizeof(int));

  // inizializzazione matrice e vettore
  for (i = 0; i < righeMat; i++)
  {
    matInput[i] = (int *)malloc(colMat * sizeof(int));
  }

  for (i = 0; i < righeMat; i++)
  {
    for (j = 0; j < colMat; j++)
    {
      matInput[i][j] = atoi(numbers[i * colMat + j]);
    }
  }

  for (i = 0; i < dimVet; i++)
  {
    vetInput[i] = atoi(numbers[righeMat * colMat + i]);
  }

  int numThreads = omp_get_max_threads();
  int blockRows = sqrt(numThreads);
  int blockCols = numThreads / blockRows;
  int rowBlockSize = righeMat / blockRows;
  int colBlockSize = colMat / blockCols;
  int rowRemainder = righeMat % blockRows;
  int colRemainder = colMat % blockCols;

  int *vet_output = (int *)calloc(righeMat, sizeof(int));

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
  for (i = 0; i < righeMat; ++i)
  {
    printf("%d\n", vet_output[i]);
  }

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    printf("Invalid arguments. Args: <strategy index [0, 2]> <vec input> <row size> <col size> <... numbers ...>\n");
    return EXIT_FAILURE;
  }

  int strategyIndex = atoi(argv[1]);

  if (strategyIndex < 0 || strategyIndex > 2)
  {
    printf("Invalid strategy index. Allowed values range from 0 to 2 (inclusive). Aborting.\n");
    return EXIT_FAILURE;
  }

  int dimVet = atoi(argv[2]);
  int righeMat = atoi(argv[3]);
  int colMat = atoi(argv[4]);
  
  if(colMat != dimVet) 
  {
    printf("Vector dimension doesn't match matrix column size. Aborting.\n");
    return EXIT_FAILURE;
  }

  int numThreads = omp_get_num_threads();

  if(numThreads > colMat) 
  {
    printf("Num threads must be less or equal than number of matrix's columns. Aborting.\n");
    return EXIT_FAILURE;
  }

  if (strategyIndex == 0)
  {
    return strategy1(dimVet, righeMat, colMat, argv + 5); // + 5 means we skip command name, strategy and dimensions
  }
  else if (strategyIndex == 2)
  {
    return strategy2(dimVet, righeMat, colMat, argv + 5); // + 5 means we skip command name, strategy and dimensions
  }
  else if (strategyIndex == 2)
  {
    return strategy3(dimVet, righeMat, colMat, argv + 5); // + 5 means we skip command name, strategy and dimensions
  }

  return EXIT_SUCCESS;
}