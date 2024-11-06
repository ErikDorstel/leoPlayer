#include <FastLED.h>

struct {
  CRGB leds[8];
  uint8_t index=0;
  void (*animation)()=nullptr;
  uint64_t timer; } aniControl;

void initLeds() {
  FastLED.addLeds<WS2812B,26,GRB>(aniControl.leds,8).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(20); }

void ledsPlay() {
  for (uint8_t i=0;i<8;i++) {
    aniControl.leds[(i+aniControl.index)%8]=CHSV(0,255,i*32+31); }
  FastLED.show(); }

void ledsWait() {
  for (uint8_t i=0;i<8;i++) {
    aniControl.leds[(i+aniControl.index)%8]=CHSV(56,255,i*32); }
  FastLED.show(); }

void ledsBreak() {
  for (uint8_t i=0;i<8;i++) {
    aniControl.leds[(i+aniControl.index)%8]=CHSV(160,255,i*32+31); }
  FastLED.show(); }

void ledsCheck() {
  for (uint8_t i=0;i<8;i++) {
    if (i>3) { aniControl.leds[(i+aniControl.index)%8]=CHSV(160,255,255); }
    else { aniControl.leds[(i+aniControl.index)%8]=CHSV(96,255,127); } }
  FastLED.show(); }

void ledsOff() {
  for (uint8_t i=0;i<8;i++) {
    aniControl.leds[i]=CHSV(0,0,0); }
  FastLED.show();
  aniControl.timer=millis()+320; }

void ledsOn() {
  for (uint8_t i=0;i<8;i++) {
    aniControl.leds[i]=CHSV(96,255,127); }
  FastLED.show();
  aniControl.timer=millis()+320; }

void ledsEvent() {
  for (uint8_t i=0;i<8;i++) {
    aniControl.leds[i]=CHSV(160,255,255); }
  FastLED.show();
  aniControl.timer=millis()+320; }

void ledsBattery() {
  for (uint8_t i=0;i<8;i++) {
    if (i<batteryControl.gauge) { aniControl.leds[i]=CHSV(96,255,127); }
    else { aniControl.leds[i]=CHSV(0,255,255); } }
  FastLED.show();
  aniControl.timer=millis()+1000; }

void ledsVolume() {
  for (uint8_t i=0;i<8;i++) {
    if (i<mp3Control.volume/13.0f) { aniControl.leds[i]=CHSV(96,255,127); }
    else { aniControl.leds[i]=CHSV(160,255,255); } }
  FastLED.show();
  aniControl.timer=millis()+510; }

void ledsAlarm() {
  for (uint8_t j=0;j<8;j++) {
    for (uint8_t i=0;i<8;i++) { aniControl.leds[i]=CHSV(0,255,255); }
    FastLED.show(); delay(200);
    for (uint8_t i=0;i<8;i++) { aniControl.leds[i]=CHSV(0,0,0); }
    FastLED.show(); delay(200); } }

void aniWorker() {
  if (millis()>=aniControl.timer) {
    aniControl.timer=millis()+200;
    aniControl.index++; aniControl.index%=8;
    aniControl.animation(); } }
