#include <stdio.h>
#include <math.h>

#include "mpi.h"

/*
int stretegyOne(int argc, char *argv[]);
int stretegyTwo(int argc, char *argv[]);
int strategyThree(int argc, char *argv[]);
*/

int stretegyOne(int argc, char *argv[])
{
  // startIndex starts from 2 because we skip the first two args (command name and strategyid)
  int startIndex = 2;

  // We take the number of numbers to sum from array index 2, then increase startIndex
  // just to have the right offset for the next loc
  int nOfNumbers = atoi(argv[startIndex++]);

  int menum, nproc;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &menum);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  int fractions = nOfNumbers / nproc; // 100 / 8

  int min = menum * fractions;
  int max = min + fractions;
  int i = 0;
  // Controller
  if (menum == 0)
  {
    MPI_Status status;

    int sum = 0;
    int mmax = max + nOfNumbers % nproc;

    printf("Executing sum from 0 to %d\n", mmax);

    for (i = min; i < mmax; ++i)
    {
      sum += atoi(argv[startIndex + i]);
    }

    printf("Waiting for process to send data:\n");
    for (i = 1; i < nproc; ++i)
    {
      int value = 0;

      MPI_Recv(&value, 1, MPI_INT, i, i, MPI_COMM_WORLD, &status);
      printf("Status from rank %d, with tag %d and error code %d.\n", status.MPI_SOURCE, status.MPI_TAG, status.MPI_ERROR);
      sum += value;
    }

    printf("Result: %d\n", sum);
  }
  else // Calculators
  {
    printf("Executing sum from %d to %d\n", min, max);

    int sum = 0;
    for (i = min; i < max; ++i)
    {
      sum += atoi(argv[startIndex + i]);
    }

    printf("Sending sum %d to process 0\n", sum);
    MPI_Send(&sum, 1, MPI_INT, 0, menum, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}

int stretegyTwo(int argc, char *argv[])
{
  // Just read comment on strategyOne
  int startIndex = 2;

  int nOfNumbers = atoi(argv[startIndex++]);

  int rank, nProcessors;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nProcessors);

  // If it's not a power of two, return -1
  if (nProcessors & (nProcessors - 1) != 0)
  {
    MPI_Finalize();
    return -1;
  }

  int fractions = nOfNumbers / nProcessors; // 100 / 8

  int min = rank * fractions;
  int max = min + fractions;

  int sum = 0;
  int value = 0;
  int currentLevel = 1;

  for (i = min; i < max; ++i)
  {
    sum += atoi(argv[startIndex + i]);
  }
  printf("[Rank %d]: Calculated SUM from %d to %d. Total is %d\n", rank, min, max, sum);

  MPI_Status status;

  /*
  if (rank % (int)pow(2, currentLevel) == 0)
  {
    int from = rank + (int)(pow(2, currentLevel) * 0.5);
    MPI_Recv(&value, 1, MPI_INT, from, 0, MPI_COMM_WORLD, &status);

    sum += value;
  }
  else
  {
    int to = rank - (rank % ((int)pow(2, currentLevel)));
    MPI_Send(&sum, 1, MPI_INT, to, 0, MPI_COMM_WORLD);
  }
  */

  int levels = (int)(log(nProcessors) / log(2));
  int currentProcess;
  for (currentLevel = 1; currentLevel <= levels; ++currentLevel)
  {
    if (rank % (int)pow(2, currentLevel) == 0)
    {
      // Receiver
      int from = rank + (int)(pow(2, currentLevel) * 0.5);
      MPI_Recv(&value, 1, MPI_INT, from, 0, MPI_COMM_WORLD, &status);
      sum += value;

      printf("[Rank %d]: Received from %d sum %d. Total sum: %d\n", rank, from, value, sum);
    }
    else if (rank % (int)pow(2, currentLevel) == (int)(pow(2, currentLevel) * 0.5))
    {
      // Sender

      int to = rank - (rank % ((int)pow(2, currentLevel)));
      printf("[Rank %d]: Sending sum %d to process %d\n", rank, sum, to);

      MPI_Send(&sum, 1, MPI_INT, to, 0, MPI_COMM_WORLD);
    }
  }

  if (rank == 0)
  {
    printf("Sum: %d\n", sum);
  }

  MPI_Finalize();
  return 0;
}

int strategyThree(int argc, char *argv[])
{
  return 0;
}

int main(int argc, char *argv[])
{
  int strategyNumber = atoi(argv[1]);

  int shouldRunOne = strategyNumber == 1;

  if (strategyNumber == 2)
  {
    shouldRunOne = stretegyTwo(argc, argv) == -1;
  }
  else if (strategyNumber == 3)
  {
    shouldRunOne = strategyThree(argc, argv) == -1;
  }

  if (shouldRunOne == 1)
  {
    return stretegyOne(argc, argv);
  }
  return 0;
}
