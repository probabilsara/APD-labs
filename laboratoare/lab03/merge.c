#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

int N;
int P;
int *v;
int *vQSort;
int *vNew;
pthread_barrier_t barrier;

void merge(int *source, int start, int mid, int end, int *destination) {
	int iA = start;
	int iB = mid;
	int i;

	for (i = start; i < end; i++) {
		if (end == iB || (iA < mid && source[iA] <= source[iB])) {
			destination[i] = source[iA];
			iA++;
		} else {
			destination[i] = source[iB];
			iB++;
		}
	}
}

void compare_vectors(int *a, int *b) {
	int i;

	for (i = 0; i < N; i++) {
		if (a[i] != b[i]) {
			printf("Sortare incorecta\n");
			return;
		}
	}

	printf("Sortare corecta\n");
}

void display_vector(int *v) {
	int i;
	int display_width = 2 + log10(N);

	for (i = 0; i < N; i++) {
		printf("%*i", display_width, v[i]);
	}

	printf("\n");
}

int cmp(const void *a, const void *b) {
	int A = *(int*)a;
	int B = *(int*)b;
	return A - B;
}

int is_power_of_two(int n)
{
	if (n == 0) {
		return 0;
	}

	return (ceil(log2(n)) == floor(log2(n)));
}

void get_args(int argc, char **argv)
{
	if(argc < 3) {
		printf("Numar insuficient de parametri: ./merge N P (N trebuie sa fie putere a lui 2)\n");
		exit(1);
	}

	N = atoi(argv[1]);
	if (!is_power_of_two(N)) {
		printf("N trebuie sa fie putere a lui 2\n");
		exit(1);
	}

	P = atoi(argv[2]);
}

void init()
{
	int i;
	v = malloc(sizeof(int) * N);
	vQSort = malloc(sizeof(int) * N);
	vNew = malloc(sizeof(int) * N);

	if (v == NULL || vQSort == NULL || vNew == NULL) {
		printf("Eroare la malloc!");
		exit(1);
	}

	srand(42);

	for (i = 0; i < N; i++)
		v[i] = rand() % N;
}


void print()
{
	printf("v:\n");
	display_vector(v);
	printf("vQSort:\n");
	display_vector(vQSort);
	compare_vectors(v, vQSort);
}

void *thread_function(void *arg)
{
	int thread_id = *(int *)arg;

	// implementati aici merge sort paralel

	// Un merge nu se poate rupe in bucati: UN SINGUR THREAD TB SA L FACA de la cap la coada
	// ---> impart MERGE-urile in THREAD-uri
	// Ex: N = 8, P = 2
	// width = 1 -> 4 merge-uri : [0,2), [2,4), [4,6), [6,8) --> t0 primele 2, t1 ultimele 2
	// width = 2 -> 2 merge-uri : [0,4), [4,8) 
	// width = 4 -> 1 merge: [0,8)

	int width, *aux;
	for (width = 1; width < N; width = 2 * width) {
		int merges = N / (2 * width);
		int start = thread_id * (double) merges / P;
		int end;
		if((thread_id + 1) * (double) merges / P < merges)
			end = (thread_id + 1) * (double) merges / P;
		else end = merges;

		for (int k = start; k < end; k++) {
			int i = k * 2 * width;
			merge(v, i, i + width, i + 2 * width, vNew);
		}
		
		pthread_barrier_wait(&barrier);

		if(thread_id == 0){ // swap-ul tb sa se faca o sg data!
			aux = v;
			v = vNew;
			vNew = aux;
		}
		pthread_barrier_wait(&barrier); // !!
	}

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	get_args(argc, argv);
	init();

	int i;
	int thread_id[P];
	pthread_t tid[P];
	
	// se sorteaza vectorul etalon
	for (i = 0; i < N; i++)
		vQSort[i] = v[i];
	qsort(vQSort, N, sizeof(int), cmp);

	pthread_barrier_init(&barrier, NULL, P);
	// se creeaza thread-urile
	for (i = 0; i < P; i++) {
		thread_id[i] = i;
		pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}

	// se asteapta thread-urile
	for (i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}

	// merge sort clasic - trebuie paralelizat
	// int width, *aux;
	// for (width = 1; width < N; width = 2 * width) {
	// 	for (i = 0; i < N; i = i + 2 * width) {
	// 		merge(v, i, i + width, i + 2 * width, vNew);
	// 	}

	// 	aux = v;
	// 	v = vNew;
	// 	vNew = aux;
	// }

	print();

	free(v);
	free(vQSort);
	free(vNew);
	pthread_barrier_destroy(&barrier);

	return 0;
}
