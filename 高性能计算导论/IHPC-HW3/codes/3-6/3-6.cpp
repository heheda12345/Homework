#include <mpi.h>
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cmath>
/*
 * Modify the "multiply, run" to implement your parallel algorihtm.
 * Compile:
 *      this is a c++ style code
 */
using namespace std;

void serial(int n, double **matrix, double *vector, double **result);
void gen(int n, double ***matrix, double **vector);
void print(int n, double **matrix, double *vector);
void free(int n, double **matrix, double *vector) ;
void run(int n, double **matrix, double *vector);

int main(int argc, char* argv[]) { 
	if (argc < 2) {
		cout << "Usage: "<<argv[0]<<" n" << endl;
		return -1;
	}
	double **matrix;
	double *vector;
	int n = atoi(argv[1]);
	run(n, matrix, vector);
}

void serial(int n, double **matrix, double *vector, double **result){
	/*
	 * It is a serial algorithm to 
	 * get the true value of matrix-vector multiplication
	 * please calculation the difference between true value and the value you obtain
	 *
	 */
	(*result) = new double[n];
	for(int i = 0; i < n; i++) {
		(*result)[i] = 0.0;
	}

	for (int i = 0; i < n; i++) {
		double temp = 0.0;
		for (int j = 0; j < n; j++) {
			temp += matrix[i][j] * vector[j];
		}
		(*result)[i] = temp;
	}
}

void gen(int n, double ***matrix, double **vector) {
     /*
      *  generate random matrix and vector,
      *  In order to debug conveniently, you can define a const matrix and vector
      *  but I will check your answer based on random matrix and vector
      */
	(*matrix) = new double*[n];
	srand((unsigned)time(0));
	for(int i = 0; i < n; i++) {
		(*matrix)[i] = new double[n];
		for(int j = 0; j < n; j++) {
			(*matrix)[i][j] = -1 + rand() * 1.0 / RAND_MAX * 2;
		}
	}
	(*vector) = new double[n];
	for(int i = 0; i < n; i++) {
		(*vector)[i] = -1 + rand() * 1.0 / RAND_MAX * 2;
	}
}

void print(int n, double **matrix, double *vector) {
	for(int i = 0; i < n; i++) {
		cout << vector[i] << endl;
	}
	for(int i = 0; i < n; i++) {
		for(int j = 0; j < n; j++) {
			cout << matrix[i][j] << " ";
		}
		cout << endl;
	}
}

void free(int n, double **matrix, double *vector) {
	delete[] vector;
	for(int i = 0; i < n; i++)
		delete[] matrix[i];
	delete[] matrix;
}

MPI_Comm comm = MPI_COMM_WORLD;
int comm_sz, my_rank, m;
MPI_Comm row_comm, col_comm;
int row_rank, col_rank;
double start_time;
void parallel_scatter(int n, double** matrix, double* vector, double** local_mat, double** local_vec) {
    int m = sqrt(comm_sz);
    int local_n = n/m;
    int local_size = local_n * local_n;
        
    (*local_mat) = new double[local_size];
    if (my_rank == 0) {
        double *buf = new double[n*n];
        for (int x=0; x<m; x++)
            for (int y=0; y<m; y++)
                for (int i=0; i<local_n; i++)
                    memcpy(buf+(x*m+y)*local_size+i*local_n, matrix[x*local_n+i]+y*local_n, sizeof(double)*local_n);
        int *sendcounts = new int[comm_sz];
        int *displs = new int[comm_sz];
        for (int i=0; i<comm_sz; i++)
            sendcounts[i] = local_size;
        for (int i=0; i<comm_sz; i++)
            displs[i] = local_size*i;
        MPI_Scatterv(buf, sendcounts, displs, MPI_DOUBLE, *local_mat, local_size, MPI_DOUBLE, 0, comm);
        delete[] buf;
        delete[] sendcounts;
        delete[] displs;
    } else {
        MPI_Scatterv(NULL, NULL, NULL, MPI_DOUBLE, *local_mat, local_size, MPI_DOUBLE, 0, comm);
    }
    (*local_vec) = new double[local_n];
    if (row_rank == 0)
        MPI_Scatter(my_rank ? NULL : vector, local_n, MPI_DOUBLE, *local_vec, local_n, MPI_DOUBLE, 0, row_comm);
    MPI_Bcast(*local_vec, local_n, MPI_DOUBLE, 0, col_comm);
}

