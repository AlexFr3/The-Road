/****************************************************************************
 *
 * graph.c -- Implementazione grafo mediante liste di adiacenza
 *
 * Copyright (C) 2021, 2022, 2024 Moreno Marzolla
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

/***
% LabASD - Implementazione del tipo "Grafo"
% Moreno Marzolla <moreno.marzolla@unibo.it>
% Ultimo aggiornamento: 2024-04-16

![Il problema dei [sette ponti di Königsberg](https://en.wikipedia.org/wiki/Seven_Bridges_of_K%C3%B6nigsberg) risolto da [Leonhard Euler](https://en.wikipedia.org/wiki/Leonhard_Euler) nel 1736 ha segnato la nascita della teoria dei grafi](konigsberg.jpg "Il problema dei sette ponti di Königsberg")

Questo file contiene alcune funzioni per gestire la rappresentazione
di grafi mediante liste di adiacenza. È possibile rappresentare sia
grafi orientati che non orientati.

```C
typedef struct Edge {
    int src;
    int dst;
    double weight;
    struct Edge *next;
} Edge;
```

La struttura `Edge` rappresenta un arco del grafo, ossia un nodo delle
liste di adiacenza. Per grafi non orientati, ogni arco $(u,v)$ deve
essere presente due volte: una volta come $(u,v)$ nella lista di
adiacenza di $u$, e una volta come $(v,u)$ nella lista di adiacenza di
$v$. Le liste di adiacenza sono implementate come liste concatenate
semplici; il campo `next` indica l'elemento successivo della lista di
adiacenza, oppure `NULL` se è l'ultimo nodo. Le liste di adiacenza non
sono mantenute ordinate.

```
typedef enum { GRAPH_UNDIRECTED, GRAPH_DIRECTED } Graph_type;

typedef struct {
    int n;
    int m;
    Graph_type t;
    Edge **edges;
    int *in_deg;
    int *out_deg;
} Graph;
```

La struttura `Graph` rappresenta l'intero grafo; la spiegazione dei
vari campi è nel file [graph.h](graph.h). Il campo `n` indica il
numero di nodi, mentre `m` è il numero di archi. Il numero di nodi va
specificato all'atto della creazione del grafo e non può cambiare.  Il
numero di archi è inizialmente zero, e deve essere mantenuto
aggiornato man mano che si inseriscono o cancellano archi.

`edges` è un array di lunghezza $n$ di puntatori: `edges[v]` punta
all'inizio della lista di adiacenza del nodo $v$, oppure `NULL` se $v$
non ha archi uscenti. `in_deg` e `out_deg` sono array di lunghezza
$n$; `in_deg[v]` e `out_deg[v]` sono, rispettivamente, il grado
entrante e il grado uscente di $v$. Nel caso di grafi non orientati,
si deve sempre avere `out_deg[v] == in_deg[v]` per ogni $v$, dato che
ogni arco viene considerato sia entrante che uscente su entrambi gli
estremi. È necessario mantenere l'informazione sui gradi
entranti/uscenti durante l'inserimento e la cancellazione di archi.

La Figura 1 mostra un esempio di rappresentazione di un grafo
orientato, mentre la Figura 2 mostra un esempio di grafo non
orientato; si noti che l'ordine con cui gli archi compaiono nelle
liste di adiacenza non è rilevante, e dipende dall'ordine con cui sono
stati inseriti nel grafo.

![Figura 1: Rappresentazione di un grafo orientato](graph-directed.svg)

![Figura 2: Rappresentazione di un grafo non orientato](graph-undirected.svg)

L'elenco delle funzioni con la descrizione dei parametri e del
comportamento atteso è presente nel file [graph.h](graph.h).

Il file [graph-main.c](graph-main.c) contiene una funzione `main()`
che legge un grafo da file, il cui nome va specificato sulla riga di
comando, e ne stampa il contenuto a video. Per rispondere ai quiz di
autovalutazione sulla piattaforma "Virtuale", è possibile modificare
il programma per stampare il grado entrante/uscente dei nodi con le
funzioni `graph_in_degree()` oppure `graph_out_degree()`.

Per compilare:

        gcc -std=c99 -Wall -Wpedantic graph.c graph-main.c -o graph-main

Per eseguire:

        ./graph-main graph10.in

## Formato di input/output

Le funzioni `graph_read_from_file()` e `graph_write_to_file()` utilizzano
un semplice formato testuale per rappresentare un grafo:

```
n m type
s[0]    d[0]    w[0]
s[1]    d[1]    w[1]
...
s[m-1]  d[m-1]  w[m-1]
```

dove:

- _n_ è il numero di nodi; i nodi sono rappresentati dagli interi $0,
  \ldots, (n-1)$;

- _m_ è il numero di archi;

- _type_ può valere 0 (grafo non orientato), oppure 1 (grafo orientato)

- $s[i], d[i], w[i]$ indicano, rispettivamente, il nodo sorgente, il nodo
  destinazione e il peso dell'arco $i$-esimo. I pesi sono valori
  reali, anche negativi.

## File

- [graph.c](graph.c)
- [graph.h](graph.h)
- [graph-main.c](graph-main.c)
- [graph10.in](graph10.in) ([output atteso](graph10.out); l'ordine con cui compaiono gli archi nelle liste di adiacenza è irrilevante)
- [graph100.in](graph100.in)
- [graph1000.in](graph1000.in)

***/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "graph.h"

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

