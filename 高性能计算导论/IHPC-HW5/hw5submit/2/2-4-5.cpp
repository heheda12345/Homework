#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <queue>
#include <limits.h>
#include <iostream>
#include <unistd.h>
pthread_cond_t cv_wait;
pthread_mutex_t mtx_tasks;
pthread_cond_t cv_serial;
pthread_mutex_t mtx_serial;
bool generateEnd;

const int MAX_VAL = 10;
const int MAX_TYPE = 5;
int todo = 0; 

struct Task {
    int type, value, id;
    static Task randomGenerate(int id) {
        return (Task){rand()%MAX_TYPE, rand()%MAX_VAL, id};
    }
};

class ThreadSafeLinkedList {
public:
    ThreadSafeLinkedList() : head(NULL, INT_MIN) {
        pthread_rwlock_init(&rwlock, NULL);
    }
    ~ThreadSafeLinkedList() {
        pthread_rwlock_destroy(&rwlock);
    }
    
    void insert(int value) {
        pthread_rwlock_wrlock(&rwlock);
        Node *cur = &head;
        while (cur->next != NULL && cur->next->value < value) {
            cur = cur->next;
        }
        Node* newNode = new Node(cur->next, value);
        cur->next = newNode;
        pthread_rwlock_unlock(&rwlock);
    }

    bool erase(int value) {
        pthread_rwlock_wrlock(&rwlock);
        Node *pre = &head, *cur = pre->next;
        while (cur != NULL && cur->value < value) {
            pre = pre->next;
            cur = cur->next;
        }
        bool ret;
        if (cur != NULL && cur->value == value) {
            pre->next = cur->next;
            ret = 1;
        } else {
            ret = 0;
        }
        pthread_rwlock_unlock(&rwlock);
        return ret;
    }

    bool exist(int value) {
        pthread_rwlock_rdlock(&rwlock);
        Node* cur = &head;
        bool ret;
        while (cur != NULL && cur->value < value) {
            cur = cur->next;
        }
        ret = cur != NULL && cur->value == value;
        pthread_rwlock_unlock(&rwlock);
        return ret;
    }
    std::string toString () {
        pthread_rwlock_rdlock(&rwlock);
        std::string out = "head";
        for (ThreadSafeLinkedList::Node* cur = head.next; cur != NULL; cur = cur->next) {
            out = out + "->" + std::to_string(cur->value);
        }
        pthread_rwlock_unlock(&rwlock);
        return out;
    }

private:
    struct Node {
        Node* next;
        int value;
        Node(Node* next_, int val) : next(next_), value(val) {}
    };
    pthread_rwlock_t rwlock;
    Node head;
} linkedList;



void Usage(char* exeName) {
    printf("usage: %s thread_count task_count\n", exeName);
    exit(1);
}

std::queue<Task> tasks;

bool getTask(Task* task, int my_rank) {
    bool ret;
    pthread_mutex_lock(&mtx_tasks);
    while (tasks.empty() && !generateEnd) {
        while (pthread_cond_wait(&cv_wait, &mtx_tasks) != 0);
        if (generateEnd) {
            ret = 0;
            goto END;
        }
    }
    if (!tasks.empty()) {
        *task = tasks.front();
        tasks.pop();
        ret = 1;
    } else {
        ret = 0;
    }

END:
    pthread_mutex_unlock(&mtx_tasks);
    return ret;
}

void* work(void* rank) { //TODO
    long my_rank = (long) rank;
    Task task;
    while (getTask(&task, my_rank)) {
        pthread_mutex_lock(&mtx_serial);
        while (1) {
            if (todo == task.id)
               break;
            while (pthread_cond_wait(&cv_serial, &mtx_serial) != 0);
        }
        pthread_mutex_unlock(&mtx_serial);
        switch (task.type) {
            case 0:
                // no break intentially
            case 1: {
                linkedList.insert(task.value);
                printf("[%d]: Successfully insert %d\n", my_rank, task.value);
                break;
            }
            case 2: {
                bool succ = linkedList.erase(task.value);
                if (succ) {
                    printf("[%d]: Successfully delete %d\n", my_rank, task.value);
                } else {
                    printf("[%d]: Delete %d fail\n", my_rank, task.value);
                }
                break;
            }
            case 3: {
                bool exist = linkedList.exist(task.value);
                if (exist) {
                    printf("[%d]: %d is in the linked list\n", my_rank, task.value);
                } else {
                    printf("[%d]: %d is not in the linked list\n", my_rank, task.value);
                }
                break;
            }
            case 4: {
                std::string st = linkedList.toString();
                printf("[%d]: %s\n", my_rank, st.c_str());
                break;
            }
        }
        pthread_mutex_lock(&mtx_serial);
        todo++;
        pthread_mutex_unlock(&mtx_serial);
        pthread_cond_broadcast(&cv_serial);
    }
}


int main(int argc, char* argv[]) {
    if(argc != 3) Usage(argv[0]);
    int thread_count = strtol(argv[1], NULL, 10);
    int n = strtol(argv[2], NULL, 10);
    pthread_t* thread_handles = new pthread_t[thread_count];
    /* Initialize mutexes and conditional variables */
    pthread_cond_init(&cv_wait, NULL);
    pthread_cond_init(&cv_serial, NULL);
    pthread_mutex_init(&mtx_tasks, NULL);
    pthread_mutex_init(&mtx_serial, NULL);
    /* Start threads */
    for (long i = 0; i < thread_count; i++)
        pthread_create(&thread_handles[i], NULL, work, (void*)i);
    /* Generate tasks */
    for (int i = 0; i < n; i++) {
        pthread_mutex_lock(&mtx_tasks);
        tasks.push(Task::randomGenerate(i));
        pthread_mutex_unlock(&mtx_tasks);
        pthread_cond_signal(&cv_wait);
        if (i % 20 == 19)
            sleep(1);
    }
    generateEnd = 1;
    pthread_cond_broadcast(&cv_wait);
    /* Wait for threads to complete */
    for (long i = 0; i < thread_count; i++)
        pthread_join(thread_handles[i], NULL);
    /* Destroy mutex and conditional variables */
    pthread_cond_destroy(&cv_wait);
    pthread_cond_destroy(&cv_serial);
    pthread_mutex_destroy(&mtx_tasks);
    pthread_mutex_destroy(&mtx_serial);
    delete thread_handles;
    return 0;
}  /* main */
