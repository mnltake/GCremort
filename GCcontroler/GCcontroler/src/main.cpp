#include <Arduino.h>
#include <M5StickC.h>
#include <esp_now.h>
#include <WiFi.h>
//********************Genaral Parameters************************************************************
int i=0;
int j=0;
int pushN=0;
int value0,value1;
uint8_t ti,ti_1;
int RTC_Pin=0;//GP0
int NTP_Pin=26;//GP26
int LED_Pin= 10;
int t=0;
int buttonA=1,buttonB=0;
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
void setup() {
M5.begin();
M5.Lcd.fillScreen(BLACK);
M5.Lcd.setRotation(3);
M5.Lcd.print("ESP-NOW Test\n");
pinMode(LED_Pin, OUTPUT);
digitalWrite(LED_Pin,0);
//========= Ext Signal Pin====
//pinMode(RTC_Pin, OUTPUT);
//pinMode(NTP_Pin, OUTPUT);
//========Interrupt Pin========
pinMode(26, INPUT_PULLUP);
attachInterrupt(26, zeroR, FALLING);
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
}
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
//============================================ESP-NOW END=========================================
void loop() {

M5.update();
if(M5.BtnA.wasPressed()){
  buttonA++;
  digitalWrite(LED_Pin,!(buttonA%2));
  i_to_char(0,data,0);
  i_to_char(buttonA,data,4);
  esp_err_t result = esp_now_send(slave.peer_addr, data, sizeof(data));
}else if(M5.BtnB.isPressed()) {
  buttonB=1;
  i_to_char(buttonB,data,0);
  esp_err_t result = esp_now_send(slave.peer_addr, data, sizeof(data));
}else{
  i_to_char(0,data,0);
  //esp_err_t result = esp_now_send(slave.peer_addr, data, sizeof(data));
}



//Serial.println(t);

// if( pushN%2==1 )// 8msec data send
// {
digitalWrite(NTP_Pin,0);




//ESP_error(result);
//====CHECK Send Data==========================================
//value0=data[0]+data[1]*256+data[2]*256*256+data[3]*256*256*256;
//value1=data[4]+data[5]*256+data[6]*256*256+data[7]*256*256*256;
Serial.print(data[0]);
Serial.print(",");
Serial.println(data[4]);

digitalWrite(NTP_Pin,1);
// }
delay(10);
}