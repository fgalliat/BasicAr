 /**************
 * Esp32 Devkit R1 Board API test
 * Xts-uBasic Pocket v3
 * Xtase - fgalliat @ Mar 2018
 ***************/


// 160 x 16 (16bpp pixels)
unsigned short color_picturebuff[ 160 * 16 ];

#define ESP32PCKv3 1
#define ESP32_I_TFT_eSPI 1
// need to be defined in <$lib$>/User_Setup_Select.h
#define ESP32_I_USE_ILI9341 1

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

  int scrW = 320;
  int scrH = 240;


  int rW = scrW / 60;
  int rH = scrH;

  // rW = 128 / 60;
  // rH = 64;
  int t0,t1;

  /* w/ AdaFruit Lib for ILI9341 -> 320x240 area -> 60 rect 15px large
  79602 micros
  82234 micros
  77179 micros
  80767 micros
  76852 micros
  75559 micros
  */
  /* w/ TFT_eSPI Lib setup for ILI9341 -> 320x240 area
  8768 micros
  9128 micros
  8793 micros
  8829 micros
  9096 micros
  9201 micros
  8644 micros
  8703 micros
  */

  // TODO : manager screen modes
  //        128x64  - 2x3px shader
  //        160x128 - could be 2x2 shader - 2x8px height
  //        320x240 - 

  // 64x3 -> 192
  // 128x2 > 256 -> pixel shader 2x3 rectangle

  t0 = micros();
  //esp32.getScreen()->clear();
  //esp32.getScreen()->drawRect(0, (240-64)/2, 128, 64, 0, 1); 
  esp32.getScreen()->drawRect(0, (240-scrH)/2, scrW, scrH, 0, 1); 

  for(int i=0; i < 60; i++) {
    int uH = random(rH);
    esp32.getScreen()->drawRect(i*rW, (240-uH)/2, rW, uH, i%5, 1); 
  }
  t1 = micros();
  esp32.getScreen()->blitt();

delay(1);
  // delay(150);
  // Serial.print( (t1-t0) ); Serial.println( " micros" );
  // delay(1000);

    // Serial.println("Alive !");
    // delay(1000);
}
