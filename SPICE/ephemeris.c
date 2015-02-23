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
#define KERNELS     			"ephemeris.tm"
#define TXT_FILE    			"ephemeris.txt"
//#define TARGET					"EARTH"
//#define TARGET					"MARS"
#define TARGET					argv[1]
//#define OBSERVER    			"SUN"
#define OBSERVER    			argv[2]
#define FRAME1   				"HCI"
#define FRAME2   				"IAU_SUN"
#define FRAME3   				"HEE"
#define ABCORR					"NONE"

//NETCDF constants
#define TIME_DIM   				"Time"
#define TIMELEN_DIM 			"TimeLength"
#define POS_DIM        			"Position"
#define TIME_VAR   				"Time"
#define POS_VAR_HCI     		"XYZ_HCI"
#define POS_VAR_IAU     		"XYZ_IAU_SUN"
#define POS_VAR_HEE             "XYZ_HEE"
#define LON_VAR_HCI 			"LON_HCI"
#define LAT_VAR_HCI 			"LAT_HCI"
#define LON_VAR_IAU_SUN 		"LON_IAU_SUN"
#define LAT_VAR_IAU_SUN 		"LAT_IAU_SUN"
#define DIST_VAR        		"R"
#define START_VAR       		"StartTime"
#define STOP_VAR        		"StopTime"
#define SOURCE          		"SPICE-NAIF"
#define POS_UNIT				"AU"
#define DIST_UNIT       		"AU" 
#define LON_UNIT       			"RAD" 
#define LAT_UNIT                "RAD"
#define TIME_STR_LEN    		 17

//Other constants
#define BODY_NAME               argv[3]
#define AU              		149597870
#define STEP    				3600.0 
#define ERRCODE 				2


//Functions prototypes
int loadKernels(char *kernelsList);
int getBoundaries(char *et_0_str, SpiceDouble *et_0, char *et_end_str, SpiceDouble *et_end);
int getPositions(const char *target, SpiceDouble et_0, SpiceInt step, SpiceInt n_iter, const char *frame, const char *ab_corr, const char *observer, SpiceDouble t[], SpiceDouble **positions);
int getLonLat(SpiceDouble **earth_pos, SpiceDouble lon[], SpiceDouble lat[], int n);
int getDistance(SpiceInt n_iter, SpiceDouble **positions, SpiceDouble dist[]);
int getFilesName(const char *bodyName, char startTime[], char ext[], char filename[]);
int createTextFile(char *filename, SpiceDouble n_iter, SpiceDouble t[], SpiceDouble **pos_hci, SpiceDouble **pos_iau_sun, SpiceDouble **pos_hee, SpiceDouble lon_hci[], SpiceDouble lat_hci[], SpiceDouble lon_iau_sun[], SpiceDouble lat_iau_sun[], SpiceDouble dist[]);
int time2DDtime(double et_time, dd_time_t *DDtime);
void nc_handle_error(int status, char* operation);
int createNc(char* ncfile, SpiceDouble n_iter, SpiceDouble t[], SpiceDouble *pos_hci[3], SpiceDouble *pos_iau_sun[3], SpiceDouble *pos_hee[3], SpiceDouble lon_hci[], SpiceDouble lat_hci[], SpiceDouble lon_iau_sun[], SpiceDouble lat_iau_sun[], SpiceDouble dist[]);
int printPositions(SpiceInt n_iter, SpiceDouble t[], SpiceDouble **pos);

/*Compiling command : gcc earth_ephemeris.c -o earth_ephemeris -I/home/arnaud/WORK/TOOLS/LIB/cspice/include /home/arnaud/WORK/TOOLS/LIB/cspice/lib/cspice.a /home/arnaud/WORK/TOOLS/LIB/libnetcdf.so.7 -lm*/

