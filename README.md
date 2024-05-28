# Bellman-Ford Pathfinding Program

## Description

This program implements the Bellman-Ford algorithm to find the shortest path in a 2D matrix, considering the movement costs between cells. The costs are determined by the height difference between cells and a fixed movement cost.

## Program Structure

### Definitions and Global Variables

- `Ccell`: Fixed cost to move from one cell to another.
- `Cheight`: Coefficient that multiplies the square of the height difference between cells.
- `totalCost`: Total cost of the found shortest path.

### Functions

#### `int relax(int src, int dst, int* dist, int* parent, int** matrix, int cols)`
This function relaxes an edge between a source cell (`src`) and a destination cell (`dst`), updating the minimum distances and predecessors if it finds a more convenient path.

#### `void BellmanFord(int src, int rows, int cols, int** matrix)`
This function implements the Bellman-Ford algorithm to find the shortest path in a 2D matrix. It initializes the distances and predecessors, and uses the `relax` function to update the path costs. Finally, it calculates the shortest path and total cost, printing the path and cost.

### Main Function

#### `int main(int argc, char *argv[])`
The main function reads input data from a file or standard input (`stdin`), initializes the matrix with the read values, and calls the `BellmanFord` function to find the shortest path. Finally, it frees the memory allocated for the matrix.

## Usage

### Compilation

To compile the program, run the following command:

```bash
gcc -std=c90 -Wall -Wpedantic bellman-ford.c -o bellman-ford
```

### Execution

The program should be executed by passing the name of the input file as an argument.

```bash
./bellman-ford test0.in
```

### Input File Format

The input file must contain the following values, in order:

1. `Ccell`: The fixed cost to move from one cell to another.
2. `Cheight`: The coefficient that multiplies the square of the height difference between cells.
3. `rows`: The number of rows in the matrix.
4. `cols`: The number of columns in the matrix.
5. The values of the matrix, one per cell, arranged in row-major order.
