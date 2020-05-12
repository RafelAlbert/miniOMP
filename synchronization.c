#include "libminiomp.h"

/* Default lock for critical sections: */
pthread_mutex_t miniomp_default_lock;

/* Lock for barrier casolà: */
pthread_mutex_t miniomp_barrier_lock;

/* Comptador de threads en un barrier casolà: */
extern int comptadorBarrier;

void GOMP_critical_start (void) 
{
  //printf("Entrant en un critical sense nom.\n");

  pthread_mutex_lock (&miniomp_default_lock);
}

void GOMP_critical_end (void) 
{
  //printf("Sortint d'un critical sense nom.\n");

  pthread_mutex_unlock (&miniomp_default_lock);
}

void GOMP_critical_name_start (void **pptr) 
{
  pthread_mutex_t * plock = *pptr;

  //printf("Entrant un critical amb nom %p (%p).\n", pptr, plock);

  if (plock == NULL)
  {
    plock = malloc (sizeof (pthread_mutex_t));
    pthread_mutex_init (plock, NULL);
    *pptr = plock;
  }
  
  pthread_mutex_lock (plock);
}

void GOMP_critical_name_end (void **pptr) 
{
  pthread_mutex_t * plock = *pptr;

  //printf("Sortint d'un critical amb nom %p (%p).\n", pptr, plock);

  /* Si plock encara és NULL, hi ha un error: */
  if (plock != NULL) pthread_mutex_unlock (plock);
  else printf ("ERROR en critical amb nom %p (%p).\n", pptr, plock);
}

pthread_barrier_t miniomp_barrier;

void GOMP_barrier () 
{
  //printf("Entrant en una barrera.\n");

  /* Barrera clàssica: */
  //pthread_barrier_wait (&miniomp_barrier);

  /* Aquí comença la barrera casolana: */
   int numThreads = omp_get_num_threads ();

  while (__atomic_load_n (&sortirBarrier, __ATOMIC_SEQ_CST));
  
  if (__atomic_add_fetch (&comptadorBarrier, 1, __ATOMIC_SEQ_CST) == numThreads)
    __atomic_store_n (&sortirBarrier, true, __ATOMIC_SEQ_CST);

  while (! __atomic_load_n (&sortirBarrier, __ATOMIC_SEQ_CST));
  
  if (__atomic_add_fetch (&comptadorBarrier, -1, __ATOMIC_SEQ_CST) == 0)
    __atomic_store_n (&sortirBarrier, false, __ATOMIC_SEQ_CST);

  return;
}
