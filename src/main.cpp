#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

#include "main.h"

Max72xxPanel matrix = Max72xxPanel(9, 1, 4);










void drawNum(uint8_t num, uint8_t x, uint8_t y) {
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 6; c++) {
      matrix.drawPixel(x + c, y + r, numbers[num][r] & (1 << c));
    }
  }  
}



void setup() {
  matrix.setIntensity(15);                    // Задаем яркость от 0 до 15
  matrix.setRotation(3);
  matrix.fillScreen(LOW);                       // Обнуление матрицы
}



void loop() {
  // for (int y = 0; y < 8; y++ ) {
  //   for (int x = 0; x < 32; x++ ) { // Передача массива
  //     matrix.drawPixel(x, y, HIGH);
  //     matrix.write();
  //     delay(200);
  //   }
  // }
  drawNum(9, 0, 0);
  matrix.write();
}