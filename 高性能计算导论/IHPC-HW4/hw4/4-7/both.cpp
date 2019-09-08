#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <sched.h>
pthread_mutex_t mtx;

char message[2010];
bool available[2010];
bool empty;
const int max_thread = 2000;

void* send_recv(void* rank_) {
    long rank = (long) rank_;
    long dest = (rank+1) % max_thread;
    bool sendSucc = 0, recvSucc = 0;
    char local_msg[110];

    while (1) {
        if (!sendSucc && empty) {
            pthread_mutex_lock(&mtx);
            if (empty) {
                sprintf(message, "Hello to %lld from %lld", dest, rank);
                empty = 0;
                available[dest] = 1;
                sendSucc = 1;
            }
            pthread_mutex_unlock(&mtx);
        }
        if (!recvSucc && available[rank]) {
            pthread_mutex_lock(&mtx);
            strcpy(local_msg, message);
            recvSucc = 1;
            empty = 1;
            pthread_mutex_unlock(&mtx);
            printf("[%lld]: %s\n", rank, local_msg);
        }
        if (sendSucc && recvSucc)
            break;
        pthread_yield();
    }
    return NULL;
}

int main() {
    assert(sizeof(void*) == sizeof(long));
    pthread_t threads[2010];
    int n = max_thread, m = n>>1;
    pthread_mutex_init(&mtx, NULL);

    memset(message, 0, sizeof(message));
    memset(available, 0, sizeof(available));
    empty = 1;

    for (long i=0; i<n; i++)
        pthread_create(&threads[i], NULL, send_recv, (void*)(i));
    for (int i=0; i<n; i++)
        pthread_join(threads[i], NULL);
    return 0;
}