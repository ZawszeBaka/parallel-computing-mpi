#include<stdio.h>
#include<mpi.h>

#define NUM_DIMS 100 
#define MASTER_TO_SLAVE_TAG 1 // tag for messages sent from master to slaves
#define SLAVE_TO_MASTER_TAG 10 // tag for messages sent from slaves to master

void makeAB(); // makeks the [A] and [B] matrices
void printArray(); // print the content of output matrix

int rank; // process rank 
int size; // number of processes 
int i, j, k, row, col, rowA, colArowB, colB; // helper variables 
double mat_a[NUM_DIMS][NUM_DIMS]; // input matrix A 
double mat_b[NUM_DIMS][NUM_DIMS]; // input matrix B 
double mat_rs[NUM_DIMS][NUM_DIMS]; // output matrix C
double start_time; 
double end_time;
int low_bound;
int upper_bound;
int portion;
MPI_Status status;
MPI_Request request;

void init_matrices_AB();
void print_matrices();

int main(int argc, char **argv)
{

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if(rank==0){	
		printf("\n[INFO] Number of processes: %d", size);
		printf("\n[INFO] Master rank = %d", 0);
		printf("\n[INFO] Number of slaves take part in calculating matrix multiplication: %d , from %d to %d", size-1, 1, size-1);
	}

	/* master initializes work */
	if(rank==0){
		init_matrices_AB();
		//print_matrices();
		start_time = MPI_Wtime();

		// for each slave other than the master 
		for(i=1; i<size; i++){
			// calculate portion without master 
			portion= (NUM_DIMS / (size - 1)); 
			low_bound = (i-1) * portion;
			if((i+1)==size && (NUM_DIMS % (size-1) != 0 )){
				upper_bound = NUM_DIMS;
			}else{
				upper_bound = low_bound + portion;
			}
			MPI_Isend(&low_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &request);
			MPI_Isend(&upper_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG+1, MPI_COMM_WORLD, &request);
			MPI_Isend(&mat_a[low_bound][0], (upper_bound-low_bound)*NUM_DIMS, MPI_DOUBLE, i, MASTER_TO_SLAVE_TAG+2, MPI_COMM_WORLD, &request);
			MPI_Isend(&mat_b[0][0], NUM_DIMS*NUM_DIMS, MPI_DOUBLE, i, MASTER_TO_SLAVE_TAG+3, MPI_COMM_WORLD, &request);
		}
	}

	//MPI_Bcast(&mat_b, NUM_DIMS*NUM_DIMS,MPI_DOUBLE,0,MPI_COMM_WORLD);

	if(rank>0){
		// receive submatrix of mat_a 
		MPI_Recv(&low_bound, 1, MPI_INT, 0 ,MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&upper_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG+1, MPI_COMM_WORLD, &status);
		MPI_Recv(&mat_a[low_bound][0], (upper_bound-low_bound)*NUM_DIMS, MPI_DOUBLE, 0, MASTER_TO_SLAVE_TAG+2, MPI_COMM_WORLD, &status);
		MPI_Recv(&mat_b[0][0], NUM_DIMS*NUM_DIMS, MPI_DOUBLE, 0, MASTER_TO_SLAVE_TAG+3, MPI_COMM_WORLD, &status);

		for(rowA=low_bound; rowA<upper_bound; rowA++){
			for(colB=0; colB<NUM_DIMS; colB++){
				mat_rs[rowA][colB]=0;
				for(colArowB=0; colArowB<NUM_DIMS; colArowB++){
					mat_rs[rowA][colB] += mat_a[rowA][colArowB]*mat_b[colArowB][colB];
					//printf("\n[DEBUG] mat_a = %.2f, mat_b = %.2f", mat_a[rowA][colArowB], mat_b[colArowB][colB]);
				}
				//printf("\n[DEBUG] %.2f ", mat_rs[rowA][colB]);
			}
		}
		// send back result 
		MPI_Isend(&low_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &request);
		MPI_Isend(&upper_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG+1, MPI_COMM_WORLD, &request);
		MPI_Isend(&mat_rs[low_bound][0], (upper_bound-low_bound)*NUM_DIMS, MPI_DOUBLE, 0, SLAVE_TO_MASTER_TAG+2, MPI_COMM_WORLD, &request);
	}

	if(rank==0){
		for(i=1; i<size; i++){
			MPI_Recv(&low_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &status);
			MPI_Recv(&upper_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG+1, MPI_COMM_WORLD, &status);
			MPI_Recv(&mat_rs[low_bound][0], (upper_bound-low_bound)*NUM_DIMS, MPI_DOUBLE, i, SLAVE_TO_MASTER_TAG+2,MPI_COMM_WORLD, &status);
		}
		end_time=MPI_Wtime();
		print_matrices();
		printf("\n\n[RESULT] Running time = %.10f second(s)\n\n", end_time-start_time);
	}

	MPI_Finalize();

	return 0;
}

void init_matrices_AB()
{
	for(row=0; row<NUM_DIMS; row++){
		for(col=0; col<NUM_DIMS; col++){
			mat_a[row][col] = (double)col + 1.0;
			//printf("\n[DEBUG] %.5f", mat_a[row][col]);
			mat_b[row][col] = (double)col + 1.0;
		}
	}
}

void print_matrices()
{
	printf("\n\n[INFO] Matrix A \n");
	for(row=0; row<NUM_DIMS; row++){
		for(col=0; col<NUM_DIMS; col++){
			printf("%.2f ", mat_a[row][col]);
			if(col==NUM_DIMS-1) printf("\n");
		}
	}

	printf("\n[INFO] Matrix B \n");
	for(row=0; row<NUM_DIMS; row++){
		for(col=0; col<NUM_DIMS; col++){
			printf("%.2f ", mat_b[row][col]);
			if(col==NUM_DIMS-1) printf("\n");
		}
	}

	printf("\n[INFO] Matrix Result \n");
	for(row=0; row<NUM_DIMS; row++){
		for(col=0; col<NUM_DIMS; col++){
			printf("%.2f ", mat_rs[row][col]);
			if(col==NUM_DIMS-1) printf("\n");
		}
	}
	
	printf("\n\n");	


}
