<?php


/********************************************************************  

                            DAYS2MONTHDAY

Return the month day of a day defined by a number
*********************************************************************/
function days2MonthDay($oldDate) { 

  $year = substr($oldDate, 0, 4);
  $day =  substr($oldDate, 4, 3);
  $hour = substr($oldDate, 7, 2);
  $min = substr($oldDate, 9, 2);
  $sec = substr($oldDate, 11, 2);
  $msec = substr($oldDate, 13, 3);
  $newDate = date("Y-m-d", strtotime("+$day days",strtotime("$year-01-01")))."T".$hour.":".$min.":".$sec.".".$msec."Z";

  return $newDate;

 } 



/********************************************************************  

                            UPDATE TAG

Update the tag value into an XML file
*********************************************************************/
function updateTag($dom, $tagName, $tagValue){

  $vi = $dom->getElementsByTagName("VI");
  $Old = $vi->item(0)->getElementsByTagName($tagName)->item(0);
  $New = $dom->createElement($tagName, $tagValue);
  $vi->item(0)->replaceChild($New, $Old);
 
}




/********************************************************************  

                            UPDATEINFOFILE

Update info files :

- prefix_info.xml
- prefix_info.nc
*********************************************************************/
function updateInfoFile($data_path, $prefix) {

  chdir($data_path);

  $infoXML = $prefix."_info.xml"; 
  $infoNC = $prefix."_info.nc"; 
  $timesNC = $prefix."_times.nc";

  $dom = new DomDocument();  
  $dom->load($infoXML); 
         
  $res = system("StartStopLocal"." ".$timesNC);  
  $time = explode("-",$res);

  //Start   
  $start = days2MonthDay($time[0]);
  updateTag($dom, 'GlobalStart', $start);
  updateTag($dom, 'LocalStart', $start);        
                             
  //Stop             
  $stop = days2MonthDay($time[1]);
  updateTag($dom, 'GlobalStop', $stop);
  updateTag($dom, 'LocalStop', $stop);     
         
  $dom->save($infoXML);

  $cmd = "infoLocal2nc"." ".$infoXML." ".$infoNC;
  exec($cmd, $outval, $retval);      

  return $retval;
} 



/********************************************************************  

                            MAKEXMLINFO

Create the prefix_info.xml file (called in makeInfosFiles function)
      
*********************************************************************/


function makeXmlInfo($mission, $instrument, $prefix, $sampling) {
                      
  $xml = new DomDocument("1.0", "utf-8");

  $root_element = $xml->createElement("VI");
  $mission_element = $xml->createElement("Mission", $mission);
  $instrument_element = $xml->createElement("Instrument", $instrument);
  $globalStart_element = $xml->createElement("GlobalStart");
  $globalStop_element = $xml->createElement("GlobalStop");
  $sampling_element = $xml->createElement("MinSampling", $sampling);
  $localStart_element = $xml->createElement("LocalStart");
  $localStop_element = $xml->createElement("LocalStop");
  
  $root_element->appendChild($mission_element);
  $root_element->appendChild($instrument_element);
  $root_element->appendChild($globalStart_element);
  $root_element->appendChild($globalStop_element);
  $root_element->appendChild($sampling_element);
  $root_element->appendChild($localStart_element);
  $root_element->appendChild($localStop_element);
  $xml->appendChild($root_element);

  $xml->preserveWhiteSpace = false;
  $xml->formatOutput = true;
  
  $xml->save($prefix."_info.xml");
}


/********************************************************************  

                            MAKEINFOSFILES

      Create : 
            
            - prefix_info.xml
            - prefix_info.nc
            - prefix_tiles.nc
            - refer.nc
            
*********************************************************************/
function makeInfosFiles($data_path, $lib_path, $prefix, $mission, $instrument, $sampling) {


  chdir($data_path);

  $times = $prefix."_times.nc";
  $info_nc = $prefix."_info.nc";
  $info_xml = $prefix."_info.xml";
  $cache = $prefix."_cache.nc";

  //Times file : NetCDF
  if (!file_exists($times)) {

    $cmd = "TimesUpdate -r"." ".$times." ".$prefix."_[0-9]*.nc";

    exec($cmd, $outval, $retval);

    if ($retval == 0) {
      
      echo "$times has been created\n";

    } else {

      echo "[ERROR] Errors creating $times\n";
    }

  } else {

    $cmd = "TimesUpdate -u"." ".$times." ".$prefix."_[0-9]*.nc";

    exec($cmd, $outval, $retval);

    if ($retval == 0) {
      
      echo "$times has been updated\n";

    } else {

      echo "[ERROR] Errors updating $times\n";
    }
  }


  //Info files : XML + NetCDF
  if (!file_exists($info_xml)) {

    makeXmlInfo($mission, $instrument, $prefix, $sampling);

    echo "$info_xml has been created\n";
  }  

  if (!file_exists($info_nc)) {

    $updated = updateInfoFile($data_path, $prefix);

    if ($updated !== 0) {

      echo "$info_nc has been created\n";

    } else {

      echo "[ERROR] Failed to create $info_nc\n";
    }

  } else {

    $updated = updateInfoFile($data_path, $prefix);

    if ($updated !== 0) {

      echo "$info_nc has been updated\n";

    } else {

      echo "[ERROR] Failed to update $info_nc\n";
    }

  }


  //Cache files : NectCDF + "clean"
  if (!file_exists("clean")) {

    if (copy ($lib_path."/"."Cash.template", $cache)) {
      
      echo "$cache has been created\n";

    } else {

      echo "[ERROR] Failed during creation of $cache\n";
    }

    if (copy ($lib_path."/"."Clean.template", "clean")) {
      
      echo "\"clean\" file has been created\n";

    } else {

      echo "[ERROR] Failed during creation of \"clean\" file\n";
    } 

    $SED = "sed -i -r 's/NAME/".$prefix."/g'"." ".$data_path."/"."clean";
    exec($SED, $outval_sed, $retval_sed);
    $chmod = "chmod ugo+x"." ".$data_path."/"."clean";
    exec($chmod, $outval_chmod, $retval_chmod);
    
    if ($retval_sed == 0 && $retval_chmod == 0) {

      echo "\"clean\" file has been configured\n";

    } else {

      echo "[ERROR] Failed during configuration of \"clean\" file\n";
    }
  
  }

  exec("./clean");

}








/***********************************************************************
                                  MAIN
***********************************************************************/


//Create files : info, time and cache
if ($argc != 7)
{
  die("[ERROR] Usage :\n\n  php setInfoFiles.php datapath DDlibPath prefix mission instrument resolution\n\n"); 
}
else
{
  // print_r($argv);
  makeInfosFiles($argv[1], $argv[2], $argv[3], $argv[4], $argv[5], $argv[6]);

  //Get infos from DDsys.xml file and create refer.nc file
  exec("makeDDsys");
}


?>