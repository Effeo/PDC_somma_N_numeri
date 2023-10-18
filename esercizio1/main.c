#include <stdio.h>
#include <math.h>

#include "mpi.h"

int stretegyTwo(int argc, char *argv[])
{
  int startIndex = 2;
  int nOfNumbers = atoi(argv[startIndex++]);

  int menum, nProcessors;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &menum);
  MPI_Comm_size(MPI_COMM_WORLD, &nProcessors);

  // If it's not a power of two, return -1
  if (nProcessors & (nProcessors - 1) != 0)
  {
    MPI_Finalize();
    return stretegyOne(argc, argv);
  }

  int fractions = nOfNumbers / nProcessors;

  int min = menum * fractions;
  int max = min + fractions;

  int sum = 0;
  int value = 0;
  int currentLevel = 1;
  int i;

  if(menum == 0)
  {
      max =  max + nOfNumbers % nProcessors;
  }
  
  for (i = min; i < max; ++i)
  {
    sum += atoi(argv[startIndex + i]);
  }

  MPI_Status status;

  int levels = (int)(log(nProcessors) / log(2));

  for (currentLevel = 1; currentLevel <= levels; ++currentLevel)
  {
    if (menum % (int)pow(2, currentLevel) == 0)
    {
      // Receiver
      int from = menum + (int)(pow(2, currentLevel) * 0.5);

      MPI_Recv(&value, 1, MPI_INT, from, 0, MPI_COMM_WORLD, &status);
      sum += value;
    }
    else if (menum % (int)pow(2, currentLevel) == (int)(pow(2, currentLevel) * 0.5))
    {
      //proviamo con break o exit
      // Sender

      int to = menum - (menum % ((int)pow(2, currentLevel)));

      MPI_Send(&sum, 1, MPI_INT, to, 0, MPI_COMM_WORLD);
    }
  }

  MPI_Finalize();

  if(menum == 0){
    printf("Result: %d\n", sum);
    return sum;
  }

  return 0;
}

int strategyThree(int argc, char *argv[])
{
  int startIndex = 2;
  int nOfNumbers = atoi(argv[startIndex++]);

  int menum, nProcessors;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &menum);
  MPI_Comm_size(MPI_COMM_WORLD, &nProcessors);

  // If it's not a power of two, return -1
  if (nProcessors & (nProcessors - 1) != 0)
  {
    MPI_Finalize();
    return stretegyOne(argc, argv);
  }

  int fractions = nOfNumbers / nProcessors;

  int min = menum * fractions;
  int max = min + fractions;

  int sum = 0;
  int value = 0;
  int currentLevel = 1;
  int i;

  if(menum == 0)
  {
      max =  max + nOfNumbers % nProcessors;
  }
  
  for (i = min; i < max; ++i)
  {
    sum += atoi(argv[startIndex + i]);
  }

  MPI_Status status;
  int levels = nProcessors / 2;
  int scarto;
  int verso;
  int salto;
 
  for(currentLevel = 1; currentLevel < levels; ++currentLevel)
  {
    scarto = (int)pow(2, currentLevel - 1);
    verso = (menum / scarto);

    if(verso % 2 == 0)
    {
      salto = menum + scarto;
    }
    else
    {
      salto = menum - scarto;
    }

    if(menum % 2 == 0){
      //Send recive

      printf("Send recive: proc %d sending to %d\n", menum, salto);
      MPI_Send(&sum, 1, MPI_INT, salto, 0, MPI_COMM_WORLD);
      printf("Inviato\n")
      MPI_Recv(&value, 1, MPI_INT, salto, 0, MPI_COMM_WORLD, &status);

      sum += value;
    }
    else 
    {
      //recive send
      printf("Recive send: proc %d sending to %d\n", menum, salto);
      MPI_Recv(&value, 1, MPI_INT, salto, 0, MPI_COMM_WORLD, &status);
      printf("Ricevuto\n")
      MPI_Send(&sum, 1, MPI_INT, salto, 0, MPI_COMM_WORLD);

      sum += value;
    }
  }


  MPI_Finalize();
  printf("Result: %d\n", sum);
  return sum;
}

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

  int fractions = nOfNumbers / nproc;

  int min = menum * fractions;
  int max = min + fractions;
  int i = 0;
  
  // Controller
  if (menum == 0)
  {
    MPI_Status status;

    int sum = 0;
    int mmax = max + nOfNumbers % nproc;

    for (i = min; i < mmax; ++i)
    {
      sum += atoi(argv[startIndex + i]);
    }

    for (i = 1; i < nproc; ++i)
    {
      int value = 0;

      MPI_Recv(&value, 1, MPI_INT, i, i, MPI_COMM_WORLD, &status);
      sum += value;
    }

    printf("Result: %d\n", sum);
    MPI_Finalize();
    return sum;
  }
  else // Calculators
  {
    int sum = 0;

    for (i = min; i < max; ++i)
    {
      sum += atoi(argv[startIndex + i]);
    }

    MPI_Send(&sum, 1, MPI_INT, 0, menum, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 0;
}

int main(int argc, char *argv[])
{
  int strategyNumber = atoi(argv[1]);
  int result;

  if (strategyNumber == 1)
  {
    result = stretegyOne(argc, argv);
  }
  else if (strategyNumber == 2)
  {
    result = stretegyTwo(argc, argv);
  }
  else if (strategyNumber == 3)
  {
    result = strategyThree(argc, argv);
  }
  else
  {
    printf("Selezionare una strategia corretta\n");
    return -1;
  }

  return 0;
}
