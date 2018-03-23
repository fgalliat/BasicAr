 /**************
 * Esp32 Devkit R1 Board API test
 * Xts-uBasic Pocket v3
 * Xtase - fgalliat @ Mar 2018
 ***************/


// 160 x 16 (16bpp pixels)
unsigned short color_picturebuff[ 160 * 16 ];

#define ESP32PCKv3 1

#include "ESP32_Pocketv3.h"

Esp32Pocketv3 esp32;

void setup() {
  Serial.begin(115200);
  esp32.setup();

  esp32.getScreen()->clear();

  esp32.getScreen()->println("Formatting.. Please Wait");
  esp32.getScreen()->blitt();
  // esp32.getFs()->format();
  esp32.getScreen()->println("Formatted");
  esp32.getScreen()->blitt();
  
}

void loop() {
  int rW = 320 / 60;
  int rH = 240;

  rW = 128 / 60;
  rH = 64;
  //esp32.getScreen()->clear();
  esp32.getScreen()->drawRect(0, (240-64)/2, 128, 64, 0, 1); 

  for(int i=0; i < 60; i++) {
    int uH = random(rH);
    esp32.getScreen()->drawRect(i*rW, (240-uH)/2, rW, uH, i%5, 1); 
  }
  esp32.getScreen()->blitt();

  delay(150);

    // Serial.println("Alive !");
    // delay(1000);
}
