#include <Arduino.h>
#include <M5StickC.h>
int bootCount = 0; // ライトスリープは通常の変数で良い
#define BUTTON_PIN_BITMASK 0x100000000
void print_wakeup_pin(){

  uint64_t wakeup_pin = esp_sleep_get_ext1_wakeup_status();

  if (wakeup_pin & 0x2000000000) {
    Serial.println(37);
  }
  if (wakeup_pin & 0x80000000000) {
    Serial.println(39);
  }
  if (wakeup_pin & 0x100000000) {
    Serial.println(32);
  }
  
}
void setup() {
  // シリアル初期化
  Serial.begin(115200);

  // シリアル初期化待ち
  delay(1000);

  // GPIO37(M5StickCのHOMEボタン)かGPIO39(M5StickCの右ボタン)がLOWになったら起動
  pinMode(GPIO_NUM_37, INPUT_PULLUP);
  pinMode(GPIO_NUM_39, INPUT_PULLUP);
  pinMode(GPIO_NUM_32, INPUT_PULLUP);
  pinMode(GPIO_NUM_33, INPUT_PULLUP);
 // gpio_wakeup_enable(GPIO_NUM_37, GPIO_INTR_LOW_LEVEL);
 // gpio_wakeup_enable(GPIO_NUM_39, GPIO_INTR_LOW_LEVEL);
 // gpio_wakeup_enable(GPIO_NUM_32, GPIO_INTR_LOW_LEVEL);
 // gpio_wakeup_enable(GPIO_NUM_33, GPIO_INTR_LOW_LEVEL);
  // GPIOをウェイクアップソースとして有効にする
 esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ALL_LOW );
  // 起動回数カウントアップ
  bootCount++;
  Serial.printf("起動回数: %d ", bootCount);

  // 起動方法取得
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason){
    case ESP_SLEEP_WAKEUP_EXT0      : Serial.printf("外部割り込み(RTC_IO)で起動\n"); break;
    case ESP_SLEEP_WAKEUP_EXT1      : Serial.printf("外部割り込み(RTC_CNTL)で起動 IO=%llX\n", esp_sleep_get_ext1_wakeup_status()); break;
    case ESP_SLEEP_WAKEUP_TIMER     : Serial.printf("タイマー割り込みで起動\n"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD  : Serial.printf("タッチ割り込みで起動\n"); break;
    case ESP_SLEEP_WAKEUP_ULP       : Serial.printf("ULPプログラムで起動\n"); break;
    case ESP_SLEEP_WAKEUP_GPIO      : Serial.printf("ライトスリープからGPIO割り込みで起動\n"); break;
    case ESP_SLEEP_WAKEUP_UART      : Serial.printf("ライトスリープからUART割り込みで起動\n"); break;
    default                         : Serial.printf("スリープ以外からの起動\n"); break;
  }
  print_wakeup_pin();
  // シリアルをすべて出力する
  Serial.flush();

  // ライトスリープ
  esp_deep_sleep_start();
}

void loop() {

}