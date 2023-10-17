#include <stdio.h>
#include "mpi.h"

int main(int argc, char* argv[]) 
{
  int startIndex = 2;

  int nOfNumbers = atoi(argv[1]);

  int menum, nproc;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &menum);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  int fractions = nOfNumbers / nproc; // 100 / 8
  
  int min = menum * fractions;
  int max = min + fractions;
  int i = 0;
  // Controller
  if(menum == 0) 
  {
    MPI_Status status;

    int sum = 0;
    int mmax = max + nOfNumbers % nproc;

    printf("Executing sum from 0 to %d\n", mmax);

    for(i = min; i < mmax; ++i) 
    {
      sum += atoi(argv[startIndex + i]);
    }

    printf("Waiting for process to send data:\n");
    for(i = 1; i < nproc; ++i) 
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
    for(i = min; i < max; ++i) 
    {
      sum += atoi(argv[startIndex + i]);
    }

    printf("Sending sum %d to process 0\n", sum);
    MPI_Send(&sum, 1, MPI_INT, 0, menum, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}