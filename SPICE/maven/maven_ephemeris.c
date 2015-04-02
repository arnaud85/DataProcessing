//C headers
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include <time.h>

//Spice headers
#include "SpiceUsr.h"

//NetCDF headers
#include <netcdf.h>

//DD lib
#include "DD.h"

//SPICE constants
#define KERNELS     			"maven_ephemeris.tm"
#define MAXIV          			1000
#define WINSIZ         			( 2 * MAXIV )
#define TIMLEN         			51
#define TARGET					argv[1]
#define OBSERVER    			argv[2]
#define FRAME_IAU_MARS  		"IAU_MARS"
#define FRAME_MSO   			"MAVEN_MSO"
#define ABCORR					"NONE"

//NETCDF constants
#define TIME_DIM   				"Time"
#define TIMELEN_DIM 			"TimeLength"
#define POS_DIM        			"Position"
#define TIME_VAR   				"Time"
#define POS_VAR_MSO     		"XYZ_MSO"
#define LON_VAR_IAU_MARS 		"LON_IAU_MARS"
#define LAT_VAR_IAU_MARS 		"LAT_IAU_MARS"
#define DIST_VAR_IAU_MARS       "R_IAU_MARS"
#define START_VAR       		"StartTime"
#define STOP_VAR        		"StopTime"
#define SOURCE          		"SPICE-NAIF"
#define POS_UNIT				"R_MARS"
#define DIST_UNIT       		"R_MARS" 
#define LON_UNIT       			"deg" 
#define LAT_UNIT                "deg"
#define TIME_STR_LEN    		 17
// #define TIME_STR_LEN    		 24

//Other constants
#define J2000 					946727935.816 		//2000-01-01T11:58:55.816 	TO USE (+3s ...) !!!
// #define J2000 					946727932.816 	//2000-01-01T11:58:52.816 	GOOD (+/- leap seconds)
#define R_MARS 					3390.0
#define STEP    				60.0
#define RAD2DEG 				180/M_PI
#define ERRCODE 				2


//Functions prototypes
int loadKernels(char *kernelsList);
int getStartStopTimes(const SpiceChar *kernel, int bodyIndex, SpiceChar *startTime, SpiceChar *stopTime);
int getBoundaries(char *et_0_str, SpiceDouble *et_0, char *et_end_str, SpiceDouble *et_end);
int getPositions(const char *target, SpiceDouble et_0, SpiceInt step, SpiceInt n_iter, const char *frame, const char *ab_corr, const char *observer, SpiceDouble t[], SpiceDouble **positions);
int getLonLat(SpiceDouble **pos, SpiceDouble lon[], SpiceDouble lat[], int n);
int getDistance(SpiceInt n_iter, SpiceDouble **positions, SpiceDouble dist[]);
int getFilesName(const char *bodyName, char startTime[], char ext[], char filename[]);
int time2DDtime(double et_time, dd_time_t *DDtime);
int utc2ddTime( char *uctTime, char *ddTime );
void nc_handle_error(int status, char* operation);
int createTextFile(char* filename, SpiceDouble n_iter, SpiceDouble t[], SpiceDouble *pos_mso_mars[3], SpiceDouble lon_iau_mars[], SpiceDouble lat_iau_mars[], SpiceDouble dist[]);
int createNc(char* ncfile, SpiceDouble n_iter, SpiceDouble t[], SpiceDouble *pos_mso_mars[3], SpiceDouble lon_iau_mars[], SpiceDouble lat_iau_mars[], SpiceDouble dist[]);


