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

#define ROWS 500
#define COLS 500
typedef struct {
    int n;              
    int m;              
    int mat[ROWS][COLS];
} Matrix;

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