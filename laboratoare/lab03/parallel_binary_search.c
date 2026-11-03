#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define INSIDE -1 // daca numarul cautat este in interiorul intervalului
#define OUTSIDE -2 // daca numarul cautat este in afara intervalului
pthread_barrier_t barrier;

struct my_arg {
	int id;
	int N;
	int P;
	int number;
	int *left;
	int *right;
	int *keep_running;
	int *v;
	int *found;
};

/*
void binary_search() {
	while (keep_running) {
		int mid = left + (right - left) / 2;
		if (left > right) {
			printf("Number not found\n");
			break;
		}

		if (v[mid] == number) {
			keep_running = 0;
			printf("Number found at position %d\n", mid);
		} else if (v[mid] > number) {
			left = mid + 1;
		} else {
			right = mid - 1;
		}
	}
}
*/

void *f(void *arg)
{
	struct my_arg* data = (struct my_arg*) arg;
	int id = data->id;
	int P = data->P;
	int number = data->number;
	int *v = data->v;

	// TODO: implementati parallel binary search
	while (1) {
		int left = *data->left;
		int right = *data->right;
		
		if (!*data->keep_running)
			break;
		if (left > right) {
			if (id == 0) printf("Number not found\n");
			break;
		}

		int size = right - left;
		int start = id * (double) size / P;
		int end;
		if((id + 1) * (double) size / P < size)
			end = (id + 1) * (double) size / P;
		else end = size;

		start += left; // !!!
		end += left;


		if(v[start] == number){
			data->found[id] = start;
		}
		else if(v[end] == number){
			data->found[id] = end;
		}
		else if(v[start] < number && number < v[end]){
			data->found[id] = INSIDE;
		}
		else data->found[id] = OUTSIDE;

		pthread_barrier_wait(&barrier);


		if(id == 0){
			int inside = 0;
			for(int p = 0; p < P; p++){
				if(data->found[p] >= 0){
					printf("Number found at position %d\n", data->found[p]);
					*data->keep_running = 0;
					inside = 1;
					break;
				}

				if(data->found[p] == INSIDE){
					// intervalul thread-ului p
					int s = left + p * (double) size / P;
					int e = left + (p + 1) * (double) size / P;
					if (e > right) e = right;

					*data->left = s + 1;
					*data->right = e - 1;
					inside = 1;
				}
			}
			if(!inside){
				printf("Number not found\n");
				*data->keep_running = 0;
			}
		}
		pthread_barrier_wait(&barrier);
	}

	return NULL;
}

void display_vector(int *v, int size) {
	int i;

	for (i = 0; i < size; i++) {
		printf("%d ", v[i]);
	}

	printf("\n");
}


int main(int argc, char *argv[])
{
	int r, N, P, number, keep_running, left, right;
	int *v;
	int *found;
	void *status;
	pthread_t *threads;
	struct my_arg *arguments;

	if (argc < 4) {
		printf("Usage:\n\t./ex N P number\n");
		return 1;
	}

	N = atoi(argv[1]);
	P = atoi(argv[2]);
	number = atoi(argv[3]);

	keep_running = 1;
	left = 0;
	right = N - 1;

	v = (int*) malloc(N * sizeof(int));
	threads = (pthread_t*) malloc(P * sizeof(pthread_t));
	arguments = (struct my_arg*) malloc(P * sizeof(struct my_arg));
	found = (int*) malloc(P * sizeof(int));

	for (int i = 0; i < N; i++) {
		v[i] = i * 2;
	}

	display_vector(v, N);

	pthread_barrier_init(&barrier, NULL, P);
	for (int i = 0; i < P; i++) {
		arguments[i].id = i;
		arguments[i].N = N;
		arguments[i].P = P;
		arguments[i].number = number;
		arguments[i].left = &left;
		arguments[i].right = &right;
		arguments[i].keep_running = &keep_running;
		arguments[i].v = v;
		arguments[i].found = found;

		r = pthread_create(&threads[i], NULL, f, &arguments[i]);

		if (r) {
			printf("Eroare la crearea thread-ului %d\n", i);
			exit(-1);
		}
	}

	for (int i = 0; i < P; i++) {
		r = pthread_join(threads[i], &status);

		if (r) {
			printf("Eroare la asteptarea thread-ului %d\n", i);
			exit(-1);
		}
	}

	free(v);
	free(threads);
	free(arguments);
	
	free(found);
	pthread_barrier_destroy(&barrier);

	return 0;
}