int main(int argc, char const *argv[])
{
	// Local variables
	SpiceDouble et_0;
	SpiceDouble et_end;
	SpiceDouble *t = NULL;
	SpiceDouble **pos_mso = NULL;
	SpiceDouble **pos_iau_mars = NULL;
	SpiceDouble *lon_iau = NULL;
	SpiceDouble *lat_iau = NULL;
	SpiceDouble *distance = NULL;
	SpiceInt n;
	SpiceInt i;
	char START_DATE[50]; 
	char STOP_DATE[50]; 
	char txt_filename[50];
	char nc_filename[50];
	const SpiceChar *spk = " kernels/Maven/maven_orb_rec.bsp";

	// Load specific kernels 
	if (!loadKernels(KERNELS))
	{
		printf("[ERROR] \"%s\" kernel list : no such file\n", KERNELS);
		exit(EXIT_FAILURE);
	}

	// Configure SPICE time parameters
	timdef_c ( "SET", "SYSTEM", 51, "UTC" );

	// Configure boundaries
	// printf("[INFO] Argc : %d\n", argc); 

	// for (i = 0; i < argc; i++)
	// {
	// 	printf("[INFO] ARGV[%d] : %s\n", i, argv[i]);
	// }
	 
	
	if ( argc == 5 )
	{
		strcpy(START_DATE, argv[3]);
		strcpy(STOP_DATE, argv[4]);
	}
	else if ( argc == 3 )
	{
		getStartStopTimes( spk, -202, START_DATE, STOP_DATE );
	}
	else
	{
		printf("[ERROR] Usage : maven_ephemeris target reference [start time] [stop time]\n");

		exit(EXIT_FAILURE);
	}
	
	printf("[INFO] Start time : %s\n", START_DATE);
	printf("[INFO] Stop  time : %s\n", STOP_DATE);

	n = getBoundaries(START_DATE, &et_0, STOP_DATE, &et_end);
	if (n <= 0)
	{
		printf("[ERROR] Enable to get a date interval to compute Earth ephemeris.\n");

		exit(EXIT_FAILURE);
	}

	// Memory allocation
	t = (SpiceDouble*) malloc(n*sizeof(SpiceDouble));
	pos_mso = (SpiceDouble**) malloc(n*sizeof(SpiceDouble*));
	pos_iau_mars = (SpiceDouble**) malloc(n*sizeof(SpiceDouble*));
	distance = (SpiceDouble*) malloc(n*sizeof(SpiceDouble));
	lon_iau = (SpiceDouble*) malloc(n*sizeof(SpiceDouble));
	lat_iau = (SpiceDouble*) malloc(n*sizeof(SpiceDouble));

	if ( (t == NULL) || (pos_mso == NULL) || (pos_iau_mars == NULL) || (lon_iau == NULL) || (lat_iau == NULL) || (distance == NULL) )
	{
		printf("[ERROR] Memory allocation has failed. Not enough memory.\n");

		exit(EXIT_FAILURE);
	}
	else
	{
		for (i = 0; i < n; i++)
		{
			pos_mso[i] = NULL;
			pos_mso[i] = (SpiceDouble*) malloc(3*sizeof(SpiceDouble));

			pos_iau_mars[i] = NULL;
			pos_iau_mars[i] = (SpiceDouble*) malloc(3*sizeof(SpiceDouble));

			if ( (pos_mso[i] == NULL) || (pos_iau_mars[i] == NULL) )
			{
				printf("[ERROR] Position : Memory allocation has failed.\n");
			}
		}
	}
	
	// Compute n positions of the celestial body wanted starting from et_0 epoch with a STEP step
	getPositions(TARGET, et_0, STEP, n, FRAME_MSO, ABCORR, OBSERVER, t, pos_mso);
	getPositions(TARGET, et_0, STEP, n, FRAME_IAU_MARS, ABCORR, OBSERVER, t, pos_iau_mars);

	// Compute longitudes and latitudes
	getLonLat(pos_iau_mars, lon_iau, lat_iau, n);

	// Compute distance to Mars
	getDistance(n, pos_iau_mars, distance);

	// Get files name
	getFilesName(TARGET, START_DATE, ".nc", nc_filename);
	getFilesName(TARGET, START_DATE, ".txt", txt_filename);

	// Write ascii text file
	createTextFile(txt_filename, n, t,  pos_mso, lon_iau, lat_iau, distance);

	// Write celestial body positions NC file
	createNc(nc_filename, n, t, pos_mso, lon_iau, lat_iau, distance);

	// Free memory
	free(t);
	free(distance);

	for (i = 0; i < n; i++)
	{
		free(pos_mso[i]);
		pos_mso[i] = NULL;

		free(pos_iau_mars[i]);
		pos_iau_mars[i] = NULL;
	}

	free(pos_mso);
	pos_mso = NULL;

	free(pos_iau_mars);
	pos_iau_mars = NULL;

	free(lon_iau);
	lon_iau = NULL;

	free(lat_iau);
	lat_iau = NULL;

	return 0;
}


