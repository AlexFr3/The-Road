#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
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
void print_matrix(Graph *g) {
    int i, j;
    assert(g != NULL);

    printf("Matrice letta dal file:\n");
    for (i = 0; i < g->n; i++) {
        for (j = 0; j < g->n; j++) {
            printf("%d ",(int) g->edges[i][j].weight);
        }
        printf("\n");
    }
}

void read_matrix_from_file(FILE *f, Graph *g) {
    int i, j;
    assert(f != NULL && g != NULL);

    /* Allocazione della matrice */
    g->edges = (Edge **)malloc(g->n * sizeof(Edge *));
    for (i = 0; i < g->n; i++) {
        g->edges[i] = (Edge *)malloc(g->n * sizeof(Edge));
        for (j = 0; j < g->n; j++) {
            if (fscanf(f, "%lf", &(g->edges[i][j].weight)) != 1) {
                fprintf(stderr, "Errore durante la lettura della matrice\n");
                abort();
            }
            g->edges[i][j].src = i;
            g->edges[i][j].dst = j;
            g->edges[i][j].next = NULL;
        }
    }
}

Graph *graph_read_from_file(FILE *f)
{
    int n, m;
    /*int src, dst;*/
    int Cheight;
    int Ccell;
    Graph *g;
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

    /* Leggi la matrice dal file */
    read_matrix_from_file(f, g);
    print_matrix(g);
    printf("Ccell = %d\n", Ccell);
    printf("Cheight = %d \n", Cheight);
    printf("n = %d\n", n);
    printf("m = %d\n", m);
    assert( n > 0 );
    assert( m >= 0 );
    return g;
}
int main( int argc, char *argv[] )
{
    Graph *G;
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
    G = graph_read_from_file(filein);/*creo la matrice dal file*/
    if (filein != stdin) fclose(filein);
    return EXIT_SUCCESS;
}