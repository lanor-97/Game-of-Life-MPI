#include "iostream"
#include "stdlib.h"
#include "time.h"
#include "mpi.h"
using namespace std;

//compiled with: mpic++ main.cpp -o main
//executed with: mpirun -np <num_procs> ./main

const int rows = 200;
const int columns = 300;
const int generations = 3000;
double totalTime = 0;

void populate(int**, int);
int neighborCount(int**, int, int[], int, int, int);
bool GoF(int**, int, int[], int[], int, int);
void print(int**, int);

int main(int argc, char** argv)  {
    int myID, procs;    //rank, numero processi
    int size;           //porzione di righe
    
    MPI_Init(&argc, &argv);
    MPI_Status status;
    MPI_Request reqR, reqS;     //serviranno per mpi_wait
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myID);

    if(rows % procs == 0)  {    //se il numero di righe è divisibile per il numero
        size = rows / procs;    //di processi, "taglio" la matrice e ogni processo
    }                           //si prende la sua porzione, altrimenti il programma
    else  {    
    	if(myID == 0)                 //termina
    		cout << "Numero righe non divisibile per numero processi\n";
        MPI_Finalize();
        return 0;
    }

    int generation = 0;     //generazione corrente
    int** matrix = new int* [size];         //allocazione dinamica della matrice
    for(int i = 0; i < size; i++)
        matrix[i] = new int [columns];

    populate(matrix, size);                 //popolo la matrice, così da avere sia celle
                                            //vive che morte
    while(generations > generation)  {

        //PRINT SYSTEM
        //purtroppo a volte stampa le righe in ordine sbagliato
        //stampa su std::out ovviamente
        /*
        MPI_Barrier(MPI_COMM_WORLD);
        if(myID == 0)  {
            print(matrix, size);

            if(procs > 1)
                MPI_Send(&temp, 1, MPI_INT, myID+1, tag, MPI_COMM_WORLD);
        }
        else  {
            MPI_Recv(&temp, 1, MPI_INT, myID-1, tag, MPI_COMM_WORLD, &status);

            //cout << myID << endl;
            print(matrix, size);

            if(myID+1 < procs)
                MPI_Send(&temp, 1, MPI_INT, myID+1, tag, MPI_COMM_WORLD);
            else
               MPI_Send(&temp, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);

            if(myID == procs-1)
                cout << endl;
        }
        */
        //sincronizzo tutti i processi
        MPI_Barrier(MPI_COMM_WORLD);
        if(myID == 0)  {
            int tempRowUP[columns];     //riga temporanea
            int tempRowDOWN[columns];   //riga temporanea
            
            if(procs > 1)  {
                MPI_Irecv(tempRowDOWN, columns, MPI_INT, myID+1, 1, MPI_COMM_WORLD, &reqR);
                MPI_Isend(matrix[size-1], columns, MPI_INT, myID+1, 1, MPI_COMM_WORLD, &reqS);

                MPI_Wait(&reqR, &status);
                MPI_Wait(&reqS, &status);
            }
            double start = MPI_Wtime();
            GoF(matrix, size, tempRowUP, tempRowDOWN, myID, procs);     //funzione che controlla e
            totalTime += (MPI_Wtime() - start);                         //decide chi vive e chi muore
        }
        else if(myID == procs-1 && procs > 1)  {
            int tempRowUP[columns];     //riga temporanea
            int tempRowDOWN[columns];   //riga temporanea
            
            MPI_Isend(matrix[size-1], columns, MPI_INT, myID-1, 1, MPI_COMM_WORLD, &reqS);
            MPI_Irecv(tempRowUP, columns, MPI_INT, myID-1, 1, MPI_COMM_WORLD, &reqR);

            MPI_Wait(&reqR, &status);
            MPI_Wait(&reqS, &status);
            
            GoF(matrix, size, tempRowUP, tempRowDOWN, myID, procs);
        }
        else  {
            int tempRowUP[columns];     //riga temporanea
            int tempRowDOWN[columns];   //riga temporanea

            MPI_Isend(matrix[0], columns, MPI_INT, myID-1, 1, MPI_COMM_WORLD, &reqS);
            MPI_Irecv(tempRowUP, columns, MPI_INT, myID-1, 1, MPI_COMM_WORLD, &reqR);
            MPI_Wait(&reqS, &status);

            MPI_Isend(matrix[size-1], columns, MPI_INT, myID+1, 1, MPI_COMM_WORLD, &reqS);
            MPI_Wait(&reqR, &status);

            MPI_Recv(tempRowDOWN, columns, MPI_INT, myID+1, 1, MPI_COMM_WORLD, &status);
            MPI_Wait(&reqS, &status);

            GoF(matrix, size, tempRowUP, tempRowDOWN, myID, procs);
        }

    	
        generation++;
    }


    for(int i = 0; i < size; i++)   //la matrice è allocata dinamicamente, quindi
        delete matrix[i];           //libero la memoria occupata
    delete[] matrix;

    if(myID == 0)
        cout << "Time: " << totalTime << endl;  //stampo il tempo impiegato

    MPI_Finalize();
    return 0;
}