/**
 * [loadKernels Load SPICE kernels]
 * @param  kernelsList [Name of the file describing kernels to load]
 * @return             [0 if OK]
 */
int loadKernels(char *kernelsList)
{
	furnsh_c(kernelsList);

	return 1;
}


/**
 * [getStartStopTimes Get time intervall of the SPICE kernel used]
 * @param  kernel    [Name of the SPICE kernel used]
 * @param  bodyIndex [Index of the space body]
 * @param  startTime [Start time]
 * @param  stopTime  [Stop time]
 * @return           [0 if OK]
 */
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
	
	// timout_c(start, "YYYY MON DD HR:MN:SC UTC ::UTC", TIMLEN, startTime);
	// timout_c(stop, "YYYY MON DD HR:MN:SC UTC ::UTC", TIMLEN, stopTime);

	start += 0.001;
	timout_c(start, "YYYY MON DD HR:MN:SC.#### UTC ::UTC", TIMLEN, startTime);
	timout_c(stop, "YYYY MON DD HR:MN:SC.#### UTC ::UTC", TIMLEN, stopTime);

	return 0;
}

/**
 * [getBoundaries Get double type boundaries values from corresponding strings]
 * @param  et_0_str   [Start time (string)]
 * @param  et_0       [Start time (double)]
 * @param  et_end_str [Stop time (string)]
 * @param  et_end     [Stop time (double)]
 * @return            [0 if OK]
 */
int getBoundaries(char *et_0_str, SpiceDouble *et_0, char *et_end_str, SpiceDouble *et_end)
{
	SpiceInt n;

	str2et_c(et_0_str, et_0);
	str2et_c(et_end_str, et_end);
	n = (int)((*et_end-*et_0)/STEP);

	return n;
}


/**
 * [getPositions Computes orbit]
 * @param  target    [Name of the celestial body]
 * @param  et_0      [Start time (double)]
 * @param  step      [Resolution used]
 * @param  n_iter    [Data number]
 * @param  frame     [Frame used]
 * @param  ab_corr   [aberration corrections (yes/no)]
 * @param  observer  [Reference body]
 * @param  t         [Time array]
 * @param  positions [Orbit array]
 * @return           [0 if OK]
 */
int getPositions(const char *target, SpiceDouble et_0, SpiceInt step, SpiceInt n_iter, const char *frame, const char *ab_corr, const char *observer, SpiceDouble t[], SpiceDouble **positions)	 
{
	SpiceInt i;
	SpiceInt j;
	SpiceDouble light_time;

	for (i = 0; i < n_iter; i++)
	{
		t[i]  =  et_0 + i*step;

		spkpos_c(target, t[i], frame, ab_corr, observer,  positions[i],  &light_time);

		// for (j = 0; j < 3; j++)
		// {
		// 	positions[i][j] /= R_MARS;
		// }
		
	}

	// printf("[INFO] t0 : %f\n", t[0]);

	return 1;
}


/**
 * [getLonLat Get spherical coordonates from orbit computed]
 * @param  pos [Orbit array]
 * @param  lon [Longitude array]
 * @param  lat [Latitude array]
 * @param  n   [Data number]
 * @return     [0 if OK]
 */
int getLonLat(SpiceDouble **pos, SpiceDouble lon[], SpiceDouble lat[], int n)
{
	SpiceInt i;
	SpiceDouble radius;
	SpiceDouble longitude;
	SpiceDouble latitude;

	for (i = 0; i < n; i++)
	{
		reclat_c(pos[i], &radius, &longitude, &latitude);

		lon[i] = longitude * RAD2DEG;
		lat[i] = latitude * RAD2DEG;
	}
                   
	return 0;
}


/**
 * [getFilesName Used to create the netCDF's filename]
 * @param  bodyName  [Name of the celestial body]
 * @param  startTime [Start time]
 * @param  ext       [File's extension]
 * @param  filename  [Name of the file]
 * @return           [0 if OK]
 */