void parallel_solve(int n, double* local_mat, double* local_vec, double** result) {
    int local_n = n/m;
    double *local_ans = new double[local_n];
    memset(local_ans, 0, sizeof(double)*local_n);
    for (int i=0; i<local_n; i++)
        for (int j=0; j<local_n; j++)
            local_ans[i] += local_mat[i*local_n+j]*local_vec[j];
    if (col_rank) {
        MPI_Reduce(local_ans, NULL, local_n, MPI_DOUBLE, MPI_SUM, 0, row_comm);
        goto end;
    }
    // reuse local_vec
    MPI_Reduce(local_ans, local_vec, local_n, MPI_DOUBLE, MPI_SUM, 0, row_comm);
    if (row_rank) {
        MPI_Gather(local_vec, local_n, MPI_DOUBLE, NULL, local_n, MPI_DOUBLE, 0, col_comm);
        goto end;
    }
    (*result) = new double[n];
    MPI_Gather(local_vec, local_n, MPI_DOUBLE, *result, local_n, MPI_DOUBLE, 0, col_comm);
end:
    delete[] local_ans;
}

void parallel_free(double* local_mat, double* local_vec) {
    delete[] local_mat;
    delete[] local_vec;
}

void start_timing() {
    MPI_Barrier(comm);
    start_time = MPI_Wtime();
}

double end_timing() {
    double cur_time = MPI_Wtime();
    double used = cur_time - start_time;
    double reduced = -1;
    MPI_Reduce(&used, &reduced, 1, MPI_DOUBLE, MPI_MAX, 0, comm);
    return reduced; // return -1 if my_rank != 0
}

void output(double* a, int n) {
    for (int i=0; i<n; i++)
        printf("%lf ", a[i]);
    printf("\n\n");
}

void run(int n, double** matrix, double* vector) {
     /*
      * Description: 
      * data partition, communication, calculation based on MPI programming in this function.
      * 
      * 1. call gen() on one process to generate the random matrix and vecotr.
      * 2. distribute the data to other processes.
      * 3. Implement matrix-vector mutiply
      * 4. calculate the diffenence between product vector and the value of serial(), I'll check this answer.
      */
    MPI_Init(NULL, NULL);
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);
    m = sqrt(comm_sz);
    MPI_Comm_split(comm, my_rank%m, my_rank, &col_comm);
    MPI_Comm_rank(col_comm, &row_rank);
    MPI_Barrier(comm);
    MPI_Comm_split(comm, my_rank/m+m, my_rank, &row_comm);
    MPI_Comm_rank(row_comm, &col_rank);
    
    if (my_rank == 0) {
        gen(n, &matrix, &vector);
        printf("gen success!\n");
#ifdef DEBUG
        for (int i=0; i<n; i++) {
            for (int j=0; j<n; j++)
                printf("%lf ", matrix[i][j]);
            printf("\n");
        }
        printf("\n");
        for (int i=0; i<n; i++)
            printf("%lf ", vector[i]);
        printf("\n\n\n");
#endif
    }

    double* ans_serial;
    double* result1;
    double* result2;
    double* local_mat;
    double* local_vec;

    start_timing();
    parallel_scatter(n, matrix, vector, &local_mat, &local_vec);
    parallel_solve(n, local_mat, local_vec, &result1);
    parallel_free(local_mat, local_vec);
    double total_time = end_timing();

    start_timing();
    parallel_scatter(n, matrix, vector, &local_mat, &local_vec);
    double com_time = end_timing();
    start_timing();
    parallel_solve(n, local_mat, local_vec, &result2);
    parallel_free(local_mat, local_vec);
    double calc_time = end_timing();

    if (my_rank != 0) {
        MPI_Finalize();
        return;
    }
    
    double local_start = MPI_Wtime();
    serial(n, matrix, vector, &ans_serial);
    double local_end = MPI_Wtime();
    double serial_time = local_end - local_start;

#ifdef DEBUG
    output(ans_serial, n);
    output(result1, n);
    output(result2, n);
#endif

    double L2 = 0;
    for (int i=0; i<n; i++)
        L2 += (result1[i]-ans_serial[i]) * (result1[i]-ans_serial[i]);
    printf("square of 2-norm: %lf\n", L2);

    printf("%d, %d, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf\n",
        n, comm_sz, serial_time, total_time, com_time, calc_time,
        serial_time/total_time, serial_time/total_time/comm_sz,
        serial_time/calc_time, serial_time/calc_time/comm_sz);

    delete[] ans_serial;
    delete[] result1;
    delete[] result2;
    free(n, matrix, vector);
    MPI_Finalize();
}