int main(int argc, char const *argv[])
{
	//Local variables
	SpiceDouble et_0;
	SpiceDouble et_end;
	SpiceDouble *t = NULL;
	SpiceDouble **pos_hci = NULL;
	SpiceDouble **pos_iau = NULL;
	SpiceDouble **pos_hee = NULL;
	SpiceDouble *lon_iau = NULL;
	SpiceDouble *lat_iau = NULL;
	SpiceDouble *lon_hci = NULL;
	SpiceDouble *lat_hci = NULL;
	SpiceDouble *distance = NULL;
	SpiceInt n;
	SpiceInt i;
	char START_DATE[50]; 
	char STOP_DATE[50]; 
	char text_filename[50];
	char nc_filename[50];


	//Load specific kernels : leap years, planets and satellites objects, planetary constants and specific heliospheric frame (to use HCI frame) 
	if (!loadKernels(KERNELS))
	{
		printf("[ERROR] \"%s\" kernel list : no such file\n", KERNELS);
		exit(EXIT_FAILURE);
	}

	//Configure boundaries
	strcpy(START_DATE, argv[4]);
	strcpy(STOP_DATE, argv[5]);
	n = getBoundaries(START_DATE, &et_0, STOP_DATE, &et_end);
	if (n <= 0)
	{
		printf("[ERROR] Enable to get a date interval to compute Earth ephemeris.\n");

		exit(EXIT_FAILURE);
	}

	//Memory allocation
	t = (SpiceDouble*) malloc(n*sizeof(SpiceDouble));
	pos_hci = (SpiceDouble**) malloc(n*sizeof(SpiceDouble*));
	pos_iau = (SpiceDouble**) malloc(n*sizeof(SpiceDouble*));
	pos_hee = (SpiceDouble**) malloc(n*sizeof(SpiceDouble*));
	distance = (SpiceDouble*) malloc(n*sizeof(SpiceDouble));
	lon_hci = (SpiceDouble*) malloc(n*sizeof(SpiceDouble));
	lat_hci = (SpiceDouble*) malloc(n*sizeof(SpiceDouble));
	lon_iau = (SpiceDouble*) malloc(n*sizeof(SpiceDouble));
	lat_iau = (SpiceDouble*) malloc(n*sizeof(SpiceDouble));

	if ((t == NULL) || (pos_hci == NULL) || (distance == NULL) || (lon_hci == NULL) || (lat_hci == NULL) || (lon_iau == NULL) || (lat_iau == NULL) )  
	{
		printf("[ERROR] Memory allocation has failed. Not enough memory.\n");

		exit(EXIT_FAILURE);
	}
	else
	{
		for (i = 0; i < n; i++)
		{
			
			pos_hci[i] = NULL;
			pos_hci[i] = (SpiceDouble*) malloc(3*sizeof(SpiceDouble));
			pos_iau[i] = NULL;
			pos_iau[i] = (SpiceDouble*) malloc(3*sizeof(SpiceDouble));
			pos_hee[i] = NULL;
			pos_hee[i] = (SpiceDouble*) malloc(3*sizeof(SpiceDouble));

			if ((pos_hci[i] == NULL) || (pos_iau[i] == NULL) || (pos_hee[i] == NULL))
			{
				printf("[ERROR] Position : Memory allocation has failed.\n");
			}
		}
	}
	
	//Compute n positions of the celestial body wanted starting from et_0 epoch with a STEP step
	getPositions(TARGET, et_0, STEP, n, FRAME1, ABCORR, OBSERVER, t, pos_hci);
	getPositions(TARGET, et_0, STEP, n, FRAME2, ABCORR, OBSERVER, t, pos_iau);
	getPositions(TARGET, et_0, STEP, n, FRAME3, ABCORR, OBSERVER, t, pos_hee);

	//Compute longitudes and latitudes
	getLonLat(pos_hci, lon_hci, lat_hci, n);
	getLonLat(pos_iau, lon_iau, lat_iau, n);

	//Compute n distances of the celestial body wanted relatively with the Sun
	getDistance(n, pos_hci, distance);

	//Print celestial body positions for the time interval
	//printPositions(t, pos);

	//Get files name
	//getFilesName(BODY_NAME, START_DATE, ".txt", text_filename);
	getFilesName(BODY_NAME, START_DATE, ".nc", nc_filename);

	//Write celestial body positions into "earth.txt" file
	//createTextFile(text_filename, n, t, pos_hci, pos_iau, pos_hee, lon_hci, lat_hci, lon_iau, lat_iau, distance);

	//Write celestial body positions into "earth.nc" file
	createNc(nc_filename, n, t, pos_hci, pos_iau, pos_hee, lon_hci, lat_hci, lon_iau, lat_iau, distance);

	//Free memory
	free(t);
	free(distance);
	for (i = 0; i < n; i++)
	{
		free(pos_hci[i]);
		pos_hci[i] = NULL;
		free(pos_iau[i]);
		pos_iau[i] = NULL;
		free(pos_hee[i]);
		pos_hee[i] = NULL;
	}
	free(pos_hci);
	pos_hci = NULL;
	free(pos_iau);
	pos_iau = NULL;
	free(pos_hee);
	pos_hee = NULL;

	free(lon_hci);
	lon_hci = NULL;

	free(lat_hci);
	lat_hci = NULL;

	free(lon_iau);
	lon_iau = NULL;

	free(lat_iau);
	lat_iau = NULL;

	return 0;
}