int getFilesName(const char *bodyName, char startTime[], char ext[], char filename[])
{
	SpiceDouble et;
	dd_time_t time_struct;

	str2et_c(startTime, &et);
	time2DDtime(et, &time_struct);

	strcpy(filename, bodyName);
	strcat(filename, "_");
	strcat(filename, time_struct);
	strcat(filename, ext);

	return 0;
}


/**
 * [getDistance Get distance from the reference body computed from celestial body's orbit]
 * @param  n_iter    [Data number]
 * @param  positions [Celestial body's orbit]
 * @param  dist      [Distance array]
 * @return           [0 if OK]
 */
int getDistance(SpiceInt n_iter, SpiceDouble **positions, SpiceDouble dist[])
{
	SpiceInt i;

	for (i = 0; i < n_iter; i++)
	{
		dist[i] = sqrt(positions[i][0]*positions[i][0] + positions[i][1]*positions[i][1] + positions[i][2]*positions[i][2]);
	}
}


/**
 * [time2DDtime Transform time into AMDA time format]
 * @param  et_time [SPICE time]
 * @param  DDtime  [AMDA time]
 * @return         [0 if OK]
 */
int time2DDtime(double et_time, dd_time_t *DDtime)
{

	strcpy(*DDtime, Double2DD_Time(et_time+J2000));
	
	return 0;
} 

/**
 * [utc2ddTime Get DD time format from a classical UTC time format]
 * @param  uctTime [UTC time]
 * @param  ddTime   [DD time]
 * @return        [0 if OK]
 */
