#ifndef __GENERIC_MCU_H__
#define __GENERIC_MCU_H__ 1
/*
* Generic MCU header
* 
* designed for Xts-uBASIC (Xtase-fgalliat @Jan16)
*
* Xtase - fgalliat @ Apr. 2018
*/

// TODO : better
#ifndef MEMORY_SIZE
 #define MEMORY_SIZE 256 * 1024
#endif

#define DBUG_NOLN(a,b) { Serial.printf( "%s %d", a, b ); }

#include "mem_utils.h"

class GenericMCU;

class GenericMCU_GPIO {
    private:
      bool ready = false;
      GenericMCU* mcu = NULL;
    public:
      GenericMCU_GPIO(GenericMCU* mcu) { this->mcu = mcu; }
      ~GenericMCU_GPIO() {}

      void setup();
      bool isReady() { return ready; }

      // 0-based
      void led(uint8_t ledNum);
      // 0-based
      bool btn(uint8_t ledNum);
};

class GenericMCU_BUZZER {
    private:
      bool ready = false;
      GenericMCU* mcu = NULL;
    public:
      GenericMCU_BUZZER(GenericMCU* mcu) { this->mcu = mcu; }
      ~GenericMCU_BUZZER() {}

      void setup();

      void playTone(int freq, int duration);
      void noTone();
};

class GenericMCU_FS {
    private:
      bool ready = false;
      GenericMCU* mcu = NULL;
    public:
      GenericMCU_FS(GenericMCU* mcu) { this->mcu = mcu; }
      ~GenericMCU_FS() {}

      void setup();
      void format();
};

class GenericMCU_SCREEN {
    private:
      bool ready = false;
      GenericMCU* mcu = NULL;
    public:
      GenericMCU_SCREEN(GenericMCU* mcu) { this->mcu = mcu; }
      ~GenericMCU_SCREEN() {}

      void setup();

      // 0 - 128x64  mono
      // 1 - 160x128 64K
      // 2 - 320x240 64K
      void setMode(uint8_t mode);

      // 0 - blit off
      // 1 - manual blitt request
      // 2 - auto blitt
      void blitt(uint8_t mode);

      // === Text routines ===
      void println(char* str) {
          print(str);
          print('\n');
      }
      void println(float val) {
          print(val);
          print('\n');
      }
      void println(int val) {
          print(val);
          print('\n');
      }

      void setCursor(int x, int y);
      void setColor(uint16_t color);

      void print(char ch);
      void print(char* str);
      void print(int   val);
      void print(float val);

      // === Picture routines ===
      void setPalette( uint16_t* pal, int length, int bpp=4 );

      // for coords : keeps int(s) instead of uint16_t(s) because coords could be negative during transforms

      // see if need to contains header : Cf fileWidth Vs drawWidth
      void drawPicture565( uint16_t* raster, int x, int y, int w=-1, int h=-1 );
      void drawPictureIndexed( uint8_t* raster, int x, int y, int w=-1, int h=-1, bool includesPalette=false );
      void drawPictureBPP( uint8_t* raster, int x, int y, int w=-1, int h=-1 );

      // === Shapes routines ===
      // 1 is always white
      // 0 is always black
      void drawRect(int x, int y, int w, int h, uint8_t mode=0, uint16_t color=1);
      void drawCircle(int x, int y, int radius, uint8_t mode=0, uint16_t color=1);

      // for fastDrawLine routines : impl. it INSIDE this routine
      // for convinience writing
      void drawLine(int x, int y, int x2, int y2, uint16_t color=1);

      // see if keep that
      void drawTriangle(int x, int y, int x2, int y2, int x3, int y3, uint8_t mode=0, uint16_t color=1);
      // see if keep that
      void drawPolyline(int* x, int* y, int nbPoints);
};

class GenericMCU_MUSIC_PLAYER {
    private:
      bool ready = false;
      GenericMCU* mcu = NULL;
    public:
      GenericMCU_MUSIC_PLAYER(GenericMCU* mcu) { this->mcu = mcu; }
      ~GenericMCU_MUSIC_PLAYER();

      void setup();

      void playTrack(int trckNum);
      void pause();
      void next();
      void prev();

      void setVolume(uint8_t volume);

      bool isPlaying();
};


class GenericMCU {

    private:
      bool lookAtISR = false;
      // see if can redef.
      void setupISR();
      void doISR();

      void builtinLED(bool state);
      bool builtinBTN();

      void init();
      void setupPreInternal();
      void setupPostInternal();

      GenericMCU_GPIO*    gpio = NULL;
      GenericMCU_BUZZER*  buzzer = NULL;
      GenericMCU_SCREEN*  screen = NULL;
      GenericMCU_FS*      fs = NULL;
      GenericMCU_MUSIC_PLAYER* musicPlayer = NULL;
    public:
      GenericMCU()  { init(); }
      ~GenericMCU() {}

      void lockISR()   { lookAtISR = false; }
      void unlockISR() { lookAtISR = true; }

      void setup() {
          setupPreInternal();

          if ( getGPIO()        != NULL ) { getGPIO()->setup();        }
          if ( getBUZZER()      != NULL ) { getBUZZER()->setup();      }
          if ( getScreen()      != NULL ) { getScreen()->setup();      }
          if ( getFS()          != NULL ) { getFS()->setup();          }
          if ( getMusicPlayer() != NULL ) { getMusicPlayer()->setup(); }

          setupISR();

          setupPostInternal();
      }

      // soft reset the MCU
      void reset();

      // -====== to be decided ... =======-

      // 0  - internal led
      // 1+ - GPIO based leds
      void led(uint8_t led, bool state=true) { 
          if ( led == 0 ) { builtinLED(state); return; }
          if ( getGPIO() == NULL ) { return; }
          getGPIO()->led( led-1 );
      }

      // 0  - internal push button
      // 1+ - GPIO based push buttons
      bool btn(uint8_t btn) { 
          if ( btn == 0 ) { return builtinBTN(); }
          if ( getGPIO() == NULL ) { return false; }
          return getGPIO()->btn( btn-1 );
      }
      // --------------------------------------
      void println(char* str) { 
          if ( getScreen() == NULL ) { Serial.println(str); return; }
          getScreen()->println( str );
      }

      void print(char ch) { 
          if ( getScreen() == NULL ) { Serial.print(ch); return; }
          getScreen()->print( ch );
      }
      // --------------------------------------
      void tone(int freq, int duration) {
          if ( getBUZZER() == NULL ) { return; }
          getBUZZER()->playTone(freq, duration);
      }
      void noTone(int freq, int duration) {
          if ( getBUZZER() == NULL ) { return; }
          getBUZZER()->noTone();
      }
      // --------------------------------------
      void delay(int millis) {
          delay(millis);
      }
      // --------------------------------------
      // read system SIGNALS
      bool getSystemMenuReqState();
      bool getSystemResetReqState();

      // -====== Hardware Getters =======-

      GenericMCU_GPIO*         getGPIO()         { return gpio; }
      GenericMCU_BUZZER*       getBUZZER()       { return buzzer; }
      GenericMCU_SCREEN*       getScreen()       { return screen; }
      GenericMCU_FS*           getFS()           { return fs; }
      GenericMCU_MUSIC_PLAYER* getMusicPlayer()  { return musicPlayer; }

};

#endif