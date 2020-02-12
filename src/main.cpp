#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <RTClib.h>

#include "main.h"

Max72xxPanel matrix = Max72xxPanel(9, 1, 4);
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void drawNum(uint8_t num, uint8_t x, uint8_t y) {
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 6; c++) {
      matrix.drawPixel(x + c, y + r, numbers[num][r] & (1 << c));
    }
  }  
}



void setup() {
  Serial.begin(9600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  matrix.setIntensity(15);                    // Задаем яркость от 0 до 15
  matrix.setRotation(3);
  matrix.fillScreen(LOW);                       // Обнуление матрицы
}



void loop() {
  matrix.write();

  DateTime now = rtc.now();

  drawNum(now.hour() / 10, 0, 0);
  drawNum(now.hour() % 10, 8, 0);
  drawNum(now.minute() / 10, 16, 0);
  drawNum(now.minute() % 10, 22, 0);


  Serial.print(now.year());
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.print("Temperature: ");
  Serial.print(rtc.getTemperature());
  Serial.println(" C");

  delay(3000);
}