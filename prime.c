#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

void *prime(void *t);
void *usage(void);
void *sleeper(void *t);
int *element_check(int N);
int *thread_check(int threads);
int *element_smaller_threads(int N, int threads);

/* lock for the counter[tid] and count vairable */
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int *counter;
int *startpoint;
int *endpoint;
int count;
int switcher;

/* usage and help */
void *usage(void)
{
printf("Usage: prime -N <value> -t <value>\n");
printf("Example: prime -N 1000 - t 10\n");
}


/* prime calculation */
void *prime(void *t)
{
	int i, tid;

	tid = (intptr_t)t;

	printf("Worker %d: Starting calculation from ", tid);
	printf("%d to %d\n", startpoint[tid], endpoint[tid]);

for (startpoint[tid]; startpoint[tid] <= endpoint[tid]; startpoint[tid]++) {
	for (i = (startpoint[tid]-1); startpoint[tid]%i; )
		i--;

	if (i == 1) {
		pthread_mutex_lock(&lock);
		counter[tid]++;
		count++;
		pthread_mutex_unlock(&lock);
		}
	}

pthread_exit((void *)t);
return NULL;
}

/* delayed output 0.5 seconds */
void *sleeper(void *t)
{
	while (switcher == 0) {
		usleep(500000);
		printf("-> %d\n", count);
	}
}

/* getopt parameter value checks / failure handling */
int *element_check(int N)
{
	if (N <= 1) {
		printf("\nERROR: N must be >= 2\n\n");
		exit(-1);
	}
}

int *thread_check(int threads)
{
	if (threads <= 0) {
		printf("\nERROR: Threads must be >= 1\n\n");
		exit(-1);
	}
}

int *element_smaller_threads(int N, int threads)
{
	if (N < threads) {
		printf("\nERROR: N must be greater then threads\n\n");
		exit(-1);
	}
}

int main(int argc, char *argv[])
{
	struct timespec start, finish;
	double elapsed;

	/* clock for the runtime output START*/
	clock_gettime(CLOCK_MONOTONIC, &start);

int i;
int j;
int t;
int N;
int slice;
int mswitch;
int threads;
int option;

/* get parameter values using getopt*/
	while ((option = getopt(argc, argv, "N:t:h")) != -1)
		switch (option) {
		case 'N':
			N = atoi(optarg);
			break;
		case 't':
			threads = atoi(optarg);
			break;
		case 'h':
			usage();
			exit(-1);
		default:
		usage();
		exit(-1);
}

mswitch = N/threads;
slice = N%threads;

/* failure handling */
element_check(N);
thread_check(threads);
element_smaller_threads(N, threads);

	/* first line of output */
	printf("Count Primes [2 .. %d] - TaskCnt: %d n", N, threads);
	printf("- Slicesize: %d\n", mswitch);

	/* allocate memory for startpoint, counter and endpoint (slices) */
	startpoint = (int *) malloc(threads * sizeof(int));
	counter = (int *) malloc(threads * sizeof(int));
	endpoint = (int *) malloc(threads * sizeof(int));

/* pthread */
pthread_t thread[threads];
pthread_attr_t attr;
pthread_t p;

	/* determine slices for every thread */
	startpoint[0] = 2;
	for (i = 1; i < threads; i++)
		startpoint[i] = (i * mswitch)+1;

	for (i = 0, j = 1; i < threads; i++, j++)
		endpoint[i] = (j * mswitch);
		endpoint[threads-1] = endpoint[threads-1] + slice;

	for (i = 0; i < threads; i++)
		counter[i] = 0;

	if (pthread_attr_init(&attr)) {
		perror("pthread_attr_init error");
		exit(-1);
	}

	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE)) {
		perror("pthread_setdetach_state");
		exit(-1);
	}

	if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM)) {
		perror("pthread_attr_setscope");
		exit(-1);
	}

	for (t = 0; t < threads; t++) {
		printf("Worker %d: Ready for Service\n", t);

	if (pthread_create(&thread[t], &attr, prime, (void *)(intptr_t)t)) {
			perror("pthread_create");
			exit(-1);
		}
	}

		if (pthread_create(&p, &attr, sleeper, (void *)(intptr_t)t)) {
			perror("pthread_create");
			exit(-1);
	}

	if (pthread_attr_destroy(&attr)) {
		perror("pthread_attr_destroy");
		exit(-1);
	}

	for (t = 0; t < threads; t++) {

		if (pthread_join(thread[t], NULL)) {
			perror("pthread_join");
			exit(-1);
		}
		printf("Worker %d: Found %d primes\n", t, counter[t]);
	}

	switcher = 1;

	if (pthread_join(p, NULL)) {
		perror("pthread_join");
		exit(-1);
}
	/* clock for the runtime output STOP */
	clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

	/* Total Prime Count and runtime output */
	printf("Total Prime Count: %d\n", count);
	printf("RunTime: %ds ", (int)elapsed);
	printf("%.0fns\n\n", (elapsed-(int)elapsed)*1000000000);

	/* free memory space allocated by malloc */
	free(startpoint);
	free(endpoint);
	free(counter);

pthread_exit(NULL);

return 0;
}
