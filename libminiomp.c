#include "libminiomp.h"
//#include "intrinsic.h"

// Library constructor and desctructor
void init_miniomp (void) __attribute__((constructor));
void fini_miniomp (void) __attribute__((destructor));

// Function to parse OMP_NUM_THREADS environment variable
void parse_env (void);

void
init_miniomp (void) 
{
  printf ("mini-omp is being initialized\n");
  // Parse OMP_NUM_THREADS environment variable to initialize nthreads_var internal control variable
  parse_env ();
  // Initialize Pthread data structures 
  // Initialize Pthread thread-specific data, now just used to store the OpenMP thread identifier
  pthread_key_create (&miniomp_specifickey, NULL);
  pthread_setspecific (miniomp_specifickey, (void *) 0); // implicit initial pthread with id=0
  // Initialize OpenMP default lock and default barrier:
 
  /* Inicialitzem els comptadors de barreres a zero: */
  comptadorBarrier = 0;

  /* Booleà de sortida del barrier casolà: */
  sortirBarrier = false;

  // Initialize OpenMP workdescriptors for for and single:
  
  /* Últim loop creat: */
  loopCreat = -1;

  /* Comptador de single : */
  comptadorSingle = 0;

  // Initialize OpenMP task queue for task and taskloop
}

void
fini_miniomp (void) 
{
  // delete Pthread thread-specific data
  pthread_key_delete (miniomp_specifickey);

  // free other data structures allocated during library initialization
  printf ("mini-omp is finalized\n");
}
