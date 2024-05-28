#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define MAX_ROWS 250
#define MAX_COLS 250

int Ccell; 
int Cheight; 
long int totalCost; 

/*
Funzione per rilassare un arco durante l'algoritmo di Bellman-Ford 

Calcolo delle righe e delle colonne di sorgente e destinazione
    int src_row = src / cols;
    int src_col = src % cols;
    int dst_row = dst / cols;
    int dst_col = dst % cols;

Calcolo della differenza di altezza tra la cella di destinazione e la cella di sorgente
    int height_diff = matrix[src_row][src_col] - matrix[dst_row][dst_col];

Calcolo del costo totale per raggiungere la cella di destinazione
    long int cost = dist[src] + Cheight * height_diff * height_diff + Ccell;
*/
int relax(int src, int dst, int* dist, int* parent, int** matrix, int cols) {
    int src_row = src / cols;
    int src_col = src % cols;
    int dst_row = dst / cols;
    int dst_col = dst % cols;
    int height_diff = matrix[src_row][src_col] - matrix[dst_row][dst_col];
    long int cost = dist[src] + Cheight * height_diff * height_diff + Ccell;

    if (dist[src] != INT_MAX && cost < dist[dst]) {
        dist[dst] = cost;
        parent[dst] = src;
        return 1;
    }
    return 0;
}
/*
V-Numero totale di vertici
dist-Distanza minima per ogni vertice
parent-Vertice precedente nel percorso minimo
path-Percorso minimo
path_length-Lunghezza del percorso minimo

Inizializzazione delle distanze e dei predecessori 
    for (i = 0; i < V; ++i) {
        dist[i] = INT_MAX;
        p[i] = -1;
    }
Imposto il peso della sorgente a 0
    dist[src] = 0;

Utilizzo la funzione relax per rilassare gli archi e calcolare i costi minimi per raggiungere ogni vertice.

Gli if verificano se è possibile spostarsi in una delle direzioni cardinali 
rispetto alla posizione corrente u, e in caso positivo, aggiornano i costi dei percorsi se il percorso attraverso 
la cella adiacente è più conveniente.

Utilizzo l'ottimizzazione di Bellman-Ford fornita in classe.
*/
void BellmanFord(int src, int rows, int cols, int** matrix) {
    int V = rows * cols; 
    int dist[MAX_ROWS * MAX_COLS]; 
    int p[MAX_ROWS * MAX_COLS]; 
    int path[MAX_ROWS * MAX_COLS]; 
    int path_length = 0; 
    int i, j, updated;
    int row, col, xi, yi, height_diff;
    int u, v;
    
    for (i = 0; i < V; ++i) {
        dist[i] = INT_MAX;
        p[i] = -1;
    }
    dist[src] = 0;

    for (i = 0; i < V - 1; ++i) {
        updated = 0;
        for (j = 0; j < V; ++j) {
            u = j;
            if (u - cols >= 0) {            /* Vicino sopra*/
                v = u - cols;
                updated |= relax(u, v, dist, p, matrix, cols);
            }
            if (u + cols < V) {             /* Vicino sotto*/
                v = u + cols;
                updated |= relax(u, v, dist, p, matrix, cols);
            }
            if (u % cols != 0) {            /* Vicino a sinistra*/
                v = u - 1;
                updated |= relax(u, v, dist, p, matrix, cols);
            }
            if ((u + 1) % cols != 0) {      /* Vicino a destra*/
                v = u + 1;
                updated |= relax(u, v, dist, p, matrix, cols);
            }
        }
        if (!updated) {
            break;
        }
    }

    
    totalCost = 0;                          /** Calcolo del percorso minimo e del costo totale */
    v = rows * cols - 1;
    while (v != -1) {
        row = v / cols;
        col = v % cols;
        xi = p[v] / cols;
        yi = p[v] % cols;
        height_diff = matrix[row][col] - matrix[xi][yi];
        totalCost += height_diff * height_diff;
        path[path_length++] = v; 
        v = p[v];
    }

   
    for (i = path_length - 1; i >= 0; --i) {  /** Stampa del percorso minimo */
        int v = path[i];
        int row = v / cols;
        int col = v % cols;
        printf("%d %d\n", row, col);
    }
    printf("-1 -1\n");
    printf("%d\n", Ccell + dist[rows * cols - 1]);
}
/* 
Riempimento della matrice con i valori letti dal file di input

    matrix = (int**)malloc(rows * sizeof(int*));
    for (i = 0; i < rows; ++i) {
        matrix[i] = (int*)malloc(cols * sizeof(int));
        for (j = 0; j < cols; ++j) {
            fscanf(filein, "%d", &matrix[i][j]);
        }
    }
*/
int main(int argc, char *argv[]) {
    int rows, cols;
    int** matrix;
    int i, j;
    FILE *filein = stdin;

    if (argc < 2 || argc > 4) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
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
    fscanf(filein, "%d", &rows);
    fscanf(filein, "%d", &cols);

    matrix = (int**)malloc(rows * sizeof(int*));
    for (i = 0; i < rows; ++i) {
        matrix[i] = (int*)malloc(cols * sizeof(int));
        for (j = 0; j < cols; ++j) {
            fscanf(filein, "%d", &matrix[i][j]);
        }
    }

    if (filein != stdin) {
        fclose(filein);
    }

    BellmanFord(0, rows, cols, matrix);

    for (i = 0; i < rows; ++i) {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}
