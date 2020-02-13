#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <RTClib.h>

#include "main.h"

Max72xxPanel matrix = Max72xxPanel(9, 1, 4);
RTC_DS3231 rtc;

const String week_days_name[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; // Дни недели
bool show_dots;
unsigned long dots_timer;


void drawNum(uint8_t num, uint8_t x, uint8_t y) {
  for (int r = 0; r <= 7; r++) {
    for (int c = 0; c <= 5; c++) {
      matrix.drawPixel(x + c, y + r, numbers[num][r] & (1 << c));
    }
  }  
}



void showDisp() {
  DateTime now = rtc.now(); // Получаем температуру

  // Выводим на диспелей время
  drawNum(now.hour() / 10, 0, 0);
  drawNum(now.hour() % 10, 7, 0);
  drawNum(now.minute() / 10, 19, 0);
  drawNum(now.minute() % 10, 26, 0);

  // Выводим точки
  if (millis() - dots_timer > 1000) {
    dots_timer = millis();
    show_dots = !show_dots;
  }
  matrix.drawRect(15, 1, 2, 2, show_dots);
  matrix.drawRect(15, 5, 2, 2, show_dots);

  matrix.write();
}






void setup() {
  Serial.begin(9600);
  if (! rtc.begin()) { // Подключаемся к ds3231
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Устанавливаем время, которое указанно на компьютере

  matrix.setIntensity(10);                    // Задаем яркость от 0 до 15
  matrix.setRotation(3);
  matrix.fillScreen(LOW);                       // Обнуление матрицы
}



void loop() {
  showDisp();
}