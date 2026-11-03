#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// nr core-uri laptopului mue: 16
#define NUM_THREADS 2

void *f(void *arg) {
  printf("Hello World din functia f!\n");
  pthread_exit(NULL);
}

void *g(void *arg) {
  printf("Saluta din functia g!\n");
  pthread_exit(NULL);
}


int main(int argc, char *argv[]) {

  pthread_t threads[NUM_THREADS];
  void *status;


  int r = pthread_create(&threads[0], NULL, f, NULL);
  if (r) {
    printf("Eroare la crearea thread-ului 0");
    exit(-1);
  }

  r = pthread_create(&threads[1], NULL, g, NULL);
  if (r) {
    printf("Eroare la crearea thread-ului 0");
    exit(-1);
  }

    for(int i = 0; i < NUM_THREADS; i++){
      pthread_join(threads[i], &status);
    }

  return 0;
}
