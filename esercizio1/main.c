#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mpi.h"

int stretegyTwo(int argc, char *argv[], int  *numbers,int menum,int nProcessors)
{
  int startIndex = 2;
  int nOfNumbers = atoi(argv[startIndex++]);
  int i;
  int sum = 0;
  double t0, t1, time;
  double timetot;

  int rest = nOfNumbers % nProcessors;

  MPI_Barrier(MPI_COMM_WORLD);
  t0 = MPI_Wtime();

  if (rest != 0)
  {
      for(i = 0; i < rest; ++i)
      {
        if(menum == i)
        {
          if(nOfNumbers <= 20)
            sum += atoi(argv[startIndex + i]);
          else
            sum += numbers[i];
        }
      }

      startIndex += rest;
  }

  int fractions = (nOfNumbers - rest) / nProcessors;


  int min = menum * fractions;
  int max = min + fractions;

  int value = 0;
  int currentLevel = 1;

  for (i = min; i < max; ++i)
  {
    if(nOfNumbers <= 20)
      sum += atoi(argv[startIndex + i]);
    else
      sum += numbers[i];
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

  t1 = MPI_Wtime();
  time = t1 - t0;
  printf("Sono %d: tempo impiegato: %e secondi\n", menum, time);
  MPI_Reduce(&time, &timetot, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

  if(menum == 0){
    printf("Tempo totale impiegato: %e secondi\n", timetot);
    printf("Result: %d\n", sum);
    MPI_Finalize();
    return sum;
  }
  
  MPI_Finalize();
  return 0;
}

int strategyThree(int argc, char *argv[], int  *numbers,int menum, int nProcessors)
{
  int startIndex = 2;
  int nOfNumbers = atoi(argv[startIndex++]);
  int sum = 0;
  int i;
  double t0, t1, time;
  double timetot;

  int rest = nOfNumbers % nProcessors;

  MPI_Barrier(MPI_COMM_WORLD);
  t0 = MPI_Wtime();

  if (rest != 0)
  {
      for(i = 0; i < rest; ++i)
      {
        if(menum == i)
        {
          if(nOfNumbers <= 20)
            sum += atoi(argv[startIndex + i]);
          else
            sum += numbers[i];
        }
      }

      startIndex += rest;
  }

  int fractions = (nOfNumbers - rest) / nProcessors;

  int min = menum * fractions;
  int max = min + fractions;

  int value = 0;
  int currentLevel = 1;
  
  for (i = min; i < max; ++i)
  {
    if(nOfNumbers <= 20)
      sum += atoi(argv[startIndex + i]);
    else
      sum += numbers[i];
  }

  MPI_Status status;
  int levels = (int)(log(nProcessors) / log(2));
  int scarto;
  int salto;
 
  for(currentLevel = 1; currentLevel <= levels; ++currentLevel)
  {
    scarto = (int)pow(2, currentLevel - 1);
    verso = ((int)pow(2, currentLevel) / 2);

    if((menum / scarto) % 2 == 0){
      //Send recive
      salto = menum + scarto;

      MPI_Send(&sum, 1, MPI_INT, salto, 0, MPI_COMM_WORLD);
      MPI_Recv(&value, 1, MPI_INT, salto, 0, MPI_COMM_WORLD, &status);

      sum += value;
    }
    else 
    {
      //recive send
      salto = menum - scarto;

      MPI_Recv(&value, 1, MPI_INT, salto, 0, MPI_COMM_WORLD, &status);
      MPI_Send(&sum, 1, MPI_INT, salto, 0, MPI_COMM_WORLD);

      sum += value;
    }
  }


  t1 = MPI_Wtime();
  time = t1 - t0;
  printf("Sono %d: tempo impiegato: %e secondi\n", menum, time);
  MPI_Reduce(&time, &timetot, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  
  if(menum == 0){
    printf("Tempo totale impiegato: %e secondi\n", timetot);
  }
  
  printf("Result: %d\n", sum);
  MPI_Finalize();

  return sum;
}

int stretegyOne(int argc, char *argv[], int  *numbers,int menum, int nproc)
{
  int startIndex = 2;
  int nOfNumbers = atoi(argv[startIndex++]);
  
  int sum = 0;
  int i = 0;
  int value;
  double t0, t1, time;
  double timetot;


  int rest = nOfNumbers % nproc;

  MPI_Barrier(MPI_COMM_WORLD);
  t0 = MPI_Wtime();

  if (rest != 0)
  {
      for(i = 0; i < rest; ++i)
      {
        if(menum == i)
        {
          if(nOfNumbers <= 20)
            sum += atoi(argv[startIndex + i]);
          else
            sum += numbers[i];
        }
      }

      startIndex += rest;
  }

  int fractions = (nOfNumbers - rest) / nproc;

  int min = menum * fractions;
  int max = min + fractions;

  if (menum == 0)
  {
    MPI_Status status;

    for (i = min; i < max; ++i)
    {
      if(nOfNumbers <= 20)
        sum += atoi(argv[startIndex + i]);
      else
        sum += numbers[i];
    }

    for (i = 1; i < nproc; ++i)
    {
      MPI_Recv(&value, 1, MPI_INT, i, i, MPI_COMM_WORLD, &status);
      sum += value;
    }
  }
  else // Calculators
  {

    for (i = min; i < max; ++i)
    {
      if(nOfNumbers <= 20)
        sum += atoi(argv[startIndex + i]);
      else
        sum += numbers[i];
    }

    MPI_Send(&sum, 1, MPI_INT, 0, menum, MPI_COMM_WORLD);
  }

  t1 = MPI_Wtime();
  time = t1 - t0;
  printf("Sono %d: tempo impiegato: %e secondi\n", menum, time);
  MPI_Reduce(&time, &timetot, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  
  
  if(menum == 0){
    printf("Tempo totale impiegato: %e secondi\n", timetot);
    printf("Result: %d\n", sum);
    MPI_Finalize();
    return sum;
  }

  MPI_Finalize();
  return 0;
}

int main(int argc, char *argv[])
{
  int menum, nProcessors;

 MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &menum);
  MPI_Comm_size(MPI_COMM_WORLD, &nProcessors);
  int strategyNumber = atoi(argv[1]);
  int nOfNumbers = atoi(argv[2]);
  int result;
  int *numbers = NULL;
  int i;

  if (nOfNumbers > 20)
  {
    numbers = malloc(nOfNumbers * sizeof(int));

    for (i = 0; i < nOfNumbers; ++i) 
    {
      numbers[i] = rand() % 100;
    }
  }

  if (strategyNumber == 1)
  {
    result = stretegyOne(argc, argv, numbers);
  }
  else if (strategyNumber == 2)
  {
    if (nProcessors & (nProcessors - 1) != 0){
      result = stretegyOne(argc,argv,numbers,menum,nProcessors);
    }
    else{
      result = stretegyTwo(argc, argv, numbers,menum,nProcessors);
    }
    
  }
  else if (strategyNumber == 3)
  {
    if (nProcessors & (nProcessors - 1) != 0){
      result = stretegyOne(argc,argv,numbers,menum,nProcessors);
    }
    else{
      result = stretegyThree(argc, argv, numbers,menum,nProcessors);
    }
  }
  else
  {
    printf("Selezionare una strategia corretta\n");
    return -1;
  }

  return 0;
}
