#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "Invalid arguments. Missing numbers to sum.\n");
    return EXIT_FAILURE;
  }

  int n = atoi(argv[1]);

  if (n <= 0)
  {
    fprintf(stderr, "Invalid numbers value. Must be greater than 0. Value: %d\n", n);
    return EXIT_FAILURE;
  }

  int *numbers = malloc(sizeof(int)*n);

  if (!numbers)
  {
    fprintf(stderr, "Not enough memory to allocate for %d numbers.\n", n);
    return EXIT_FAILURE;
  }

  int i = 0;
  for (; i < n; ++i)
  {
    numbers[i] = 1;
  }

  int sumtot = 0;
  
  #pragma omp parallel for num_threads(omp_get_num_threads()) reduction(+:sumtot)
  for (i = 0; i < n; ++i)
  {
    sumtot = sumtot + numbers[i];
  }

  fprintf(stdout, "Total sum is %d\n", sumtot);

  free(numbers);

  return EXIT_SUCCESS;
}
