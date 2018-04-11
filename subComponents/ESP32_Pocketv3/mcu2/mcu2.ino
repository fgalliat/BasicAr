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
  // mcu.getScreen()->setMode( SCREEN_MODE_160 );

}


int cpt = 0;
bool yetDrawn = false;
bool inCmdMode = false;

#define DBUG_TIME 1


uint16_t bridge_readU16() {
  uint8_t d0 = mcuBridge.read();
  uint8_t d1 = mcuBridge.read();
return ( uint16_t ) ((uint16_t)d0)*256 + ((uint16_t)d1);
}

// 0-terminated
void bridge_readString(char* dest, int start, int max) {
  int i = start, tmp;
  while( true ) {
    if ( i-start >= max ) { break; }
    tmp = mcuBridge.read();
    if ( tmp <= 0 ) { break; }
    dest[i++] = tmp;
  }
  dest[i] = 0x00;
}

void loop() {

  if ( mcuBridge.available() > 0 ) {
    uint8_t cmd = mcuBridge.read();
    if ( cmd > 0 && cmd <= SIG_LAST ) {
      uint8_t nbToRead = __bridge_params[cmd];

      if ( nbToRead == -2 ) {
        mcu.print("Unknown cmd...");
        mcu.print(cmd);
        mcu.print('\n');
      }

      // TMP ????
      inCmdMode = true;

      static int tmp, i, x, y;
      static float tmpf;
      static char str[256+1];
      static unsigned char num[4];

      switch(cmd) {
        case SIG_MCU_RESET:
          mcu.reset();
          break;
        case SIG_MCU_UPLOAD_BDG:
          mcu.getFS()->uploadViaBridge();
          break;
        case SIG_MP3_PLAY:
        case SIG_MP3_PAUSE:
        case SIG_MP3_NEXT:
        case SIG_MP3_PREV:
        case SIG_MP3_VOL:
          break;

        case SIG_SCR_MODE:
          tmp = mcuBridge.read();
          if ( tmp == 0xFF ) { tmp = SCREEN_MODE_320; }
          mcu.getScreen()->setMode( tmp );
          break;

        case SIG_SCR_CLEAR:
          mcu.getScreen()->clear();
          break;

        case SIG_SCR_CURSOR:
        case SIG_SCR_COLOR:
        case SIG_SCR_BLITT:
          break;

        case SIG_SCR_PRINT_CH:
          tmp = mcuBridge.read();
          mcu.getScreen()->print( (char)tmp );
          break;
        case SIG_SCR_PRINT_STR:
          tmp = mcuBridge.available();
          // BEWARE w/ THAT
          // tmp = mcuBridge.readBytes( str, tmp );
          bridge_readString(str, 0, 256);
          mcu.getScreen()->print(str);
          break;
        case SIG_SCR_PRINT_INT:
          tmp = mcuBridge.readBytes( num, 4 );
          // from mem_utils.h
          tmpf = getFloatFromBytes(num, 0);
          mcu.getScreen()->print( (int)tmpf );
          break;
        case SIG_SCR_PRINT_NUM:
          tmp = mcuBridge.readBytes( num, 4 );
          // from mem_utils.h
          tmpf = getFloatFromBytes(num, 0);
          mcu.getScreen()->print(tmpf);
          break;

        case SIG_SCR_DRAW_PIX:
        case SIG_SCR_DRAW_LINE:
        case SIG_SCR_DRAW_RECT:
        case SIG_SCR_DRAW_CIRCLE:
        case SIG_SCR_DRAW_TRIANGLE:
          break;
        case SIG_SCR_DRAW_BPP:
          break;
        case SIG_SCR_DRAW_PCT:
          x = bridge_readU16();
          y = bridge_readU16();
          tmp = mcuBridge.available();
          bridge_readString(str, 0, 256);

          // mcu.print( x );
          // mcu.print( ' ' );
          // mcu.print( y );
          // mcu.print( '/' );
          // // mcu.print( tmp );
          // // mcu.print( ' ' );
          // mcu.print( (int)strlen( str ) );
          // mcu.print( ' ' );
          // mcu.print( str );
          // mcu.print( '\n' );


          if ( strlen( str ) == 0 ) {
            // recall last GFX area
            mcu.getScreen()->drawPicture565((char*)NULL, x, y);
          } else {
            mcu.getScreen()->drawPicture565(str, x, y);
          }
          break;
        default:
          mcu.print("NYI cmd...");
          mcu.print(cmd);
          mcu.print('\n');

          delay(500);
      }

    } else {
      mcu.print("Invalid cmd...");
      mcu.print(cmd);
      mcu.print('\n');

      delay(500);
    }
  }

  // temp ????
  if ( inCmdMode ) { 
    delay(10);
    yield();
    return; 
  }

  int scrW = mcu.getScreen()->getWidth();
  int scrH = mcu.getScreen()->getHeight();

  int scrPctX = (scrW - 160)/2;
  int scrPctY = (scrH - 128)/2;

  int rW = scrW / 60;
  int rH = scrH;
  
  int t0,t1;

  // TODO : manager screen modes
  //        128x64  - 2x3px shader
  //        160x128 - could be 2x2 shader - 2x8px height
  //        320x240 - no shaders
  // 64x3 -> 192
  // 128x2 > 256 -> pixel shader 2x3 rectangle

  GenericMCU_SCREEN* screen = mcu.getScreen();

  t0 = micros();
  screen->blitt( SCREEN_BLITT_LOCKED );
  //mcu.getScreen()->clear();
  for(int i=0; i < 60; i++) {
    int uH = random(rH);
    // cleaner
    screen->drawRect(i*rW, 0, rW, mcu.getScreen()->getHeight(), 1, 0); 
    // drawer
    screen->drawRect(i*rW, (mcu.getScreen()->getHeight()-uH)/2, rW, uH, 1, i%5); 
  }
  screen->blitt( SCREEN_BLITT_AUTO );
  t1 = micros();

  if ( (cpt++) % 30 == 0 ) {
    #ifdef DBUG_TIME
      // 80ms @ 27MKz SPI
      // 52ms @ 40MKz SPI
      Serial.print( (t1-t0) );
      Serial.print(" micros\n");
    #endif

    t0 = micros();
    if ( !yetDrawn ) {
      screen->drawPicture565("/TEST.PCT", scrPctX, scrPctY);
      yetDrawn = true;
    } else {
      // recall last memArea : here 160x128
      // 19ms @ 27MKz SPI
      // 9ms @ 40MKz SPI
      screen->drawPicture565( (char*)NULL, scrPctX, scrPctY);
    }
    t1 = micros();
    #ifdef DBUG_TIME
      Serial.print( (t1-t0) );
      Serial.print(" micros (picture)\n");
    #endif
    delay(700);
  }

  delay(1);
}
