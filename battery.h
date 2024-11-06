#define batteryMin 3.5f
#define batteryMax 4.2f

struct { float battery[2]={10,10}; uint8_t gauge=10; } batteryControl;

void initBattery() {
  pinMode(32,INPUT); // Analog Battery 1
  pinMode(33,INPUT); // Analog Battery 2
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_DB_12);
  adc1_config_channel_atten(ADC1_CHANNEL_5,ADC_ATTEN_DB_12);
  pinMode(4,OUTPUT); digitalWrite(4,LOW); // Battery Charger Control
  pinMode(27,INPUT); } // 5 Volt from USB C

void getGauge() {
  float battery=min(batteryControl.battery[0],batteryControl.battery[1]);
  if (battery<batteryMin) { batteryControl.gauge=0; }
  else { batteryControl.gauge=(battery-batteryMin)/(batteryMax-batteryMin)*8.0f+1.0f; } }

void checkBattery() {
  static uint64_t batteryTimer=0;
  static uint32_t rawBattery1=0;
  static uint32_t rawBattery2=0;
  static uint8_t count=0;
  rawBattery1+=adc1_get_raw(ADC1_CHANNEL_4);
  rawBattery2+=adc1_get_raw(ADC1_CHANNEL_5);
  if (count>=120) {
    rawBattery1/=count;
    rawBattery2/=count;
    batteryControl.battery[0]=rawBattery1/580.0f;
    batteryControl.battery[1]=rawBattery2/580.0f;
    count=0;
    rawBattery1=0;
    rawBattery2=0;
    getGauge(); } else { count++; } }

bool getUsbcStatus() { return digitalRead(27); }

void switchOff() {
  digitalWrite(4,HIGH); delay(100);
  digitalWrite(4,LOW); delay(100);
  digitalWrite(4,HIGH); delay(100);
  digitalWrite(4,LOW); delay(100);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_34,0);
  esp_deep_sleep_start(); }
