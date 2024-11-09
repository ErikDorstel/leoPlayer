#define requestTimeout 1000
WiFiServer tcpServer(80);

#include <Preferences.h>
Preferences preferences;

#include "index_html.h"

void manageCredentials() {
  static bool init=1;
  static bool ready=0;
  if (init) {
    init=0;
    ledsEvent();
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAddress(192,168,4,1),IPAddress(192,168,4,1),IPAddress(255,255,255,0));
    WiFi.softAP("leoPlayer","");
    tcpServer.begin(); }

  WiFiClient httpServerClient=tcpServer.available(); String header="";
  if (httpServerClient) { String currentLine=""; uint64_t requestTimer=millis()+requestTimeout;
    while (httpServerClient.connected()) {
      if (millis()>=requestTimer) { httpServerClient.println("Request timeout"); break; }
      if (httpServerClient.available()) { char c=httpServerClient.read(); header+=c;
        if (c=='\n') {
          if (currentLine.length()==0) {
            httpServerClient.println("HTTP/1.1 200 OK");
            httpServerClient.println("Content-Type: text/html; charset=utf-8");
            httpServerClient.println("Connection: keep-alive");
            httpServerClient.println("Keep-Alive: timeout=5, max=1000");
            httpServerClient.println("Server: ESP32 leoPlayer Erik Dorstel");
            httpServerClient.println();
            int a=header.indexOf("GET "); int b=header.indexOf(" ",a+4);
            if (a>=0) { String request=header.substring(a+4,b); String response="";

              if (request.indexOf("/credentials,")>=0) {
                int a=request.indexOf(",")+1; int b=request.indexOf(",",a)+1;
                String ssid=h2a(request.substring(a,b-1));
                String secret=h2a(request.substring(b));
                if (ssid!="" && secret!="") { response="OK," + urlRedirect; ready=1;
                  preferences.begin("wlanAuth",false);
                  preferences.putString("ssid",ssid);
                  preferences.putString("secret",secret);
                  preferences.end(); } }
              else if (request.indexOf("/favicon.ico")>=0) { }
              else { response=index_html; }

              httpServerClient.println(response);
              Serial.println("HTTP Request " + header.substring(a+4,b) + " from " + httpServerClient.remoteIP().toString() + " received."); }
            break; }
          else { currentLine=""; } }
        else if (c!='\r') { currentLine+=c; } } }
    header = ""; httpServerClient.stop(); }

  if (ready) { ledsOff(); delay(500); ESP.restart(); }

  checkBattery();
  if ((batteryControl.gauge==0 && (!getUsbcStatus())) || millis()>=120000UL) { ledsAlarm(); ledsOff(); switchOff(); } }
