#!/usr/bin/env php
<?php
$tellscript = "~/bin/setadiumstatus.scpt";
$url = 'http://www.weather.gov/xml/current_obs/KMSP.xml';

$file = @file_get_contents($url);
if (!$file) {
    error_log(basename(__FILE__) . " cannot get $url", 0);
    exit(1);
}

$xml = (array)simplexml_load_string($file);
if (!$xml) {
    error_log(basename(__FILE__) . " cannot parse xml", 0);
    exit(2);
}

$otime = $xml['observation_time'];
$tempc = (int) $xml['temp_c'];
$tempf = (int) $xml['temp_f'];
$station = $xml['station_id'];

list(, $otime) = explode(',', $otime);

// $statmsg = sprintf("%dF (%dC) %s at %s", $tempf, $tempc, trim($station), trim($otime));
$statmsg = sprintf("%dF/%dC Minneapolis %s ", $tempf, $tempc, trim($otime));

echo "Status msg: '$statmsg'\n"; /* DEBUG */

exec("/usr/bin/osascript $tellscript '$statmsg'");
?>
