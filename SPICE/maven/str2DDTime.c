//C headers
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include <time.h>


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

	sprintf(Time,"%04d%03d%02d%02d%02d%03d\0", YEAR, DOY-1, HOUR, MINUTE, SEC, MSK);

	return 0;
}


int main(int argc, char const *argv[])
{
	char *strDate = "2012-03-01T12:00:00.000";
	char ddDate[17];

	printf("[INFO] UTC date : %s\n", strDate);

	str2dd( strDate, ddDate );

	printf("[INFO] DD date : %s\n", ddDate);

	return 0;
}
