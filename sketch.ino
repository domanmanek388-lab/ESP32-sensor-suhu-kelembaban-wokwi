#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// Konfigurasi DHT22
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

float suhu = 0;
float kelembaban = 0;

void TaskBacaSensor(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    kelembaban = dht.readHumidity();
    suhu = dht.readTemperature();

    if (isnan(kelembaban) || isnan(suhu)) {
      Serial.println("Gagal membaca sensor DHT22!");
    } else {
      Serial.printf("Suhu: %.2f °C | Kelembaban: %.2f %%\n", suhu, kelembaban);
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}


void TaskTampilLCD(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Suhu: ");
    lcd.print(suhu);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print(kelembaban);
    lcd.print("%");

    vTaskDelay(1500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.backlight();


  xTaskCreatePinnedToCore(
    TaskBacaSensor,    // Fungsi task
    "BacaSensor",      // Nama task
    2048,              // Stack size
    NULL,              // Parameter
    1,                 // Prioritas
    NULL,              // Handle task
    1                  // Jalankan di core 1
  );

  xTaskCreatePinnedToCore(
    TaskTampilLCD,     // Fungsi task
    "TampilLCD",       // Nama task
    2048,
    NULL,
    1,
    NULL,
    0      
  );
}

void loop() {
}
