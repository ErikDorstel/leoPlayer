#define batteryMin 3.5f
#define batteryMax 4.2f
#define batteryChargeMax 4.3f

struct { float battery[2]={10,10}; uint8_t gauge=10; } batteryControl;

void initBattery() {
  pinMode(32,INPUT); // Analog Battery 1
  pinMode(33,INPUT); // Analog Battery 2
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_DB_12);
  adc1_config_channel_atten(ADC1_CHANNEL_5,ADC_ATTEN_DB_12);
  pinMode(4,OUTPUT); digitalWrite(4,LOW); // Battery Charger Control
  pinMode(27,INPUT); } // 5 Volt from USB C

bool getUsbcStatus() { return digitalRead(27); }

void getGauge() {
  float battery=min(batteryControl.battery[0],batteryControl.battery[1]);
  if (battery<batteryMin) { batteryControl.gauge=0; return; }
  battery=(batteryControl.battery[0]+batteryControl.battery[1])/2.0f;
  if (getUsbcStatus()) { batteryControl.gauge=(battery-batteryMin)/(batteryChargeMax-batteryMin)*7.0f+1.0f; }
  else { batteryControl.gauge=(battery-batteryMin)/(batteryMax-batteryMin)*7.0f+1.0f; } }

void checkBattery() {
  static uint64_t batteryTimer=0;
  static uint32_t rawBattery[2]={0,0};
  static uint8_t count=0;
  rawBattery[0]+=adc1_get_raw(ADC1_CHANNEL_4);
  rawBattery[1]+=adc1_get_raw(ADC1_CHANNEL_5);
  if (count>=150) {
    rawBattery[0]/=count;
    rawBattery[1]/=count;
    batteryControl.battery[0]=rawBattery[0]/580.0f;
    batteryControl.battery[1]=rawBattery[1]/580.0f;
    count=0;
    rawBattery[0]=0;
    rawBattery[1]=0;
    getGauge(); } else { count++; } }

void switchOff() {
  digitalWrite(4,HIGH); delay(100);
  digitalWrite(4,LOW); delay(100);
  digitalWrite(4,HIGH); delay(100);
  digitalWrite(4,LOW); delay(100);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_34,0);
  esp_deep_sleep_start(); }
