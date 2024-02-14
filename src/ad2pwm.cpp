#include <driver/adc.h>
#include <driver/ledc.h>
#include <M5StickC.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define ADC_CHANNEL ADC1_CHANNEL_0
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_HS_MODE LEDC_HIGH_SPEED_MODE
#define LEDC_FREQUENCY 15000
#define LEDC_RESOLUTION LEDC_TIMER_10_BIT
#define SAMPLING_FREQUENCY 15000
#define LED_PIN 10

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile unsigned long dutySum = 0; // Duty比の合計
volatile unsigned long sampleCount = 0; // サンプリング回数
volatile bool pwmOutputEnabled = true; // PWM出力の有効/無効フラグ

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  uint32_t adc_value = adc1_get_raw(ADC_CHANNEL);
  uint32_t duty = (adc_value * 1023) / 4095;
  dutySum += duty;
  sampleCount++;

  //平均Dutyが10%超えるとPWM出力停止
  if (pwmOutputEnabled) {
    ledcWrite(LEDC_CHANNEL, duty);
  } else {
    ledcWrite(LEDC_CHANNEL, 0); // PWM出力を停止
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}

//平均Dutyが10%超えるとPWM出力停止
void controlPWMTask(void *pvParameters) {
  while (1) {
    if (sampleCount > 0) {
      unsigned long averageDuty = (dutySum * 100) / (sampleCount * 1023); // 平均Duty比を計算
      if (averageDuty > 10) { // 平均Duty比が10%を超える場合
        digitalWrite(LED_PIN, LOW); //LOWでLED点灯
        pwmOutputEnabled = false; // PWM出力を停止
        vTaskDelay(5000); // 5秒待機
        pwmOutputEnabled = true; // PWM出力を再開
        digitalWrite(LED_PIN, HIGH); //HIGHでLED消灯
      }
    }
    dutySum = 0; // 合計とサンプリング回数をリセット
    sampleCount = 0;
    
    vTaskDelay(2000); // 次のチェックまで10秒待機
  }
}

void setup() {
  M5.begin();
  Serial.begin(115200);
  // 以降の設定は省略...

  M5.Lcd.setRotation(1);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextSize(2);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.println("DoLeMiFa");
  M5.Lcd.println("  SoLeNoiDo");
  M5.Lcd.println("");
  M5.Lcd.println("Speaker Mode");

  // ADCの設定
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_0);

  // LEDC（PWM）の設定
  ledcSetup(LEDC_CHANNEL, LEDC_FREQUENCY, LEDC_RESOLUTION);
  #ifdef M5StickC__
  //ledcAttachPin(26, LEDC_CHANNEL); // GPIO25をPWM出力として使用
  ledcAttachPin(32, LEDC_CHANNEL); // GPIO25をPWM出力として使用
  #else
  ledcAttachPin(32, LEDC_CHANNEL); // GPIO26をPWM出力として使用
  #endif

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); //HIGHでLED消灯

  // サンプリング用のタイマーを設定
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000 / SAMPLING_FREQUENCY, true);
  timerAlarmEnable(timer);

  // PWM制御タスクの作成
  xTaskCreate(controlPWMTask, "Control PWM Task", 4096, NULL, 1, NULL);
}

void loop() {
  // FreeRTOSを使用しているため、loop内での処理は不要です。
  vTaskDelay(10000);
}
