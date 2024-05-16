#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <float.h>

typedef struct Edge {
    int src;            /* nodo sorgente        */
    int dst;            /* nodo destinazione    */
    double weight;      /* peso dell'arco       */
    struct Edge *next;
} Edge;
typedef struct {
    int n;              /* numero di nodi               */
    int m;              /* numero di archi              */
    Edge **edges;       /* array di liste di adiacenza  */
    int *in_deg;        /* grado entrante dei nodi      */
    int *out_deg;       /* grado uscente dei nodi       */
} Graph;

#define ROWS 500
#define COLS 500
typedef struct {
    int n;              
    int m;              
    int mat[ROWS][COLS];
} Matrix;
void test_initialize(const Graph *g, int s, double *d, int *p) {
    int i;
    
    /* Verifica della distanza del nodo sorgente*/
    if (d[s] != 0.0) {
        printf("ERRORE: La distanza del nodo sorgente non è stata impostata correttamente.\n");
        return;
    }
    
    /*Verifica delle altre distanze*/
    for (i = 0; i < g->n; i++) {
        if (i != s && d[i] != DBL_MAX) {
            printf("ERRORE: La distanza del nodo %d non è stata impostata correttamente.\n", i);
            return;
        }
    }
    
    /* Verifica dei predecessori*/
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
        d[i] = DBL_MAX; /* Imposto tutte le distanze a "infinito teorico" (valore massimo per gli interi)*/
        p[i] = -1;       /*Imposto tutti predecessori non definiti ovvero -1*/
    }
    d[s] = 0; /* Imposto la distanza del nodo sorgente a 0*/
}

int bellman_ford( const Graph *g, int s, double *d, int *p, const Edge **sp )
{ 
  initialize(g,s,d,p);/*s punto di partenza da 0, G è il grafo*/
  test_initialize(g, s, d, p);
  return 0;

}
void graph_destroy(Graph *g)
{
    int i;
    assert(g != NULL);

    for (i=0; i<g->n; i++) {
        Edge *edge = g->edges[i];
        while (edge != NULL) {
            Edge *next = edge->next;
            free(edge);
            edge = next;
        }
        g->edges[i] = NULL; /* e' superfluo */
    }
    free(g->edges);
    free(g->in_deg);
    free(g->out_deg);
    g->n = 0;
    g->edges = NULL;
    free(g);
}
Graph *graph_create( int n)
{
    int i;
    Graph *g = (Graph*)malloc(sizeof(*g));
    assert(g != NULL);
    assert(n > 0);

    g->n = n;
    g->m = 0;
    g->edges = (Edge**)malloc(n * sizeof(Edge*));
    assert(g->edges != NULL);
    g->in_deg = (int*)malloc(n * sizeof(*(g->in_deg)));
    assert(g->in_deg != NULL);
    g->out_deg = (int*)malloc(n * sizeof(*(g->out_deg)));
    assert(g->out_deg != NULL);
    for (i=0; i<n; i++) {
        g->edges[i] = NULL;
        g->in_deg[i] = g->out_deg[i] = 0;
    }
    return g;
}

void print_matrix(Matrix* mat) {
    int i, j;
    for (i = 0; i < mat->n; i++) {
        for (j = 0; j < mat->m; j++) {
            printf("%d ", mat->mat[i][j]);
        }
        printf("\n");
    }
}

Matrix* read_matrix_from_file(FILE *f, int n, int m) {
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

Graph *graph_read_from_file(FILE *f)
{
    int n, m;
    /*int src, dst;*/
    int Cheight;
    int Ccell;
    Graph *g;
    Matrix *matrix;

    assert(f != NULL);
    if (4 != fscanf(f, "%d \n %d \n %d \n %d \n",&Ccell, &Cheight, &n, &m)) {
        fprintf(stderr, "ERRORE durante la lettura dell'intestazione del grafo\n");
        abort();
    };
    g = (Graph *)malloc(sizeof(Graph));
    g->n = n;
    g->m = m;
    g->in_deg = (int *)malloc(n * sizeof(int));
    g->out_deg = (int *)malloc(n * sizeof(int));

    printf("Ccell = %d\n", Ccell);
    printf("Cheight = %d \n", Cheight);
    printf("n = %d\n", n);
    printf("m = %d\n", m);
    assert( n > 0 );
    assert( m >= 0 );
    matrix=(Matrix *)malloc(sizeof(Matrix));
    matrix->n = n;
    matrix->m = m;  
    matrix=read_matrix_from_file(f,n,m);
    print_matrix(matrix);
    g= graph_create(n);
    return g;
}
int main( int argc, char *argv[] )
{
    Graph *G;
    FILE *filein = stdin;
  const Edge **sp; /* sp[v] è il puntatore all'arco nel grafo che
                        collega v con il suo predecessore nell'albero
                        dei cammini minimi */
    double *d;       /* d[v] è la distanza del nodo v dalla
                        sorgente */
    int *p;          /* p[v] è il predecessore di v lungo il cammino
                        minimo dalla sorgente a v */
    int neg, src = 0;
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
    G = graph_read_from_file(filein);/*creo la matrice dal file*/
     d = (double*)malloc(G->n * sizeof(*d)); assert(d != NULL);
    p = (int*)malloc(G->n  * sizeof(*p)); assert(p != NULL);
    sp = (const Edge**)malloc(G->n  * sizeof(*sp)); assert(sp != NULL);

    neg = bellman_ford(G, src, d, p, sp);
 
    graph_destroy(G);
    if (filein != stdin) fclose(filein);
    return EXIT_SUCCESS;
}