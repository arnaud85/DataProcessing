<?php 
/******************************************************************************************
Author 			: 			Arnaud BIEGUN
Laste update 	: 			04/12/2014 (V1.0)
Brief 			: 			Download PDS .LBL and .TAB files if needed comparing with 
							the last update of these files
*******************************************************************************************/



/**********************************************************************************
						GETMONTHNUMBER

Brief : Convert a 3 letters string month into a number in order to compare
		somes dates
arg   : string month
***********************************************************************************/
function getMonthNumber($month) 
{

	$months = array(

		'Jan' => '01',
		'Feb' => '02',
		'Mar' => '03',
		'Apr' => '04',
		'May' => '05',
		'Jun' => '06',
		'Jul' => '07',
		'Aug' => '08',
		'Sep' => '09',
		'Oct' => '10',
		'Nov' => '11',
		'Dec' => '12',
	);

	foreach ($months as $str_month => $int_month) {
		
		if ($str_month == $month) {
			
			$month_number = $int_month;

			return $month_number;
		}
	}
}


/******************************************************************************************************************************
						FORMATPDSDATE

Brief : Convert a string date into an UNIX timestamp
arg   : PDS date with format : yyyy/mm/dd HH:MM:SS
*******************************************************************************************************************************/

function FormatPDSDate($PDS_Date)
{
	$day = substr($PDS_Date, 5, 2);
	$str_month = substr($PDS_Date, 8, 3);
	$month = getMonthNumber($str_month);
	$year = substr($PDS_Date, 12, 4);
	$hour = substr($PDS_Date, 17, 2);
	$minute = substr($PDS_Date, 20, 2);
	$second = substr($PDS_Date, 23, 2);
	
	$Formatted_PDS_date = "$year/$month/$day $hour:$minute:$minute";

	return $Formatted_PDS_date;
}




/******************************************************************************************************************************
						MAKETIMESTAMP

Brief : Convert a string date into an UNIX timestamp
arg   : PDS date with format : yyyy/mm/dd HH:MM:SS
*******************************************************************************************************************************/

function makeTimestamp($date)
{
	$year = substr($date, 0, 4);
	$month = substr($date, 5, 2);
	$day = substr($date, 8, 2);
	$hour = substr($date, 11, 2);
	$minute = substr($date, 14, 2);
	$second = substr($date, 17, 2);

	$timestamp = mktime($hour, $minute, $second, $month, $day, $year);

	return $timestamp;
}




/**********************************************************************************
						GETLOCALUPDATE

Brief 	: 	Get local update of the dataset considered
arg1	:	Path to the dataset directory
***********************************************************************************/
function getLocalUpdate($dataset_directory)
{
	$date_file = fopen($dataset_directory."/timestamp.txt", 'r');
	
	if ($date_file !=  FALSE)
	{
		$last_update = fgets($date_file); 

		fclose($date_file);

		return $last_update;
	}
	else
	{
		die("[ERROR] Unable to read $dataset_directory/timestamp.txt\n");
	}
}

/**********************************************************************************
						SETLASTUPDATE

Brief 	: 	Set local timestamp into the timestamp.txt file
arg1	:	Last update
arg2	:	Path to the dataset directory
***********************************************************************************/
function setLastUpdate($lastUpdate, $dataset_directory)
{
	$timestamp_file = fopen($dataset_directory."/timestamp.txt", 'w');
	
	if ($timestamp_file !=  FALSE)
	{
		fseek($timestamp_file, 0); 
		fputs($timestamp_file, $lastUpdate); 

		fclose($timestamp_file);
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
arg2	:	Directory to save files
***********************************************************************************/
function getPDSFiles($URL, $localTimestamp, $ouputDir) {

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
		$str_size = strlen($PDS_date);
		$PDS_timestamp = 0;

		if ($str_size < 30) 
		{
			// echo "[INFO] PDS_date size : $str_size\n";
			// echo "[INFO] PDS_date : $PDS_date\n";

			$formatted_PDS_date = FormatPDSDate($PDS_date);
			echo "[INFO] PDS date : $formatted_PDS_date\n";

			$PDS_timestamp = makeTimestamp($formatted_PDS_date);
			// echo "[INFO] PDS timestamp : $PDS_timestamp\n";
		}
		

		//Find data file link to download it if needed
		$link = $element->first_child()->first_child()->href;

		// Find data file link
		if ($PDS_timestamp > $localTimestamp) 
		{
			if ($link[strlen($link)-1] == '/' && $link != $PDS_ROOT) {
				
				getPDSFiles($link, $localTimestamp, $ouputDir);

			}else {

				exec("wget -N -P ".$ouputDir." ".$PDS_ROOT.$link);
				//echo "[INFO] Downloading $PDS_ROOT$link\n";

				setLastUpdate($formatted_PDS_date, $ouputDir);
			}

		}else {

			if (!empty($link)) {
				
				echo "[INFO] $PDS_ROOT$link is on date\n\n";
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

// Dataset environment
$PDS_volume = $argv[1];
$ouputDir = $argv[2];
$URL = "/archive1/$PDS_volume/DATA/";

// Check local timestamp
$localUpdate = getLocalUpdate($ouputDir);
echo "[INFO] LOCAL UPDATE : $localUpdate\n\n";
$localTimestamp = makeTimestamp($localUpdate);
// echo "[INFO] Local timestamp : $localTimestamp\n\n";

getPDSFiles($URL, $localTimestamp, $ouputDir);

?>