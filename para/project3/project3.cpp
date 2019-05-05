
#ifndef
#define NUMT 10
#endif

// "state" of the system involves these globals
int	NowYear;		// 2019 - 2024
int	NowMonth;		// 0 - 11
float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population

// Other parameters
const float GRAIN_GROWS_PER_MONTH =		8.0;    // in inches
const float ONE_DEER_EATS_PER_MONTH =		0.5;

const float AVG_PRECIP_PER_MONTH =		6.0;	// average (in inches)
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				50.0;	// average (in Farenheit)
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;


// temp and precip are functions of particular month:
float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );
float temp = AVG_TEMP - AMP_TEMP * cos( ang );

unsigned int seed = 0;
NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );
float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
if ( NowPrecip < 0. ) NowPrecip = 0.;

// starting date and time:
NowMonth =    0;
NowYear  = 2019;

// starting state (feel free to change this if you want):
NowNumDeer = 1;
NowHeight =  1.;

/* In addition to this, you must add in some other phenomenon that directly or indirectly controls the growth of the grain and/or the graindeer population. Your choice of this is up to you. */ 

omp_set_num_threads( 4 );	// same as # of sections
initBarrier(4);

void MyAgent() 
{
  while (NowYear < 2025) {
    printf("Agent waiting at #1\n");
    #pragma omp barrier       
    printf("Agent resuming at #1\n");

    printf("Agent waiting at #2\n");
    #pragma omp barrier       
    printf("Agent resuming at #2\n");

    // write out "Now" state of data
    // Advance line and re-compute environmental variables
    
    printf("Agent waiting at #3\n");
    #pragma omp barrier       
    printf("Agent resuming at #3\n");
  }
}

float
SQR( float x )
{
  return x*x;
}

void GrainDeer() 
{
  while (NowYear < 2025) {
    
    // compute temporary next value based on current state
    float tempFactor = exp(-SQR(( NowTemp - MIDTEMP ) / 10.  ));
    float precipFactor = exp(-SQR(( NowPrecip - MIDPRECIP ) / 10.));

    // done computing barrier
    #pragma omp barrier       

    NowHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
    NowHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

    // done assigning barrier 
    #pragma omp barrier       

    // done printing barrier
    #pragma omp barrier       
  }
}

int main() {

#pragma omp parallel sections
  {
    #pragma omp section
    {
      GrainDeer( );
    }

    #pragma omp section
    {
      Grain( );
    }

    #pragma omp section
    {
      Watcher( );
    }

    #pragma omp section
    {
      MyAgent( );
    }
  }       
}
