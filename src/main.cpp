#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <RTClib.h>
#include <OneButton.h>

#include "main.h"
#include "digit.h"

// TODO: Добавить датчик температуры
// TODO: Сделать эффект перехода между страницами


Max72xxPanel matrix = Max72xxPanel(MATRIX_CS_PIN, 1, 4);
RTC_DS3231 rtc;
OneButton button1(BUTT_1_PIN, false);
OneButton button2(BUTT_2_PIN, false);

MainMode main_mode = mmClock;

const String week_days_name[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; // Дни недели
bool show_dots;
unsigned long timer;



MainMode switchMainMode(MainMode curr, bool clockwice) { // Переключение режимов
  int n = static_cast<int>(curr);

  n += clockwice ? 1 : -1; // Если по часовой стрелке, то ставим следующий

  if ( n > 2) {
    n = 2;
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




void drawNum(uint8_t num, uint8_t x, uint8_t y) { // Функция для вывода цифр
  for (int r = 0; r <= 7; r++) {
    for (int c = 0; c <= 5; c++) {
      matrix.drawPixel(x + c, y + r, numbers[num][r] & (1 << c));
    }
  }  
}



void showDisp() {

  switch (main_mode) {
    case mmClock: {
      if (millis() - timer > 1000) {
        DateTime now = rtc.now(); // Получаем температуру
        // Выводим на диспелей время
        drawNum(now.hour() / 10, 0, 0);
        drawNum(now.hour() % 10, 7, 0);
        drawNum(now.minute() / 10, 19, 0);
        drawNum(now.minute() % 10, 26, 0);

        // Выводим точки
        show_dots = !show_dots;
        matrix.drawRect(15, 1, 2, 2, show_dots);
        matrix.drawRect(15, 5, 2, 2, show_dots);
        timer = millis();
      }
    }
      break;
    
    case mmTemp: {
      drawNum(int(rtc.getTemperature()) / 10, 0, 0);
      drawNum(int(rtc.getTemperature()) % 10, 7, 0);
    }
    break;
  }

  matrix.write();
}






void setup() {
  Serial.begin(9600);

  button1.attachClick(butt1Click);
  button2.attachClick(butt2Click);

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
  button1.tick();
  button2.tick();
  showDisp();
}