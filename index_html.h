const char *index_html=R"(
<!DOCTYPE html>
<html lang="de"><head>
<title>leoPlayer Credentials</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<meta charset="utf-8">
<style type="text/css">
body    { font-family:Arial,sans-serif; font-size:1em; font-weight:500; color:#000000; background-color:#b0b0b0;
          padding:0em; margin:0em; border:0em; }
div     { text-decoration:none; user-select:none; display:inline-block; width:100%; text-align:center; }
input   { padding:0.5em; margin:0em; border:0em; width:calc(100% - 2em); font-size:1em; font-weight:500; }
.title  { padding:0.5em; margin:0.2em; background-color:#c2d5ed; width:calc(100% - 2em); }
.text   { padding:0.5em; margin:0.2em 0.2em 0em; background-color:#e0e0e0; width:calc(100% - 2em); text-align:left; }
.input  { padding:0.5em; margin:0em 0.2em 0.2em; background-color:#ffffff; width:calc(100% - 2em); text-align:left; }
.button { padding:0.5em; margin:0.2em; background-color:#c2d5ed; width:calc(40% - 2em); border-radius:1em; cursor:pointer; }
</style>
<script>

function initCredentials() {
  red="#E09090"; green="#90E090"; yellow="#FFE460"; gray="#f0f0f0"; blue="#c2d5ed";
  id("section3").style.display="none"; urlRedirect=""; }

function sendCredentials() {
  requestAJAX("credentials," + a2h(id("ssid").value) + "," + a2h(id("secret").value)); }

function requestAJAX(value) {
  ajaxObj=new XMLHttpRequest; ajaxObj.open("GET",value,false);
  ajaxObj.setRequestHeader("Content-Type","application/x-www-form-urlencoded");
  ajaxObj.send();
  if (ajaxObj.responseText.startsWith("OK,")) {
    id("section1").style.display="none";
    id("section2").style.display="none";
    id("section3").style.display="inline-block";
    urlRedirect=ajaxObj.responseText.replace("OK,","");
    setTimeout(function() { window.location.assign(urlRedirect); },10000); } }

function id(id) { return document.getElementById(id); }
function a2h(aValue) { hValue=[]; for (idx=0;idx<aValue.length;idx++) { hValue.push(Number(aValue.charCodeAt(idx)).toString(16)); } return hValue.join(''); }

</script></head>
<body onload="initCredentials();">
<div><div class="title">leoPlayer</div></div>
<div id="section1">
  <div class="text">WLAN Name</div>
  <div class="input"><input type="text" id="ssid"></div>
  <div class="text">WLAN Secret</div>
  <div class="input"><input type="password" id="secret"></div>
</div>
<div id="section2">
  <br>
  <div class="button" onclick="sendCredentials();">Send</div>
</div>
<div id="section3">
  <div class="text">... please Wait ...</div>
</div>
</body></html>)";

String h2a(String hValue) {
  String aValue=""; for (int index=0;index<hValue.length();index+=2) {
    int h1=hValue.charAt(index); if (h1>96) { h1-=87; } else if (h1>64) { h1-=55; } else { h1-=48; }
    int h2=hValue.charAt(index+1); if (h2>96) { h2-=87; } else if (h2>64) { h2-=55; } else { h2-=48; }
    char h=h1*16+h2; aValue+=h; } return aValue; }