//FUNCTION : loadKernels
int loadKernels(char *kernelsList)
{
	furnsh_c(kernelsList);

	return 1;
}

//FUNCTION : getBoundaries
int getBoundaries(char *et_0_str, SpiceDouble *et_0, char *et_end_str, SpiceDouble *et_end)
{
	SpiceInt n;

	str2et_c(et_0_str, et_0);
	str2et_c(et_end_str, et_end);
	n = (int)((*et_end-*et_0)/STEP);

	return n;
}

//FUNCTION : getPositions
int getPositions(const char *target, SpiceDouble et_0, SpiceInt step, SpiceInt n_iter, const char *frame, const char *ab_corr, const char *observer, SpiceDouble t[], SpiceDouble **positions)	 
{
	SpiceInt i;
	SpiceInt j;
	SpiceDouble light_time;

	for (i = 0; i < n_iter; i++)
	{
		t[i]  =  et_0 + i*step;

		spkpos_c(target, t[i], frame, ab_corr, observer,  positions[i],  &light_time);

		for (j = 0; j < 3; j++)
		{
			positions[i][j] /= AU;
		}
		
	}

	return 1;
}


//FUNCTION : getLonLat
int getLonLat(SpiceDouble **earth_pos, SpiceDouble lon[], SpiceDouble lat[], int n)
{
	SpiceInt i;
	SpiceDouble radius;
	SpiceDouble longitude;
	SpiceDouble latitude;

	for (i = 0; i < n; i++)
	{
		reclat_c(earth_pos[i], &radius, &longitude, &latitude);

		lon[i] = longitude;
		lat[i] = latitude;
	}
                   
	return 0;
}


//FUNCTION : getFilesName
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


//FUNCTION : getDistance
int getDistance(SpiceInt n_iter, SpiceDouble **positions, SpiceDouble dist[])
{
	SpiceInt i;

	for (i = 0; i < n_iter; i++)
	{
		dist[i] = sqrt(positions[i][0]*positions[i][0] + positions[i][1]*positions[i][1] + positions[i][2]*positions[i][2]);
	}
}

//FUNCTIONS : printPositions
int printPositions(SpiceInt n_iter, SpiceDouble t[], SpiceDouble **pos)
{
	SpiceInt i;

	for (i = 0; i < n_iter; i++)
	{
		printf("================================================================\n\n");
		printf( "\tTime = %.0f hour(s)\n\n", t[i]/3600);
		printf( "\tX (km) = %f\n", pos[i][0]);
		printf( "\tY (km) = %f\n", pos[i][1]);
		printf( "\tZ (km) = %f\n\n", pos[i][2]);
		printf("================================================================\n\n");
	}
}

