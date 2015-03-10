//C headers
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include <time.h>

//Spice headers
#include "SpiceUsr.h"


//SPICE constants
#define 	KERNELS     	"maven_ephemeris.tm"
#define  	MAXIV          	1000
#define  	WINSIZ         	( 2 * MAXIV )
#define  	TIMLEN         	51

int getStartStopTimes(const SpiceChar *kernel, int bodyIndex, SpiceChar *startTime, SpiceChar *stopTime)
{
	// Local variables
	SPICEDOUBLE_CELL ( cover, MAXIV );
	SpiceDouble  start; 
	SpiceDouble  stop;

	// Get coverage for MAVEN (id : -202)
	spkcov_c ( kernel, bodyIndex, &cover );

	// Get start and stop times
	wnfetd_c ( &cover, 0, &start, &stop );
	timout_c(start, "YYYY MON DD HR:MN:SC UTC ::UTC", TIMLEN, startTime);
	timout_c(stop, "YYYY MON DD HR:MN:SC UTC ::UTC", TIMLEN, stopTime);

	return 0;
}

int main(int argc, char const *argv[])
{

	const SpiceChar *spk = " kernels/Maven/maven_orb_rec.bsp";
	SpiceChar startTime [ TIMLEN ];
	SpiceChar stopTime [ TIMLEN ];

	// Load kernel
	furnsh_c(KERNELS);
	printf("[INFO] SPK : %s\n", spk);
	
	// Get kernel's boundaries
	getStartStopTimes( spk, -202, startTime, stopTime );
	printf("[INFO] Start time : %s\n", startTime);
	printf("[INFO] Stop  time : %s\n", stopTime);

	return 0;
}
