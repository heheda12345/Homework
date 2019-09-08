#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <pthread.h>
#include <assert.h>

pthread_mutex_t mtx;

char message[1010];
bool available;
const int max_thread = 2000;

void* produce(void* rank_) {
    long rank = (long) rank_;
    while (1) {
        pthread_mutex_lock(&mtx);
        if (!available) {
            sprintf(message, "Hello from %lld", rank);
            available = 1;
            pthread_mutex_unlock(&mtx);
            break;
        }
        pthread_mutex_unlock(&mtx);
    }
    printf("[%lld]: Sends hello\n", rank);
    return NULL;
}

void* consume(void* rank_) {
    long rank = (long) rank_;
    char local_msg[110];
    while (1) {
        pthread_mutex_lock(&mtx);
        if (available) {
            strcpy(local_msg, message);
            available = 0;
            pthread_mutex_unlock(&mtx);
            break;
        }
        pthread_mutex_unlock(&mtx);
    }
    printf("[%lld]: %s\n", rank, local_msg);
    return NULL;
}

int main() {
    assert(sizeof(void*) == sizeof(long));
    pthread_t threads[2010];
    int n = max_thread, m = n>>1;
    pthread_mutex_init(&mtx, NULL);

    memset(message, 0, sizeof(message));
    available = 0;
    for (long i=0; i<n; i++)
        if (i&1) {
            pthread_create(&threads[i], NULL, consume, (void*)(i));
        } else {
            pthread_create(&threads[i], NULL, produce, (void*)(i));
        }
    for (int i=0; i<n; i++)
        pthread_join(threads[i], NULL);
    printf("======================================\n");
    
    memset(message, 0, sizeof(message));
    available = 0;
    for (long i=n-1; i>=0; i--)
        if (i&1) {
            pthread_create(&threads[i], NULL, consume, (void*)(i));
        } else {
            pthread_create(&threads[i], NULL, produce, (void*)(i));
        }
    for (int i=0; i<n; i++)
        pthread_join(threads[i], NULL);
    
    return 0;
}