//FUNCTION : writePositions
int createTextFile(char *filename, SpiceDouble n_iter, SpiceDouble t[], SpiceDouble **pos_hci, SpiceDouble **pos_iau_sun, SpiceDouble **pos_hee, SpiceDouble lon_hci[], SpiceDouble lat_hci[], SpiceDouble lon_iau_sun[], SpiceDouble lat_iau_sun[], SpiceDouble dist[])
{
	FILE *f = NULL;
	SpiceInt i;

	if ((f = fopen(filename, "w")) != NULL)
	{
		for (i = 0; i < n_iter; i++)
		{
			fprintf(f, "%.0f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n", t[i], pos_hci[i][0], pos_hci[i][1], pos_hci[i][2], pos_iau_sun[i][0], pos_iau_sun[i][1], pos_iau_sun[i][2], pos_hee[i][2], pos_hee[i][2], pos_hee[i][2], dist[i], lon_hci[i], lat_hci[i], lon_iau_sun[i], lat_iau_sun[i]);
		}

		printf("[INFO] %s has been created\n", filename);

		fclose(f);
	}
	else
	{
		printf("[ERROR] Enable to open %s file\n", filename);
		exit(EXIT_FAILURE);
	}
}



//FUNCTION : time2DDtime
int time2DDtime(double et_time, dd_time_t *DDtime)
{
	char *date_unix_zero = "1970-01-01T00:00:00";
	SpiceDouble et_unix_zero;

	str2et_c(date_unix_zero, &et_unix_zero);

	strcpy(*DDtime, Double2DD_Time(et_time-et_unix_zero));
	
	return 0;
} 




//FUNCTION : nc_handle_error
void nc_handle_error(int status, char* operation) {

	printf("[ERROR] %s : %s\n", operation, nc_strerror(status));
	exit(ERRCODE);
}

