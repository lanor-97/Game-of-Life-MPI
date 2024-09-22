# 🧬 Game of Life

## Overview
The **Game of Life** is a cellular automaton simulation developed for a university project. It takes inspiration by Conway's Game of Life, simulating the evolution of cells on a grid based on a set of predefined rules. The project contains both a **sequential version with graphical visualization** (using Allegro) and an **MPI parallel version** for more efficient execution on multiple processors.

## How It Works
### General Rules
- The game is played on an **N x M grid** of cells, which can either be **alive** or **dead**.
- Each cell's state updates over discrete time steps, called **generations**:
  - A **live cell** remains alive if it has **2 or 3 live neighbors** in its 3x3 surrounding (excluding itself); otherwise, it dies.
  - A **dead cell** becomes alive if it has **exactly 3 live neighbors**.

## Sequential Version (with Graphics)
The sequential version displays the grid graphically:
- **Alive cells** are colored **yellow**, while **dead cells** are **red**.
- The project includes three source files:
  - `main.cpp`: Contains the core logic of the game.
  - `Graphics.h`: Handles graphical display using Allegro.
  - `Matrix.h`: Provides a utility class for managing the grid's data.

### 🛠️ How to Compile and Run
Make sure you have **Allegro** installed.
```bash
g++ main.cpp -o main -lallegro -lallegro_image
```

Execute the game with random initialization (cells are initialized randomly with a predefined probability):
```bash
./main r
```

Execute the game with manual Initialization (you can then manually select cells using mouse clicks to set up your desired configuration):
```bash
./main any_other_argument
```

## Parallelized Version
The MPI version runs in parallel using multiple processors.
The grid is divided among processes, each handling a section of the grid.

### 🛠️ How to Compile and Run
Ensure that MPI is installed.

```bash
mpic++ main.cpp -o main
```

To execute with a specific number of processes:
```bashì
mpirun -np <number_of_processes> ./main
```

### MPI Implementation Details
The number of rows must be divisible by the number of processes.
Each process handles a portion of the grid, and the boundary rows are shared among neighboring processes.
The main logic involves non-blocking sends and receives, synchronized using MPI_Wait to ensure accurate communication between processes.

### 📊 Performance Analysis
The following table presents the performance analysis of the Game of Life using different numbers of threads (processes), rows, columns, and generations. The "Time (sec)" column shows the execution time in seconds, and the "Speedup" column demonstrates how much faster the program runs compared to the single-threaded (1 process) version.

| **Threads** | **Rows** | **Columns** | **Generations** | **Time (sec)** | **Speedup** |
|-------------|----------|-------------|------------------|----------------|-------------|
| 1           | 200      | 200         | 1000             | 4.52           | 1.00        |
| 2           | 200      | 200         | 1000             | 2.28           | 1.98        |
| 4           | 200      | 200         | 1000             | 1.83           | 2.47        |
| 1           | 200      | 200         | 1500             | 6.90           | 1.00        |
| 2           | 200      | 200         | 1500             | 3.40           | 2.03        |
| 4           | 200      | 200         | 1500             | 2.80           | 2.46        |
| 1           | 200      | 200         | 2000             | 9.10           | 1.00        |
| 2           | 200      | 200         | 2000             | 4.47           | 2.04        |
| 4           | 200      | 200         | 2000             | 3.71           | 2.45        |
| 1           | 300      | 300         | 500              | 5.25           | 1.00        |
| 2           | 300      | 300         | 500              | 2.60           | 2.02        |
| 4           | 300      | 300         | 500              | 2.20           | 2.39        |
| 1           | 300      | 300         | 1000             | 10.20          | 1.00        |
| 2           | 300      | 300         | 1000             | 5.04           | 2.02        |
| 4           | 300      | 300         | 1000             | 4.09           | 2.49        |
| 1           | 300      | 300         | 1500             | 15.00          | 1.00        |
| 2           | 300      | 300         | 1500             | 7.40           | 2.03        |
| 4           | 300      | 300         | 1500             | 6.12           | 2.45        |
| 1           | 300      | 300         | 2000             | 19.80          | 1.00        |
| 2           | 300      | 300         | 2000             | 9.90           | 2.00        |
| 4           | 300      | 300         | 2000             | 8.50           | 2.33        |
| 1           | 300      | 300         | 3000             | 29.80          | 1.00        |
| 2           | 300      | 300         | 3000             | 14.70          | 2.03        |
| 4           | 300      | 300         | 3000             | 12.20          | 2.44        |

#### Observations
- As the number of threads increases, the execution time decreases, demonstrating effective parallelization.
- The speedup values indicate the efficiency gained with multiple processes, though diminishing returns are observed beyond a certain point.
- In some cases, the speedup is higher than expected (greater than 2x with 2 threads) due to better cache utilization. The data is divided more efficiently across the threads, resulting in improved data locality and reduced memory access times. This leads to enhanced performance beyond simple linear scaling.
