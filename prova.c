#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
int sum = 0;
int Ccell, Cheight;

int calc_weight(int **matrice, int **path, int righe, int colonne) {
   int weight = 0;
    int sum_diff_heights_squared = 0;
    int k = 0;
    int curr_height, prev_height;
    int i = righe - 1, j = colonne - 1;

    while (i > 0 || j > 0) {
        curr_height = matrice[i][j];
        if (k > 0) {
            sum_diff_heights_squared += (curr_height - prev_height) * (curr_height - prev_height);
        }
        prev_height = curr_height;
        k++;
        if (path[i][j] == 1 || path[i][j] == 3) {
            i--;
        } else if (path[i][j] == 2 || path[i][j] == 4) {
            j--;
        }
    }

    curr_height = matrice[0][0];
    if (k > 0) {
        sum_diff_heights_squared += (curr_height - prev_height) * (curr_height - prev_height);
    }
    k++;

    weight = Ccell * k + Cheight * sum_diff_heights_squared;
    return weight;
}

int calc_num_cells(int **path, int righe, int colonne) {
    int count = 0;
    int i = righe - 1, j = colonne - 1;
    while (i > 0 || j > 0) {
        count++;
        if (path[i][j] == 1 || path[i][j] == 3) {
            i--;
        } else if (path[i][j] == 2 || path[i][j] == 4) {
            j--;
        }
    }
    return count + 1; 
}

void stampaPercorso(int **path, int i, int j) {
    if (i == 0 && j == 0) {
        printf("(%d, %d)\n", i, j);
        return;
    }
    if (path[i][j] == 1 || path[i][j] == 3) {
        stampaPercorso(path, i - 1, j);
    } else if (path[i][j] == 2 || path[i][j] == 4) {
        stampaPercorso(path, i, j - 1);
    }
    printf("(%d, %d)\n", i, j);
}
/*0 se non esiste*/
int pathRightExists(int **path, int righe, int colonne, int i, int j) {
    return (j < colonne - 1) && (path[i][j + 1] != 0);
}

int pathLeftExists(int **path, int righe, int colonne, int i, int j) {
    return (j > 0) && (path[i][j - 1] != 0);
}

int pathDownExists(int **path, int righe, int colonne, int i, int j) {
    return (i < righe - 1) && (path[i + 1][j] != 0);
}

void trovaCammino(int **matrice, int righe, int colonne) {
    int i, j, nCell, weight;
    int **dist = (int **)malloc(righe * sizeof(int *));
    int **path = (int **)malloc(righe * sizeof(int *));
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

            if (i > 0 && dist[i][j] > dist[i - 1][j] + matrice[i][j] && path[i][j] == 0) {
                dist[i][j] = dist[i - 1][j] + matrice[i][j];
                path[i][j] = 1; /* Viene dall'alto */
            }else if (j > 0 && dist[i][j] > dist[i][j - 1] + matrice[i][j] && path[i][j] == 0) {
                dist[i][j] = dist[i][j - 1] + matrice[i][j];
                path[i][j] = 2; /* Viene da sinistra */
            }else if (i < righe - 1 && dist[i][j] > dist[i + 1][j] + matrice[i][j] && path[i][j] == 0) {
                dist[i][j] = dist[i + 1][j] + matrice[i][j];
                path[i][j] = 3; /* Scendere */
            }else if (j < colonne - 1 && dist[i][j] > dist[i][j + 1] + matrice[i][j] && path[i][j] == 0) {
                dist[i][j] = dist[i][j + 1] + matrice[i][j];
                path[i][j] = 4; /* Andare a destra */
            }
                        printf("Percorso parziale [%d][%d] = %d\n", i, j, path[i][j]);
        }
    }

    printf("Il percorso minimo dal primo all'ultimo posto e':\n");
    stampaPercorso(path, righe - 1, colonne - 1);
    nCell = calc_num_cells(path, righe, colonne);
    printf("Il numero delle celle del percorso e': %d\n", nCell);
    weight = calc_weight(matrice, path, righe, colonne);
    printf("Il peso totale del percorso e': %d\n", weight);

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

    if (argc < 2 || argc > 4) {
        fprintf(stderr, "Usage: %s filename [src [dst]]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-") != 0) {
        filein = fopen(argv[1], "r");
        if (filein == NULL) {
            fprintf(stderr, "Cannot open %s\n", argv[1]);
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

