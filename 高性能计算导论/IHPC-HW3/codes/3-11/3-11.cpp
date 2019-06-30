#include <cstdio>
#include <algorithm>
#include <mpi.h>
using namespace std;
const int n = 10;
int main() {
    MPI_Init(NULL, NULL);
    int comm_sz, my_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    srand(my_rank+1);
    int a[10];
    for (int i=0; i<n; i++)
        a[i] = rand() % 10;

    for (int i=0; i<comm_sz; i++) {
        if (i == my_rank) {
            printf("proc %d: ", my_rank);
            for (int j=0; j<10; j++)
                printf("%d ", a[j]);
            printf("\n");
            fflush(stdout);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
   
    int local_sum=0, pre_sum;
    for (int i=0; i<n; i++)
        local_sum += a[i];
    MPI_Scan(&local_sum, &pre_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    
    a[0] = pre_sum - local_sum + a[0];
    for (int i=1; i<n; i++)
        a[i] += a[i-1];
    if (my_rank == 0) {
        int* output_buf = new int[comm_sz * n];
        MPI_Gather(a, n, MPI_INT, output_buf, n, MPI_INT, 0, MPI_COMM_WORLD);
        printf("sum\n");
        for (int i=0; i < comm_sz; i++) {
            for (int j=0; j<n; j++)
                printf("%d ", output_buf[i*n+j]);
            printf("\n");
        }
        delete[] output_buf;
    } else {
        MPI_Gather(a, n, MPI_INT, NULL, n, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
