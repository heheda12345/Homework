#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void gen(float*** A, int** b, int n) {
    int i,j;
    (*A) = (float**)malloc(sizeof(float*) * n);
    (*b) = (int*)malloc(sizeof(int) * n);
    for(i = 0; i < n; i++) {
        (*A)[i] = (float*)malloc(sizeof(float) * n);
        for(j = 0; j < n; j++) {
            (*A)[i][j] = -100 + rand() * 1.0 / RAND_MAX * 200;
        }
    }
    for( i = 0 ; i < n; i++) {
        (*b)[i] = rand() % n + 1;
    }
}

void print(float** A, int* b, int n) {
    int i, j;
    for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
            printf("%f\n", A[i][j]);
        }
    }
    for(i = 0; i < n; i++) {
        printf("%d\n", b[i]);
    }
}

void printResult(float* s, int n) {
    int i;
    for(i = 0; i < n; i++) {
        printf("%f\n", s[i]);
    }
}

void freeRes(float** A, int*b, float* s, int n) {
    int i;
    free(s);
    free(b);
    for(i = 0; i < n; i++) {
        free(A[i]);
    }
    free(A);
}
