#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <limits.h>
int sum=0;
void stampaPercorso(int **path, int i, int j) {
    if (i == 0 && j == 0) {
        printf("(%d, %d) ", i, j);
        return;
    }
    if (path[i][j] == 1) {
        stampaPercorso(path, i-1, j);
    } else if (path[i][j] == 2) {
        stampaPercorso(path, i, j-1);
    }
    printf("(%d, %d) ", i, j);
}

void trovaCammino(int **matrice, int righe, int colonne) {
    int i, j;
    int **dist = (int **)malloc(righe * sizeof(int *));
    int **path = (int **)malloc(righe * sizeof(int *));
    
    for (i = 0; i < righe; i++) {
        dist[i] = (int *)malloc(colonne * sizeof(int));
        path[i] = (int *)malloc(colonne * sizeof(int));
        for (j = 0; j < colonne; j++) {
            dist[i][j] = INT_MAX;
        }
    }
    
    dist[0][0] = matrice[0][0];
    
    for (i = 0; i < righe; i++) {
        for (j = 0; j < colonne; j++) {
            if (i > 0 && dist[i][j] > dist[i-1][j] + matrice[i][j]) {
                dist[i][j] = dist[i-1][j] + matrice[i][j];
                path[i][j] = 1; /* 1 significa che proviene dall'alto */
            }
            if (j > 0 && dist[i][j] > dist[i][j-1] + matrice[i][j]) {
                dist[i][j] = dist[i][j-1] + matrice[i][j];
                path[i][j] = 2; /* 2 significa che proviene da sinistra */
            }
        }
    }

    printf("Il percorso minimo dal primo all'ultimo posto e':\n");
    stampaPercorso(path, righe-1, colonne-1);
    printf("\nCon peso totale: %d\n", dist[righe-1][colonne-1]);

    for (i = 0; i < righe; i++) {
        free(dist[i]);
        free(path[i]);
    }
    free(dist);
    free(path);
}

int main(int argc, char *argv[]) {
    
    int righe, colonne;
    int Ccell, Cheight;
    int **matrice;
    int i, j;
    FILE *filein = stdin;

    if ( (argc < 2) || (argc > 4) ) {
        fprintf(stderr, "Usage: %s filename [src [dst]]\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (strcmp(argv[1], "-") != 0) {
        filein = fopen(argv[1], "r");
        if (filein == NULL) {
            fprintf(stderr, "Can not open %s\n", argv[1]);
            return EXIT_FAILURE;
        }
    }
    fscanf(filein, "%d", &Ccell);
    fscanf(filein, "%d", &Cheight);
    fscanf(filein, "%d", &righe);
    fscanf(filein, "%d", &colonne);

   
    matrice = (int **)malloc(righe * sizeof(int *));
    for (i = 0; i < righe; i++) {
        matrice[i] = (int *)malloc(colonne * sizeof(int));
        for (j = 0; j < colonne; j++) {
            fscanf(filein, "%d", &matrice[i][j]);
        }
    }

    fclose(filein);

    trovaCammino(matrice, righe, colonne);

    for (i = 0; i < righe; i++) {
        free(matrice[i]);
    }
    free(matrice);

    return 0;
}


