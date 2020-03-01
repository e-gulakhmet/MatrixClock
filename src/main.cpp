#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <Wire.h>
#include <RTClib.h>
#include <OneButton.h>
#include <DHT.h>
#include <LowPower.h>

#include "main.h"
#include "digit.h"
#include "power.h"
#include "object.h"

// TODO: Сделать эффект перехода между страницами
// TODO: Добавить отключение экрана ночью

Max72xxPanel matrix = Max72xxPanel(MATRIX_CS_PIN, 1, 4);
RTC_DS3231 rtc;
OneButton left_butt(LEFT_BUTT_PIN, false);
OneButton right_butt(RIGHT_BUTT_PIN, false);
OneButton main_butt(MAIN_BUTT_PIN, false);
DHT dht(A1, DHT11); 
Power battery(POWER_SENSOR_PIN);

MainMode main_mode = mmClock;
PowerMode power_mode = pmPower;
DateTime time;

bool show_dots;
unsigned long disp_timer;
unsigned long batt_timer;
unsigned long wait_timer;
bool is_waiting;
bool second_object;
bool is_active = true;
bool is_setting;
uint8_t sett_mode;
int set_year;
uint8_t set_month;
uint8_t set_day;
uint8_t set_hour;
uint8_t set_minute;
uint8_t set_second;





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





void rightButtClick() {
  if(!is_setting){
    main_mode = switchMainMode(main_mode, true);
    matrix.fillScreen(LOW);
  }
  else if (is_setting){
    switch (sett_mode) {
      case 0: set_day++; break;
      case 1: set_month++; break;
      case 2: set_year++; break;
      case 3: set_hour++; break;
      case 4: set_minute++; break;
    }
  }
}

void leftButtClick() {
  if (!is_setting) {
    main_mode = switchMainMode(main_mode, false);
    matrix.fillScreen(LOW);
  }
  else if (is_setting){
    switch (sett_mode) {
      case 0: set_day--; break;
      case 1: set_month--; break;
      case 2: set_year--; break;
      case 3: set_hour--; break;
      case 4: set_minute--; break;
    }
  }
}

void mainButtClick() {
  if (is_setting) {
    sett_mode++;
    matrix.fillScreen(LOW);
    matrix.write();
  }
}

void mainButtLongPress() {
  if ((main_mode == mmClock || main_mode == mmDate) && !is_setting) {
    is_setting = true;
    matrix.fillScreen(LOW);
    matrix.write();
    set_year = time.year();
    set_month = time.month();
    set_day = time.day();
    set_hour = time.hour();
    set_minute = time.minute();
    set_second = time.second();
  }
  
}



void wakeUp() {
  is_waiting = true;
  wait_timer = millis();
  detachInterrupt(0);
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




void showSett() {
  if (sett_mode >= 3){
    drawNum(set_hour / 10, 6, numbers, 0, 0);
    drawNum(set_hour % 10, 6, numbers, 7, 0);
    drawNum(set_minute / 10, 6, numbers, 19, 0);
    drawNum(set_minute % 10, 6, numbers, 26, 0);
    matrix.drawRect(15, 1, 2, 2, HIGH);
    matrix.drawRect(15, 5, 2, 2, HIGH);
  }
  else {
    drawNum(set_day / 10, 4, small_numbers, 0, 0);
    drawNum(set_day % 10, 4, small_numbers, 5, 0);
    matrix.drawPixel(9, 7, HIGH);
    drawNum(set_month / 10, 4, small_numbers, 11, 0);
    drawNum(set_month % 10, 4, small_numbers, 16, 0);
    matrix.drawPixel(20, 7, HIGH);
    drawNum((set_year - 2000) / 10, 4, small_numbers, 22, 0);
    drawNum((set_year - 2000) % 10, 4, small_numbers, 27, 0);
  }

  if (sett_mode > 4) {
    rtc.adjust(DateTime(set_year, set_month, set_day, set_hour, set_minute, set_second));
    is_setting = false;
    matrix.fillScreen(LOW);
  }
  matrix.write();
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
      if (int(dht.readTemperature()) > 24) {
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
  pinMode(MAIN_BUTT_PIN, INPUT);

  left_butt.attachClick(leftButtClick);
  right_butt.attachClick(rightButtClick);
  main_butt.attachClick(mainButtClick);
  main_butt.attachDuringLongPress(mainButtLongPress);

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
  left_butt.tick();
  main_butt.tick();
  right_butt.tick();
  if (millis() - batt_timer > 300000) { // Получаем данные о батареи раз в 5 минут
    batt_timer = millis();
    battery.update();
  }

  if (is_setting) {
    showSett();
  }

  else {
    if (millis() - disp_timer > 1000) {
      disp_timer = millis();
      time = rtc.now(); // Получаем время
      showDisp(); // Выводим данные на дисплей

      int brightness = analogRead(LIGHT_SENSOR_PIN);
      if (brightness > 850 && brightness <= 1100) {
        matrix.setIntensity(6);
      }
      else {
        matrix.setIntensity(10);
      }
    }

  }

  // Если прошло 15 минут после выхода из сна и кнопка не была нажата
  if (is_waiting) {
    if (millis() - wait_timer > 900000) {
      wait_timer = millis();
      // Меняем флаг, отвечающий за проверку выхода в сон
      is_waiting = false;
    }
  }

  else {
    switch (power_mode) {
      case pmPower: {
        // Если время равно 0 и свет в комноте выключен
        if (time.hour() >= 0 && time.hour() <= 8 && analogRead(LIGHT_SENSOR_PIN) > 850) {
          matrix.fillScreen(LOW); // Выключаем дисплей
          matrix.write();
          attachInterrupt(0, wakeUp, HIGH); // Активируем прерывения на 2 пине
          LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF); // Укладывем ардуинку спать
        }
      } break;
    }
  }
}