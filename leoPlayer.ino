#include <WiFi.h>
#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"

const char *urls[] = { "http://leoplayerhttp.xxx.de/mp3/Welcome.mp3" };
const String urlPrefix = "http://leoplayerhttp.xxx.de/mp3/";
const char *urlBackend = "http://leoplayerhttp.xxx.de/html/backend.php";
const String urlRedirect = "https://leoplayer.xxx.de/";

WiFiClient network;
URLStream urlStream(network);
AudioSourceDynamicURL source(urlStream, urls, "audio/mp3");
I2SStream i2s;
MP3DecoderHelix decoder;
AudioPlayer player(source, i2s, decoder);

#include "control.h"

void setMetaData(MetaDataType type, const char* value, int len){
  if (!mp3Control.suppress) { return; }
  if (mp3Control.volume==0) { mp3Control.volume=mp3Control.defaultVolume; }
  mp3Control.suppress=false;
  player.setVolume(mp3Control.volume/100.0f);
  mp3Control.copierCounter=0;
  aniControl.animation=ledsPlay;
  mp3Control.file=String(source.toStr());
  mp3Control.file=mp3Control.file.substring(mp3Control.file.lastIndexOf("/")+1);
  checkStatus(1);
  Serial.print("Play: ");
  Serial.println(mp3Control.file); }

void setup() {
  Serial.begin(115200);

  initButtons();
  initLeds();
  ledsOn();
  initBattery();
  //AudioLogger::instance().begin(Serial, AudioLogger::Info);

  if (!digitalRead(mp3Control.inputPins[3])) { while (true) { manageCredentials(); } }

  preferences.begin("wlanAuth",true);
  String ssid=preferences.getString("ssid","empty");
  String secret=preferences.getString("secret","empty");
  preferences.end();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(),secret.c_str());

  aniControl.animation=ledsCheck;

  while (WiFi.status() != WL_CONNECTED) {
    aniWorker(); delay(50);
    checkBattery();
    if ((batteryControl.gauge==0 && (!getUsbcStatus())) || millis()>=60000UL) {
      Serial.println("WLAN not connected!");
      ledsAlarm(); ledsOff(); switchOff(); } }

  while (!mp3Control.fileCount) {
    getMP3Index(); aniWorker(); delay(50);
    checkBattery();
    if ((batteryControl.gauge==0 && (!getUsbcStatus())) || millis()>=60000UL) {
      Serial.println("MP3 index not found!");
      ledsAlarm(); ledsOff(); switchOff(); } }

  ledsOn();

  auto cfg = i2s.defaultConfig(TX_MODE);
  cfg.pin_data = 16;
  cfg.pin_bck = 17;
  cfg.pin_ws = 25;
  i2s.begin(cfg);

  player.setMetadataCallback(setMetaData);
  player.begin();
  player.setVolume(mp3Control.volume/100.0f);
  player.setAutoNext(false); }

void loop() { mp3Control.copierCounter+=player.copy(); buttonWorker(); checkStatus(); aniWorker(); checkBattery(); asyncHttpCheck(); }