static Edge *new_edge(int src, int dst, double weight, Edge *next)
{
    Edge *edge = (Edge*)malloc(sizeof(Edge));
    assert(edge != NULL);

    edge->src = src;
    edge->dst = dst;
    edge->weight = weight;
    edge->next = next;
    return edge;
}

/* Inserisce l'arco (src, dst, weight) nel grafo; NON viene inserito
   l'arco opposto nel caso di grafo non orientato (occorre invocare
   due volte questa funzione). Questa funzione ritorna sempre 0; se si
   effettua il controllo sulla presenza di archi duplicati (controllo
   che aumenta il costo asintotico di questa operazione), la funzione
   restituisce true (nonzero) se e solo se l'arco (src, dst) esiste
   già e quindi non viene inserito. */
static int graph_adj_insert(Graph *g, int src, int dst, double weight)
{
#if 0
    const Edge *e;

    /* Se si vuole rendere l'operazione più robusta, controlliamo che
       l'arco non esista già. */
    e = graph_adj(g, src);
    while ((e != NULL) && (e->dst != dst)) {
        e = e->next;
    }
    /* Arrivati qui abbiamo che e == NULL se e solo se l'arco
       non è stato trovato. */
    if (e != NULL) {
        return 1;
    }
#endif
    /* Inseriamo l'arco all'inizio della lista di adiacenza.  Se non
       ci fosse il controllo precedente, l'inserimento di un arco
       richiederebbe tempo O(1) */
    g->edges[src] = new_edge(src, dst, weight, g->edges[src]);
    g->in_deg[dst]++;
    g->out_deg[src]++;
    return 0;
}

void graph_add_edge(Graph *g, int src, int dst, double weight)
{
    int status = 0;

    /*assert(g != NULL);*/

    /*assert((src >= 0) && (src < graph_n_nodes(g)));*/
    /*assert((dst >= 0) && (dst < graph_n_nodes(g)));*/

    status = graph_adj_insert(g, src, dst, weight);
    if (status == 0)
        g->m++;
    else
        fprintf(stderr, "Ho ignorato l'arco duplicato (%d,%d)\n", src, dst);
}

/* Funzione ricorsiva ausiliaria che rimuove l'arco che porta al nodo
   `dst` dalla lista di adiacenza `adj`. Se tale arco non è presente,
   non fa nulla. Ritorna la testa della lista modificata. Al termine
   pone `*deleted = 1` se l'arco è stato rimosso. */
