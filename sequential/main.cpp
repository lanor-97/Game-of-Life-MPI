#include "Graphics.h"
#include "stdlib.h"
#include "time.h"
#include "chrono"
#include "ratio"
#include "time.h"

Graphics* G = new Graphics;
Matrix M(G->dim);

const int generazioni = 50;

void populate();
int neighborCount(int, int);
bool GoF();

int main(int argc, char** argv)  {
	bool exit = false;

	if(!G->initializeGraphics())  {
		cerr << "failed to initialize graphics\n";
		return -1;
	}
	
	if(argv[1][0] == 'r')  {
		populate();
		G->print(M);
	}
	else  {
		exit = !G->inputPhase(M);
	}

	int generazione = 0;
    while(!exit && generazioni > generazione)  {
    	GoF();
        G->print(M);
        generazione++;
    }

    delete G;
    return 0;
}

int neighborCount(int i, int j)  {
    int count = 0;

    for(int k1 = i-1; k1 <= i+1; k1++)  {
        for(int k2 = j-1; k2 <= j+1; k2++)  {
            if(k1 == i && k2 == j)
                continue;

            if(k1 >= 0 && k1 < M.dim && k2 >= 0 && k2 < M.dim && M.data[k1][k2])  {
                count++;
            }
        }
    }

    return count;
}

bool GoF()  {
    Matrix support(M.dim);
    bool allDead = true;
    
    for(int i = 0; i < M.dim; i++)  {
        for(int j = 0; j < M.dim; j++)  {
            int alive = neighborCount(i, j);

            if(M.data[i][j])  {
                if(alive >= 4 || alive <= 1)
                    continue;
                else  {
                    support.data[i][j] = true;
                    allDead = false;
                }
            }
            else if(alive == 3)  {
                support.data[i][j] = true;
                allDead = false;
            }
        }
    }

    for(int i = 0; i < M.dim; i++)  {
        for(int j = 0; j < M.dim; j++)  {
            M.data[i][j] = support.data[i][j];
        }
    }

    return !allDead;
}

void populate()  {
	srand(time(0));

	for(int i = 0; i < M.dim; i++)  {
        for(int j = 0; j < M.dim; j++)  {
            M.data[i][j] = (rand() % 100) < 30;
        }
    }
}