#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <pthread.h>
#include <assert.h>

const int max_num_thread = 4096;
int tot_thread = 2; // the main thread and the thread forked by main
pthread_mutex_t mtx;


long fib_local(int x) {
    long a = 0, b = 1, c;
    for (int i=2; i<=x; i++) {
        c = b; b = a + b; a = c;
    }
    return b;
}

void* fib_thread(void* x_) {
    long x = (long)x_;
    if (x == 0)
        return (void*)(0);
    if (x == 1)
        return (void*)(1);
    bool new_thread = 0;
    pthread_mutex_lock(&mtx);
    if (tot_thread+2 <= max_num_thread) {
        new_thread = 1;
        tot_thread += 2;
    }
    pthread_mutex_unlock(&mtx);
    long ans;
    if (new_thread) {
        pthread_t thd1, thd2;
        void *ans1, *ans2;
        pthread_create(&thd1, NULL, fib_thread, (void*)(x-1));
        pthread_create(&thd2, NULL, fib_thread, (void*)(x-2));
        pthread_join(thd1, &ans1);
        pthread_join(thd2, &ans2);
        pthread_mutex_lock(&mtx);
        tot_thread -= 2;
        pthread_mutex_unlock(&mtx);
        return (void*)(long(ans1)+long(ans2));
    } else {
        return (void*)(fib_local(x));
    }

}

int main() {
    long n;
    scanf("%lld", &n);
    pthread_t thd;
    pthread_mutex_init(&mtx, NULL);
    pthread_create(&thd, NULL, fib_thread, (void*)(n));
    void* ans;
    pthread_join(thd, &ans);
    printf("%lld\n", long(ans));
    assert(tot_thread == 2);
    return 0;
}