static Edge *graph_adj_remove(Edge *adj, int dst, int *deleted)
{
    /* Questo è l'algoritmo ricorsivo standard per la rimozione da una
       lista concatenata singola. */
    if (adj == NULL) { /* caso base 1: lista vuota */
        *deleted = 0;
        return adj;
    } else if (adj->dst == dst) { /* caso base 2: il nodo da cancellare è il primo della lista. */
        Edge *result = adj->next;
        free(adj);
        *deleted = 1;
        return result;
    } else { /* caso ricorsivo */
        adj->next = graph_adj_remove(adj->next, dst, deleted);
        return adj;
    }
}

int graph_n_nodes(const Matrix *mat)
{
    assert(mat != NULL);

    return mat->n;
}

int graph_n_edges(const Graph *g)
{
    assert(g != NULL);

    return g->m;
}

int graph_out_degree(const Graph *g, int v)
{
    assert(g != NULL);
    assert((v >= 0) && (v < graph_n_nodes(g)));
    return g->out_deg[v];
}

int graph_in_degree(const Graph *g, int v)
{
    assert(g != NULL);
    assert((v >= 0) && (v < graph_n_nodes(g)));
    return g->in_deg[v];
}

Edge *graph_adj(const Graph *g, int v)
{
    assert(g != NULL);
    assert((v >= 0) && (v < graph_n_nodes(g)));

    return g->edges[v];
}

void graph_print(const Graph *g)
{
    int i;

    assert(g != NULL);

    for (i=0; i<g->n; i++) {
        const Edge *e;
        int out_deg = 0; /* ne approfittiamo per controllare la
                            correttezza dei gradi uscenti */
        printf("[%2d] -> ", i);
        for (e = graph_adj(g, i); e != NULL; e = e->next) {
            printf("(%d, %d, %f) -> ", e->src, e->dst, e->weight);
            out_deg++;
        }
        assert(out_deg == graph_out_degree(g, i));
        printf("NULL\n");
    }
}

int create_matrix(n,m)
{
    int i,j;
    int mat[n][m];
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            mat[i][j] = 0;
        }
    }
    return mat;
}
Graph *graph_read_from_file(FILE *f)
{
    int n, m;
    int src, dst;
    int i,j; /* numero archi letti dal file */
    int weight;
    int Cheight;
    Graph *g;
    const int Ccell;
    char c; 
    int mat[ROWS][COLS];
    assert(f != NULL);
    
    if (4 != fscanf(f, "%d \n %d \n %d \n %d \n",&Ccell, &Cheight, &n, &m)) {
        fprintf(stderr, "ERRORE durante la lettura dell'intestazione del grafo\n");
        abort();
    };
   
    
    printf("Ccell = %d\n", Ccell);
    printf("weight = %d \n", Cheight);
    printf("n = %d\n", n);
    printf("m = %d\n", m);
    assert( n > 0 );
    assert( m >= 0 );
    mat=create_matrix(n,m);
    i = 0; 
    j = 0; 
    for (i = 0; i < n; i++) { 
        for (j = 0; j <m; j++) { 
            fscanf(f, "%c", &c); 
            mat[i][j] =(int)c; 
        } /*
        if (j == m-1) { 
            fscanf(f, "%c\n", &c); 
            mat[i][j] = (int)c; 
        } */
    } 
    i = 0;
    j=0;
    for ( i = 0; i < n; i++)
    {
        for ( j = 0; j < m; j++)
        {
            printf(" %d", mat[i][j]);
        }
        printf("\n");
    }
    g = graph_create(n);
    /* Ciclo di lettura degli archi. Per rendere il programma più
       robusto, meglio non fidarsi del valore `m` nell'intestazione
       dell'input. Leggiamo informazioni sugli archi fino a quando ne
       troviamo, e poi controlliamo che il numero di archi letti (i)
       sia uguale a quello dichiarato (m) */
    
    if (i != m) {
        fprintf(stderr, "WARNING 1: ho letto %d archi, ma l'intestazione ne dichiara %d\n", i, m);
    }
    return g;
}

