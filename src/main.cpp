#include <Arduino.h>
#define LED_RED   23
#define PUSH_BUTTON 19

SemaphoreHandle_t xSemaphore;
int g_nCount=0;

IRAM_ATTR void isrPushButton() {
  g_nCount++;
  xSemaphoreGive(xSemaphore);
}

void taskPushButtonCounter(void *pvParameters) {
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_RED, LOW);

  while (1) {
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    digitalWrite(LED_RED, HIGH);
    Serial.printf("Counter: %d \r\n", g_nCount); fflush(stdout);
    vTaskDelay(20 / portTICK_PERIOD_MS);
    digitalWrite(LED_RED, LOW); 
  }
}

void setup()
{
  Serial.begin(115200);
  xSemaphore = xSemaphoreCreateBinary();
  // init push button interrupt
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  attachInterrupt(PUSH_BUTTON, isrPushButton, FALLING);
  xTaskCreatePinnedToCore(taskPushButtonCounter, "taskPushButtonCounter", configMINIMAL_STACK_SIZE+2048, NULL, 2, NULL, 0);
  Serial.println("System ready");
}

void loop()
{
  vTaskDelay(0);
}