int neighborCount(int** matrix, int size, int rowUP[], int rowDOWN[], int myID, int i, int j, int procs)  {
    int count = 0;

    for(int k1 = i-1; k1 <= i+1; k1++)  {
        for(int k2 = j-1; k2 <= j+1; k2++)  {
            if(k1 == i && k2 == j)
                continue;

            if(myID == 0 && k1 == size)  {
                if(k2 >= 0 && k2 < columns && rowDOWN[k2] == 1)
                    count++;

                continue;
            }
            else if(myID == procs-1 && k1 == -1)  {
                if(k2 >= 0 && k2 < columns && rowUP[k2] == 1)
                    count++;

                continue;
            }
            else if(myID != 0 && myID != procs-1)  {
                if(k1 == -1)  {
                    if(k2 >= 0 && k2 < columns && rowUP[k2] == 1)
                        count++;

                    continue;
                }
                else if(k1 == size)  {
                    if(k2 >= 0 && k2 < columns && rowDOWN[k2] == 1)
                        count++;

                    continue;
                }
            }

            if(k1 >= 0 && k1 < size && k2 >= 0 && k2 < columns && matrix[k1][k2] == 1)  {
                count++;
            }
        }
    }

    return count;
}

bool GoF(int** matrix, int size, int rowUP[], int rowDOWN[], int myID, int procs)  {
    int support[size][columns];

    for(int i = 0; i < size; i++)  {
        for(int j = 0; j < columns; j++)  {
            int alive = neighborCount(matrix, size, rowUP, rowDOWN, myID, i, j, procs);

            if(matrix[i][j])  {
                if(alive >= 4 || alive <= 1)  {
                    support[i][j] = 0;
                }
                else  {
                    support[i][j] = 1;
                }
            }
            else if(alive == 3)  {
                support[i][j] = 1;
            }
            else  {
                support[i][j] = 0;
            }
        }
    }

    for(int i = 0; i < size; i++)  {
        for(int j = 0; j < columns; j++)  {
            matrix[i][j] = support[i][j];
        }
    }

    return true;
}

void populate(int** matrix, int size)  {
	srand(time(0));
    
	for(int i = 0; i < size; i++)  {
        for(int j = 0; j < columns; j++)  {
            if((rand() % 100) < 40)
                matrix[i][j] = 1;
            else
                matrix[i][j] = 0;
        }
    }
}

void print(int** matrix, int size)  {
    for(int i = 0; i < size; i++)  {
        for(int j = 0; j < columns; j++)  {
            if(matrix[i][j])
                cout << "O ";
            else
                cout << "# ";
        }
        cout << endl;
    }
}