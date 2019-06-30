#include <sys/time.h>
#include <pthread.h>
#include "gen.h"
pthread_mutex_t mtx_schedule;
int n, i, j, thread_count, block_size;
float** A;
float* s;
int* b;
int remain;

inline int min(int x, int y) {
    return x < y ? x : y;
}

inline int max(int x, int y) {
    return x > y ? x : y;
}


inline void solve(int l, int r) {
    for(i = l; i < r; i++) {
        s[i] = 0.0;
        for(j = 0; j < b[i]; j++) {
            s[i] += A[i][j];
        }
        s[i] = s[i] * 1.0 / b[i];
    }
}

void* run_default(void* rank) {
    long my_rank = (long) rank;
    int l, r, i;
    if (my_rank <= n % thread_count) {
        int upper = (n-1)/thread_count+1;
        l = my_rank * upper;
        r = (my_rank + 1) * upper;
    } else {
        int lower = n/thread_count;
        l = my_rank * lower + n % thread_count;
        r = (my_rank + 1) * lower + n % thread_count; 
    }
    solve(l, r);
}

void* run_static(void* rank) {
    long my_rank = (long) rank;
    int der = thread_count * block_size;
    int i;
    for (i=block_size * my_rank; i<n; i+=der) {
        solve(i, min(i+block_size ,n));
    }
}

void dynamic_allocate(int *l, int *r) {
    pthread_mutex_lock(&mtx_schedule);
    if (remain >= n) {
        *l = -1;
    } else {
        *l = remain;
        remain += block_size;
        *r = min(remain, n);
    }
    pthread_mutex_unlock(&mtx_schedule);
}

void* run_dynamic(void* rank) {
    int l, r;
    while (1) {
        dynamic_allocate(&l, &r);
        if (l == -1)
            break;
        solve(l, r);
    }
}

void guided_allocate(int *l, int *r) {
    pthread_mutex_lock(&mtx_schedule);
    if (remain >= n) {
        *l = -1;
    } else {
        *l = remain;
        int alloc = max((n-remain)/thread_count, block_size);
        remain += alloc;
        *r = min(remain, n);
    }
    pthread_mutex_unlock(&mtx_schedule);
}


void* run_guided(void* rank) {
    int l, r;
    while (1) {
        guided_allocate(&l, &r);
        if (l == -1)
            break;
        solve(l, r);
    }
}

inline void compute() {
    pthread_t* thread_handlers;
    long thread;
    thread_handlers = malloc(thread_count * sizeof(pthread_t));
    for (thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handlers[thread], NULL, run_guided, (void*) thread);
    }
    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handlers[thread], NULL);
    free(thread_handlers);
}

int main(int argc, char* argv[])
{
    struct timeval start, end;

    // get problem size n and number of threads from command line
    if(argc < 4) {
        printf("Usage: ./target n size blocksize\n");
        exit(-1);
    }
    n = atoi(argv[1]);
    thread_count = atoi(argv[2]);
    block_size = atoi(argv[3]);
    thread_count = min(thread_count, n);
    printf("%d %d %d\n", n, thread_count, block_size);
    // generating n-by-n martrix A, generating integer vector b of length n, where 0<b(i)<n
    gen(&A, &b, n);
    printf("gen end\n");
    // do the computation of s
    s = (float*)malloc(sizeof(float) * n);
    gettimeofday(&start, NULL);
    compute();
    gettimeofday(&end, NULL);
    printf("time: %lf\n", (end.tv_sec - start.tv_sec) + (1e-6) * (end.tv_usec - start.tv_usec));
#   ifdef DEBUG
    printResult(s,n);
#   endif
    freeRes(A, b, s, n);
    return 0;
}
