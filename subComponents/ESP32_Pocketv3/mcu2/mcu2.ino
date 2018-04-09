 /**************
 * Esp32 Devkit R1 Board API test
 * Xts-uBasic Pocket v3
 * Xtase - fgalliat @ Mar 2018
 ***************/

// __________________

// just to identify main cpp sourcecode
#define MAIN_INO_FILE 1

// else is slave
// #define MCU_MASTER 1

#include "GenericMCU.h"
#include "ESP32TwinMCU2.h"

GenericMCU mcu;
// __________________



// // 160 x 16 (16bpp pixels)
// unsigned short color_picturebuff[ 160 * 16 ];

#define ESP32PCKv3 1
#define ESP32_I_TFT_eSPI 1
// need to be defined in <$lib$>/User_Setup_Select.h
#define ESP32_I_USE_ILI9341 1


#ifdef ESP32PCKv3
  // #include "ESP32_Pocketv3.h"
  // Esp32Pocketv3 esp32;
  
  // ======== inner GPIO ====================
  void led(bool state) {
    mcu.led(0, state);
  }

#endif



void setup() {
  // Serial.begin(115200); // MUST NOT !!!!
  mcu.setup();

  // Blink the LED a few times before we start:
  for (int i=0; i<5; i++) {
    led(true);
    delay(100);
    led(false);
    delay(100);
  }

  // BRIDGE UART provided by "ESP32TwinMCU1.h"
  // mcuBridge.println("/READY");

  mcu.getScreen()->setMode( SCREEN_MODE_320 );
  mcu.getScreen()->setMode( SCREEN_MODE_160 );

}


int cpt = 0;

void loop() {

  if ( mcuBridge.available() > 0 ) {
    uint8_t cmd = mcuBridge.read();
    if ( cmd > 0 && cmd <= SIG_LAST ) {
      uint8_t nbToRead = __bridge_params[cmd];
      // ...
    } else {
      mcu.println("Unknown cmd...");
    }
  }

  int scrW = mcu.getScreen()->getWidth();
  int scrH = mcu.getScreen()->getHeight();

  int rW = scrW / 60;
  int rH = scrH;
  
  int t0,t1;

  /* w/ AdaFruit Lib for ILI9341 -> 320x240 area -> 60 rect 15px large
  79602 micros
  82234 micros
  77179 micros
  */
  /* w/ TFT_eSPI Lib setup for ILI9341 -> 320x240 area
  8768 micros
  9128 micros
  8793 micros
  */

  // TODO : manager screen modes
  //        128x64  - 2x3px shader
  //        160x128 - could be 2x2 shader - 2x8px height
  //        320x240 - no shaders
  // 64x3 -> 192
  // 128x2 > 256 -> pixel shader 2x3 rectangle

  t0 = micros();
  mcu.getScreen()->blitt( SCREEN_BLITT_LOCKED );
  mcu.getScreen()->clear();
  for(int i=0; i < 60; i++) {
    int uH = random(rH);
    mcu.getScreen()->drawRect(i*rW, (mcu.getScreen()->getHeight()-uH)/2, rW, uH, 1, i%5); 
  }
  mcu.getScreen()->blitt( SCREEN_BLITT_AUTO );
  t1 = micros();

  if ( (cpt++) % 50 == 0 ) {
    Serial.print( (t1-t0) );
    Serial.print(" micros\n");
  }

  delay(1);
}
