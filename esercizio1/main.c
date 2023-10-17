#include <stdio.h>
#include <math.h>

#include "mpi.h"

int main(int argc, char *argv[])
{
  int startIndex = 2;

  int nOfNumbers = atoi(argv[1]);

  int rank, nProcessors;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nProcessors);

  // If it's not a power of two, return -1
  if (nProcessors & (nProcessors - 1) != 0)
  {
    return -1;
  }

  int fractions = nOfNumbers / nproc; // 100 / 8

  int min = rank * fractions;
  int max = min + fractions;

  int sum = 0, i = 0;
  int value = 0;

  for (i = min; i < max; ++i)
  {
    sum += atoi(argv[startIndex + i]);
  }

  if (rank % pow(2, currentLevel) == 0)
  {
    MPI_Recv(&value, 1, MPI_INT, i, i, MPI_COMM_WORLD, &status);

    sum += value;
  }
  else
  {
    MPI_Send(&sum, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
  }

  int levels = (int)log2(nProcessors);
  int currentLevel = 0, currentProcess;
  for (currentLevel = 2; currentLevel <= levels; ++currentLevel)
  {
    for (currentProcess = 0; currentProcess < nProcessors; currentProcess += pow(2, currentLevel - 1))
    {
      if (rank % pow(2, currentLevel) == 0)
      {
        // Receiver
        MPI_Recv(&value, 1, MPI_INT, i, i, MPI_COMM_WORLD, &status);

        sum += value;
      }
      else
      {

        // Sender
        printf("Executing sum from %d to %d\n", min, max);

        printf("Sending sum %d to process 0\n", sum);
        MPI_Send(&sum, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
      }
    }
  }

  if (rank == 0)
  {
    printf("Sum: %d\n", sum);
  }

  MPI_Finalize();
  return 0;
}