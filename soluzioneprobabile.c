#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define MAX_ROWS 150
#define MAX_COLS 150
#define MAX_VERTICES (MAX_ROWS * MAX_COLS)
#define MAX_EDGES (2 * (MAX_ROWS - 1) * MAX_COLS + 2 * MAX_ROWS * (MAX_COLS - 1))

typedef struct Edge {
    int src, dest, weight;
} Edge;

typedef struct Graph {
    Edge edges[MAX_EDGES];
    int V, E;
} Graph;

int Ccell;
int Cheight;

void BellmanFord(Graph* graph, int src, int rows, int cols, int** matrix) {
    int dist[MAX_VERTICES];
    int parent[MAX_VERTICES];
    int V = graph->V;
    int E = graph->E;
    int i, j;
    int v;
    int sum_quad_pow2 = 0;
    int nCell = 0;
    int total;

    for (i = 0; i < V; ++i) {
        dist[i] = INT_MAX;
        parent[i] = -1;
    }
    dist[src] = matrix[0][0];

    for (i = 1; i < V; ++i) {
        for (j = 0; j < E; ++j) {
            int u = graph->edges[j].src;
            int v = graph->edges[j].dest;
            int weight = graph->edges[j].weight;
            if (dist[u] != INT_MAX && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
            }
        }
    }

    for (i = 0; i < E; ++i) {
        int u = graph->edges[i].src;
        int v = graph->edges[i].dest;
        int weight = graph->edges[i].weight;
        if (dist[u] != INT_MAX && dist[u] + weight < dist[v]) {
            printf("Il grafo contiene un ciclo di pesi negativi\n");
            return;
        }
    }

    printf("Il percorso minimo dal primo all'ultimo posto è:\n");
    v = rows * cols - 1;
    while (v != -1) {
        int row = v / cols;
        int col = v % cols;
        printf("(%d, %d) ", row, col);
        v = parent[v];
        nCell++;
    }

    for (v = rows * cols - 1; parent[v] != -1; v = parent[v]) {
        int u = parent[v];
        int xi = u / cols;
        int yi = u % cols;
        int xi1 = v / cols;
        int yi1 = v % cols;
        int diff = matrix[xi][yi] - matrix[xi1][yi1];
        sum_quad_pow2 += diff * diff;
    }

    total = Ccell * nCell + Cheight * sum_quad_pow2;

    printf("\nCon peso totale: %d\n", dist[rows * cols - 1]);
    printf("Il costo totale è: %d\n", total);
}

int main(int argc, char *argv[]) {
    Graph graph;
    int rows, cols;
    int** matrix;
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
    fscanf(filein, "%d", &rows);
    fscanf(filein, "%d", &cols);

    matrix = (int**)malloc(rows * sizeof(int*));
    for (i = 0; i < rows; ++i) {
        matrix[i] = (int*)malloc(cols * sizeof(int));
        for (j = 0; j < cols; ++j) {
            fscanf(filein, "%d", &matrix[i][j]);
        }
    }

    fclose(filein);

    graph.V = rows * cols;
    graph.E = 0;

    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j) {
            if (i < rows - 1) {
                graph.edges[graph.E].src = i * cols + j;
                graph.edges[graph.E].dest = (i + 1) * cols + j;
                graph.edges[graph.E].weight = matrix[i + 1][j];
                graph.E++;
            }
            if (j < cols - 1) {
                graph.edges[graph.E].src = i * cols + j;
                graph.edges[graph.E].dest = i * cols + (j + 1);
                graph.edges[graph.E].weight = matrix[i][j + 1];
                graph.E++;
            }
        }
    }

    BellmanFord(&graph, 0, rows, cols, matrix);

    for (i = 0; i < rows; ++i) {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}
