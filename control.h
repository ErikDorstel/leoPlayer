struct { const uint8_t inputPins[4]={34,35,36,39}; // Button 1,2,3,4
         uint8_t defaultVolume=30;
         uint8_t volume=0;
         bool active=true;
         bool suppress=true;
         uint64_t copierCounter=0;
         String file="";
         uint16_t fileCount=0; } mp3Control;

#include "battery.h"
#include "animation.h"
#include "httpclient.h"
#include "httpserver.h"

void initButtons() {
  for (uint8_t pin : mp3Control.inputPins) pinMode(pin,INPUT); }

uint8_t getButton() {
  static uint8_t inputStateOld=0;
  static uint64_t inputTimer;
  uint8_t inputState=0;
  uint8_t index=0;
  for (uint8_t pin : mp3Control.inputPins) {
    inputState|=(!digitalRead(pin))<<index;
    index++; }
  if (inputState!=inputStateOld) {
    inputStateOld=inputState;
    if (inputState && millis()-inputTimer>300) {
      inputTimer=millis();
      return inputState; } }
  else if (inputState && millis()-inputTimer>500) {
    inputTimer=millis();
    return inputState+128; }
  return 0; }

void checkStatus(bool force=0) {
  static uint64_t checkTimer=0;
  static uint8_t checkCount=0;
  static uint8_t logCount=0;
  static uint64_t lastCopierCounter=0;
  if (force) { if (httpControl.app) { checkTimer=millis()+1000; } else { checkTimer=millis()+100; } logCount=100; return; }
  if (millis()>=checkTimer) {
    checkTimer=millis()+2000;
    if (mp3Control.copierCounter==lastCopierCounter && (!mp3Control.suppress)) {
      if (mp3Control.active) { mp3Control.active=false; player.stop();
        Serial.println("Active: " + String(mp3Control.active));
        aniControl.animation=ledsBreak; }
      checkCount++; }
    else if (mp3Control.active && mp3Control.suppress) { checkCount++; }
    else { checkCount=0; }
    if (checkCount>=15 && (!getUsbcStatus())) { postStatus(1,1,0); ledsOff(); switchOff(); }
    logCount++; if (logCount>=5) { logCount=0; postStatus(); }
    if (batteryControl.gauge==0 && (!getUsbcStatus())) { mp3Control.active=false; player.stop(); ledsAlarm(); postStatus(1,1,1); ledsOff(); switchOff(); }
    lastCopierCounter=mp3Control.copierCounter; } }

void buttonWorker() {
  uint8_t button=getButton();
  if (!button) { return; }
  if (button==1) {
    ledsOn();
    mp3Control.active=true; aniControl.animation=ledsWait; player.setVolume(0.0f); player.next(); mp3Control.suppress=true;
    Serial.println("Active: " + String(mp3Control.active)); }
  else if (button==2) {
    if (mp3Control.active && mp3Control.suppress) { return; }
    ledsOn();
    if (mp3Control.active) { mp3Control.active=false; aniControl.animation=ledsBreak; player.stop(); }
    else { mp3Control.active=true; aniControl.animation=ledsWait; player.setVolume(0.0f); player.previous(); mp3Control.suppress=true; }
    Serial.println("Active: " + String(mp3Control.active)); }
  else if (button==4 || button==132) {
    if (mp3Control.suppress) { return; }
    if (!mp3Control.active) { ledsVolume(); mp3Control.active=true; aniControl.animation=ledsPlay; player.play();
      if (mp3Control.volume==0) { mp3Control.volume=10; ledsVolume(); player.setVolume(mp3Control.volume/100.0f); } }
    else if (mp3Control.volume<100) { mp3Control.volume+=10; ledsVolume(); player.setVolume(mp3Control.volume/100.0f); }
    Serial.println("Active: " + String(mp3Control.active));
    Serial.println("Volume: " + String(mp3Control.volume)); }
  else if (button==8 || button==136) {
    if (mp3Control.suppress) { return; }
    if (!mp3Control.active) { ledsBattery(); return; }
    if (mp3Control.volume>0) { mp3Control.volume-=10; ledsVolume(); player.setVolume(mp3Control.volume/100.0f); }
    if (mp3Control.volume==0) { ledsVolume(); mp3Control.active=false; aniControl.animation=ledsBreak; player.stop(); }
    Serial.println("Active: " + String(mp3Control.active));
    Serial.println("Volume: " + String(mp3Control.volume)); } }
