#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int n=100;
int rank, size;
int i, j, row, col;
MPI_Status status;
MPI_Comm comm1, comm2;

//double mat_a[NUM_DIMS][NUM_DIMS];
//double mat_b[NUM_DIMS][NUM_DIMS];
//double mat_rs[NUM_DIMS][NUM_DIMS];

int *sendA, *sendB, *finalC, *recA, *recC;
int *A, *B, *R, *subA, *subB, *subR;// matrix A, matrix B, submatrix C, matrix result
int tagn=0, tagA=1, tagB=2, tagC=3;
int row_per_process;
char processor_name[MPI_MAX_PROCESSOR_NAME];

int rowA, colArowB, colB;

double time, start, end;

void initialize_matrices_AB();
void print_matrices();

int main(int argc, char *argv[])
{

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(n%(size)!=0 ){ if(rank==0) printf("\n[ERROR] Please use number of processes n that (n) is divisble by %d. For example n = %d. .\n[INFO] Terminating ... \n", n, 20);  }
	else{

	row_per_process=n/(size-1);

	// master 
	if(rank==0){
		A = (int*) malloc(n*n*sizeof(int*));
		B = (int*) malloc(n*n*sizeof(int*));
		R = (int*) malloc(n*n*sizeof(int*));
		initialize_matrices_AB();
		printf("\n[INFO] Process 0 (master) create matrices A, B" );
		//print_matrices();
	}

	// this part is added to start timing, all processors are doing this part
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank==0){ 
		start = MPI_Wtime(); /* only 1 process needs to do this */
		printf("\n[DEBUG] Rank %d , start = %.5f", rank, start);	
	}
	
	// first broadcast the n to all others 
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if(rank==0) printf("\n[INFO] each process takes care of %d row(s) \n", row_per_process);

	if(rank>=0){
		// allocating local data 
		subA = (int*) malloc(row_per_process*n*sizeof(int*));
		//subB = (int*) malloc(n*n*sizeof(int*));
		subR = (int*) malloc(row_per_process*n*sizeof(int*));
	}
	//printf("\n[INFO] process %d create rec buff \n", rank);
	
	// Scatter matrix sendA to all 
	MPI_Scatter(A, row_per_process*n, MPI_INT, subA, row_per_process*n, MPI_INT,0,MPI_COMM_WORLD);

	//for(i=0; i<row_per_process; i++){
	//	printf("\n[DEBUG] process %d A[%d] = % d\n", rank, i, recA[i]);
	//}

	if(rank>0){
		B=(int*) malloc(n*n*sizeof(int*));
	}	
	// Broadcast sendB to all others 
	MPI_Bcast(B, n*n, MPI_INT, 0, MPI_COMM_WORLD);	

	if(rank>=0){
		for(rowA=0; rowA<row_per_process; rowA++){
			for(colB=0; colB<n; colB++){
				subR[rowA*n+colB]=0;
				for(colArowB=0; colArowB<n; colArowB++){
					subR[rowA*n+colB] += subA[rowA*n+colArowB]*B[colArowB*n+colB];
				}
			}
		}
	}

	// Now master (rank=0) gather all result data from all proesses
	MPI_Gather(subR, row_per_process*n, MPI_INT, R, row_per_process*n, MPI_INT, 0, MPI_COMM_WORLD);

	// The last point of calculating the time 
	MPI_Barrier(MPI_COMM_WORLD);
	
	if(rank==0){ 
		end = MPI_Wtime();
		time = end-start;
		print_matrices();
		printf("\n\n[INFO] Running time : %.10f \n", time);
	}
	

	}

	MPI_Finalize();

	return 0;
}

void initialize_matrices_AB()
{
	for(row=0; row<n; row++){
		for(col=0; col<n; col++){
			//mat_a[row][col] = col;
			//mat_b[row][col] = col;
			A[row*n+col]=col+1;
			B[row*n+col]=col+1;
			//printf("\n[DEBUG] %d ", A[row*n+col]);	
		}
	}
}

void print_matrices()
{
	printf("\n\n[INFO] Matrix A:\n");
	for(row=0; row<n; row++){
		for(col=0; col<n; col++){
			printf("%d ",A[row*n+col]);
			if(col==n-1) printf("\n");
		}
	}


	printf("\n\n[INFO] Matrix B:\n");
	for(row=0; row<n; row++){
		for(col=0; col<n; col++){
			printf("%d ",B[row*n+col]);
			if(col==n-1) printf("\n");
		}
	}


	printf("\n\n[INFO] Matrix Result :\n");
	for(row=0; row<n; row++){
		for(col=0; col<n; col++){
			printf("%d ",R[row*n+col]);
			if(col==n-1) printf("\n");
		}
	}


}
