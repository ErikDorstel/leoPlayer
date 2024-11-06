#include <AsyncHTTPRequest_Generic.h>
AsyncHTTPRequest request;

struct { uint8_t app=0; } httpControl;

void asyncHttpCheck() {
  if (!httpControl.app) { return; }
  if (request.readyState()==readyStateDone) {
    if (request.responseHTTPcode() == 200) {
      if (httpControl.app==1) {
        String list=request.responseText();
        list.replace("\r","");
        uint32_t start=0;
        for (uint32_t i=0;i<list.length();i++) {
          if (list.charAt(i)=='\n') {
            mp3Control.fileCount++;
            source.addURL(String(urlPrefix + list.substring(start,i)).c_str());
            start=i+1; } } }
      if (httpControl.app==2) {
        String command=request.responseText();
        int a=command.indexOf(",")+1; int b=command.indexOf(",",a)+1;
        if (command.indexOf("volume")>=0) {
          if (!mp3Control.active) { mp3Control.active=true; aniControl.animation=ledsPlay; player.play(); }
          mp3Control.volume=command.substring(a).toInt();
          if (!mp3Control.suppress) { ledsVolume(); player.setVolume(mp3Control.volume/100.0f); } }
        else if (command.indexOf("next")>=0) {
          mp3Control.active=true; aniControl.animation=ledsWait; player.setVolume(0.0f); player.next(); mp3Control.suppress=true;
          if (a>0) { mp3Control.volume=command.substring(a).toInt(); } }
        else if (command.indexOf("previous")>=0) {
          mp3Control.active=true; aniControl.animation=ledsWait; player.setVolume(0.0f); player.previous(); mp3Control.suppress=true;
          if (a>0) { mp3Control.volume=command.substring(a).toInt(); } }
        else if (command.indexOf("stop")>=0) {
          mp3Control.active=false; mp3Control.suppress=false; aniControl.animation=ledsBreak; player.stop();
          if (a>0) { mp3Control.volume=command.substring(a).toInt(); player.setVolume(mp3Control.volume/100.0f); } }
        else if (command.indexOf("go")>=0) {
          mp3Control.active=true; aniControl.animation=ledsPlay; player.play(); 
          if (a>0) { mp3Control.volume=command.substring(a).toInt(); if (mp3Control.volume==0) { mp3Control.volume=10; }
            if (!mp3Control.suppress) { player.setVolume(mp3Control.volume/100.0f); } } }
        else if (command.indexOf("play")>=0) {
          mp3Control.active=true; aniControl.animation=ledsWait; player.setVolume(0.0f); mp3Control.suppress=true;
          if (b>0) { mp3Control.volume=command.substring(b).toInt(); player.begin(command.substring(a,b-1).toInt()); }
          else { player.begin(command.substring(a).toInt()); } }
        if (mp3Control.volume==0) { mp3Control.active=false; mp3Control.suppress=false; aniControl.animation=ledsBreak; player.stop(); } } }
    httpControl.app=0; } }

void getMP3Index() {
  if (httpControl.app) { return; }
  if (request.readyState() == readyStateUnsent || request.readyState() == readyStateDone) {
    bool requestOpenResult = request.open("POST", urlBackend);
    request.setReqHeader("Content-Type","application/x-www-form-urlencoded");
    if (requestOpenResult) { httpControl.app=1; request.send("index=");
      while (httpControl.app) { asyncHttpCheck(); } } } }

String urlencode(String value) {
  String result=""; char x; char x0; char x1;
  for (int idx=0;idx<value.length();idx++) {
    x=value.charAt(idx);
    if (x==' ') { result+='+'; }
    else if (isalnum(x)) { result+=x; }
    else { x1=(x&0xf)+'0';
      if ((x & 0xf)>9) { x1=(x&0xf)-10+'A'; }
      x=(x>>4)&0xf; x0=x+'0';
      if (x>9) { x0=x-10+'A'; }
      result+='%'; result+=x0; result+=x1; } }
  return result; }

void postStatus(bool sync=0, bool off=0, bool undervoltage=0) {
  if (sync) { while (httpControl.app) { asyncHttpCheck(); } }
  else if (httpControl.app) { return; }
  if (request.readyState() == readyStateUnsent || request.readyState() == readyStateDone) {
    bool requestOpenResult = request.open("POST", urlBackend);
    request.setReqHeader("Content-Type","application/x-www-form-urlencoded");
    if (requestOpenResult) { httpControl.app=2;
      uint8_t status=mp3Control.active|(mp3Control.suppress<<1)|(getUsbcStatus()<<2)|(off<<3)|(undervoltage<<4);
      request.send("update="+String(batteryControl.battery[0],2)+","+String(batteryControl.battery[1],2)+","+String(millis()/1000)+","+String(mp3Control.volume)+","+String(status)+","+urlencode(mp3Control.file));
      if (sync) { while (httpControl.app) { asyncHttpCheck(); } } } } }
