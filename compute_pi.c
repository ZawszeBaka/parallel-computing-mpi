#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	int rank, size;
	MPI_Status status;
	double x,y, Pi, error;
	long long allsum;
	const long long num_iters = 1000000;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	srand((unsigned)time(0));

	if(rank==0){
		printf("\n[INFO] Starting ... \n");
	}
	
	long long sum=0;
	long long i;
	for(i=0; i<num_iters;i++){
		x=(double)rand()/RAND_MAX;
		y=(double)rand()/RAND_MAX;
		if(x*x + y*y<1) sum++;
	}

	// Sum the local sum into the Master's allsum
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(&sum, &allsum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	//printf("\n[INFO] rank %d", rank);

	// Master prints the result
	if(rank==0){
		// calculate Pi, compare to the Pi in math.h
		Pi = (4.0*allsum)/(num_iters * size);
		error = fabs(Pi - M_PI);
		printf("[INFO] Pi from math.h: %.12f\n", M_PI);
		printf("[INFO] Pi after calculating using Mont Carlo with num iterations = %ld, %.12f\n", num_iters, Pi);
		printf("[INFO] Error: %.12f\n", error);
	}
}
