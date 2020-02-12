#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

Max72xxPanel matrix = Max72xxPanel(9, 1, 4);

void setup() {
  matrix.setIntensity(15);                    // Задаем яркость от 0 до 15
  matrix.setRotation(3);
}

void loop() {
  matrix.fillScreen(LOW);                       // Обнуление матрицы
  for (int y = 0; y < 8; y++ ) {
    for (int x = 0; x < 32; x++ ) { // Передача массива
      matrix.drawPixel(x, y, HIGH);
      matrix.write();
      delay(200);
    }
  }
}