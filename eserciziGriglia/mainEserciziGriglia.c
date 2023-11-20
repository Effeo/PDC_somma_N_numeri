#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

void esercizio1(int argc, char* argv[]){
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
    MPI_Comm_rank(comm_grid, &menum);

    // definizione delle coordinate di ciascun processo nella griglia bidimensionale
    MPI_Cart_coords(comm_grid, menum, dim, coordinate);

    // stampa delle coordinate
    printf("Processore %d coordinate nella griglia: (%d, %d)\n", menum, coordinate[0], coordinate[1]);

    MPI_Finalize();
}

void esercizio2(int argc, char* argv[]){
    int menum, nproc, row, col, menum_grid;
    int dim, *ndim, reorder, *period, *coordinate;
    int i;
    MPI_Comm comm_grid;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &menum);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    // numero di righe della griglia di processo
    row = atoi(argv[1]);
    int n = atoi(argv[2]);
    int* v = (int*) calloc(n, sizeof(int));

    for (i = 0; i < n; i++){
        v[i] = atoi(argv[i+3]);
    }

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
    MPI_Comm_rank(comm_grid, &menum);

    // definizione delle coordinate di ciascun processo nella griglia bidimensionale
    MPI_Cart_coords(comm_grid, menum, dim, coordinate);

    if (n % nproc == 0){
        int* v_local = (int*) calloc(n / nproc, sizeof(int));
        MPI_Scatter(v, n / nproc, MPI_INT, v_local, n / nproc, MPI_INT, 0, comm_grid);
        int somma = 0;
        for (i = 0; i < n / nproc; i++){
            somma += v_local[i];
        }
        printf("Processore %d coordinate nella griglia: (%d, %d) somma: %d\n", menum, coordinate[0], coordinate[1], somma);
        free(v_local);
    } else {
        int* v_local = (int*) calloc(n / nproc + 1, sizeof(int));
        int* sendcounts = (int*) malloc(nproc * sizeof(int));
        int* displs = (int*) malloc(nproc * sizeof(int));
        for (i = 0; i < nproc; i++){
            sendcounts[i] = n / nproc + (i < n % nproc);
            displs[i] = i * (n / nproc) + (i < n % nproc ? i : n % nproc);
        }
        MPI_Scatterv(v, sendcounts, displs, MPI_INT, v_local, sendcounts[menum], MPI_INT, 0, comm_grid);
        int somma = 0;
        for (i = 0; i < sendcounts[menum]; i++){
            somma += v_local[i];
        }
        printf("Processore %d coordinate nella griglia: (%d, %d) somma: %d\n", menum, coordinate[0], coordinate[1], somma);
        free(sendcounts);
        free(displs);
        free(v_local);
    }
}

void esercizio3(int argc, char* argv[]){
    int menum, nproc, row, col, menum_grid;
    int dim, *ndim, reorder, *period, *coordinate;
    int i, j;
    MPI_Comm comm_grid;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &menum);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    // numero di righe della griglia di processo
    row = atoi(argv[1]);
    int n = atoi(argv[2]);
    int m = atoi(argv[3]);

    int** mat = (int**) malloc(n * sizeof(int*));

    for (i = 0; i < n; i++){
        mat[i] = (int*) malloc(m * sizeof(int));
    }

    for(i = 0; i < n; i++){
        for(j = 0; j < m; j++){
            mat[i][j] = atoi(argv[4 + i * m + j]);
        }
    }

    
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
    MPI_Comm_rank(comm_grid, &menum);

    // definizione delle coordinate di ciascun processo nella griglia bidimensionale
    MPI_Cart_coords(comm_grid, menum, dim, coordinate);
    
    if (n % row == 0 && m % col == 0){
        int num_elements = (m * n) / (row * col);
        
        int** proc_elements = (int**) malloc(nproc * sizeof(int*));
        for (i = 0; i < nproc; i++){
            proc_elements[i] = (int*) malloc(num_elements * sizeof(int));
        }

        int row_jump = n / row;
        int col_jump = m / col; 
        int k;
        
        for (k = 0; k < nproc; k++){
            int proc_row = k / col;
            int proc_col = k % col;
            int proc_index = 0;
            for (i = proc_row * row_jump; i < (proc_row + 1) * row_jump; i++){
                for (j = proc_col * col_jump; j < (proc_col + 1) * col_jump; j++){
                    proc_elements[k][proc_index] = mat[i][j];
                    proc_index++;
                }
            }
        }

        
    }
    else{
        printf("Impossibile dividere la matrice in sottomatrici\n");
    }

    MPI_Finalize();
}

int main(int argc, char* argv[]) {
    //esercizio1(argc, argv);
    //esercizio2(argc, argv);
    esercizio3(argc, argv);
    return 0;
}
