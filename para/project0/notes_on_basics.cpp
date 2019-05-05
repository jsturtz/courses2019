#include<iostream>
#include<omp.h>

using namespace std;

int main() {

#ifndef _OPENMP
    fprintf(stderr, "OpenMP is not supported\n");
    exit(0);
#endif
    
    printf("
    /* // will return number of cores my system has */    
    /* int numprocs = omp_get_num_procs(); */

    /* // will set how many OpenMP threads to reserver starting now */
    /* // It doesn't create the threads yet; it */
    /* // just says how many wil be used the next */
    /* // time you ask for them */
    /* omp_set_num_threads(2); */

    /* // how to use one thread per core: */
    /* omp_set_num_threads(omp_get_num_procs()); */

    /* // this creates a team of threads from */
    /* // the thread pool and divides the for- */
    /* // loop passes up among those threads. */ 
    
/* #pragma omp parallel for default(none) */
    /* for (int i = 0; i < 10; i++) { */
    /*     // This illustrates a typical use */ 
    /*     // case to parallelize the for-loop */
    /*     // into multiple threads. Each thread */
    /*     // gets its own personal copy of the */ 
    /*     // variable i because it is defined */
    /*     // within the for-loop body */
        
    /* } // will wait until all threads are finished before continuing */

    return 0;
}