//FUNCTION : createNc
int createNc(char* ncfile, SpiceDouble n_iter, SpiceDouble t[], SpiceDouble *pos_hci[3], SpiceDouble *pos_iau_sun[3], SpiceDouble *pos_hee[3], SpiceDouble lon_hci[], SpiceDouble lat_hci[], SpiceDouble lon_iau_sun[], SpiceDouble lat_iau_sun[], SpiceDouble dist[])
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
	int position1_varid;
	int position2_varid;
	int position3_varid;
	int longitude_hci_varid;
	int latitude_hci_varid;
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
    dd_time_t *DDdates = NULL;

	//Other
	int step;
	int retval;
	size_t i;
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
	retval = nc_def_var(ncid, POS_VAR_HCI, NC_DOUBLE, 2, data_tab_dimid, &position1_varid);
  	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "HCI Position variable");
	} 
	retval = nc_def_var(ncid, POS_VAR_IAU, NC_DOUBLE, 2, data_tab_dimid, &position2_varid);
  	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "IAU_SUN Position variable");
	}
	retval = nc_def_var(ncid, POS_VAR_HEE, NC_DOUBLE, 2, data_tab_dimid, &position3_varid);
  	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "HEE Position variable");
	}
	retval = nc_def_var(ncid, LON_VAR_HCI, NC_DOUBLE, 1, &time_dimid, &longitude_hci_varid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "LON HCI variable");
	}
	retval = nc_def_var(ncid, LAT_VAR_HCI, NC_DOUBLE, 1, &time_dimid, &latitude_hci_varid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "LAT HCI variable");
	} 
	retval = nc_def_var(ncid, LON_VAR_IAU_SUN, NC_DOUBLE, 1, &time_dimid, &longitude_iau_varid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "LON IAU_SUN variable");
	}
	retval = nc_def_var(ncid, LAT_VAR_IAU_SUN, NC_DOUBLE, 1, &time_dimid, &latitude_iau_varid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "LAT IAU_SUN variable");
	}
	retval = nc_def_var(ncid, DIST_VAR, NC_DOUBLE, 1, &time_dimid, &distance_varid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Distance variable");
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
	retval = nc_put_att_text(ncid, position1_varid, "units", strlen(POS_UNIT), POS_UNIT);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Position unit");
	}
	retval = nc_put_att_text(ncid, position2_varid, "units", strlen(POS_UNIT), POS_UNIT);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Position unit");
	}
	retval = nc_put_att_text(ncid, position3_varid, "units", strlen(POS_UNIT), POS_UNIT);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Position unit");
	}
	retval = nc_put_att_text(ncid, distance_varid, "units", strlen(DIST_UNIT), DIST_UNIT);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Distance unit");
	}
	retval = nc_put_att_text(ncid, longitude_hci_varid, "units", strlen(LON_UNIT), LON_UNIT);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Longitude unit");
	}
	retval = nc_put_att_text(ncid, latitude_hci_varid, "units", strlen(LAT_UNIT), LAT_UNIT);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Latitude unit");
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
	DDdates = (dd_time_t*)malloc((int)n_iter*sizeof(dd_time_t));
	if(DDdates == NULL)
	{
		printf("[ERROR] Unable to build DD dates\n");

		exit(EXIT_FAILURE);
	}
	
	for (i = 0; i < n_iter; i++)
	{
		time2DDtime(t[i], &DDdates[i]);
	}

	//Write datas
    for (i = 0; i < n_iter; i++)
    {
    	start[0] = i;
	    retval = nc_put_vara_text(ncid, time_varid, start, timeCount, DDdates[i]);
		if (retval != NC_NOERR)
		{
			nc_handle_error(retval, "Write time variable");
		}
    }

	for (i = 0; i < n_iter; i++)
	{
		start[0] = i;
		retval = nc_put_vara_double(ncid, position1_varid, start, dataCount, pos_hci[i]);
	 	if (retval != NC_NOERR)
		{
			nc_handle_error(retval, "Write HCI position variable");
		}
	}

	for (i = 0; i < n_iter; i++)
	{
		start[0] = i;
		retval = nc_put_vara_double(ncid, position2_varid, start, dataCount, pos_iau_sun[i]);
	 	if (retval != NC_NOERR)
		{
			nc_handle_error(retval, "Write IAU_SUN position variable");
		}
	}

	for (i = 0; i < n_iter; i++)
	{
		start[0] = i;
		retval = nc_put_vara_double(ncid, position3_varid, start, dataCount, pos_hee[i]);
	 	if (retval != NC_NOERR)
		{
			nc_handle_error(retval, "Write HEE position variable");
		}
	}

	retval = nc_put_var_double(ncid, longitude_hci_varid, lon_hci);
 	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Write LON HCI variable");
	}

	retval = nc_put_var_double(ncid, latitude_hci_varid, lat_hci);
 	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Write LAT HCI variable");
	}

	retval = nc_put_var_double(ncid, longitude_iau_varid, lon_iau_sun);
 	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Write LON IAU_SUN variable");
	}

	retval = nc_put_var_double(ncid, latitude_iau_varid, lat_iau_sun);
 	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Write LAT IAU_SUN variable");
	}

	retval = nc_put_var_double(ncid, distance_varid, dist);
 	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Write distance variable");
	}

	retval = nc_put_var_text(ncid, startTime_varid, DDdates[0]);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Write start time variable");
	}

	retval = nc_put_var_text(ncid, stopTime_varid, DDdates[(int)n_iter-1]);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Write stop time variable");
	}

	//Free memory
	free(DDdates);

	/************************ END OF WRITING MODE ********************************/



	/**************** CLOSE NC FILE ***********************/
	//Close the file
	retval = nc_close(ncid);
	if (retval != NC_NOERR)
	{
		nc_handle_error(retval, "Close nc file");
	}

	printf("[INFO] %s has been created\n", ncfile);
	/**************** END OF CLOSE NC FILE ***********************/

	return 0;
}