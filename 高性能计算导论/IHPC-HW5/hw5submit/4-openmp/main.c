#include <sys/time.h>
#include "gen.h"
#include "omp.h"

int main(int argc, char* argv[])
{
    int n, i, j, thread_count;
    float** A;
    float* s;
    int* b;
    struct timeval start, end;
    // get problem size n and number of threads from command line
    if(argc < 3) {
        printf("Usage: ./target n size.\n");
        exit(-1);
    }
    n = atoi(argv[1]);
    thread_count = atoi(argv[2]);

    // generating n-by-n martrix A, generating integer vector b of length n, where 0<b(i)<n
    gen(&A, &b, n);

    // do the computation of s
    s = (float*)malloc(sizeof(float) * n);
    gettimeofday(&start, NULL);
#   pragma omp parallel for num_threads(thread_count) \
        default(none) private(i, j) shared(s, A, b, n) schedule(dynamic, 48)
    for(i = 0; i < n; i++) {
        s[i] = 0.0;
        for(j = 0; j < b[i]; j++) {
            s[i] += A[i][j];
        }
        s[i] = s[i] * 1.0 / b[i];
    }
    gettimeofday(&end, NULL);
    printf("time %d %d: %lf\n", n, thread_count, (end.tv_sec - start.tv_sec) + (1e-6) * (end.tv_usec - start.tv_usec));
#   ifdef DEBUG
    printResult(s,n);
#   endif
    freeRes(A, b, s, n);
    return 0;
}
