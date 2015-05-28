//C headers
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include <time.h>

//Spice headers
#include "SpiceUsr.h"

#define LEAPSECOND30JUN2015 488980867


int str2dd( char *buffer, char *Time ) {

	int days[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int i, YEAR, MONTH, DAY, HOUR, MINUTE, SEC, MSK, DOY=0;

	sscanf(buffer, "%d-%d-%dT%d:%d:%d.%d", &YEAR, &MONTH, &DAY, &HOUR, &MINUTE, &SEC, &MSK);

	// Leap year
	// if (YEAR % 4 == 0) days[1] += 1;

	if ( ( (YEAR%4 == 0) && (YEAR%100 != 0) ) || ( YEAR%400 == 0 ) )
	{
		days[1]++;
	}

	// Sum of days from Jan 1 to the input date
	for (i = 0; i < MONTH - 1; i++) 
	{
		DOY += days[i];	
	} 

	DOY += DAY;

	sprintf(Time,"%04d%03d%02d%02d%02d%03d", YEAR, DOY-1, HOUR, MINUTE, SEC, MSK);

	return 0;
}

int double2Str (double et, int n,  char *utc) {

	

	return 0;
}


int main(int argc, char const *argv[])
{
	/*************** String to DD time *********/
	// char *strDate = "2012-03-01T12:00:00.000";
	// char ddDate[17];

	// printf("[INFO] UTC date : %s\n", strDate);

	// str2dd( strDate, ddDate );

	// printf("[INFO] DD date : %s\n", ddDate);
	/*****************************************/
	
	 
	/****** String time <-> et time  (SPICE) ******/
	char *strDate = "2015-06-30T23:55:00.000";
	SpiceDouble et[10];
	char utc[50];
	int i;

	furnsh_c("maven_ephemeris.tm");

	printf("[INFO] Start date : %s\n\n", strDate);

	str2et_c(strDate, &et[0]);
	et2utc_c ( et[0], "ISOC", 3, 50, utc );

	for (i = 1; i < 10; i++)
	{
		et[i] = et[0] + i*60;

		if ((int)et[i] == LEAPSECOND30JUN2015)
		{
			printf("[INFO] Leap second !!\n\n");

			// Translate to the next round time --> + 1s
			et[0]++;

			// Not taking account the leap second value
			i--;
		}
	}

	et[0]--;

	for (i = 0; i < 10; i++)
	{
		et2utc_c ( et[i], "ISOC", 3, 50, utc );

		printf("[INFO] %d --> et : %f --> str : %s\n", i, et[i], utc);
	}
	/**********************************************/

	

	return 0;
}
