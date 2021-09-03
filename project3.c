#include "buffer.h"
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

sem_t empty;
sem_t full;
pthread_mutex_t mutex;
buffer_item buffer[BUFFER_SIZE] = {0};
int final_pos = -1;

int insert_item(buffer_item item){
    if(sem_wait(&empty)==-1)
        return -1;
    pthread_mutex_lock(&mutex);
    int pos = (final_pos + 1) % BUFFER_SIZE;
    buffer[pos] = item;
    final_pos = pos;
    pthread_mutex_unlock(&mutex);
    if(sem_post(&full)==-1)
        return -1;
    return 0;
}

int remove_item(buffer_item *item){
    if(sem_wait(&full)==-1)
        return -1;
    pthread_mutex_lock(&mutex);
    *item = buffer[final_pos];
    final_pos = (final_pos-1)%BUFFER_SIZE;
    pthread_mutex_unlock(&mutex);
    if(sem_post(&empty)==-1)
        return -1;
    return 0;
}

void *producer(void *param){
    buffer_item item;
    int sleep_time;
    while(1){
        sleep_time = (rand() % 10) + 1;;
        sleep(sleep_time);
        item = rand();
        if(insert_item(item))
            printf("Error on the item insertion\n");
        else
            printf("Producer produced: %d\n", item);
    }
}

void *consumer(void *param){
    buffer_item item;
    int sleep_time;
    while(1){
        sleep_time = (rand() % 10) + 1;
        sleep(sleep_time);
        if(remove_item(&item))
            printf("Error on the consumer\n");
        else
            printf("Consumer consumed: %d\n", item);
    }
}

int main(int argc, char *argv[]){
    int sleep_time, prod_thr, cons_thr;
    if(argc !=4){
        printf("There are required 3 arguments\n");
        return -1;
    }
    srand(time(0));
    sleep_time = atoi(argv[1]);
    prod_thr = atoi(argv[2]);
    cons_thr = atoi(argv[3]);
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if(sem_init(&empty, 0, BUFFER_SIZE))
        return -1;
    if(sem_init(&full, 0, 0))
        return -1;
    pthread_mutex_init(&mutex, NULL);
    for(int i =0; i < prod_thr; i++){
        pthread_create(&tid, &attr, producer, NULL);
    }
    for(int i =0; i < cons_thr; i++){
        pthread_create(&tid, &attr, consumer, NULL);
    }
    sleep(sleep_time);
    return 1;
}

