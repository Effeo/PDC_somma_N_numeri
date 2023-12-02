#include <stdio.h>
#include <stdlib.h>

#include <omp.h>
#include <math.h>

int strategy1(int argc, char *argv[])
{
  int dimVet = atoi(argv[1]);
  int righeMat = atoi(argv[2]);
  int colMat = atoi(argv[3]);
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
      mat[i][j] = atoi(argv[4 + i * colMat + j]);
    }
  }

  for (i = 0; i < dimVet; i++)
  {
    vettore[i] = atoi(argv[4 + righeMat * colMat + i]);
  }

  int *resVec = (int *)calloc(colMat, sizeof(int));

#pragma omp parallel for shared(resVec, vettore, dimVet) private(i, j) num_threads(omp_get_num_threads())
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

int strategy2(int argc, char *argv[])
{
  int dimVetInput = atoi(argv[1]);
  int righeMatInput = atoi(argv[2]);
  int colMatInput = atoi(argv[3]);
  int i, j;

  int **matInput = (int **)malloc(righeMatInput * sizeof(int *));
  int *vetInput = (int *)malloc(dimVetInput * sizeof(int));

  // inizializzazione matrice e vettore
  for (i = 0; i < righeMatInput; i++)
  {
    matInput[i] = (int *)malloc(colMatInput * sizeof(int));
  }

  for (i = 0; i < righeMatInput; i++)
  {
    for (j = 0; j < colMatInput; j++)
    {
      matInput[i][j] = atoi(argv[4 + i * colMatInput + j]);
    }
  }

  for (i = 0; i < dimVetInput; i++)
  {
    vetInput[i] = atoi(argv[4 + righeMatInput * colMatInput + i]);
  }

#pragma omp parallel for shared(matInput, vetInput, righeMatInput, colMatInput) private(i, j)
  for (j = 0; j < colMatInput; ++j)
  {
    for (i = 0; i < righeMatInput; ++i)
    {
      matInput[i][j] = matInput[i][j] * vetInput[j];
    }
  }

  printf("matrice risultato:\n");
  for (i = 0; i < righeMatInput; i++)
  {
    for (j = 0; j < omp_get_max_threads(); j++)
    {
      printf("%d ", matInput[i][j]);
    }
    printf("\n");
  }
  printf("\n");

  int *vet_output = (int *)calloc(righeMatInput, sizeof(int));
  int sumtot;
#pragma omp parallel for reduction(+ : sumtot)
  for (i = 0; i < righeMatInput; ++i)
  {
    sumtot = 0;
    for (j = 0; j < colMatInput; ++j)
    {
      sumtot += matInput[i][j];
    }
    vet_output[i] = sumtot;
  }

  printf("risultato:\n");
  for (i = 0; i < righeMatInput; ++i)
  {
    printf("%d\n", vet_output[i]);
  }
  return EXIT_SUCCESS;
}

int strategy3(int argc, char *argv[])
{
  int dimVetInput = atoi(argv[1]);
  int righeMatInput = atoi(argv[2]);
  int colMatInput = atoi(argv[3]);
  int i, j;

  int **matInput = (int **)malloc(righeMatInput * sizeof(int *));
  int *vetInput = (int *)malloc(dimVetInput * sizeof(int));

  // inizializzazione matrice e vettore
  for (i = 0; i < righeMatInput; i++)
  {
    matInput[i] = (int *)malloc(colMatInput * sizeof(int));
  }

  for (i = 0; i < righeMatInput; i++)
  {
    for (j = 0; j < colMatInput; j++)
    {
      matInput[i][j] = atoi(argv[4 + i * colMatInput + j]);
    }
  }

  for (i = 0; i < dimVetInput; i++)
  {
    vetInput[i] = atoi(argv[4 + righeMatInput * colMatInput + i]);
  }

  int numThreads = omp_get_max_threads();
  int blockRows = sqrt(numThreads);
  int blockCols = numThreads / blockRows;
  int rowBlockSize = righeMatInput / blockRows;
  int colBlockSize = colMatInput / blockCols;
  int rowRemainder = righeMatInput % blockRows;
  int colRemainder = colMatInput % blockCols;

  int *vet_output = (int *)calloc(righeMatInput, sizeof(int));

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
  for (i = 0; i < righeMatInput; ++i)
  {
    printf("%d\n", vet_output[i]);
  }

  free(vet_output);
  return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    printf("Invalid arguments. Args: <strategy index [0, 2]> <vec input> <row size> <col size> <... numbers ...>\n");
    return EXIT_FAILURE;
  }

  int index = 1;

  int strategyIndex = atoi(argv[index++]);

  if (strategyIndex < 0 || strategyIndex > 2)
  {
    printf("Invalid strategy index. Allowed values range from 0 to 2 (inclusive).\n");
    return EXIT_FAILURE;
  }

  if (strategyIndex == 0)
  {
    return strategy1(argc, argv + index);
  }
  else if (strategyIndex == 1)
  {
    return strategy2(argc, argv + index);
  }
  else if (strategyIndex == 2)
  {
    return strategy3(argc, argv + index);
  }
  return EXIT_FAILURE;
}