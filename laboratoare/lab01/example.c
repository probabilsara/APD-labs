#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// nr core-uri laptopului mue: 16
//#define NUM_THREADS 50

void *f(void *arg) {
  long id = *(long *)arg;

  for(int i = 0; i < 100; i++){
    printf("Hello World %d din thread-ul %ld!\n", i, id);
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

  long cores = sysconf(_SC_NPROCESSORS_CONF);
  //printf("Numarul de core-uri de pe masina este: %ld \n", cores);

  const int NUM_THREADS = cores;
  pthread_t threads[NUM_THREADS];
  int r;
  long id;
  void *status;
  long ids[NUM_THREADS];


  for (id = 0; id < NUM_THREADS; id++) {
    ids[id] = id;
    r = pthread_create(&threads[id], NULL, f, &ids[id]);

    if (r) {
      printf("Eroare la crearea thread-ului %ld\n", id);
      exit(-1);
    }
  }

  for (id = 0; id < NUM_THREADS; id++) {
    r = pthread_join(threads[id], &status);

    if (r) {
      printf("Eroare la asteptarea thread-ului %ld\n", id);
      exit(-1);
    }
  }

  return 0;
}
