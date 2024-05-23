#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int sum = 0;
int Ccell, Cheight;

int calc_weight(int **matrice, int **path, int righe, int colonne) {
    int i = righe - 1, j = colonne - 1;
    int k = 0; /* Numero di celle nel percorso*/
    int sum_diff_heights_squared = 0;
    int weight;
    int current_height, first_height, first_next_height, next_height;

    /* Calcola il percorso e raccoglie le altezze*/
    while (i != 0 || j != 0) {
        k++;
        current_height = matrice[i][j];
        if (path[i][j] == 1) {
            next_height = matrice[i-1][j];
        } else if (path[i][j] == 2) {
            next_height = matrice[i][j-1];
        }
        sum_diff_heights_squared += (current_height - next_height) * (current_height - next_height);
        if (path[i][j] == 1) {
            i--;
        } else if (path[i][j] == 2) {
            j--;
        }
    }

    /* Aggiungi la cella di partenza*/
    k++;
    first_height = matrice[0][0];
    if (righe > 1) {
        first_next_height = matrice[1][0];
    } else if (colonne > 1) {
        first_next_height = matrice[0][1];
    }
    sum_diff_heights_squared += (first_height - first_next_height) * (first_height - first_next_height);

    /* Calcola il peso totale usando la formula*/
    weight = Ccell * k + Cheight * sum_diff_heights_squared;
    return weight;
}

int calc_num_cells(int **path, int righe, int colonne) {
    int count = 0;
    int i = righe - 1, j = colonne - 1;

    while (i != 0 || j != 0) {
        count++;
        if (path[i][j] == 1) {
            i--;
        } else if (path[i][j] == 2) {
            j--;
        }
    }
    count++; /* Conta la cella di partenza (0,0)*/
    return count;
}

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
    int nCell;
    int weight;

    for (i = 0; i < righe; i++) {
        dist[i] = (int *)malloc(colonne * sizeof(int));
        path[i] = (int *)malloc(colonne * sizeof(int));
        for (j = 0; j < colonne; j++) {
            dist[i][j] = INT_MAX;
            path[i][j] = 0;
        }
    }

    dist[0][0] = matrice[0][0];

    for (i = 0; i < righe; i++) {
        for (j = 0; j < colonne; j++) {
            if (i > 0 && dist[i][j] > dist[i-1][j] + matrice[i][j]) {
                dist[i][j] = dist[i-1][j] + matrice[i][j];
                path[i][j] = 1; /* 1 significa che proviene dall'alto*/
            }
            if (j > 0 && dist[i][j] > dist[i][j-1] + matrice[i][j]) {
                dist[i][j] = dist[i][j-1] + matrice[i][j];
                path[i][j] = 2; /* 2 significa che proviene da sinistra*/
            }
        }
    }

    printf("Il percorso minimo dal primo all'ultimo posto è:\n");
    stampaPercorso(path, righe-1, colonne-1);
    printf("\n");

    nCell = calc_num_cells(path, righe, colonne);
    printf("Il numero delle celle del percorso è: %d\n", nCell);

    weight = calc_weight(matrice, path, righe, colonne);
    printf("Il peso totale del percorso è: %d\n", weight);
    

    for (i = 0; i < righe; i++) {
        free(dist[i]);
        free(path[i]);
    }
    free(dist);
    free(path);
}

int main(int argc, char *argv[]) {
    int righe, colonne;
    int **matrice;
    int i, j;
    FILE *filein = stdin;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return EXIT_FAILURE;
    }

    filein = fopen(argv[1], "r");
    if (filein == NULL) {
        fprintf(stderr, "Cannot open %s\n", argv[1]);
        return EXIT_FAILURE;
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
