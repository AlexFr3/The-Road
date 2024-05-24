#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define ROWS 500
#define COLS 500

typedef struct Edge
{
    int src;
    int dst;
    double weight;
    struct Edge *next;
} Edge;

typedef struct
{
    int n;
    int m;
    Edge **edges;
} Graph;

typedef struct
{
    int n;
    int m;
    int mat[ROWS][COLS];
    int visited[ROWS][COLS];
} Matrix;

int Cheight;
int Ccell;
void print_graph(const Graph *g)
{
    int i;
    for (i = 0; i < g->n; ++i)
    {
        Edge *edge = g->edges[i];
        printf("Nodo %d: ", i);
        while (edge != NULL)
        {
            printf("(%d, %f) ", edge->dst, edge->weight);
            edge = edge->next;
        }
        printf("\n");
    }
}

Edge *new_edge(int src, int dst, double weight, Edge *next)
{
    Edge *edge = (Edge *)malloc(sizeof(Edge));
    assert(edge != NULL);
    edge->src = src;
    edge->dst = dst;
    edge->weight = weight;
    edge->next = next;
    return edge;
}

void graph_adj_insert(Graph *g, int src, int dst, double weight)
{
    g->edges[src] = new_edge(src, dst, weight, g->edges[src]);
}

void lateral_control(Graph *g, Matrix *mat, int i, int j, int k, const int weight)
{
    if (j + 1 < mat->m )
    {
        graph_adj_insert(g, k, k + 1, weight);
        graph_adj_insert(g, k + 1, k, weight);
        g->m++;
    }
}

void under_control(Graph *g, Matrix *mat, int i, int j, int k, const int weight)
{
    if (i + 1 < mat->n )
    {
        graph_adj_insert(g, k, k + mat->m, weight);
        graph_adj_insert(g, k + mat->m, k, weight);
        g->m++;
    }
}

void matrix_into_graph(Matrix *mat, Graph *g)
{
    int i, j, k;
    assert(g != NULL);
    assert(mat != NULL);
    i = 0;
    j = 0;
    k = 0;

    for (i = 0; i < mat->n; i++)
    {
        for (j = 0; j < mat->m; j++)
        {
            lateral_control(g, mat, i, j, k, Ccell);
            under_control(g, mat, i, j, k, Ccell);
            k++;
        }
    }
}

void print_path(int *p, const Graph *g)
{
    int i;
    for (i = 0; i < g->n; ++i)
    {
        printf("Indice: %d, Predecessore: %d\n", i, p[i]);
    }
}
/*double effort = (double)Cheight * pow((mat->mat[src_row][src_col] - mat->mat[dst_row][dst_col]), 2) + Ccell;*/
void relax(int src, int dst, double weight, double *d, int *p, Matrix *mat) {
    if (d[src] != HUGE_VAL && d[src] + weight < d[dst]) {
        d[dst] = d[src] + weight;
        p[dst] = src;
        printf("Relax: aggiornato predecessore di %d a %d con sforzo %f\n", dst, src, weight);
    }
}


void test_initialize(const Graph *g, int s, double *d, int *p)
{
    int i;

    if (d[s] != 0.0)
    {
        printf("ERRORE: La distanza del nodo sorgente non è stata impostata correttamente.\n");
        return;
    }

    for (i = 0; i < g->n; i++)
    {
        if (i != s && d[i] != HUGE_VAL)
        {
            printf("ERRORE: La distanza del nodo %d non è stata impostata correttamente.\n", i);
            return;
        }
    }

    for (i = 0; i < g->n; i++)
    {
        if (p[i] != -1)
        {
            printf("ERRORE: Il predecessore del nodo %d non è stato impostato correttamente.\n", i);
            return;
        }
    }

    printf("Tutti i test hanno superato con successo. La funzione initialize funziona correttamente.\n");
}

void initialize(const Graph *g, int s, double *d, int *p)
{
    int i;
    for (i = 0; i < g->n; i++)
    {
        d[i] = HUGE_VAL;
        p[i] = -1;
    }
    d[s] = 0;

    /*printf("Initial distances and predecessors:\n");
    for (i = 0; i < g->n; i++)
    {
        printf("Nodo %d: distanza = %f, predecessore = %d\n", i, d[i], p[i]);
    }*/
}

int bellman_ford(const Graph *g, int src, double *d, int *p, Matrix *mat)
{
    int u, i;
    Edge *edge;
    int *visited = (int *)malloc(g->n * sizeof(int));

    for (i = 0; i < g->n; i++)
    {
        visited[i] = 0;
    }

    initialize(g, src, d, p);
    /*test_initialize(g, src, d, p);*/

    for (i = 0; i < (g->n * g->m); i++)
    {
        for (u = 0; u < g->n; u++)
        {
            if (visited[u])
                continue;

            edge = g->edges[u];
            while (edge != NULL)
            {
                relax(edge->src, edge->dst, edge->weight, d, p, mat);
                visited[edge->src] = 1;
                edge = edge->next;
            }
        }
    }

    for (u = 0; u < g->n; u++)
    {
        Edge *edge = g->edges[u];
        while (edge != NULL)
        {
            if (d[edge->src] != HUGE_VAL && d[edge->src] + edge->weight < d[edge->dst])
            {
                printf("Il grafo contiene un ciclo di peso negativo.\n");
                free(visited);
                return 1;
            }
            edge = edge->next;
        }
    }

    free(visited);
    return 0;
}

