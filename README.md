# Bellman-Ford Pathfinding Program

## Descrizione

Questo programma implementa l'algoritmo di Bellman-Ford per trovare il percorso minimo in una matrice 2D, considerando i costi di movimento tra le celle. I costi sono determinati dalla differenza di altezza tra le celle e un costo fisso di movimento.

## Struttura del Programma

### Definizioni e Variabili Globali

- `Ccell`: Costo fisso per spostarsi da una cella all'altra.
- `Cheight`: Coefficiente che moltiplica il quadrato della differenza di altezza tra le celle.
- `totalCost`: Costo totale del percorso minimo trovato.

### Funzioni

#### `int relax(int src, int dst, int* dist, int* parent, int** matrix, int cols)`
Questa funzione rilassa un arco tra una cella sorgente (`src`) e una cella destinazione (`dst`), aggiornando le distanze minime e i predecessori se trova un percorso più conveniente.

#### `void BellmanFord(int src, int rows, int cols, int** matrix)`
Questa funzione implementa l'algoritmo di Bellman-Ford per trovare il percorso minimo in una matrice 2D. Inizializza le distanze e i predecessori, e utilizza la funzione `relax` per aggiornare i costi dei percorsi. Alla fine, calcola il percorso minimo e il costo totale, stampando il percorso e il costo.

### Funzione Principale

#### `int main(int argc, char *argv[])`
La funzione principale legge i dati di input da un file o dallo standard input (`stdin`), inizializza la matrice con i valori letti, e chiama la funzione `BellmanFord` per trovare il percorso minimo. Infine, libera la memoria allocata per la matrice.

## Utilizzo

### Compilazione

Per compilare il programma, eseguire il seguente comando:

```bash
 gcc -std=c90 -Wall -Wpedantic bellman-ford.c -o bellman-ford
```

### Esecuzione

Il programma può essere eseguito passando il nome del file di input come argomento. Se il file di input è "-", il programma leggerà i dati dallo standard input.

```bash
./bellman-ford test0.in
```

### Formato del File di Input

Il file di input deve contenere i seguenti valori, in ordine:

1. `Ccell`: Il costo fisso per spostarsi da una cella all'altra.
2. `Cheight`: Il coefficiente che moltiplica il quadrato della differenza di altezza tra le celle.
3. `rows`: Il numero di righe della matrice.
4. `cols`: Il numero di colonne della matrice.
5. I valori della matrice, uno per cella, disposti in ordine di riga.

