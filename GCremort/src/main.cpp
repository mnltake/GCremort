#include <Arduino.h>
#include <ESP8266WiFi.h>
extern "C" {
#include <espnow.h>
}

struct __attribute__((packed)) SENSOR_DATA {
  int direction;
  int valve;
} recvData;

uint8_t bs[sizeof(recvData)];
int R_Pin = 4;
int L_Pin = 5;
int STOP_Pin = 12;
int KAISOKU_Pin = 15;
boolean direction_R=false,direction_L=false,stop=false ;
int heartBeat;
int nowdata =0 ;
boolean dbg = false, kaisoku = false;

void printMacAddress(uint8_t* macaddr) {
  Serial.print("{");
  for (int i = 0; i < 6; i++) {
    Serial.print("0x");
    Serial.print(macaddr[i], HEX);
    if (i < 5) Serial.print(',');
  }
  Serial.print("} ");
}

void recv_cb(uint8_t *mac, uint8_t *data, uint8_t len) {
  //digitalWrite(16,dbg);
  memcpy(&recvData, data, sizeof(recvData));
  Serial.print(recvData.direction);
  Serial.print(",");
  Serial.print(recvData.valve);

  //Serial.print(" from ");
  //printMacAddress(mac);
  Serial.println("");

  kaisoku =recvData.valve%2;
  if(recvData.direction==1){
    direction_R=true;
    direction_L=false;
    stop = false;
  }else if(recvData.direction==2){
    direction_L=true;
    direction_R=false;
    stop = false;
  }else if(recvData.direction==3){
    direction_R=false;
    direction_L=false;
    stop = true;
  }else{
  direction_R=false;
  direction_L=false;
  stop = false;
  }

  digitalWrite(KAISOKU_Pin ,kaisoku);
  digitalWrite(16 ,dbg);
  heartBeat = millis();
  dbg = !dbg;

}

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(16,OUTPUT);//led
  pinMode(KAISOKU_Pin,OUTPUT); //
  pinMode(STOP_Pin,OUTPUT); //
  pinMode(L_Pin,OUTPUT); //left
  pinMode(R_Pin,OUTPUT); //right
  digitalWrite(STOP_Pin ,false);
  Serial.print("This node STA mac: ");
  Serial.println(WiFi.macAddress());
  if (esp_now_init() != 0) {
    Serial.println("*** ESP_Now init failed");
    ESP.restart();
  }
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  digitalWrite(16,false);
  esp_now_register_recv_cb(recv_cb);
}

void loop() {
  digitalWrite(R_Pin ,direction_R);
  digitalWrite(L_Pin ,direction_L);
  digitalWrite(STOP_Pin ,stop);
  if (millis() - heartBeat > 300) {
   // Serial.println("Waiting for ESP-NOW messages...");
    heartBeat = millis();
    direction_R=false;
    direction_L=false;
    stop =false;
    digitalWrite(16 ,false);
  }

}