int utc2ddTime( char *uctTime, char *ddTime ) {

	int days[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int i, YEAR, MONTH, DAY, HOUR, MINUTE, SEC, MSK, DOY=0;

	// Get the init classical format UTC time and extract year, month, etc ...
	sscanf(uctTime, "%d-%d-%dT%d:%d:%d.%d", &YEAR, &MONTH, &DAY, &HOUR, &MINUTE, &SEC, &MSK);

	// Test for leap years
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

	// Write DD time
	sprintf(ddTime,"%04d%03d%02d%02d%02d%03d\0", YEAR, DOY-1, HOUR, MINUTE, SEC, MSK);

	return 0;
}


/**
 * [createTextFile Celestial body's orbit (text format)]
 * @param  filename 		[Name of the file]
 * @param  n_iter  			[Data number]
 * @param  t  				[Time array]
 * @param  pos_mso_mars  	[Orbit array]
 * @param  lon_iau_mars  	[Longitude array] 
 * @param  lat_iau_mars  	[Latitude array] 
 * @param  dist  			[Distance array] 
 * @return         			[0 if OK]
 */
int createTextFile(char* filename, SpiceDouble n_iter, SpiceDouble t[], SpiceDouble *pos_mso_mars[3], SpiceDouble lon_iau_mars[], SpiceDouble lat_iau_mars[], SpiceDouble dist[])
{
	FILE* file = NULL;
	int i;
 
    file = fopen(filename, "w");

    char utc[50];
 
    if (file != NULL)
    {

        // for (i = 0; i < n_iter; i++)
        // {
        // 	fprintf(file, "%.0f %.2f %.2f %.2f %.2f\n", t[i]+J2000, pos_mso_mars[i][0], pos_mso_mars[i][1], pos_mso_mars[i][2], dist[i]);
        // }


    	for (i = 0; i < n_iter; i++)
        {
        	et2utc_c ( t[i], "ISOC", 3, 50, utc );

        	// fprintf(file, "%s %.2f %.2f %.2f %.2f\n", utc, pos_mso_mars[i][0], pos_mso_mars[i][1], pos_mso_mars[i][2], dist[i]);
       		fprintf(file, "%s %e %e %e %e\n", utc, pos_mso_mars[i][0], pos_mso_mars[i][1], pos_mso_mars[i][2], dist[i]);
        }

         


        fclose(file);

        // printf("[INFO] %s has been created\n", filename);
    }

	return 0;
}




/**
 * [nc_handle_error Catch errors when writing the netCDF file]
 * @param status    [Status index]
 * @param operation [Operation]
 */
void nc_handle_error(int status, char* operation) {

	printf("[ERROR] %s : %s\n", operation, nc_strerror(status));
	exit(ERRCODE);
}




/**
 * [createNc Celestial body's orbit (netCDF format)]
 * @param  ncfile 		[Name of the nc file]
 * @param  n_iter  			[Data number]
 * @param  t  				[Time array]
 * @param  pos_mso_mars  	[Orbit array]
 * @param  lon_iau_mars  	[Longitude array] 
 * @param  lat_iau_mars  	[Latitude array] 
 * @param  dist  			[Distance array] 
 * @return         			[0 if OK]
 */
int createNc(char* ncfile, SpiceDouble n_iter, SpiceDouble t[], SpiceDouble *pos_mso_mars[3], SpiceDouble lon_iau_mars[], SpiceDouble lat_iau_mars[], SpiceDouble dist[])
{
	
	/**************** LOCAL VARIABLES ***********************/
	//NC file ID
	int ncid;

	//Dimensions IDs
	int time_dimid;
	int timelength_dimid;
	int dataId;
	int time_tab_dimid[2];
	int data_tab_dimid[2];

	////Variables IDs
	int time_varid;
	int position_varid;
	int longitude_iau_varid;
	int latitude_iau_varid;
	int distance_varid;
	int startTime_varid;
	int stopTime_varid;

	//Stocking time reference
	time_t p;
	char *s;

	//Buffers
	size_t start[2];
	size_t timeCount[2];
	size_t dataCount[2];

	//Dates
    // dd_time_t *DDdates = NULL;
    char **utc = NULL;
    char **ddTime = NULL;
    size_t i;

  	utc = (char**) malloc(n_iter*sizeof(char*));
  	ddTime = (char**) malloc(n_iter*sizeof(char*));

    for (i = 0; i < n_iter; i++)
    {
    	utc[i] = (char*) malloc(50*sizeof(char));
    	ddTime[i] = (char*) malloc(17*sizeof(char));
    }

    for (i = 0; i < n_iter; i++)
    {
    	et2utc_c ( t[i], "ISOC", 3, 50, utc[i] );

    	utc2ddTime( utc[i], ddTime[i] );

    	// printf("[INFO] utc[%d] = %s\n", i, utc[i]);
    }
	

	//Other
	int step;
	int retval;
	// size_t i;
	size_t j;


	/**************** END OF LOCAL VARIABLES ***********************/


	/**************** OPEN NC FILE ***********************/
	//Create nc file (NC_CLOBBER to overwrite)
	retval = nc_create(ncfile, NC_CLOBBER, &ncid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Create nc file");
	} 
	/**************** END OF OPEN NC FILE ***********************/



	/*************************** DEFINE MODE ****************************/

	//Define dimensions
	retval = nc_def_dim(ncid, TIME_DIM, NC_UNLIMITED, &time_dimid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Time dimension definition");
	} 
	retval = nc_def_dim(ncid, TIMELEN_DIM, TIME_STR_LEN, &timelength_dimid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "TimeLength dimension definition");
	} 
	retval = nc_def_dim(ncid, POS_DIM, 3L, &dataId);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Position dimension definition");
	} 
	

	//Define the netCDF variables
	time_tab_dimid[0] = time_dimid;
	time_tab_dimid[1] = timelength_dimid;
  	data_tab_dimid[0] = time_dimid;
  	data_tab_dimid[1] = dataId;

  	retval = nc_def_var(ncid, TIME_VAR, NC_CHAR, 2, time_tab_dimid, &time_varid);
  	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Time variable");
	}

	retval = nc_def_var(ncid, POS_VAR_MSO, NC_DOUBLE, 2, data_tab_dimid, &position_varid);
  	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "MSO Position variable");
	}


	retval = nc_def_var(ncid, LON_VAR_IAU_MARS, NC_DOUBLE, 1, &time_dimid, &longitude_iau_varid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "LON IAU_MARS variable");
	}
	retval = nc_def_var(ncid, LAT_VAR_IAU_MARS, NC_DOUBLE, 1, &time_dimid, &latitude_iau_varid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "LAT IAU_MARS variable");
	}

	retval = nc_def_var(ncid, DIST_VAR_IAU_MARS, NC_DOUBLE, 1, &time_dimid, &distance_varid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Distance IAU_MARS variable");
	}

	retval = nc_def_var(ncid, START_VAR, NC_CHAR, 1, &timelength_dimid, &startTime_varid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "StartTime variable");
	}

	retval = nc_def_var(ncid, STOP_VAR, NC_CHAR, 1, &timelength_dimid, &stopTime_varid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "StopTime variable");
	}

	//Units attributes for netCDF variables
	retval = nc_put_att_text(ncid, position_varid, "units", strlen(POS_UNIT), POS_UNIT);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Position unit");
	}

	retval = nc_put_att_text(ncid, distance_varid, "units", strlen(DIST_UNIT), DIST_UNIT);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Distance unit");
	}

	retval = nc_put_att_text(ncid, longitude_iau_varid, "units", strlen(LON_UNIT), LON_UNIT);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Longitude unit");
	}

	retval = nc_put_att_text(ncid, latitude_iau_varid, "units", strlen(LAT_UNIT), LAT_UNIT);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Latitude unit");
	}

	retval = nc_put_att_text(ncid, NC_GLOBAL, "Source", strlen(SOURCE), SOURCE);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Source");
	}

	time(&p);
	s = ctime(&p);

	retval = nc_put_att_text(ncid, NC_GLOBAL, "Created", 24, s);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Created argument");
	}
	

	//End of define mode
	retval = nc_enddef(ncid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "End define mode");
	}

	/*********************** END OF DEFINE MODE ****************************/





	/************************ WRITING MODE ********************************/
  	//Configure buffers
	timeCount[0] = 1L;
	timeCount[1] = TIME_STR_LEN;
	dataCount[0] = 1;
	dataCount[1] = 3L;
	start[1] = 0L;

	//Build DDdate
	// DDdates = (dd_time_t*)malloc((int)n_iter*sizeof(dd_time_t));
	// if(DDdates == NULL)
	// {
	// 	printf("[ERROR] Unable to build DD dates\n");

	// 	exit(EXIT_FAILURE);
	// }
	
	// for (i = 0; i < n_iter; i++)
	// {
	// 	time2DDtime(t[i], &DDdates[i]);
	// }


	//Write datas
    for (i = 0; i < n_iter; i++)
    {
    	start[0] = i;
	    // retval = nc_put_vara_text(ncid, time_varid, start, timeCount, DDdates[i]);
	    retval = nc_put_vara_text(ncid, time_varid, start, timeCount, ddTime[i]);
		if (retval != NC_NOERR)
		{
			nc_handle_error(retval, "Write time variable");
		}
    }

	for (i = 0; i < n_iter; i++)
	{
		start[0] = i;
		retval = nc_put_vara_double(ncid, position_varid, start, dataCount, pos_mso_mars[i]);
	 	if (retval != NC_NOERR)
		{
			nc_handle_error(retval, "Write MSO position variable");
		}
	}


	retval = nc_put_var_double(ncid, longitude_iau_varid, lon_iau_mars);
 	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Write LON IAU_MARS variable");
	}

	retval = nc_put_var_double(ncid, latitude_iau_varid, lat_iau_mars);
 	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Write LAT IAU_MARS variable");
	}

	retval = nc_put_var_double(ncid, distance_varid, dist);
 	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Write distance variable");
	}

	// retval = nc_put_var_text(ncid, startTime_varid, DDdates[0]);
	retval = nc_put_var_text(ncid, startTime_varid, ddTime[0]);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Write start time variable");
	}

	// retval = nc_put_var_text(ncid, stopTime_varid, DDdates[(int)n_iter-1]);
	retval = nc_put_var_text(ncid, stopTime_varid, ddTime[(int)n_iter-1]);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Write stop time variable");
	}

	//Free memory
	// free(DDdates);

	/************************ END OF WRITING MODE ********************************/



	/**************** CLOSE NC FILE ***********************/
	//Close the file
	retval = nc_close(ncid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Close nc file");
	}

	// printf("[INFO] %s has been created\n", ncfile);
	/**************** END OF CLOSE NC FILE ***********************/

	return 0;
}