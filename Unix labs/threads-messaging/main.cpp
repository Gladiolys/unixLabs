#include <unistd.h>
#include <cstdio>
#include <pthread.h>

pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int ready = 0;

void *consumer(void *unusedParam)
{
    while (true) {
        pthread_mutex_lock(&lock);

        while (ready == 0) {
            pthread_cond_wait(&cond1, &lock);
            printf("awoke \n");
        }

        ready = 0;
        printf("consumed \n");

        pthread_mutex_unlock(&lock);
    }
}


void *provider(void *unusedParam)
{
    while (true) {
        pthread_mutex_lock(&lock);

        if(ready == 1) {
            pthread_mutex_unlock(&lock);
            continue;
        }

        ready = 1;

        printf("provided \n");
        pthread_cond_signal(&cond1);
        pthread_mutex_unlock(&lock);

        sleep (1);
    }
}

int main()
{
    pthread_t thread1,thread2;

    pthread_create(&thread1, nullptr, consumer, nullptr);
    pthread_create(&thread2, nullptr, provider, nullptr);

    pthread_join(thread1,nullptr);
    pthread_join(thread2,nullptr);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond1);
}