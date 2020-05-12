#include "libminiomp.h"

// This file implements the PARALLEL construct

// Declaration of array for storing pthread identifier from pthread_create function
pthread_t *miniomp_threads;

// Global variable for parallel descriptor
miniomp_parallel_t *miniomp_parallel;

// Declaration of per-thread specific key
pthread_key_t miniomp_specifickey;

// This is the prototype for the Pthreads starting function
void *worker (void *args) 
{
  miniomp_parallel_t *parallel = args;

  // save thread-specific data
  pthread_setspecific (miniomp_specifickey, (void *) (long) parallel -> id);
    
  // invoke the per-threads instance of function encapsulating the parallel region
  parallel -> fn (parallel -> fn_data);

  // exit the function
  pthread_exit (NULL);
  return (NULL);
}

void
GOMP_parallel (void (*fn) (void *), void *data, unsigned num_threads, 
  unsigned int flags) 
{
  if (! num_threads) num_threads = omp_get_num_threads ();

  printf ("Starting a parallel region using %d threads\n", num_threads);
  
  miniomp_parallel = calloc (num_threads, sizeof (miniomp_parallel_t));

  miniomp_threads = calloc (num_threads, sizeof (pthread_t));

  for (int i = 0; i < num_threads; i++)
  {
    miniomp_parallel[i].fn = fn;
    miniomp_parallel[i].fn_data = data;
    miniomp_parallel[i].id = i;
    miniomp_parallel[i].loop = 0;
    miniomp_parallel[i].comptadorSingle = 0;
    
    pthread_create (&miniomp_threads[i], NULL, &worker, (void *) &miniomp_parallel[i]);
  }  

  for (int j = 0; j < num_threads; j++) pthread_join (miniomp_threads[j], NULL);
}

