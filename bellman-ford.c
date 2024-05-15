/****************************************************************************
 

Questo esercizio richiede di implementare l'algoritmo di Bellman-Ford
per il calcolo del cammini minimi da singola sorgente in un grafo
orientato pesato. Partendo da un nodo sorgente $s$ dato, l'algoritmo
calcola la distanza $d[v]$ di ogni nodo $v$ raggiungibile da $s$, $v
\in \{0, \ldots, n-1\}$. Se un nodo $v$ non è raggiungibile si pone
$d[v] = +\infty$. Se il grafo contiene un ciclo negativo raggiungibile
dalla sorgente, il programma si deve interrompere stampando un
opportuno messaggio di errore.

L'algoritmo di Bellman-Ford si basa su _passi di rilassamento_. Dato
un arco orientato $(u, v)$ di peso $w$, un passo di rilassamento
consiste nelle operazioni:

```C
if (d[u] + w < d[v]) {
    d[v] = d[u] + w;
    p[v] = u;
}
```

dove `p[v]` indica il predecessore del nodo $v$ lungo il cammino di
costo minimo che va dalla sorgente a $v$; se $v$ non ha predecessore
(ad es., perché è il nodo sorgente o perché non è raggiungibile dalla
sorgente) si pone `p[v] = -1` oppure `p[v] = NODE_UNDEF`. L'array dei
predecessori `p[]` consente di ricostruire i cammini.

L'operazione di rilassamento serve per aggiornare la stima $d[v]$
della distanza del nodo $v$ dalla sorgente. In particolare, se è
possibile raggiungere il nodo $v$ attraversando l'arco $(u,v)$ con un
costo inferiore alla precedente stima $d[v]$, si aggiorna $d[v]$. Il
costo per raggiungere $v$ attraverso l'argo $(u,v)$ di peso $w$ è
$d[u] + w$. Inizialmente tutte le distanze si assumono $+\infty$, ad
esclusione della sorgente $s$ per cui si ha `d[s] = 0`.

Per rappresentare $+\infty$ si può usare il simbolo `HUGE_VAL` che ha
un valore di tipo `float` o `double` definito in `<math.h>`[^1].
Questo simbolo è parte dello standard C90, per cui è presente in tutti
i compilatori conformi.

[^1]: In realtà la documentazione afferma che

      > The macros `HUGE_VAL`, `HUGE_VALF`, `HUGE_VALL` expand to
      > constants of types `double`, `float` and `long double`,
      > respectively, that represent a large positive value, possibly
      > positive infinity.

      Il simbolo `INFINITY` è l'unico che garantisce di avere il
      "vero" infinito su processori che supportano lo standard
      [IEEE754](https://en.wikipedia.org/wiki/IEEE_754), ma purtroppo
      è disponibile solo da C99 in poi. `INFINITY` non è semplicemente
      "un valore grande": lo standard IEEE754 impone che se $x$ è un
      qualunque valore positivo finito, allora
      `INFINITY + x == INFINITY`, mentre `HUGE_VAL + x` _potrebbe_ causare overflow
      perché `HUGE_VAL` _potrebbe_ essere definito come un valore
      "grande" ma non infinito. I compilatori che ho a disposizione
      _sembrano_ definire `HUGE_VAL` come `INFINITY`, e quindi
      producono il risultato corretto.

Questo programma memorizza l'albero dei cammini minimi in due modi:
come array di predecessori `p[v]` di lunghezza $n$, e in un array di
puntatori ad archi `sp[]` sempre di lunghezza $n$. Specificamente,
`p[v]` è un intero e rappresenta l'identificativo (indice) del nodo
che precede $v$ lungo il cammino minimo dalla sorgente a $v$; se $v$
non è raggiungibile dalla sorgente, oppure coincide con la sorgente e
quindi non ha predecessore, si pone `p[v] = -1` oppure `p[v] =
NODE_UNDEF` (il simbolo `NODE_UNDEF` vale -1 ed è definito all'inizio
del programma). `sp[v]` è un puntatore all'arco entrante nel nodo $v$
lungo il cammino minimo dalla sorgente a $v$; se $v$ non è
raggiungibile dalla sorgente, oppure coincide con la sorgente, si pone
`sp[v] = NULL`; si faccia riferimento alla Figura 1.

![Figura 1: Esempio di grafo orientato pesato con l'albero dei cammini
minimi usando il nodo 0 come sorgente. `sp[v]` punta all'arco in
ingresso verso il nodo `v`, se `v` è raggiungibile dalla
sorgente.](shortest-paths.svg)

Poiché l'array `sp[]` non viene utilizzato in questo programma,
suggerisco di iniziare lasciandolo indefinito.

Il programma riceve da uno a tre parametri sulla riga di comando:

- il primo parametro è il nome del file di input; il formato
  dell'input è quello utilizzato dal tipo [grafo](graph.html);

- il secondo parametro è un intero che indica il nodo sorgente (se
  manca, si assume 0 come sorgente);

- il terzo parametro è un intero che indica il nodo destinazione. Si
  noti che il programma calcola sempre le distanze di *tutti* i nodi
  dalla sorgente, per cui il nodo destinazione influenza solo ciò che
  viene stampato a video ma non il costo asintotico dell'algoritmo. Se
  la destinazione non viene specificata, stampa le distanze e i
  cammini minimi verso tutti i nodi del grafo.

Per compilare:

        gcc -std=c90 -Wall -Wpedantic graph.c bellman-ford.c -o bellman-ford

Per stampare le distanze e i cammini minimi dal nodo 0:

        ./bellman-ford graph10.in 0

su Windows:

        .\bellman-ford graph10.in 0

Per stampare la distanza e il cammino minimo dal nodo 3 al nodo 4:

        ./bellman-ford graph10.in 3 4

su Windows:

        .\bellman-ford graph10.in 3 4


## Suggerimento per una possibile ottimizzazione

Nella versione base, l'algoritmo di Bellman-Ford effettua $(n-1)$ fasi
di rilassamento; in ogni fase si esaminano tutti gli archi del grafo,
per cui il costo asintotico è $\Theta(nm)$.

È possibile adottare una semplice ottimizzazione: se al termine di una
fase di rilassamento le distanze dei nodi non cambiano, allora
l'algoritmo può terminare immediatamente perché i rilassamenti
successivi non modificheranno le distanze. Questo comportamento si può
realizzare con una modifica alla versione base dell'algoritmo vista a
lezione. Si noti che la funzione `relax()` proposta in questo
programma restituisce 1 se la distanza del nodo destinazione viene
modificata, 0 altrimenti. Ciò è stato fatto proprio per consentire
l'ottimizzazione di cui stiamo parlando.

```C
int i, v, updated;

\/\* Inizializzazione non riportata... \*\/

for (updated=1, i=0; i<n && updated; i++) {
  updated = 0;
  for (v=0; v<n; v++) {
    const Edge *e;
    for (e = graph_adj(g, v); e != NULL; e = e->next) {
      updated |= relax(e, d, p, sp);
    }
  }
}

\/\*
  Se updated == 1 -> ci sono cicli negativi. perché sono uscito dal
                     ciclo perché i>=n, quindi dopo le (n-1) iterazioni
                     max di Bellman-Ford e l'iterazione extra ci sono
                     state ancora modifiche nelle distanze.

  Se updated == 0 -> non ci sono cicli negativi perché nell'ultima
                     fase di rilassamento non ci sono state modifiche
                     nelle distanze, e da quel punto in poi ulteriori
                     fasi di rilassamento non avrebbero prodotto modifiche.
\*\/
```

Il codice sopra effettua un passo di rilassamento in più rispetto a
quanto mostrato a lezione. Questo è voluto, e serve per verificare se
ci sono cicli negativi: se alla fine si ha `updated == 1` significa
che dopo le $n$ iterazioni (una in più del massimo necessario) ci sono
distanze che continuano a diminuire, e questo denota la presenza di
cicli negativi.

Con questa ottimizzazione è possibile gestire grafi di dimensioni
maggiori; quelli nella Tabella 1 rappresentano porzioni delle reti
stradali di alcuni stati americani e della città di Roma. Viste le
dimensioni degli input, conviene disattivare la stampa dei cammini
minimi (commentare la chiamata alla funzione `print_path()` dentro
`print_dist()`).

:Tabella 1: Alcuni input di medio/grandi dimensioni

Dataset                     Nodi ($n$)   Archi ($m$)
------------------------- ------------ -------------
[Nevada](graph-NV.in)           261155        618175
[Maine](graph-ME.in)            194505        425708
[Vermont](graph-VT.in)           97975        212979
[Delaware](graph-DE.in)          49109        119744
[Roma](graph-rome.in)             3353          8859
------------------------- ------------ -------------

I dati in tabella sono relativi al [9th DIMACS implementation
challenge--Shortest
Paths](https://www.diag.uniroma1.it/challenge9/download.shtml), una
competizione internazionale in cui sono state confrontate
implementazioni efficienti di alcuni algoritmi su grafi. Si presti
attenzione al fatto che i grafi non sono necessariamente fortemente
connessi, quindi non tutti i nodi sono raggiungibili da tutti gli
altri.


## Attenzione ad un bug di alcuni compilatori

La procedura di rilassamento può causare problemi su alcune
combinazioni di processori/compilatori. Consideriamo un arco $(u, v)$
di peso $w$; ci aspettiamo che l'asserzione seguente sia sempre vera:

```C
if (d[u] + w < d[v]) {
   d[v] = d[u] + w;
   assert(d[v] == d[u] + w);
}
```

Purtroppo, in certi casi può risultare falsa! Questo implica che
l'algoritmo non converga mai perché certe distanze non diminuiscono.

Ho riscontrato il problema su un processore Intel Atom con `gcc`
7.x/8.x oppure con `tcc` (un altro compilatore per sistemi Unix), ma
non con `clang` (che è il compilatore usato anche da MacOSX).
Curiosamente, sul processore Atom non funziona nemmeno una
implementazione Pascal dell'algoritmo, il che fa sospettare che il
problema sia legato alla CPU. L'errore si è manifestato anche durante
il laboratorio su un portatile dotato di processore Intel i7 usando
una vecchia versione di `gcc` (4.x).

Il problema *non* si manifesta con processori recenti, usando versioni
recenti di `gcc` (ho provato: Xeon, i7, e ARMv7l).

Riassumendo:

Processore           Compilatore      Funziona?
-------------------  ---------------- ------------
Intel Atom N750      gcc 7.x/8.x         No
Intel Atom N750      tcc                 No
Intel Atom N750      Free Pascal         No
Intel Atom N750      clang               Sì
Intel i7             gcc 4.x (Win)       No
Intel i7             gcc 7.x             Sì
Intel i7             tcc                 Sì
Intel i7             clang               Sì
Intel i7             Free Pascal         Sì
ARMv7l               gcc 7.x             Sì
ARMv7l               tcc                 Sì
ARMv7l               clang               Sì
ARMv7l               Free Pascal         Sì
-------------------  ---------------- ------------

Il problema sembra quello descritto in
<https://gcc.gnu.org/bugzilla/show_bug.cgi?id=323> e deriva dal fatto
che il coprocessore matematico 8087 non era totalmente conforme allo
standard IEEE 754; in particolare, i registri in virgola mobile
avevano (e hanno tutt'ora) una ampiezza maggiore di 32/64 bit, per cui
la somma viene calcolata con un numero maggiore di cifre ma viene
salvata in memoria con meno (_excess precision_).

Possibili soluzioni:

- Se si usa `gcc` è possibile forzare l'uso delle istruzioni SSE
  compilando con i flag `-msse2 -mfpmath=sse`.  In base a quanto si
  legge in <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=323#c109>, le
  istruzioni SSE sono conformi IEEE 754 e il risultato dovrebbe essere
  corretto. Sul processore Atom, il compilatore `clang` (che produce
  codice funzionante) sembra generare istruzioni SIMD per il calcolo
  in virgola mobile, che non vengono usate di default da `gcc`.

- È possibile riscrivere la procedura di rilassamento introducendo una
  variabile temporanea:
  ```C
  const double dnew = d[u] + w;
  if (dnew < d[v]) {
    d[v] = dnew;
  }
  ```

Nei processori recenti e usando una versione recente di `gcc` non
dovrebbe servire nessuna delle "pezze" di cui sopra.
***/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include "graph.h"

