#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define NUM_DIMS 12

int n, rank, size;
int i, j, row, col;
MPI_Status status;
MPI_Comm comm1, comm2;

double mat_a[NUM_DIMS][NUM_DIMS];
double mat_b[NUM_DIMS][NUM_DIMS];
double mat_rs[NUM_DIMS][NUM_DIMS];

void initialize_matrices_AB();
void print_matrices();

int main(int argc, char *argv[])
{

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	// master 
	if(rank==0){
		initialize_matrices_AB();
		

	}


	return 0;
}

void initialize_matrices_AB()
{
	for(int row=0; row<NUM_DIMS; row++){
		for(int col=0; col<NUM_DIMS; col++){
			mat_a[row][col] = col;
			mat_b[row][col] = col;	
		}
	}
}

void print_matrices()
{
	printf("\n\n[INFO] Matrix A:\n");
	for(int row=0; row<NUM_DIMS; row++){
		for(int col=0; col<NUM_DIMS; col++){
			printf("%.2f ",mat_a[row][col]);
			if(col==NUM_DIMS-1) printf("\n");
		}
	}

	printf("\n\n[INFO] Matrix B:\n");
	for(int row=0; row<NUM_DIMS; row++){
		for(int col=0; col<NUM_DIMS; col++){
			printf("%.2f ",mat_b[row][col]);
			if(col==NUM_DIMS-1) printf("\n");
		}
	}

	printf("\n\n[INFO] Matrix Result:\n");
	for(int row=0; row<NUM_DIMS; row++){
		for(int col=0; col<NUM_DIMS; col++){
			printf("%.2f ",mat_rs[row][col]);
			if(col==NUM_DIMS-1) printf("\n");
		}
	}
}
