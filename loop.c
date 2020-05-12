#include "libminiomp.h"

/* Declaration of global variable for loop work descriptor: */
miniomp_loop_t miniomp_loop;

miniomp_loop_t *loops;

/* Si hi ha feina disponible per fer retorna cert i els paràmetres *isart
   i *iend tindran assignats un nou bloc d'iteració. En cas contrari
   retornarà fals. */

bool alocatar_feina (long *istart, long *iend)
{
  int i = miniomp_parallel[omp_get_thread_num ()].loop;
  bool ret = true;
  long end = loops[i].end;
  long incr = loops[i].incr;
  long chunk_size = loops[i].chunk_size;

  *istart = loops[i].next;
  loops[i].next += chunk_size * incr;

  if (incr > 0)
  {
    if (*istart < end)
    {
      if ((*istart + (chunk_size * incr)) < end)
        *iend = *istart + (chunk_size * incr);
      else *iend = end;
      ret = true;
    }
    else ret = false;
  }
  else
  {
    if (*istart > end)
    {
      if ((*istart + (chunk_size * incr)) > end)
        *iend = *istart + (chunk_size * incr);
      else *iend = end;
      ret = true;
    }
    else ret = false;
  }
  
  return ret;
}

/* The *_next routines are called when the thread completes processing of 
   the iteration block currently assigned to it.  If the work-share 
   construct is bound directly to a parallel construct, then the iteration
   bounds may have been set up before the parallel.  In which case, this
   may be the first iteration for the thread.

   Returns true if there is work remaining to be performed; *ISTART and
   *IEND are filled with a new iteration block.  Returns false if all work
   has been assigned.  */

bool
GOMP_loop_dynamic_next (long *istart, long *iend) 
{
  return alocatar_feina (istart, iend);
}

/* The *_start routines are called when first encountering a loop construct
   that is not bound directly to a parallel construct.  The first thread 
   that arrives will create the work-share construct; subsequent threads
   will see the construct exists and allocate work from it.

   START, END, INCR are the bounds of the loop; CHUNK_SIZE is the
   scheduling parameter. 

   Returns true if there's any work for this thread to perform.  If so,
   *ISTART and *IEND are filled with the bounds of the iteration block
   allocated to this thread.  Returns false if all work was assigned to
   other threads prior to this thread's arrival.  */

bool
GOMP_loop_dynamic_start (long start, long end, long incr, long chunk_size,
                         long *istart, long *iend)
{
  pthread_mutex_lock (&mutex_for_start_lock);
  
  /* Cal alocatar memòria pels miniomp_loop: */
  if (__sync_bool_compare_and_swap (&loopCreat, -1, loopCreat + 1))
  {
    loops = (miniomp_loop_t *) malloc (sizeof (miniomp_loop_t));
    loops[0].creat = true;
    loops[0].iniciat = false;
    /*
    for (int i = 0; i < numThreads; i++)
    {
      loops[i].iniciat = false;
      loops[i].comptadorThreads = 0;
    }
    */
  }
  
  while (loopCreat == -1); 
  
  int id = miniomp_parallel[omp_get_thread_num ()].loop;

  if (__sync_bool_compare_and_swap (&loopCreat, id - 1, loopCreat + 1))
  {
    loops = (miniomp_loop_t *) realloc (loops, (loopCreat + 1) * sizeof (miniomp_loop_t));
    loops[id].iniciat = false;
  }
  
  if (__sync_bool_compare_and_swap (&loops[id].iniciat, false, true))
  {
    loops[id].start = start;
    loops[id].end = end;
    loops[id].incr = incr;
    loops[id].chunk_size = chunk_size;
    loops[id].next = start;
    loops[id].schedule = ws_DYNAMIC;
  }

  while (! loops[id].iniciat);
  
  pthread_mutex_unlock (&mutex_for_start_lock);
  
  return (alocatar_feina (istart, iend));
}

/* The GOMP_loop_end* routines are called after the thread is told that
   all loop iterations are complete.  The first version synchronize
   all threads; the nowait version does not. */

void
GOMP_loop_end (void) 
{
  GOMP_barrier ();
  miniomp_parallel[omp_get_thread_num ()].loop++;
}

void
GOMP_loop_end_nowait (void) 
{
  miniomp_parallel[omp_get_thread_num ()].loop++;
}

#if 0
// Only implement this if really needed, i.e. you find a case in which it is invoked

/* The GOMP_parallel_loop_* routines pre-initialize a work-share construct
   to avoid one synchronization once we get into the loop. The compiler 
   does not invoke the *_start routine for the work-share. And of course, 
   the compiler does not invoke GOMP_loop_end. These routines should create 
   the team of threads to execute the work-share in parallel */

void
GOMP_parallel_loop_dynamic (void (*fn) (void *), void *data,
                            unsigned num_threads, long start, long end,
                            long incr, long chunk_size, unsigned flags)
{
  printf("TBI: What another mess! Directly starting a parallel and a non-static for worksharing construct! I am lost!\n");
  // Here you should pre-initialize the work-sharing structures as done in 
  // GOMP_loop_dynamic_start; only the master thread is doing this, the other
  // threads in the team do not reach this point 
  GOMP_parallel (fn, data, num_threads, flags);
}
#endif
