#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// Jose Pablo Fernández Cubillo
// Roberto Vidal Patiño

pthread_mutex_t mutex;
pthread_cond_t  condVar;

int forksUsed[5] = {0,0,0,0,0};

int pickup_forks(int philosopher_number){
  int forkRight;
  int forkLeft = philosopher_number;
  if(philosopher_number == 0){
    forkRight = 4;
  } else {
    forkRight = philosopher_number - 1;
  }

  while(forksUsed[forkLeft] == 1 && forksUsed[forkRight] == 1){
    if(pthread_cond_wait(&condVar, &mutex) != 0){
      perror("Error en cond_wait");
      exit(1);
    }
  }

  if(forksUsed[forkLeft] == 0 && forksUsed[forkRight] == 0){
    forksUsed[forkLeft] = 1;
    forksUsed[forkRight] = 1;
    return 1;
  } else {
    return 0;
  }
}

void return_forks(int philosopher_number){
  int forkRight;
  int forkLeft = philosopher_number;
  
  if(philosopher_number == 0){
    forkRight = 4;
  } else {
    forkRight = philosopher_number - 1;
  }
  forksUsed[forkLeft] = 0;
  forksUsed[forkRight] = 0;

  if(pthread_cond_signal(&condVar) != 0){
    perror("Error en cond_signal\n");
    exit(1);
  }
  
  if(pthread_mutex_unlock(&mutex) != 0){
    perror("Error en mutex_unlock\n");
    exit(1);
  }
}

void *philosopher(void *param){
  int philosopherId = *((int *) param);
  int time;
  while(1){
    time = rand() % 3 + 1;
    printf("Filósofo %d pensando por %ds\n", philosopherId, time);
    sleep(time);
    printf("\tFilósofo %d tiene hambre\n", philosopherId);
    if(pthread_mutex_lock(&mutex) != 0){
      perror("Error en mutex_lock");
      exit(1);
    }
    if(pickup_forks(philosopherId)){
      time = rand() % 3 + 1;
      printf("\t\tFilósofo %d comiendo por %ds\n", philosopherId, time);
      sleep(time);
      printf("\t\tFilósofo %d terminó de comer\n", philosopherId);
      return_forks(philosopherId);
      if(pthread_mutex_unlock(&mutex) != 0){
        perror("Error en mutex_unlock\n");
        exit(1);
      }
    }
    if(pthread_mutex_unlock(&mutex) != 0){
      perror("Error en mutex_unlock\n");
      exit(1);
    }
  }
}

int main(){
  time_t t;
  srand((unsigned) time(&t));
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&condVar,NULL);
  int philosopherIds[5];

  pthread_t threadP[5];
  for(int i = 0; i < 5; i++){
    philosopherIds[i] = i;
    pthread_create(&threadP[i], NULL, philosopher, &philosopherIds[i]);
  }
  for(int i = 0; i < 5; i++){
    pthread_join(threadP[i], NULL);
  }

  return 0;
}