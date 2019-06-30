#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <omp.h>

void Get_input(int *thread_num, int* bin_count_p, float* min_meas_p, float* max_meas_p, 
      int* data_count_p) {
    printf("Enter the number of threads\n");
    scanf("%d", thread_num);
    printf("Enter the number of bins\n");
    scanf("%d", bin_count_p);
    printf("Enter the minimum measurement\n");
    scanf("%f", min_meas_p);
    printf("Enter the maximum measurement\n");
    scanf("%f", max_meas_p);
    printf("Enter the number of data\n");
    scanf("%d", data_count_p);
}

int Which_bin(float data, float bin_maxes[], int bin_count, 
      float min_meas) {
    int i;
    for (i=0; i<bin_count-1; i++)
        if (data < bin_maxes[i])
            return i;
    return bin_count-1;
}


int main() {
    int thread_num, bin_count, data_count;
    float min_meas, max_meas;
    Get_input(&thread_num, &bin_count, &min_meas, &max_meas, &data_count);
    float* bin_maxes = new float[bin_count];
    int* bin_counts = new int[bin_count];
    float* data = new float[data_count];

    float bin_width = (max_meas - min_meas) / bin_count;

    // set bins
#   pragma omp parallel for num_threads(thread_num)
    for (int i = 0; i < bin_count; i++) {
        bin_counts[i] = 0;
        bin_maxes[i] = min_meas + (i+1)*bin_width;
    }

    // gen data
    srand(1);
    for (int i = 0; i < data_count; i++)
        data[i] = min_meas + (max_meas - min_meas)*rand()/((double) RAND_MAX);
#     ifdef DEBUG
      printf("Generated data:\n   ");
      for (int i = 0; i < data_count; i++)
         printf("%.3f ", data[i]);
      printf("\n\n");
#     endif
    
    // find bins
    omp_lock_t *lock_bin_counts = new omp_lock_t[bin_count];
#   pragma omp parallel num_threads(thread_num)\
        default(none) shared(lock_bin_counts, data, bin_maxes, bin_count, data_count, min_meas, bin_counts)
    {
#       pragma omp for
        for (int i=0; i<bin_count; i++)
            omp_init_lock(&lock_bin_counts[i]);
#       pragma omp for
        for (int i = 0; i<data_count; i++) {
            int bin = Which_bin(data[i], bin_maxes, bin_count, min_meas);
            omp_set_lock(&lock_bin_counts[bin]);
            bin_counts[bin]++;
            omp_unset_lock(&lock_bin_counts[bin]);
        }
#       pragma omp for
        for (int i=0; i<bin_count; i++)
            omp_destroy_lock(&lock_bin_counts[i]);
    }
    delete[] lock_bin_counts;

    // print histogram
    float bin_max, bin_min;
    for (int i = 0; i < bin_count; i++) {
        bin_max = bin_maxes[i];
        bin_min = (i == 0) ? min_meas: bin_maxes[i-1];
        printf("%.3f-%.3f:\t", bin_min, bin_max);
        for (int j = 0; j < bin_counts[i]; j++)
            printf("X");
        printf("\n");
    }
    
    delete[] bin_maxes;
    delete[] bin_counts;
    delete[] data;
    return 0;
}