/* Si può usare il simbolo NODE_UNDEF per indicare che il predecessore
   della lista dei padri non esiste. */
const int NODE_UNDEF = -1;

/* Esegue un passo di rilassamento sull'arco e. Restituisce 1 se la
   distanza viene modificata, 0 altrimenti. Aggiorna l'array dei
   predecessori `p[]`, delle distanze `d[]` e degli archi `s[]` se
   necessario. */
int relax( const Edge *e, double *d, int *p, const Edge **sp )
{
    const int u = e->src;
    const int v = e->dst;
    const double w = e->weight;
    /* TODO: eseguire un passo di rilassamento relativamente all'arco
       (u, v) di peso w. */
    return 0;
}

/* Calcola l'albero dei cammini minimi dalla sorgente `s` usando
   l'algoritmo di Bellman-Ford. Restituisce true (nonzero) se e solo
   se ci sono cicli negativi.

   Parametri:

   - `g`: grafo con n nodi

   - `s`: nodo sorgente (0 <= s < n)

   - `d[v]`: distanza di `v` dalla sorgente; `d[]` è un array di `n`
     elementi allocato dal chiamante e viene riempito da questa
     funzione

   - `p[v]`: predecessore di `v` lungo il cammino minimo dalla
     sorgente a `v`; `p[]` è un array di `n` elementi allocato dal
     chiamante e viene riempito da questa funzione. Se un nodo
     `v` non ha predecessore, ad esempio perché è il nodo sorgente
     o perché non è raggiungibile dalla sorgente, si deve porre
     `p[v] = -1` oppure `p[v] = NODE_UNDEF`

   - `sp[v]` punta all'arco in ingresso a `v` lungo il cammino dalla
     sorgente a `v`; `sp[]` è un array di `n` puntatori allocato dal
     chiamante e viene riempito da questa funzione

   Risultato:

   - true (non-zero) se ci sono cicli negativi, 0 altrimenti.
*/
void inizialize(const Graph *g, int s){
  
}

