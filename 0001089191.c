#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include <math.h>

typedef struct Edge {
    int src;            /* nodo sorgente */
    int dst;            /* nodo destinazione */
    double weight;      /* peso dell'arco */
    struct Edge *next;
} Edge;

typedef struct {
    int n;
    int m;
    Edge **edges;       /* array di liste di adiacenza */
    int *in_deg;        /* grado entrante dei nodi */
    int *out_deg;       /* grado uscente dei nodi */
} Graph;

#define ROWS 500
#define COLS 500
typedef struct {
    int n;
    int m;
    int mat[ROWS][COLS];
    int visited[ROWS][COLS]; /*0 se non visitato, 1 se visitato*/
} Matrix;

void print_path(int *p, const Graph *g) {
    int i;
    for (i = 0; i < g->n; ++i) {
        printf("Indice: %d, Predecessore: %d\n", i, p[i]);
    }
}

void relax(int src, int dst, double weight, double *d, int *p) {
    if (d[src] != HUGE_VAL && d[src] + weight < d[dst]) {
        d[dst] = d[src] + weight;
        p[dst] = src;
        printf("Relax: aggiornato predecessore di %d a %d\n", dst, src);
        /*new_effort=edge->src->effort+(edge->weight*Cheight)+Ccell;*/
    }
}

void test_initialize(const Graph *g, int s, double *d, int *p) {
    int i;

    if (d[s] != 0.0) {
        printf("ERRORE: La distanza del nodo sorgente non è stata impostata correttamente.\n");
        return;
    }

    for (i = 0; i < g->n; i++) {
        if (i != s && d[i] != HUGE_VAL) {
            printf("ERRORE: La distanza del nodo %d non è stata impostata correttamente.\n", i);
            return;
        }
    }

    for (i = 0; i < g->n; i++) {
        if (p[i] != -1) {
            printf("ERRORE: Il predecessore del nodo %d non è stato impostato correttamente.\n", i);
            return;
        }
    }

    printf("Tutti i test hanno superato con successo. La funzione initialize funziona correttamente.\n");
}

void initialize(const Graph *g, int s, double *d, int *p) {
    int i;
    for (i = 0; i < g->n; i++) {
        d[i] = HUGE_VAL;
        p[i] = -1;
    }
    d[s] = 0;

    printf("Initial distances and predecessors:\n");
    for (i = 0; i < g->n; i++) {
        printf("Nodo %d: distanza = %f, predecessore = %d\n", i, d[i], p[i]);
    }
}

int bellman_ford(const Graph *g, int src, double *d, int *p, const Edge **sp) {
    int u, i;
    int *visited = (int *)malloc(g->n * sizeof(int));

    for (i = 0; i < g->n; i++) {
        visited[i] = 0;  /* Inizializzo tutti i nodi come non visitati */
    }

    initialize(g, src, d, p);
    test_initialize(g, src, d, p);

    for (i = 0; i < (g->n * g->m); i++) {
        for (u = 0; u < g->n; u++) {
            if (visited[u]) continue; /* Salta i nodi già visitati */

            Edge *edge = g->edges[u];
            while (edge != NULL) {
                relax(edge->src, edge->dst, edge->weight, d, p);
                visited[edge->src] = 1; /* Imposta il nodo come visitato */
                edge = edge->next;
            }
        }
    }

    for (u = 0; u < g->n; u++) {
        Edge *edge = g->edges[u];
        while (edge != NULL) {
            if (d[edge->src] != HUGE_VAL && d[edge->src] + edge->weight < d[edge->dst]) {
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

void graph_destroy(Graph *g) {
    int i;
    assert(g != NULL);

    for (i = 0; i < g->n; i++) {
        Edge *edge = g->edges[i];
        while (edge != NULL) {
            Edge *next = edge->next;
            free(edge);
            edge = next;
        }
        g->edges[i] = NULL;
    }
    free(g->edges);
    free(g->in_deg);
    free(g->out_deg);
    g->n = 0;
    g->edges = NULL;
    free(g);
}

Graph *graph_create(int n, int m) {
    int i;
    Graph *g = (Graph *)malloc(sizeof(*g));
    assert(g != NULL);
    assert(n > 0);

    g->n = n;
    g->m = m;
    g->edges = (Edge **)malloc(n * sizeof(Edge *));
    assert(g->edges != NULL);
    g->in_deg = (int *)malloc(n * sizeof(*(g->in_deg)));
    assert(g->in_deg != NULL);
    g->out_deg = (int *)malloc(n * sizeof(*(g->out_deg)));
    assert(g->out_deg != NULL);
    for (i = 0; i < n; i++) {
        g->edges[i] = NULL;
        g->in_deg[i] = g->out_deg[i] = 0;
    }
    return g;
}

void print_matrix(Matrix *mat) {
    int i, j;
    for (i = 0; i < mat->n; i++) {
        for (j = 0; j < mat->m; j++) {
            printf("%d ", mat->mat[i][j]);
        }
        printf("\n");
    }
}

Matrix *read_matrix_from_file(FILE *f, int n, int m) {
    Matrix *matrix;
    int i, j;
    matrix = (Matrix *)malloc(sizeof(Matrix));
    matrix->n = n;
    matrix->m = m;
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            if (1 != fscanf(f, "%d", &matrix->mat[i][j])) {
                fprintf(stderr, "ERRORE durante la lettura della matrice\n");
                abort();
            }
        }
    }
    return matrix;
}

Graph *graph_read_from_file(FILE *f) {
    int n, m, i, j;
    int Ccell, Cheight;
    Graph *g;
    Matrix *matrix;

    assert(f != NULL);
    if (4 != fscanf(f, "%d \n %d \n %d \n %d \n", &Ccell, &Cheight, &n, &m)) {
        fprintf(stderr, "ERRORE durante la lettura dell'intestazione del grafo\n");
        abort();
    }

    g = graph_create(n, m);
    matrix = read_matrix_from_file(f, n, m);
    print_matrix(matrix);

    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            if (matrix->mat[i][j] != 0) {
                Edge *edge = (Edge *)malloc(sizeof(Edge));
                edge->src = i;
                edge->dst = j;
                edge->weight = matrix->mat[i][j];
                edge->next = g->edges[i];
                g->edges[i] = edge;
                g->out_deg[i]++;
                g->in_deg[j]++;
            }
        }
    }

    free(matrix);
    return g;
}

int main(int argc, char *argv[]) {
    Graph *G;
    FILE *filein = stdin;
    const Edge **sp;
    double *d;
    int *p;
    int neg, src = 0;

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

    G = graph_read_from_file(filein);
    d = (double *)malloc(G->n * sizeof(*d));
    assert(d != NULL);
    p = (int *)malloc(G->n * sizeof(*p));
    assert(p != NULL);
    sp = (const Edge **)malloc(G->n * sizeof(*sp));
    assert(sp != NULL);

    neg = bellman_ford(G, src, d, p, sp);
    print_path(p, G);
    graph_destroy(G);

    if (filein != stdin) fclose(filein);

    return EXIT_SUCCESS;
}
