#include <cstdio>
#include <omp.h>
const int n = 100;
const int m = 10;
int a[n];
int main() {
#   pragma omp parallel for num_threads(m)
    for (int i=0; i<n; i++)
        a[i] = i * (i+1) / 2;
    for (int i=0; i<n; i++)
        printf("%d: %d    ", i, a[i]);
    return 0;
}