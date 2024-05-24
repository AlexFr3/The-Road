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
long int totalCost;
int relax(int src, int dst, int weight, int* dist, int* parent) {
    long int quad_pow2 = (dist[src] * weight) * (dist[src] * weight);
    if (dist[src] != INT_MAX && quad_pow2 < dist[dst]) {
        dist[dst] = quad_pow2;
        parent[dst] = src;
        return 1;
    }
    return 0;
}
/*
long int sum;
int relax(int src, int dst, int weight, int* dist, int* parent) {
    long int quad_pow2 = (dist[src] * weight) * (dist[src] * weight);
    long int quad_pow2Dst = (dist[dst] * weight) * (dist[dst] * weight);
    if (dist[dst] != INT_MAX) {
        dist[dst] = quad_pow2Dst;
    }
    if (dist[src] != INT_MAX && quad_pow2 < dist[dst]) {
        dist[dst] = quad_pow2;
        parent[dst] = src;
        sum += quad_pow2;
        return 1;
    }
    return 0;
}
*/
void BellmanFord(Graph* graph, int src, int rows, int cols, int** matrix) {
    int dist[MAX_VERTICES];
    int parent[MAX_VERTICES];
    int V = graph->V;
    int E = graph->E;
    int i, j, updated;

    for (i = 0; i < V; ++i) {
        dist[i] = INT_MAX;
        parent[i] = -1;
    }
    dist[src] = matrix[0][0];

    for (i = 0; i < V - 1; ++i) {
        updated = 0;
        for (j = 0; j < E; ++j) {
            int u = graph->edges[j].src;
            int v = graph->edges[j].dest;
            int weight = graph->edges[j].weight;
            updated |= relax(u, v, weight, dist, parent);
        }
        if (!updated) {
            break;
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

    int path[MAX_VERTICES];
    int path_length = 0;
    int v = rows * cols - 1;
    while (v != -1) {
        path[path_length++] = v;
        v = parent[v];
    }

    printf("Il percorso minimo dal primo all'ultimo posto è:\n");
    for (i = path_length - 1; i >= 0; --i) {
        int row = path[i] / cols;
        int col = path[i] % cols;
        printf("(%d, %d) ", row, col);
    }

    totalCost = 0;
    for (i = path_length - 1; i > 0; --i) {
        int xi = path[i] / cols;
        int yi = path[i] % cols;
        int xi1 = path[i - 1] / cols;
        int yi1 = path[i - 1] % cols;
        int diff = matrix[xi][yi] - matrix[xi1][yi1];
        totalCost += diff * diff;
    }

    printf("\nCon peso totale: %d\n", dist[rows * cols - 1]);
    printf("Il costo totale è: %ld\n", Ccell * path_length + Cheight * totalCost);
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
                graph.edges[graph.E].weight = abs(matrix[i + 1][j] - matrix[i][j]);
                graph.E++;
            }
            if (j < cols - 1) {
                graph.edges[graph.E].src = i * cols + j;
                graph.edges[graph.E].dest = i * cols + (j + 1);
                graph.edges[graph.E].weight = abs(matrix[i][j + 1] - matrix[i][j]);
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