int bellman_ford( const Graph *g, int s, double *d, int *p, const Edge **sp )
{ 
  inizialize(g,s);/*s punto di partenza da 0, G è il grafo*/
}

/* Stampa l'elenco degli archi dell'albero dei cammini minimi nello
   stesso formato usato per il grafo di input. Questa funzione non
   viene usata in questo programma. */
void print_sp( const Graph *g, const Edge **sp )
{
    const int n = graph_n_nodes(g);
    int v;
    int nsp = 0; /* numero di archi che fanno parte dell'albero dei cammini minimi */

    /* Contiamo gli archi che fanno parte dell'albero dei cammini minimi */
    for (v=0; v<n; v++) {
        if (sp[v] != NULL) nsp++;
    }

    printf("%d %d 1\n", n, nsp);
    for (v=0; v<n; v++) {
        const Edge *e = sp[v];
        if (e != NULL) {
            printf("%d %d %f\n", e->src, e->dst, e->weight);
        }
    }
}

/* Questa funzione deve stampare la sequenza di nodi del cammino
   minimo da src a dst, se esiste, oppure "Non raggiungibile" se la
   destinazione non è raggiungibile.  Il formato deve essere:

   src->v0->v1->...->dst

   dove src, vi, dst sono interi che rappresentano i nodi del
   cammino minimo. Non bisogna andare a capo alla fine della
   stampa.

   L'array p[] è l'array dei predecessori, calcolato dall'algoritmo
   di Dijkstra.

   Suggerimento: suggerisco una implementazione ricorsiva, che è molto
   semplice. */
