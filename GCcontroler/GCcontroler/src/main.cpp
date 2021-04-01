#include <Arduino.h>
#include <M5StickC.h>
#include <esp_now.h>
#include <WiFi.h>
#include "AXP192.h"
#include "esp_sleep.h"
#include "esp_wifi.h"
//********************Genaral Parameters************************************************************
int i=0;
int j=0;
int pushN=0;
int value0,value1;
uint8_t ti,ti_1;
int LED_Pin= 10;
int t=0;
int buttonA=1,buttonB=0;
int R_button=32;
int L_button= 33;

int goZero=0;
int sTime=0;
uint8_t data[8];


//+++++++++++++++i_to_char+++++++++++++++++
// i=IntegerValueData,*d=Array pointer, n=Array start No
void i_to_char(int i,uint8_t *d, int n)
{
t=millis();
d[n] = i & 0x000000ff;
d[n+1] = (i & 0x0000ff00) >> 8;
d[n+2] = (i & 0x00ff0000) >> 16;
d[n+3] = (i & 0xff000000) >> 24;
}
//++++++++++++++++++++++++++++++++++++++++++
//=====================================ESP NOW Sub ================================================
esp_now_peer_info_t slave;// 送信コールバック
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
char macStr[18];
snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);}// 受信コールバック
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
char macStr[18];
pushN++;
/*
Serial.print("M5StickRcved:");
for (i=0;i<data_len;i++)
{
Serial.print(data[i],HEX);
Serial.print(",");
}
Serial.println("");
//j=0;//reset Data couter
*/
}
void zeroR()
{
j=0;
Serial.println("=========zeroR_j===========");
}//============================================ESP-NOW Sub end==========================================================================================

void ESP_error(esp_err_t result)
{
Serial.print("Send Status: ");
if (result == ESP_OK) {
Serial.println("Success");
} else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
Serial.println("ESPNOW not Init.");
} else if (result == ESP_ERR_ESPNOW_ARG) {
Serial.println("Invalid Argument");
} else if (result == ESP_ERR_ESPNOW_INTERNAL) {
Serial.println("Internal Error");
} else if (result == ESP_ERR_ESPNOW_NO_MEM) {
Serial.println("ESP_ERR_ESPNOW_NO_MEM");
} else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
Serial.println("Peer not found.");
} else {
Serial.println("Not sure what happened");
}
j=0;
}
void remocon(){
M5.update();
M5.Lcd.fillScreen(BLACK) ;
M5.Lcd.setTextColor(WHITE);
M5.Lcd.setTextSize(2);
M5.Lcd.setCursor(10, 0);
M5.Lcd.printf("%.2fv%.0fma\r\n", M5.Axp.GetBatVoltage(), M5.Axp.GetBatCurrent());
M5.Lcd.setTextSize(4);
M5.Lcd.setCursor(10, 30);
if(M5.BtnA.pressedFor(100)){//kaisoku
  
  buttonA++;
  boolean kai = !(buttonA%2);
  if (kai){
    M5.Lcd.setTextColor(ORANGE);
    M5.Lcd.print("FAST");
  }else{
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.print("SLOW");
  }
  
  digitalWrite(LED_Pin,kai);
  i_to_char(0,data,0);
  i_to_char(buttonA,data,4);
  esp_err_t result = esp_now_send(slave.peer_addr, data, sizeof(data));
  delay(500);

}else if(M5.BtnB.wasPressed()){//STOP
  M5.Lcd.fillScreen(RED) ;
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.print("STOP");
  i_to_char(3,data,0);
  esp_err_t result = esp_now_send(slave.peer_addr, data, sizeof(data));
  delay(500);
}else if (digitalRead(R_button)==0){//Right

  M5.Lcd.setTextColor(RED);
  M5.Lcd.print("Right");
  i_to_char(1,data,0);
  esp_err_t result = esp_now_send(slave.peer_addr, data, sizeof(data));
  delay(50);
}else if (digitalRead(L_button)==0){//Left

  M5.Lcd.setTextColor(BLUE);
  M5.Lcd.print("Left");
  i_to_char(2,data,0);
  esp_err_t result = esp_now_send(slave.peer_addr, data, sizeof(data));
  delay(50);
}else{
  i_to_char(0,data,0);
  //esp_err_t result = esp_now_send(slave.peer_addr, data, sizeof(data));
}
}

void setup() {

M5.begin();
M5.Axp.EnableCoulombcounter();
M5.Lcd.fillScreen(BLACK);
M5.Lcd.setRotation(3);
M5.Lcd.setTextSize(1);
M5.Lcd.print("ESP-NOW Test\n");
pinMode(LED_Pin, OUTPUT);
digitalWrite(LED_Pin,0);
//pinMode(R_button,INPUT_PULLUP);
//pinMode(L_button, INPUT_PULLUP);
  pinMode(GPIO_NUM_37, INPUT);
  pinMode(GPIO_NUM_39, INPUT);
  pinMode(GPIO_NUM_32, INPUT_PULLUP);
  pinMode(GPIO_NUM_33, INPUT_PULLUP);
  gpio_wakeup_enable(GPIO_NUM_37, GPIO_INTR_LOW_LEVEL);
  gpio_wakeup_enable(GPIO_NUM_39, GPIO_INTR_LOW_LEVEL);
  gpio_wakeup_enable(GPIO_NUM_32, GPIO_INTR_LOW_LEVEL);
  gpio_wakeup_enable(GPIO_NUM_33, GPIO_INTR_LOW_LEVEL);
//========Interrupt Pin========
//pinMode(26, INPUT_PULLUP);
//attachInterrupt(26, zeroR, FALLING);
//========= ESP-NOW初期化=============
WiFi.mode(WIFI_STA);
WiFi.disconnect();
if (esp_now_init() == ESP_OK) {
Serial.println("ESPNow Init Success");
M5.Lcd.print("ESPNow Init Success\n");
} else {
Serial.println("ESPNow Init Failed");
M5.Lcd.print("ESPNow Init Failed\n");
ESP.restart();
}// マルチキャスト用Slave登録
memset(&slave, 0, sizeof(slave));
for (int i = 0; i < 6; ++i) {
slave.peer_addr[i] = (uint8_t)0xff;
}
esp_err_t addStatus = esp_now_add_peer(&slave);
if (addStatus == ESP_OK) {
// Pair success
//Serial.println("Pair success");
}// ESP-NOWコールバック登録
esp_now_register_send_cb(OnDataSent);
esp_now_register_recv_cb(OnDataRecv);
esp_sleep_enable_gpio_wakeup();
}
//============================================ESP-NOW END=========================================
void loop() {
esp_wifi_start();
remocon();
Serial.print(data[0]);
Serial.print(",");
Serial.println(data[4]);

// }
delay(10);
esp_wifi_stop();
esp_light_sleep_start();
  
}
