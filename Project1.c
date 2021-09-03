#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t semTa;
sem_t semSt;

pthread_mutex_t mutex;

int studentInSession = 0;
int numberOfChairs = 3;
int studentsWaiting = 0;
int currIndex = 0;
int currentStudent = 0;
int sleeping = 0;
int available = 0;
int chairs[3] = {-1,-1,-1};
int first = 1;

void *student(void *param){
  int studentId = *((int *) param);
  int time;
  while(1){
    if(currentStudent == studentId){
      continue;
    }

    time = rand() % 7+1;

    printf("-Estudiante %d empezó a programar por %ds\n", studentId, time);
    sleep(time);
    printf("\n-Estudiante %d busca por ayuda\n", studentId);

    if(studentsWaiting < numberOfChairs){
      chairs[available] = studentId;
      studentsWaiting++;
      available = (available + 1) % 3;
      printf("-Estudiante %d esperando, estudiantes en silla: %d\n", studentId, studentsWaiting);
      while (studentId != currentStudent && first != 0){}
      first = 1;

      if(sleeping){
        sem_post(&semTa);
      }
    }else{
      printf("-No hay más sillas disponibles para estudiante %d\n", studentId);
    }
  }
}

void *teachingAssistant(void *param){
  while(1){
    if(studentsWaiting > 0){
      if(pthread_mutex_lock(&mutex) != 0){
        perror("Error en mutex_lock");
        exit(1);
      }
      currentStudent = chairs[currIndex];
      studentsWaiting--;
      int time = rand() % 7+1;
      printf("\n+Ayudando al estudiante %d por %ds\n\n", currentStudent, time);
      sleep(time);
      printf("\n+Ayuda al estudiante %d terminada\n\n", currentStudent);
      currIndex = (currIndex + 1) % 3;
      if(pthread_mutex_unlock(&mutex) != 0){
        perror("Error en mutex_unlock\n");
        exit(1);
      }
      
    } else{
      if(!sleeping){
        printf("\n+Asistente se duerme\n\n");
        sleeping = 1;
        sem_wait(&semTa);
      }
    }
  }
}


int main(){
  time_t t;
  srand((unsigned) time(&t));
  int n = 0;
  sem_init(&semTa, 0, 1);
  sem_init(&semSt, 0, 0);
  pthread_mutex_init(&mutex, NULL);
  printf("Ingrese la cantidad de estudiantes: ");
  scanf("%d", &n);
  int studentIds[n];
  pthread_t threadTA;
  pthread_create(&threadTA, NULL, teachingAssistant, NULL);

  pthread_t threadS[n];
  for(int i = 0; i < n; i++){
    studentIds[i] = i+1;
    pthread_create(&threadS[i], NULL, student, &studentIds[i]);
  }
  pthread_join(threadTA, NULL);
  for(int i = 0; i < n; i++){
    pthread_join(threadS[i], NULL);
  }


  return 0;
}