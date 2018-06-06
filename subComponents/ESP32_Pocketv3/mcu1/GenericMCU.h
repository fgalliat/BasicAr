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

#ifndef DBUG_NOLN
  #define DBUG_NOLN(a,b) { Serial.printf( "%s %d", a, b ); }
  #define DBUG(a)        { Serial.printf( "%s \n", a ); }
#endif

#include "mem_utils.h"

// 0 - blitt locked
// 1 - manual blitt request
// 2 - auto blitt
#define SCREEN_BLITT_LOCKED  0
#define SCREEN_BLITT_REQUEST 1
#define SCREEN_BLITT_AUTO    2

// 128x64/1 -- 160x128/64K -- 320x240/64K
#define SCREEN_MODE_128    0
#define SCREEN_MODE_160    1
#define SCREEN_MODE_320    2

#define WHITE 1
#define BLACK 0


#define FS_TYPE_EOF  0
#define FS_TYPE_FILE 1
#define FS_TYPE_DIR  2

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
      void led(uint8_t ledNum, bool state);
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

      bool isReady() { return this->ready; }

      void setup();
      void format();
      void remove(char* filename);

      bool openCurrentTextFile(char* filename, bool readMode=true);
      char* readCurrentTextLine();
      void writeCurrentText(char* line, bool autoflush=true);
      void writeCurrentTextBytes(char* bytes, int len);
      void closeCurrentTextFile();

      int readBinFile(char* filename, uint8_t* dest, int maxLen, int start=0);

      void uploadViaSerial();
      // receive from bridge
      void uploadViaBridge();
      // copy to bridge
      void copyToBridge(char* filename);

      void ls(char* filter, void (*callback)(char*, int, uint8_t, int) );
      void lsMCU2();

};

class GenericMCU_SCREEN {
    private:
      bool ready = false;
      GenericMCU* mcu = NULL;
    public:
      GenericMCU_SCREEN(GenericMCU* mcu) { this->mcu = mcu; }
      ~GenericMCU_SCREEN() {}

      bool isReady() { return this->ready; }

      void setup();
      void lock();

      int getWidth();
      int getHeight();

      // 0 - 128x64  mono
      // 1 - 160x128 64K
      // 2 - 320x240 64K
      void setMode(uint8_t mode);

      // mode : TEXT_OVERWRITE / TEXT_INCRUST
      void setTextMode(uint8_t mode, uint16_t fg, uint16_t bg);

      // one of : SCREEN_BLITT_xxx
      // BASIC no more calls doBlitt() itself
      // it just sets the mode
      // Screen impl. will manage it
      void blitt(uint8_t mode);

      uint8_t getBlittMode();

      void clear();

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
      void drawPicture565( char* filename, int x, int y, int w=-1, int h=-1 );
      void drawPicture565( uint16_t* raster, int x, int y, int w=-1, int h=-1 );
      void drawPictureIndexed( uint8_t* raster, int x, int y, int w=-1, int h=-1, bool includesPalette=false );
      void drawPictureBPP( char* filename, int x, int y );
      void drawPictureBPP( uint8_t* raster, int x, int y );

      void drawPicture565Sprite( uint16_t* raster, int dx, int dy, int dw=-1, int dh=-1, int sx=-1, int sy=-1, int sw=-1, int sh=-1 );
      void drawPicture565Sprite( char* filename, int dx, int dy, int dw=-1, int dh=-1, int sx=-1, int sy=-1 );


      // === Shapes routines ===
      // 1 is always white
      // 0 is always black
      void drawRect(int x, int y, int w, int h, uint8_t mode=0, uint16_t color=1);
      void drawCircle(int x, int y, int radius, uint8_t mode=0, uint16_t color=1);

      // for fastDrawLine routines : impl. it INSIDE this routine
      // for convinience writing
      void drawLine(int x, int y, int x2, int y2, uint16_t color=1);

      void drawPixel(int x, int y, uint16_t color=1);

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
      void stop();

      void setVolume(uint8_t volume);

      bool isPlaying();
};


class GenericMCU {

    private:
      bool lookAtISR = false;
      // see if can redef.
      void setupISR();
      //void doISR();

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

    //   void lockISR()   { lookAtISR = false; }
    //   void unlockISR() { lookAtISR = true; }
    void lockISR();
    void unlockISR();

    void MASTERlockISR();
    void MASTERunlockISR();

      void doISR();

      void setup() {
          lockISR();

          setupPreInternal();

          if ( getGPIO()        != NULL ) { getGPIO()->setup();        }
          if ( getBUZZER()      != NULL ) { getBUZZER()->setup();      }
          if ( getScreen()      != NULL ) { getScreen()->setup();      }
          if ( getFS()          != NULL ) { getFS()->setup();          }
          if ( getMusicPlayer() != NULL ) { getMusicPlayer()->setup(); }


          setupPostInternal();

          setupISR();

          unlockISR();
      }

      // soft reset the MCU
      void reset();

      // -====== to be decided ... =======-

      // 0  - internal led
      // 1+ - GPIO based leds
      void led(uint8_t led, bool state=true) { 
          if ( led == 0 ) { builtinLED(state); return; }
          if ( getGPIO() == NULL ) { return; }
          getGPIO()->led( led-1, state );
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

      void print(char* str) { 
          if ( getScreen() == NULL ) { Serial.print(str); return; }
          getScreen()->print( str );
      }

      void print(char ch) { 
          if ( getScreen() == NULL ) { Serial.print(ch); return; }
          getScreen()->print( ch );
      }

      void print(int val) { 
          if ( getScreen() == NULL ) { Serial.print(val); return; }
          getScreen()->print( val );
      }
      // --------------------------------------
      void tone(int freq, int duration) {
          if ( getBUZZER() == NULL ) { return; }
          getBUZZER()->playTone(freq, duration);
      }
      void noTone() {
          if ( getBUZZER() == NULL ) { return; }
          getBUZZER()->noTone();
      }
      // --------------------------------------

    //   void delay(int millis) {
    //       // infinite loop impl.
    //       delay(millis);
    //   }

      // --------------------------------------
      // read system SIGNALS
      bool getSystemMenuReqState();
      bool getSystemMenuReqEnd();
      bool getSystemResetReqState();
      bool getSystemBreakReqState();

      // -====== Hardware Getters =======-

      GenericMCU_GPIO*         getGPIO()         { return gpio; }
      GenericMCU_BUZZER*       getBUZZER()       { return buzzer; }
      GenericMCU_SCREEN*       getScreen()       { return screen; }
      GenericMCU_FS*           getFS()           { return fs; }
      GenericMCU_MUSIC_PLAYER* getMusicPlayer()  { return musicPlayer; }

};

#endif