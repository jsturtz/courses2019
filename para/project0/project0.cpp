#include<stdio.h>
#include<omp.h>
#include<math.h>

#define NUMT            4
#define ARRAYSIZE       1000000
#define NUMTRIES        100

float A[ARRAYSIZE];
float B[ARRAYSIZE];
float C[ARRAYSIZE];

int main() {

#ifndef _OPENMP
    fprintf(stderr, "OpenMP is not supported\n\n");
    return 1;;
#endif
    
    omp_set_num_threads(NUMT);
    fprintf(stderr, "Using %d threads\n", NUMT);
    
    // used to find peak time of all tries
    double maxMegaMults = 0;
    double sumMegaMults = 0;

    for (int t = 0; t < NUMTRIES; t++) {

        double time0 = omp_get_wtime(); // begin time

        #pragma omp parallel for
        for( int i = 0; i < ARRAYSIZE; i++) 
        {
            C[i] = A[i] * B[i];
        }

        double time1 = omp_get_wtime(); // end time

        // convert to Mega Multiples per Second
        double megaMults = (double) ARRAYSIZE/(time1 - time0)/1000000.;
        sumMegaMults += megaMults;

        if (megaMults > maxMegaMults) {
            maxMegaMults = megaMults; // set peak
        }
    } 
    
    double avgMegaMults = sumMegaMults / NUMTRIES;

    printf("Peak Performance = %8.2lf Mega Mults/Sec\n", maxMegaMults);
    printf("Avg Performance = %8.2lf Mega Mults/Sec\n", avgMegaMults);

    return 0;
}
