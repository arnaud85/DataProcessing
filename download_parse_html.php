<?php

/******************************************************************************************
Author 			: 			Arnaud BIEGUN
Laste update 	: 			04/12/2014
Brief 			: 			Download PDS .LBL and .TAB files if needed comparing with 
							the last update of these files
*******************************************************************************************/


/**********************************************************************************
						GETPDSLASTMODIFIEDMONTHINT

Brief : Convert a 3 letters string month into a number in order to compare
		somes dates
arg   : string month
***********************************************************************************/
function getPDSLastModifiedMonthInt($month) {

	$months = array(

		'Jan' => 1,
		'Feb' => 2,
		'Mar' => 3,
		'Apr' => 4,
		'May' => 5,
		'Jun' => 6,
		'Jul' => 7,
		'Aug' => 8,
		'Sep' => 9,
		'Oct' => 10,
		'Nov' => 11,
		'Dec' => 12,
	);

	foreach ($months as $str_month => $int_month) {
		
		if ($str_month == $month) {
			
			$month_number = $int_month;

			return $month_number;
		}
	}
}


/**********************************************************************************
						buildPDSLASTMODIFIEDDATESTR

Brief : Build a string date with the format 'day/month/year' (just a number for
		these 3 arguments, NO ZEROS!!) 
arg   : PDS date with the format : dd MON yyyy (month is a 3 letters string
		abreviation)
***********************************************************************************/
function buildPDSLastModifiedDateSTR($date) {

	$day = substr($date, 5, 2);
	if (substr($day, 0, 1) == '0') {
		
		$day = substr($day, 1, 1);
	}
	$month = substr($date, 8, 3);
	$month = getPDSLastModifiedMonthInt($month);
	$year = substr($date, 12, 4);

	return $day."/".$month."/".$year;
}


/**********************************************************************************
						getPDSLASTMODIFIEDYOUNGERDATE

Brief : Compare 2 dates and return the younger of these
arg1  : Date 1
arg2  : Date 2
***********************************************************************************/
function getPDSLastModifiedYoungerDate($PDS_formated_date, $local_last_update) {

	$PDS_date = explode("/", $PDS_formated_date);
	$day = $PDS_date[0];
	$month = $PDS_date[1];
	$year = $PDS_date[2];

	$LOCAL_date = explode("/", $local_last_update);
	$d = $LOCAL_date[0];
	$m = $LOCAL_date[1];
	$y = $LOCAL_date[2];

	if ($year > $y) {
		
		$youngerDate = $PDS_formated_date;

	} else {

		if ($year == $y) {
			
			if ($month > $m) {
				
				$youngerDate = $PDS_formated_date;

			} else {

				if ($month == $m) {

					if ($day > $d) {
						
						$youngerDate = $PDS_formated_date;

					} else {

						if ($day == $d) {
							
							$youngerDate = $PDS_formated_date;

						} else {

							$youngerDate = $local_last_update;
						}
					}


				} else {

					$youngerDate = $local_last_update;
				}
			}

		} else {

			$youngerDate = $local_last_update;
		}

	}

	return $youngerDate;
}





/**********************************************************************************
						GETTIMESTAMP

Brief 	: 	Get local timestamp to know the last update of a dataset
arg1	:	Path to the dataset directory
***********************************************************************************/
function getTimestamp($dataset_directory)
{
	$timestamp_file = fopen($dataset_directory."/timestamp.txt", 'r');
	
	if ($timestamp_file !=  FALSE)
	{
		$timestamp = fgets($timestamp_file); 

		fclose($timestamp_file);

		return $timestamp;
	}
	else
	{
		die("[ERROR] Unable to read $dataset_directory/timestamp.txt\n");
	}
}





/**********************************************************************************
						SETTIMESTAMP

Brief 	: 	Set local timestamp into the timestamp.txt file
arg1	:	Timestamp
arg2	:	Path to the dataset directory
***********************************************************************************/
function setTimestamp($timestamp, $dataset_directory)
{
	$timestamp_file = fopen($dataset_directory."/timestamp.txt", 'w');
	
	if ($timestamp_file !=  FALSE)
	{
		fseek($timestamp_file, 0); 
		fputs($timestamp_file, $timestamp); 

		fclose($timestamp_file);

		return $timestamp;
	}
	else
	{
		die("[ERROR] Unable to read $dataset_directory/timestamp.txt\n");
	}
}





/**********************************************************************************
						GETPDSFILES

Brief 	: 	Download .LBL and .TAB files on the PDS website if these files are updated
			comparing with the same fiels conatined in the local database
arg1   	: 	Path to the PDS volume on the PDS website
arg2	: 	Date of last update for the local database
arg3	:	Directory to save files
***********************************************************************************/
function getPDSFiles($URL, $timeStamp, $ouputDir) {

	// Create DOM from URL or file
	$PDS_ROOT = "http://ppi.pds.nasa.gov";
	$html = file_get_html($PDS_ROOT.$URL);
	if (!is_object($html)){
	
		return;
	}

	//For all 'tr' element
	foreach($html->find('tr') as $element) {

		// Find data file last update and compare it with our last update time
		$PDS_date = $element->last_child()->first_child()->innertext;
		$PDS_date = buildPDSLastModifiedDateSTR($PDS_date);
		$youngerDate = getPDSLastModifiedYoungerDate($PDS_date, $timeStamp);
		//echo "PDS DATE : $PDS_date\n";
		//echo "LOCAL DATE : $timeStamp\n";
		//echo "YOUNGER DATE : $youngerDate\n\n";

		//Find data file link to download it if needed
		$link = $element->first_child()->first_child()->href;
		//echo "LINK : $link\n";

		// Find data file link
		if ($youngerDate == $PDS_date) {
			
			if ($link[strlen($link)-1] == '/' && $link != $PDS_ROOT) {
				
				getPDSFiles($link, $timeStamp, $ouputDir);

			}else {

				exec("wget -N -P ".$ouputDir." ".$PDS_ROOT.$link);
				//echo "[INFO] Downloading $PDS_ROOT$link\n";

				setTimestamp($youngerDate, $ouputDir);
			}

		}else {

			if (!empty($link)) {
				
				echo "[INFO] $PDS_ROOT$link is on date\n";
			}
			
		}
		
	}
}




/***********************************************************************************
								MAIN
************************************************************************************/
require_once 'simple_html_dom.php';

if ($argc != 3) {
	
	die("[ERROR] Usage : php download_parse_html.php (PDS_VOLUME) (LOCAL_DIR_PATH)\n\n");
}
		
$PDS_volume = $argv[1];
$ouputDir = $argv[2];

$URL = "/archive1/$PDS_volume/DATA/";
	
//Our local last update with format : d/m/y (just number, no zeros !!!)
$timestamp = getTimestamp($ouputDir);	
echo "[INFO] Last update : $timestamp\n";

getPDSFiles($URL, $timestamp, $ouputDir);


?>