void print_path(const int *p, int src, int dst)
{
    printf("TODO");
    /* [TODO] */
}

/* Stampa la distanza e il cammino minimo da src a dst; se dst < 0,
   stampa le distanze e i cammini minimi da src a tutti i nodi del
   grafo. */
void print_dist( const Graph *g, int src, int dst, const int *p, const double *d )
{
    const int n = graph_n_nodes(g);
    int v, from_idx = 0, to_idx = n-1;

    assert(p != NULL);
    assert(d != NULL);

    if (dst >= 0) {
        from_idx = to_idx = dst;
    }

    printf(" src  dst            d path\n");
    printf("---- ---- ------------ -------------------------\n");
    for (v=from_idx; v<=to_idx; v++) {
        printf("%4d %4d %12.4f ", src, v, d[v]);
        print_path(p, src, v);
        printf("\n");
    }
}

int main( int argc, char *argv[] )
{
    Graph *G;
    const Edge **sp; /* sp[v] è il puntatore all'arco nel grafo che
                        collega v con il suo predecessore nell'albero
                        dei cammini minimi */
    double *d;       /* d[v] è la distanza del nodo v dalla
                        sorgente */
    int *p;          /* p[v] è il predecessore di v lungo il cammino
                        minimo dalla sorgente a v */
    FILE *filein = stdin;
    int n, neg, src = 0, dst = NODE_UNDEF;

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

    /*if (argc > 2)
        src = atoi(argv[2]);

    if (argc > 3)
        dst = atoi(argv[3]);*/
    G = graph_read_from_file(filein);/*creo la matrice dal file*/
    
    d = (double*)malloc(n * sizeof(*d)); assert(d != NULL);
    p = (int*)malloc(n * sizeof(*p)); assert(p != NULL);
    sp = (const Edge**)malloc(n * sizeof(*sp)); assert(sp != NULL);

    neg = bellman_ford(G, src, d, p, sp);
    if (neg) {
        printf("Ci sono cicli negativi\n");
    } else {
        print_dist(G, src, dst, p, d);
    }
    graph_destroy(G);

    free(d);
    free(p);
    free(sp);
    if (filein != stdin) fclose(filein);

    return EXIT_SUCCESS;
}
