#include <sys/time.h>
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <omp.h>
/*
 * Modify the "multiply, run" to implement your parallel algorihtm.
 * Compile:
 *      this is a c++ style code
 */
using namespace std;

int thread_count;
const double eps = 1e-10;

void serial(int n, double **matrix, double *vector, double **result);
void gen(int n, double ***matrix, double **vector);
void print(int n, double **matrix, double *vector);
void free(int n, double **matrix, double *vector) ;
void run(int n, double **matrix, double *vector);

int main(int argc, char* argv[]) { 
	if(argc < 3) {
        printf("Usage: ./target n thread\n");
        return 0;
    }
    int n = atoi(argv[1]);
    thread_count = min(atoi(argv[2]), n);
	double **matrix;
	double *vector;
	run(n, matrix, vector);
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

void serial(int n, double **matrix, double *vector, double **result){
	/*
	 * It is a serial algorithm to 
	 * get the true value of matrix-vector multiplication
	 * please calculation the difference between true value and the value you obtain
	 *
	 */
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

void parallel(int n, double** matrix, double* vector, double** result) {
    #pragma omp parallel num_threads(thread_count)
    {
        int interval = (n-1)/thread_count+1;
        #pragma omp for
        for (int i=0; i<n; i+=interval) {
            memset((*result)+i, 0, sizeof(int) * std::min(n-i, interval));
        }

        #pragma omp for schedule(dynamic, 48)
        for (int i = 0; i < n; i++) {
            double temp = 0.0;
            for (int j = 0; j < n; j++) {
                temp += matrix[i][j] * vector[j];
            }
            (*result)[i] += temp;
        }

    }
}

void free(int n, double **matrix, double *vector) {
	delete[] vector;
	for(int i = 0; i < n; i++)
		delete[] matrix[i];
	delete[] matrix;
}


void output(double* a, int n) {
    for (int i=0; i<n; i++)
        printf("%lf ", a[i]);
    printf("\n\n");
}

inline double mabs(double a) {
    return a < 0 ? -a : a;
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
    gen(n, &matrix, &vector);
    // printf("gen success!\n");
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
    double* ans_parallel = new double[n];
    double* ans_serial = new double[n];
    printf("n %d thread %d\n", n, thread_count);
    timeval parallel_start, parallel_end;
    gettimeofday(&parallel_start, NULL);
    parallel(n, matrix, vector, &ans_parallel);
    gettimeofday(&parallel_end, NULL);
    printf("parallel %lf\n", (parallel_end.tv_sec - parallel_start.tv_sec) + (1e-6) * (parallel_end.tv_usec - parallel_start.tv_usec));
    
    timeval serial_start, serial_end;
    gettimeofday(&serial_start, NULL);
    serial(n, matrix, vector, &ans_serial);
    gettimeofday(&serial_end, NULL);
    printf("serial %lf\n", (serial_end.tv_sec - serial_start.tv_sec) + (1e-6) * (serial_end.tv_usec - serial_start.tv_usec));

#ifdef DEBUG
    output(ans_serial, n);
    output(ans_parallel, n);
#endif
    
    double L2 = 0;
    timeval L2_start, L2_end;
    gettimeofday(&L2_start, NULL);
    #pragma omp parallel for num_threads(thread_count)
    for (int i=0; i<n; i++)
        if (mabs(ans_parallel[i] - ans_serial[i]) > eps) {
            printf("warning! not equal\n");
            L2 += (ans_parallel[i]-ans_serial[i]) * (ans_parallel[i]-ans_serial[i]);
        }
    gettimeofday(&L2_end, NULL);
    printf("L2 time %lf\n", (L2_end.tv_sec - L2_start.tv_sec) + (1e-6) * (L2_end.tv_usec - L2_start.tv_usec));
    printf("square of 2-norm: %lf\n", L2);

    delete[] ans_serial;
    delete[] ans_parallel;
    free(n, matrix, vector);
}
