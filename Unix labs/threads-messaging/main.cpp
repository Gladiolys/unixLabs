#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <pthread.h>

int descriptorArr[2];

void *consumer(void *unusedParam)
{
    while (true) {
        int receiveMessage = 0;
        int receivedMessageSize;

        receivedMessageSize = read (descriptorArr[0], &receiveMessage, sizeof(receiveMessage));

        if (receivedMessageSize != sizeof(receiveMessage)) {
            perror("Consumer: read from pipe error");
            exit(2);
        }

        printf ("Consumer: %d\n",receiveMessage);
    }
}


void *provider(void *unusedParam)
{
    int count = 0;
    int sendedMessageSize;

    while (true) {

        sendedMessageSize = write (descriptorArr[1], &count, sizeof(count));

        if (sendedMessageSize != sizeof(count)) {
            perror ("Provider: write in pipe error");
            exit (3);
        }

        printf ("Provider: %d\n", count);

        count++;

        sleep (1);
    }
}

int main()
{
    pthread_t thread1,thread2;
    int result;

    result = pipe(descriptorArr);

    if (result < 0) {
        perror("Create pipe Error");
        exit(1);
    }

    pthread_create(&thread1, nullptr, consumer, nullptr);
    pthread_create(&thread2, nullptr, provider, nullptr);

    pthread_join(thread1,nullptr);
    pthread_join(thread2,nullptr);
}