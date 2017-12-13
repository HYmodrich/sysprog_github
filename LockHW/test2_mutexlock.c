#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long g_count = 0;
pthread_mutex_t lock;

void *thread_func(void *arg)
{
	long i, j, k, count = (long)arg;
	long long l;
	long thread = pthread_self();

	for (i = 0; i<count; i++) {
		pthread_mutex_lock(&lock);
		/********************** Critical Section **********************/

		/*
		 * The purpose of this code is to occupy cpu for long time.
		 */
		for (j = 0; j<100000; j++)
			for (k = 0; k<3000; k++)
				l += j * k;
		g_count++;
		//printf("%ld : %ld\n",thread,g_count);
		pthread_mutex_unlock(&lock);
/*
		for (j = 0; j<100000; j++)
			for (k = 0; k<1000; k++)
				l += j * k;
*/
		/**************************************************************/
	}
}

int main(int argc, char *argv[])
{
	pthread_t *tid;
	long i, thread_count, value;
	int rc;
	pthread_mutex_init(&lock,NULL);


	/*
	 * Check that the program has three arguments
	 * If the number of arguments is not 3, terminate the process.
	 */
	if (3 != argc) {
		fprintf(stderr, "usage: %s <thread count> <value>\n", argv[0]);
		exit(0);
	}

	/*
	 * Get the values of the each arguments as a long type.
	 * It is better that use long type instead of int type,
	 * because sizeof(long) is same with sizeof(void*).
	 * It will be better when we pass argument to thread
	 * that will be created by this thread.
	 */
	thread_count = atol(argv[1]);
	value = atol(argv[2]);

	/*
	 * Create array to get tids of each threads that will
	 * be created by this thread.
	 */
	tid = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
	if (!tid) {
		fprintf(stderr, "malloc() error\n");
		exit(0);
	}

	/*
	 * Create a threads by the thread_count value received as
	 * an argument. Each threads will increase g_count for
	 * value times.
	 */
	for (i = 0; i<thread_count; i++) {
		rc = pthread_create(&tid[i], NULL, thread_func, (void*)value);
		if (rc) {
			fprintf(stderr, "pthread_create() error\n");
			free(tid);
			pthread_mutex_destroy(&lock);
			exit(0);
		}
	}

	/*
	 * Wait until all the threads you created above are finished.
	 */
	for (i = 0; i<thread_count; i++) {
		rc = pthread_join(tid[i], NULL);
		if (rc) {
			fprintf(stderr, "pthread_join() error\n");
			free(tid);
			pthread_mutex_destroy(&lock);
			exit(0);
		}
	}
	
	pthread_mutex_destroy(&lock);

	/*
	 * Print the value of g_count.
	 * It must be (thread_count * value)
	 */ 
	printf("value: %ld\n", g_count);

	free(tid);
}
