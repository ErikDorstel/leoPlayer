<?php

$dataPath="../data/"; $storeHours="120";

header('Content-type: text/html');
$now=time(); $hour=$now-($now%3600);

if (isset($_POST['update'])) {
  // write data to file
  file_put_contents($dataPath.$hour.".data",$now.",".$_POST["update"]."\n",FILE_APPEND);
  file_put_contents($dataPath."status.data",$now.",".$_POST["update"]);
  // reply command
  if (file_exists($dataPath."command.data")) {
    echo file_get_contents($dataPath."command.data"); unlink($dataPath."command.data"); } }
else if (isset($_POST['command'])) {
  // store command
  file_put_contents($dataPath."command.data",$_POST["command"]); }
else if (isset($_POST['status'])) {
  // reply status
  if (file_exists($dataPath."status.data")) {
    echo file_get_contents($dataPath."status.data"); } }
else if (isset($_POST['index'])) {
  // reply index
  if (file_exists($dataPath."index.data")) {
    echo file_get_contents($dataPath."index.data"); }
  // clear command
  if (file_exists($dataPath."command.data")) {
    unlink($dataPath."command.data"); } }
else {
  // read data files
  for ($x=$storeHours-1;$x>=0;$x--) { $file=$dataPath.($hour-($x*3600)).".data";
    if (file_exists($file)) { echo file_get_contents($file); } }
  // delete data files
  $dobj=dir($dataPath); while (($name=$dobj->read())!==false) {
    if (!is_dir($dataPath.$name)) { $name=explode(".",$name)[0];
      if ($name<($hour-(($storeHours-1)*3600)) && is_numeric($name)) { unlink($dataPath.$name.".data"); } } } }

?>
