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

// TODO: Сделать эффект перехода между страницами
// TODO: Добавить отключение экрана ночью

Max72xxPanel matrix = Max72xxPanel(MATRIX_CS_PIN, 1, 4);
RTC_DS3231 rtc;
OneButton button1(BUTT_1_PIN, false);
OneButton button2(BUTT_2_PIN, false);
DHT dht(2, DHT11); 
Power battery(POWER_SENSOR_PIN);

MainMode main_mode = mmClock;
DateTime time;

bool show_dots;
unsigned long disp_timer;
unsigned long batt_timer;
unsigned long bright_timer;
bool second_object;




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
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < num_len; c++) {
      matrix.drawPixel(x + c, y + r, numbers[num][r] & (1 << c));
    }
  }  
}



void drawObject(const byte object[8], uint8_t w, uint8_t h, uint8_t x, uint8_t y) { // Функция вывода объектов
  // Первый параметр - объект, который хотим вывести.
  // Второй параметр - ширина объекта
  // Третий параметр - длина объекта
  // Четвертый и пятый парамерт - расположение
  for (int r = 0; r < h; r++) {
    for (int c = 0; c < w; c++) {
      matrix.drawPixel(x + c, y + r, object[r] & (1 << c));
    }
  }  

}



void showDisp() {
  switch (main_mode) {
    case mmClock: { // Выводим время на дисплей
      if (rtc.begin()) { // Если модуль времени подключен
        drawNum(time.hour() / 10, 6, numbers, 0, 0);
        drawNum(time.hour() % 10, 6, numbers, 7, 0);
        drawNum(time.minute() / 10, 6, numbers, 19, 0);
        drawNum(time.minute() % 10, 6, numbers, 26, 0);
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
      show_dots = !show_dots;
      drawNum(time.day() / 10, 4, small_numbers, 0, 0);
      drawNum(time.day() % 10, 4, small_numbers, 5, 0);
      matrix.drawPixel(9, 7, show_dots);
      drawNum(time.month() / 10, 4, small_numbers, 11, 0);
      drawNum(time.month() % 10, 4, small_numbers, 16, 0);
      matrix.drawPixel(20, 7, show_dots);
      drawNum((time.year() - 2000) / 10, 4, small_numbers, 22, 0);
      drawNum((time.year() - 2000) % 10, 4, small_numbers, 27, 0);
    }break;

    case mmTemp: { // Выводим данные о погоде на матрицу
      // Показываем температуру
      drawNum(int(dht.readTemperature()) / 10, 4, small_numbers, 0, 0);
      drawNum(int(dht.readTemperature()) % 10, 4, small_numbers, 5, 0);
      drawObject(gradus, 3, 3, 10, 0);
      drawObject(C, 4, 8, 14, 0);
      second_object = !second_object;
      // Показываем погоду по температуре
      if (int(dht.readTemperature()) / 10 > 25) {
        drawObject(sun[second_object], 7, 7, 21, 1);
      }
      else {
        drawObject(cloud[second_object], 8, 8, 21, 0);
      }
    }break;

    case mmPower: { // Выводим данные о батареи
      if (battery.getProcent() == 100) {
        drawNum(battery.getProcent() / 100, 4, small_numbers, 8, 0);
        drawNum(0, 4, small_numbers, 13, 0);
        drawNum(0, 4, small_numbers, 18, 0);
      }
      else {
        drawNum(battery.getProcent() / 10, 4, small_numbers, 8, 0);
        drawNum(battery.getProcent() % 10, 4, small_numbers, 13, 0);
      }
        drawObject(procent, 6, 8, 18, 0);
    }break;
  }
  matrix.write();
}



void setup() {
  Serial.begin(9600);
  pinMode(POWER_SENSOR_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);

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
  if (millis() - disp_timer > 1000) {
    disp_timer = millis();
    time = rtc.now();
    showDisp();
  }

  if (millis() - bright_timer > 60000) {
    bright_timer = millis();
    int brightness = analogRead(LIGHT_SENSOR_PIN);
    if (brightness > 850 && brightness <= 1100) {
      matrix.setIntensity(6);
    }
    else {
      matrix.setIntensity(12);
    }
  }


}