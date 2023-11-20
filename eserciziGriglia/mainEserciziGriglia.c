#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int mainEserciziGriglia(int argc, char* argv[]){
    int menum, nproc, row, col, menum_grid;
    int dim, *ndim, reorder, *period, *coordinate;
    MPI_Comm comm_grid;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &menum);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    // numero di righe della griglia di processo
    row = atoi(argv[1]);
    printf("Numero di processori: %d\n", nproc);
    
    // spedizione id row da parte di 0 a tutti i processi
    MPI_Bcast(&row, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // definizione del numero di colonne della griglia
    col = nproc / row;

    // numero di dimensioni della griglia
    dim = 2;
    coordinate = (int*) calloc(dim, sizeof(int));

    // vettore contenente le lunghezze di ciascuna dimesione
    ndim = (int*) calloc(dim, sizeof(int));
    ndim[0] = row;
    ndim[1] = col;

    // vettore contenente le periodicità delle dimensioni
    period = (int*) calloc(dim, sizeof(int));
    period[0] = period[1] = 0;
    reorder = 0;

    // definizione della griglia bidimensionale
    MPI_Cart_create(MPI_COMM_WORLD, dim, ndim, period, reorder, &comm_grid);
    // nella slide c'è scritto &menum_grid, ma non è definito
    MPI_Cpmm_rank(comm_grid, &menum);

    // definizione delle coordinate di ciascun processo nella griglia bidimensionale
    MPI_Cart_coords(comm_grid, menum, dim, coordinate);

    // stampa delle coordinate
    printf("Processore %d coordinate nella griglia: (%d, %d)\n", menum, coordinate[0], coordinate[1]);

    MPI_Finalize();
    return 0;
}