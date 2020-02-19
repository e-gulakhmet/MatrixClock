#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <Wire.h>
#include <RTClib.h>
#include <OneButton.h>
#include <DHT.h>

#include "main.h"
#include "digit.h"
#include "power.h"
#include "object.h"


// TODO: Добавить датчик температуры
// TODO: Сделать эффект перехода между страницами


Max72xxPanel matrix = Max72xxPanel(MATRIX_CS_PIN, 1, 4);
RTC_DS3231 rtc;
OneButton button1(BUTT_1_PIN, false);
OneButton button2(BUTT_2_PIN, false);
DHT dht(2, DHT11); 
Power battery(POWER_SENSOR_PIN);

MainMode main_mode = mmClock;

bool show_dots;
unsigned long timer;
unsigned long batt_timer;



MainMode switchMainMode(MainMode curr, bool clockwice) { // Переключение режимов
  int n = static_cast<int>(curr);

  n += clockwice ? 1 : -1; // Если по часовой стрелке, то ставим следующий

  if ( n > 3) {
    n = 3;
  }
  if ( n < 0 ) {
    n = 0;
  }
  return static_cast<MainMode>(n);
}



void butt1Click() {
  main_mode = switchMainMode(main_mode, true);
  matrix.fillScreen(LOW);
}



void butt2Click() {
  main_mode = switchMainMode(main_mode, false);
  matrix.fillScreen(LOW);
}




void drawNum(uint8_t num, uint8_t num_len, const byte numbers[10][8], uint8_t x, uint8_t y) { // Функция для вывода цифр
  // Первый параметр - цифра, которую хотим вывести.
  // Второй параметр - цифра, которая указывает длину цифры(смотреть в digit.h)
  // Третий параметр - массив в котором содержатся цифры
  // Четвертый и пятый парамерт - расположение цифр
  for (int r = 0; r <= 7; r++) {
    for (int c = 0; c <= num_len; c++) {
      matrix.drawPixel(x + c, y + r, numbers[num][r] & (1 << c));
    }
  }  
}



void drawChar(const byte object[8], uint8_t w, uint8_t h, uint8_t x, uint8_t y) { // Функция вывода объектов
  // Первый параметр - объект, который хотим вывести.
  // Второй параметр - ширина объекта
  // Третий параметр - длина объекта
  // Четвертый и пятый парамерт - расположение
  for (int r = 0; r <= h; r++) {
    for (int c = 0; c <= w; c++) {
      matrix.drawPixel(x + c, y + r, object[r] & (1 << c));
    }
  }  

}



void showDisp() {
  switch (main_mode) {
    case mmClock: { // Выводим время на дисплей
      if (rtc.begin()) { // Если модуль времени подключен
        DateTime now = rtc.now();
        drawNum(now.hour() / 10, 5, numbers, 0, 0);
        drawNum(now.hour() % 10, 5, numbers, 7, 0);
        drawNum(now.minute() / 10, 5, numbers, 19, 0);
        drawNum(now.minute() % 10, 5, numbers, 26, 0);
        // Выводим точки
        show_dots = !show_dots;
        matrix.drawRect(15, 1, 2, 2, show_dots);
        matrix.drawRect(15, 5, 2, 2, show_dots);
      }
      else { // Если модуль времени не подключен
        matrix.print("ERROR");
      }
    }break;
    
    case mmDate: { // Выводим дату на матрицу
      DateTime now = rtc.now();
      show_dots = !show_dots;
      drawNum(now.day() / 10, 4, small_numbers, 0, 0);
      drawNum(now.day() % 10, 4, small_numbers, 5, 0);
      matrix.drawPixel(9, 7, show_dots);
      drawNum(now.month() / 10, 4, small_numbers, 11, 0);
      drawNum(now.month() % 10, 4, small_numbers, 16, 0);
      matrix.drawPixel(20, 7, show_dots);
      drawNum((now.year() - 2000) / 10, 4, small_numbers, 22, 0);
      drawNum((now.year() - 2000) % 10, 4, small_numbers, 27, 0);
    }break;

    case mmTemp: {
      matrix.print(dht.readTemperature());
    }break;

    case mmPower: { // Выводим данные о батареи
      if (battery.getProcent() == 100) {
        drawNum(battery.getProcent() / 100, 4, small_numbers, 0, 0);
        drawNum(0, 4, small_numbers, 5, 0);
        drawNum(0, 4, small_numbers, 10, 0);
      }
        drawNum(battery.getProcent() / 10, 4, small_numbers, 0, 0);
        drawNum(battery.getProcent() % 10, 4, small_numbers, 5, 0);
        drawChar(procent, 6, 8, 10, 0);
    }break;
  }

  matrix.write();
}



void setup() {
  Serial.begin(9600);
  pinMode(POWER_SENSOR_PIN, INPUT);

  button1.attachClick(butt1Click);
  button2.attachClick(butt2Click);

  if (! rtc.begin()) { // Подключаемся к ds3231
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Устанавливаем время, которое указанно на компьютере

  dht.begin();

  matrix.setIntensity(10); // Задаем яркость от 0 до 15
  matrix.setRotation(3);
  matrix.fillScreen(LOW); // Обнуление матрицу

  battery.update(); // Получаем данные о батареи
}



void loop() {
  button1.tick();
  button2.tick();
  if (millis() - batt_timer > 300000) { // Получаем данные о батареи раз в 5 минут
    batt_timer = millis();
    battery.update();
  }  
  if (millis() - timer > 1000) {
    timer = millis();
    showDisp();
  }
  Serial.println(dht.readTemperature());
}