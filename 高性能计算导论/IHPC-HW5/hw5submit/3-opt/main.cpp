#include <cstdio>
#include <cstdlib>
#include <omp.h>
#include <cstring>
#include <sys/time.h>
#include <algorithm>
using namespace std;

int thread_count;

void Usage(char* exeName) {
    printf("usage: %s thread_count n\n", exeName);
    exit(1);
}

void generate(int n, int a[]) {
    for (int i=0; i<n; i++)
        a[i] = rand();
#   ifdef DEBUG
    for (int i=0; i<n; i++)
        printf("%d ", a[i]);
    printf("\n");
#   endif
}

inline int getInterval(int *l, int *r, int n, int thread_count, int my_rank) {
    if (my_rank < n % thread_count) {
        int upper = (n-1)/thread_count+1;
        *l = my_rank * upper;
        *r = (my_rank + 1) * upper;
    } else {
        int lower = n/thread_count;
        *l = my_rank * lower + n % thread_count;
        *r = (my_rank + 1) * lower + n % thread_count; 
    }
}

void count_sort(int n, int* cur, timeval* mid1, timeval* mid2) {
    int *pre = new int[n];
    int my_rank;
    bool needCpy = 0;
    int *left = new int[thread_count + 1];
#   pragma omp parallel num_threads(thread_count) \
    default(none) private(my_rank) shared(pre, cur, n, mid1, mid2, left, needCpy, thread_count)
    {
#       pragma omp for
        for (my_rank = 0; my_rank < thread_count; my_rank++) {
            int l, r;
            getInterval(&l, &r, n, thread_count, my_rank);
            //printf("[%d] %d %d\n", my_rank, l, r);
            left[my_rank] = l;
            sort(cur+l, cur+r);
        }
// #       pragma omp single
//         {
//             for (int i=0; i<n; i++)
//                 printf("%d ", cur[i]);
//             printf("\n");
//         }
#       pragma omp single
        left[thread_count] = n;
        for (int pw=0; (1<<pw) < thread_count; pw++) {
#           pragma omp single
            { swap(pre, cur); needCpy ^= 1; }
            
#           pragma omp for
            for (my_rank = 0; my_rank < thread_count; my_rank++) {
                int my_group = my_rank >> pw;
                int nei_group = my_group ^ 1;
                if ((nei_group<<pw) >= thread_count) { //最后一个，而且落单
                    int my_left = left[my_group << pw];
                    memcpy(cur+my_left, pre+my_left, sizeof(int)*(n-my_left));
                } else {
                    int my_left = left[my_group << pw];
                    int nei_left = left[nei_group << pw];
                    int nei_right = left[min((nei_group+1) << pw, thread_count)];
                    // printf("[%d] my_left %d nei (%d, %d)\n", my_rank, my_left, nei_left, nei_right);
                    if (my_group & 1) { //组内后面的，要计入前面相等的
                        int pos = upper_bound(pre + nei_left, pre + nei_right, pre[left[my_rank]]) - pre;
                        int rk = pos + left[my_rank] - my_left;
                        // printf("[%d] %d: pos %d rank %d\n", my_rank, pre[left[my_rank]], pos, rk);
                        cur[rk] = pre[left[my_rank]];
                        for (int i = left[my_rank] + 1; i < left[my_rank+1]; i++) {
                            rk++; //不小于前一个数
                            while (pre[pos] <= pre[i] && pos < nei_right)
                                pos++, rk++;
                            cur[rk] = pre[i];
                            // printf("[%d] %d: pos %d rank %d\n", my_rank, pre[i], pos, rk);
                        }
                    } else { // 组内前面的，不计入后面相等的
                        int pos = lower_bound(pre + nei_left, pre + nei_right, pre[left[my_rank]]) - pre;
                        int rk = pos - nei_left + left[my_rank];
                        // printf("[%d] %d: pos %d rank %d\n", my_rank, pre[left[my_rank]], pos, rk);
                        cur[rk] = pre[left[my_rank]];
                        for (int i = left[my_rank] + 1; i < left[my_rank+1]; i++) {
                            rk++; //不小于前一个数
                            while (pre[pos] < pre[i] && pos < nei_right)
                                pos++, rk++;
                            cur[rk] = pre[i];
                            // printf("[%d] %d: pos %d rank %d\n", my_rank, pre[i], pos, rk);
                        }
                    }
                }
            }
// #           pragma omp single
//             {
//                 printf("=====finish %d====\n", 1<<pw);
//                 for (int i=0; i<n; i++)
//                     printf("%d ", cur[i]);
//                 printf("\n");
//             }
        }
// #       pragma omp single
//         printf("need copy %d\n", needCpy);
        if (needCpy) {
#           pragma omp for
            for (my_rank=0; my_rank<thread_count; my_rank++) {
                memcpy(pre+left[my_rank], cur+left[my_rank], sizeof(int)*(left[my_rank+1] - left[my_rank]));
            }
#           pragma omp single
            {
                swap(pre, cur);
            }
        }

    }
    delete[] pre;
    delete[] left;
}

void output(int n, int a[]) {
    for (int i=0; i<n; i++)
        printf("%d ", a[i]);
    printf("\n");
}

void std_sort(int n, int *a, int* target) {
    memcpy(target, a, sizeof(int)*n);
    sort(target, target+n);
}

bool isCorrect(int n, int* a, int* target) {
    for (int i=0; i<n; i++)
        if (a[i] != target[i])
            return 0;
    return 1;
}

int main(int argc, char* argv[]) {
    if(argc != 3) Usage(argv[0]);
    thread_count = strtol(argv[1], NULL, 10);
    int n = strtol(argv[2], NULL, 10);
    thread_count = min(thread_count, n);
    printf("n=%d thread=%d\n", n, thread_count);
    int *a = new int[n];
    int *target = new int[n];
    generate(n, a);
    std_sort(n, a, target);
    timeval start, mid1, mid2, end;
    gettimeofday(&start, NULL);
    count_sort(n, a, &mid1, &mid2);
    gettimeofday(&end, NULL);
#   ifdef DEBUG
    output(n, a);
    output(n, target);
#   endif
    if (!isCorrect(n, a, target)) {
        printf("WA\n");
        exit(-1);
    }
    // printf("time1: %lf\n", (mid1.tv_sec - start.tv_sec) + (1e-6) * (mid1.tv_usec - start.tv_usec));
    // printf("time2: %lf\n", (mid2.tv_sec - mid1.tv_sec) + (1e-6) * (mid2.tv_usec - mid1.tv_usec));
    // printf("time3: %lf\n", (end.tv_sec - mid2.tv_sec) + (1e-6) * (end.tv_usec - mid2.tv_usec));
    printf("tot: %lf\n", (end.tv_sec - start.tv_sec) + (1e-6) * (end.tv_usec - start.tv_usec));
    delete[] a;
    return 0;
}