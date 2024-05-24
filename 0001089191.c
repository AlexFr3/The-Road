#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define ROWS 500
#define COLS 500

typedef struct Edge {
    int src;           
    int dst;            
    double weight;      /* peso dell'arco */
    struct Edge *next;
} Edge;

typedef struct {
    int n;
    int m;
    Edge **edges;       /* array di liste di adiacenza */
} Graph;

typedef struct {
    int n;
    int m;
    int mat[ROWS][COLS];
} Matrix;

int Cheight;
int Ccell;
 
Edge* new_edge(int src, int dst, double weight, Edge* next)
{
    Edge* edge = (Edge*)malloc(sizeof(Edge));
    assert(edge != NULL);

    edge->src = src;
    edge->dst = dst;
    edge->weight = weight;
    edge->next = next;
    return edge;
}


/*Inserisce l'arco (src, dst, weight) nel grafo.*/
void graph_adj_insert(Graph* g, int src, int dst, double weight)
{
    g->edges[src] = new_edge(src, dst, weight, g->edges[src]);
}


/*Esegue il controllo per poter aggiungere un arco tra il nodo corrente 'k' e quello affianco a destra
'k + 1' del grafo (controllando le corrispondenti posizioni della matrice). Mi assicuro che il controllo
vi sia solo se effettivamente affianco vi Ë un'altra colonna di matrice (ovvero se j+1 sia minore delle
colonne della matirce. Nel caso affermativo, controllo se la cella in cui siamo per il controllo sia uguale
a quella affianco, ovvero se Ë 0, e poi aggiungo un arco da entrambe le direzioni non essendo un grafo
orientato quello che stiamo costruendo. L'arco ha sempre peso 1.*/
void lateral_control(Graph* g, Matrix* mat, int i, int j, int k, const int weight) {
    if (j + 1 < mat->m) {
        graph_adj_insert(g, k, k + 1, weight);
        graph_adj_insert(g, k + 1, k, weight);
        g->m++;
    }
}


/*Esegue il controllo per poter aggiungere un arco tra il nodo corrente 'k' e quello sottostante 'k + nrighe' del
grafo (controllando le corrispondenti posizioni della matrice). Mi assicuro che il controllo vi sia solo se
effettivamente sotto vi Ë un'altra riga di matrice (ovvero se i+1 sia minore delle righe della matirce.
Nel caso affermativo, controllo se la cella in cui siamo per il controllo sia uguale a quella sotto, ovvero se Ë 0,
e poi aggiungo un arco da entrambe le direzioni non essendo un grafo orientato quello che stiamo costruendo.
L'arco ha sempre peso 1.*/
void under_control(Graph* g, Matrix* mat, int i, int j, int k, const int weight) {
    if (i + 1 < mat->n) {
        graph_adj_insert(g, k, k + mat->m, weight);
        graph_adj_insert(g, k + mat->m, k, weight);
        g->m++;
    }
}

void matrix_into_graph(Matrix* mat, Graph* g) {
    int i;
    int j;
    int k;

    assert(g != NULL);
    assert(mat != NULL);

    i = 0;
    j = 0;
    k = 0;

    for (i = 0; i < mat->n; i++) {
        for (j = 0; j < mat->m; j++) {
            lateral_control(g, mat, i, j, k, Ccell);
            under_control(g, mat, i, j, k, Ccell);
            k++;
        }
    }
}
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
    Edge *edge;
    

    initialize(g, src, d, p);
    /*test_initialize(g, src, d, p);*/

    for (i = 0; i < (g->n * g->m)-1; i++) {
        for (u = 0; u < g->n; u++) {

            edge = g->edges[u];
            while (edge != NULL) {
                relax(edge->src, edge->dst, edge->weight, d, p);
                edge = edge->next;
            }
        }
    }

    for (u = 0; u < g->n; u++) {
        Edge *edge = g->edges[u];
        while (edge != NULL) {
            if (d[edge->src] != HUGE_VAL && d[edge->src] + edge->weight < d[edge->dst]) {
                printf("Il grafo contiene un ciclo di peso negativo.\n");
                return 1;
            }
            edge = edge->next;
        }
    }

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
    g->n = 0;
    g->edges = NULL;
    free(g);
}

Graph *graph_create(int n) {
    int i;
    Graph *g = (Graph *)malloc(sizeof(*g));
    assert(g != NULL);
    assert(n > 0);

    g->n = n;
    g->m = 0;
    g->edges = (Edge **)malloc(n * sizeof(Edge *));
    assert(g->edges != NULL);
    for (i = 0; i < n; i++) {
        g->edges[i] = NULL;
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
    int n, m;
    Graph *g;
    Matrix *matrix;
    assert(f != NULL);
    if (4 != fscanf(f, "%d %d %d %d", &n, &m, &n, &m)) {
        fprintf(stderr, "ERRORE durante la lettura dell'intestazione del grafo\n");
        abort();
    }

    g = graph_create(n*m);
    matrix = read_matrix_from_file(f, n, m);
    print_matrix(matrix);
    matrix_into_graph(matrix, g);
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
