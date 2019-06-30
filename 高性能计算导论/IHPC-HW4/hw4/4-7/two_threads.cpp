#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <pthread.h>

pthread_mutex_t mtx;

std::string message;
bool available;

void* produce(void* rank) {
    pthread_mutex_lock(&mtx);
    message = "hello!";
    available = 1;
    pthread_mutex_unlock(&mtx);
    printf("Producer: Sends hello to consummer\n");
    return NULL;
}

void* consume(void* rank) {
    while (1) {
        pthread_mutex_lock(&mtx);
        if (available) {
            printf("Consumer receives: %s\n", message.c_str());
            pthread_mutex_unlock(&mtx);
            break;
        }
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

int main() {
    pthread_t producer, consumer;
    pthread_mutex_init(&mtx, NULL);
    message = "";
    available = 0;
    pthread_create(&producer, NULL, produce, NULL);
    pthread_create(&consumer, NULL, consume, NULL);
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);
    message = "";
    available = 0;
    pthread_create(&consumer, NULL, consume, NULL);
    pthread_create(&producer, NULL, produce, NULL);
    pthread_join(consumer, NULL);
    pthread_join(producer, NULL);
    return 0;
}