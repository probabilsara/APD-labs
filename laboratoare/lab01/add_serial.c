#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int *arr;
int array_size;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Specificati dimensiunea array-ului\n");
    exit(-1);
  }

  array_size = atoi(argv[1]);

  arr = malloc(array_size * sizeof(int));
  for (int i = 0; i < array_size; i++) {
    arr[i] = i;
  }

  // for (int i = 0; i < array_size; i++) {
  //   printf("%d", arr[i]);
  //   if (i != array_size - 1) {
  //     printf(" ");
  //   } else {
  //     printf("\n");
  //   }
  // }
  struct timespec start, finish; 
  double elapsed; 
  clock_gettime(CLOCK_MONOTONIC, &start); 

  // Vom folosi varianta implementata serial ca referinta (baseline) pentru calculul acceleratiei (speedup-ului)
  for (int i = 0; i < array_size; i++) {
    arr[i] += 100;
  }
  clock_gettime(CLOCK_MONOTONIC, &finish);
  elapsed = (finish.tv_sec - start.tv_sec);
  elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  printf("Timp: %f s\n", elapsed);
  
  // for (int i = 0; i < array_size; i++) {
  //   printf("%d", arr[i]);
  //   if (i != array_size - 1) {
  //     printf(" ");
  //   } else {
  //     printf("\n");
  //   }
  // }

  return 0;
}