void graph_destroy(Graph *g)
{
    int i;
    assert(g != NULL);

    for (i = 0; i < g->n; i++)
    {
        Edge *edge = g->edges[i];
        while (edge != NULL)
        {
            Edge *next = edge->next;
            free(edge);
            edge = next;
        }
        g->edges[i] = NULL;
    }
    free(g->edges);
    g->n = 0;
    g->edges = NULL;
    free(g);
}

Graph *graph_create(int n)
{
    int i;
    Graph *g = (Graph *)malloc(sizeof(*g));
    assert(g != NULL);
    assert(n > 0);

    g->n = n;
    g->m = 0;
    g->edges = (Edge **)malloc(n * sizeof(Edge *));
    assert(g->edges != NULL);
    for (i = 0; i < n; i++)
    {
        g->edges[i] = NULL;
    }
    return g;
}

void print_matrix(Matrix *mat)
{
    int i, j;
    for (i = 0; i < mat->n; i++)
    {
        for (j = 0; j < mat->m; j++)
        {
            printf("%d ", mat->mat[i][j]);
        }
        printf("\n");
    }
}

Matrix *read_matrix_from_file(FILE *f, int n, int m)
{
    Matrix *matrix;
    int i, j;
    matrix = (Matrix *)malloc(sizeof(Matrix));
    matrix->n = n;
    matrix->m = m;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            if (1 != fscanf(f, "%d", &matrix->mat[i][j]))
            {
                fprintf(stderr, "ERRORE durante la lettura della matrice\n");
                abort();
            }
        }
    }
    return matrix;
}

Graph *graph_read_from_file(FILE *f, Matrix **mat)
{
    int n, m;
    Graph *g;
    assert(f != NULL);
    if (4 != fscanf(f, "%d %d %d %d", &Ccell, &Cheight, &n, &m))
    {
        fprintf(stderr, "ERRORE durante la lettura dell'intestazione del grafo\n");
        abort();
    }

    *mat = read_matrix_from_file(f, n, m);

    g = graph_create(n * m);
    printf("Grafo creato:\n");
    print_graph(g);
    matrix_into_graph(*mat, g);
    printf("Grafo dopo la creazione:\n");
    print_graph(g);
    return g;
}

void run_graph_algorithm(FILE *f)
{
    int prev;
    int src_row;
    int src_col;
    int dst_row;
    int dst_col;
    double effort;
    int current;
    double total_weight = 0;
    double *distances;
    int *predecessors;
    int success;
    Graph *g;
    Matrix *matrix;
    int source = 0;
    int dest;

    g = graph_read_from_file(f, &matrix);
    fclose(f);
    printf("Grafo in run:\n");
    print_graph(g);
    dest = (matrix->n)* (matrix->m )-1;
    distances = (double *)malloc(g->n * sizeof(double));
    predecessors = (int *)malloc(g->n * sizeof(int));

    success = bellman_ford(g, source, distances, predecessors, matrix);
    if (success)
    {
        printf("Errore durante l'esecuzione di Bellman-Ford\n");
        exit(EXIT_FAILURE);
    }

    printf("Percorso minimo da %d a %d:\n", source, dest);

    current = dest;
    while (current != source)
    {
        if (predecessors[current] == -1)
        {
            printf("Non esiste un percorso valido da %d a %d\n", source, dest);
            free(distances);
            free(predecessors);
            graph_destroy(g);
            free(matrix);
            return;
        }
        prev = predecessors[current];
        src_row = prev / matrix->m;
        src_col = prev % matrix->m;
        dst_row = current / matrix->m;
        dst_col = current % matrix->m;
        effort = (double)Cheight * pow((matrix->mat[src_row][src_col] - matrix->mat[dst_row][dst_col]), 2) + Ccell;
        total_weight += effort;
        printf("%d %d\n", dst_row, dst_col);
        current = prev;
    }
    printf("%d %d\n", source / matrix->m, source % matrix->m);

    printf("Costo totale del percorso: %f\n", total_weight);

    free(distances);
    free(predecessors);
    graph_destroy(g);
    free(matrix);
}



int main(int argc, char **argv)
{
    FILE *filein = stdin;

    if (argc < 2 || argc > 4)
    {
        fprintf(stderr, "Usage: %s filename [src [dst]]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-") != 0)
    {
        filein = fopen(argv[1], "r");
        if (filein == NULL)
        {
            fprintf(stderr, "Cannot open %s\n", argv[1]);
            return EXIT_FAILURE;
        }
    }
    run_graph_algorithm(filein);

    return 0;
}
