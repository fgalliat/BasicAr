 /**************
 * Esp32 Devkit R1 Board API test
 * 
 * Xts-uBasic Pocket v3
 * 
 * MCU#1
 * 
 * Xtase - fgalliat @ Mar 2018
 ***************/

// __________________

// just to identify main cpp sourcecode
#define MAIN_INO_FILE 1

// else is slave
#define MCU_MASTER 1

#include "GenericMCU.h"
#include "ESP32TwinMCU1.h"

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

  
}


int cpt = 0;

void loop() {

  if ( mcu.getScreen()->isReady() ) {
    if (cpt % 20 == 0) { mcu.getScreen()->clear(); }

    int dx = (int)(random(320-64) );
    int dy = (int)(random(240-32) );

    mcu.getScreen()->drawPicture565Sprite( cpt == 0 ? (char*)"/MP3_GUI.PCT" : (char*)NULL, dx, dy, 64, 32, 0, 0);
  }

  mcu.println("Hello from #1 Bridge");
  // delay(500); // since there is a delay(300) further
  cpt++;

  if ( mcu.btn(0) ) {
    mcu.println("Switch to bridge");
    __mcuBridgeReady = true;
    mcu.getScreen()->setup();
    mcu.getMusicPlayer()->setup();
    cpt = 0;
    return;
  }


  if ( mcu.getGPIO()->isReady() ) {
    for(int btn=0; btn < 7; btn++) {
      //if ( readGPIOBtn(btn) ) {
      if ( mcu.getGPIO()->btn(btn) ) {
        mcu.print('#');
      } else {
        mcu.print('-');
      }

      if ( btn == 3 ) {
        mcu.print(' ');
        mcu.print('|');
      }

      mcu.print(' ');
    }
    mcu.println("");
  }

  delay( 300 );

}


// // TODO : LATER : remove screen feature on MCU1
// void ___loop() {

//   int scrW = 320;
//   int scrH = 240;


//   int rW = scrW / 60;
//   int rH = scrH;

//   // rW = 128 / 60;
//   // rH = 64;
//   int t0,t1;

//   /* w/ AdaFruit Lib for ILI9341 -> 320x240 area -> 60 rect 15px large
//   79602 micros
//   82234 micros
//   77179 micros
//   80767 micros
//   76852 micros
//   75559 micros
//   */
//   /* w/ TFT_eSPI Lib setup for ILI9341 -> 320x240 area
//   8768 micros
//   9128 micros
//   8793 micros
//   8829 micros
//   9096 micros
//   9201 micros
//   8644 micros
//   8703 micros
//   */

//   // TODO : manager screen modes
//   //        128x64  - 2x3px shader
//   //        160x128 - could be 2x2 shader - 2x8px height
//   //        320x240 - 

//   // 64x3 -> 192
//   // 128x2 > 256 -> pixel shader 2x3 rectangle

//   t0 = micros();
//   //esp32.getScreen()->clear();
//   //esp32.getScreen()->drawRect(0, (240-64)/2, 128, 64, 0, 1); 
//   esp32.getScreen()->drawRect(0, (240-scrH)/2, scrW, scrH, 0, 1); 

//   for(int i=0; i < 60; i++) {
//     int uH = random(rH);
//     esp32.getScreen()->drawRect(i*rW, (240-uH)/2, rW, uH, i%5, 1); 
//   }
//   t1 = micros();
//   esp32.getScreen()->blitt();

// delay(1);
//   // delay(150);
//   // Serial.print( (t1-t0) ); Serial.println( " micros" );
//   // delay(1000);

//     // Serial.println("Alive !");
//     // delay(1000);
// }
