#include "libminiomp.h"

/* Declaration of global variable for single work descriptor: */
miniomp_single_t miniomp_single;

/* This routine is called when first encountering a SINGLE construct. 
   Returns true if this is the thread that should execute the clause.  */

bool
GOMP_single_start (void)
{
  int id = omp_get_thread_num ();
  miniomp_parallel[id].comptadorSingle++;

  int valor = miniomp_parallel[id].comptadorSingle;

  return (__sync_bool_compare_and_swap (&comptadorSingle, valor - 1, valor));
}


