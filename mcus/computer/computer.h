#ifndef __COMPUTER_MCU_H__
#define __COMPUTER_MCU_H__ 1
/*
* Fake MCU (computer) MCU header
* 
* designed for Xts-uBASIC (Xtase-fgalliat @Jan16)
*
* Xtase - fgalliat @ Jun. 2018
*/

// else Arduino IDE will try to compile it !!!!
#ifdef COMPUTER

#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <dirent.h>

#include <ncurses.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/kd.h>

#include <math.h>
#include <SDL2/SDL.h>



  static void PC_halt() {
    clrtoeol();
    refresh();
    endwin();

    //SDL_DestroyWindow(window);
    display.close();
    SDL_Quit();

    exit(0);
  }


  // TO REMOVE
  static void setupComputer() {
  }

  // TO REMOVE
  static void closeComputer() {
    PC_halt();
  }

  // @@@@@@@@@@@ Serial emulation @@@@@@@@@@@@@@@@@
  #define KBD_CTRL_C 3
  #define KBD_BREAK 1000


  void GenericMCU::init() { 
    Serial.begin(115200);

    gpio = new GenericMCU_GPIO(this);
    buzzer = new GenericMCU_BUZZER(this);
    fs = new GenericMCU_FS(this);

    musicPlayer = new GenericMCU_MUSIC_PLAYER(this);
    screen = new GenericMCU_SCREEN(this);

    // display.__init();
  }
  void GenericMCU::setupPreInternal() { 
    // printf("pre-internal\n"); 
  }
  void GenericMCU::setupPostInternal() { 
    // Serial.println(">>coucou from Serial");
    // this->println(">>coucou from MCU");
    // printf("post-internal\n"); 
  }

  void GenericMCU::reset() { this->println("> RESET REQUEST"); PC_halt(); }
  void GenericMCU::builtinLED(bool state) { this->println("> LED REQUEST"); }
  bool GenericMCU::builtinBTN() { this->println("> BTN REQUEST"); return false; }

  static bool isISRLOCKED = false;
  static bool MASTER_LOCK_ISR = false; // beware w/ that

  #define IRAM_ATTR 

  void GenericMCU::setupISR() {  }
  void GenericMCU::lockISR() { isISRLOCKED = true; }
  void GenericMCU::unlockISR() { isISRLOCKED = false; }
  void GenericMCU::MASTERlockISR() { MASTER_LOCK_ISR = true; }
  void GenericMCU::MASTERunlockISR() { MASTER_LOCK_ISR = false; }

  void IRAM_ATTR GenericMCU::doISR() {  }

  #define BTN_1 1
  #define BTN_2 2
  #define BTN_3 3

  #define BTN_UP    4
  #define BTN_DOWN  5
  #define BTN_LEFT  6
  #define BTN_RIGHT 7

  void GenericMCU_GPIO::setup() { this->ready = true; }
  void GenericMCU_GPIO::led(uint8_t ledNum, bool state) { }
  bool GenericMCU_GPIO::btn(uint8_t btnNum) { return false;  }


  void GenericMCU_BUZZER::setup() { this->ready = true; }
  void GenericMCU_BUZZER::playTone(int freq, int duration) { }
  void GenericMCU_BUZZER::noTone() { }

  void GenericMCU_FS::setup() { this->ready = true; }
  void GenericMCU_FS::format() { mcu->println("> FS FORMAT REQUEST"); }

  void GenericMCU_FS::remove(char* filename) {
    mcu->println("> remove()");
  }

  FILE *file;
  bool fileOpened = false;

  bool  GenericMCU_FS::openCurrentTextFile(char* filename, bool readMode) {
    // "data"+"/TOTO.BAS"
    char entryName[4 + 1+ 8+1+3 +1];
    strcpy(entryName, "data");
    strcat(entryName, filename);

    // printf(">> openFIle(%s, %d)", entryName, readMode?1:0);

    file = fopen(entryName, readMode ? "r" : "w");
    if (!file) {
      fileOpened = false;
      return false;
    }

    fileOpened = true;
    return true;
  }

  void  GenericMCU_FS::closeCurrentTextFile() { 
    if (file != NULL && fileOpened) {
      fclose(file);
    }
  }

  void GenericMCU_FS::writeCurrentText(char* line, bool autoflush) {
    if (file != NULL && fileOpened) {
      fputs(line, file);
    }
  }

  void GenericMCU_FS::writeCurrentTextBytes(char* line, int len) {
    if (file != NULL && fileOpened) {
      for(int i=0; i < len; i++) {
        fputc( line[i], file );
      }
    }
  }

    const int __myLineLen = 256;
    char __myLine[__myLineLen+1];

  char* GenericMCU_FS::readCurrentTextLine() { 

    const int MAX_LINE_LEN = 256;
    memset(__myLine, 0x00, MAX_LINE_LEN +1);

    char c;
    int cpt = 0;
    while ((c = fgetc(file)) != EOF && cpt < MAX_LINE_LEN) {
        if (c == '\r' ) {
          //continue;
          break;
        }
        if (c == '\n') {
          break;
        }
        __myLine[cpt++] = c;
    }
    __myLine[cpt] = 0x00;

    if ( c == EOF && cpt == 0 ) {
      return NULL;
    }

    // printf( ">%s<\n", __myLine );

    return __myLine;
  }

  int GenericMCU_FS::readBinFile(char* filename, uint8_t* dest, int maxLen, int start) {
    // "data"+"/TOTO.BAS"
    char entryName[4 + 1+ 8+1+3 +1];
    strcpy(entryName, "data");
    strcat(entryName, filename);

    printf(">> readBinFile(%s, %d)", entryName);

    file = fopen(entryName, "r");
    if ( !file ) {
      return -1;
    }
    fseek(file, start, SEEK_SET);
    int readed = fread(dest, 1, maxLen, file);
    fclose( file );

    return readed;
  }

  void GenericMCU_FS::lsMCU2() { }
  void GenericMCU_FS::ls(char* filter, void (*callback)(char*, int, uint8_t, int) ) { }

  void GenericMCU_FS::copyToBridge(char* filename) { }

  void GenericMCU_MUSIC_PLAYER::setup() { this->ready = true; }
  void GenericMCU_MUSIC_PLAYER::playTrack(int trckNum) {
    mcu->println("play MP3()");
  }
  void GenericMCU_MUSIC_PLAYER::pause() {  }
  void GenericMCU_MUSIC_PLAYER::stop() {  }
  void GenericMCU_MUSIC_PLAYER::next() {  }
  void GenericMCU_MUSIC_PLAYER::prev() {  }
  void GenericMCU_MUSIC_PLAYER::setVolume(uint8_t volume) {  }
  bool GenericMCU_MUSIC_PLAYER::isPlaying() {
    return false;
  }

  #define MINI_LCD_WIDTH 320
  #define MINI_LCD_HEIGHT 240

  void GenericMCU_SCREEN::setup() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
    this->ready = true;
  }

  // void GenericMCU_SCREEN::clear()          { Serial.print("\n\n\n\n\n\n\n\n\n"); }
  // void GenericMCU_SCREEN::print(char* str) { Serial.print(str);  }
  // void GenericMCU_SCREEN::print(char ch)   { Serial.print(ch); }
  // void GenericMCU_SCREEN::print(int   val) { Serial.print(val); }
  // void GenericMCU_SCREEN::print(float val) { Serial.print(val); }

  int screenOffsetX=0, screenOffsetY=0;
  int __screenMode = SCREEN_MODE_320;
  #define CLR_WHITE 1
  #ifdef MAIN_INO_FILE
    uint8_t bpp_picturebuff[ 1024 ];
  #else
    extern uint8_t bpp_picturebuff[];
  #endif

  static char tmpStr[64+1];
  void GenericMCU_SCREEN::clear()          { display.clearDisplay(); }
  void GenericMCU_SCREEN::print(char* str) { display.print(str);  }
  void GenericMCU_SCREEN::print(char ch)   { display.print(ch); }
  void GenericMCU_SCREEN::print(int   val) { sprintf(tmpStr, "%d", val); display.print(tmpStr); }
  void GenericMCU_SCREEN::print(float val) { sprintf(tmpStr, "%f", val); display.print(tmpStr); }

  void GenericMCU_SCREEN::setCursor(int x, int y) { display.setCursor(x,y); }
  void GenericMCU_SCREEN::setColor(uint16_t color) { }

  void GenericMCU_SCREEN::setMode(uint8_t mode) { }
  void GenericMCU_SCREEN::setTextMode(uint8_t mode, uint16_t fg, uint16_t bg) { }
  void GenericMCU_SCREEN::blitt(uint8_t mode) { }
  
  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  // destMode is SCREEN_MODE_320
  void drawPixShaded(int x, int y, uint16_t color, int fromMode ) {
    int sx = 0; // screenOffsetX;
    int sy = 0; // screenOffsetY;

    if ( __screenMode != SCREEN_MODE_320 ) {
      display.drawPixel(screenOffsetX+x, screenOffsetY+y, color);
      return;
    }

    if ( fromMode == SCREEN_MODE_128 ) {
      sx = (320-(128*2))/2;
      sy = (240-(64*3))/2;
      x*=2; int w=2;
      y*=3; int h=3;
      display.fillRect(sx+x, sy+y, w, h, color);
    } else if ( fromMode == SCREEN_MODE_160 ) {
      sx = (320-(160*2))/2;
      sy = (240-(128*2))/2; // check if not overflow (256 Vs 240)
      x*=2; int w=2;
      y*=2; int h=2;
      display.fillRect(sx+x, sy+y, w, h, color);
    } else if ( fromMode == SCREEN_MODE_320 ) {
      display.fillRect(sx+x, sy+y, 1, 1, color);
    } 

  }
  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



  void GenericMCU_SCREEN::drawTriangle(int x, int y, int x2, int y2, int x3, int y3, uint8_t mode, uint16_t color) { }
  
  void GenericMCU_SCREEN::drawRect(int x, int y, int w, int h, uint8_t mode, uint16_t color) {
    if ( mode == 1 ) {
      display.fillRect(x,y,w,h,color);
    } else {
      display.drawRect(x,y,w,h,color);
    }
  }

  void GenericMCU_SCREEN::drawCircle(int x, int y, int radius, uint8_t mode, uint16_t color) { 
    // TODO : fillCIrlce
    display.drawCircle(x, y, radius, color);
  }

  void GenericMCU_SCREEN::drawLine(int x, int y, int x2, int y2, uint16_t color) { 
    display.drawLine(x, y, x2, y2, color);
  }

  void GenericMCU_SCREEN::drawPixel(int x, int y, uint16_t color) { 
    display.drawPixel( x, y, color );
  }

  #define MEM_RAST_HEIGHT  128
  #define MEM_RAST_WIDTH   160
  #define MEM_RAST_LEN_u16 MEM_RAST_WIDTH * MEM_RAST_HEIGHT
  #define MEM_RAST_LEN_u8  MEM_RAST_LEN_u16 * 2

  #define PCT_HEADER_LEN 7

  #ifdef MAIN_INO_FILE
    uint16_t color_picturebuff[ MEM_RAST_LEN_u16 ];
  #else
    extern uint16_t color_picturebuff[];
  #endif

  void GenericMCU_SCREEN::drawPicture565( char* filename, int x, int y, int _w, int _h ) {
    #ifndef COMPUTER
    static int w=-1, h=-1; // img size
    static char header[PCT_HEADER_LEN];
    #else
    int w=-1, h=-1; // img size
    uint8_t header[PCT_HEADER_LEN];
    #endif


    if ( !ready ) { return; }
    // uint8_t prevBlittMode = __screenBlittMode;
    // __screenBlittMode = SCREEN_BLITT_LOCKED;

    if ( filename != NULL ) {

    char entryName[4 + 1+ 8+1+3 +1];
    strcpy(entryName, "data");
    strcat(entryName, filename);

    //printf("try to open picture '%s' \n", entryName);


      //FILE* f = fopen(filename, "r");
      FILE* f = fopen(entryName, "r");
      if ( !f ) { mcu->println("File not ready"); return; }
      fseek(f, 0, SEEK_SET);
      //int readed = f.readBytes( (char*)header, PCT_HEADER_LEN);
      int readed = fread( header, 1, PCT_HEADER_LEN, f );

//printf("PCT.1\n");
      if ( header[0] == '6' && header[1] == '4' && header[2] == 'K' ) {
          w = ((uint16_t)header[3]*256) + ((uint16_t)header[4]);
          h = ((uint16_t)header[5]*256) + ((uint16_t)header[6]);
//printf("PCT.2\n");
      } else {
          mcu->println( "Wrong PCT header" );
          mcu->print( (int)header[0] );
          mcu->print( ',' );
          mcu->print( (int)header[1] );
          mcu->print( ',' );
          mcu->print( (int)header[2] );
          mcu->println( "");
          // mcu->println( header );
      }
printf("PCT.3 %d x %d \n", w, h);
      // Serial.print("A.2 "); Serial.print(w); Serial.print('x');Serial.print(h);Serial.println("");
      if( w <= 0 || h <= 0 ) {
        mcu->print("Wrong size ");
        mcu->print(w);
        mcu->print("x");
        mcu->print(h);
        mcu->println("");
        fclose(f);
        return;
      }
//printf("PCT.4\n");
      int scanZoneSize = w*MEM_RAST_HEIGHT*2; // *2 -> u16
      int startX = screenOffsetX+x;
      int startY = screenOffsetY+y;
//printf("PCT.5\n");
      int yy = 0;
      while( true ) { 
        // BEWARE : @ this time : h need to be 128
        // readed = f.readBytes( (char*)color_picturebuff, scanZoneSize);
        readed = fread( (uint8_t*)color_picturebuff, 1, scanZoneSize, f );

        // // Intel endian ?
        // for(int i=0; i < scanZoneSize-2; i+=2) {
        //   uint8_t swap = (((uint8_t*)color_picturebuff)[ i+0 ]);
        //   ((uint8_t*)color_picturebuff)[ i+0 ] = ((uint8_t*)color_picturebuff)[ i+1 ];
        //   ((uint8_t*)color_picturebuff)[ i+1 ] = swap;
        // }

//printf("PCT.6\n");
        //Serial.print("A.2 bis"); Serial.print(readed); Serial.print(" of ");Serial.print(w*h*2);Serial.println("");

        display.pushImage(startX, startY+yy, w, MEM_RAST_HEIGHT, color_picturebuff);
//printf("PCT.7\n");
        yy += MEM_RAST_HEIGHT;
        if ( yy + (MEM_RAST_HEIGHT) > h ) { break; }
      }

printf("PCT.8 %d x %d \n", w, h);
      fclose(f);
//printf("PCT.9\n");
    } else {
      // recall last MEM_RAST area
      int scanZoneSize = w*MEM_RAST_HEIGHT*2; // *2 -> u16
      int startX = screenOffsetX+x;
      int startY = screenOffsetY+y;
      display.pushImage(startX, startY, w, MEM_RAST_HEIGHT, color_picturebuff);
    }
//printf("PCT.10\n");
    //__screenBlittMode = prevBlittMode;
    //__blittIfNeeded();
  }
  


  void GenericMCU_SCREEN::drawPicture565Sprite( char* filename, int x, int y, int w, int h, int sx, int sy ) { }

  void GenericMCU_SCREEN::drawPictureBPP( char* filename, int x, int y ) {
    if ( !ready ) { return; }

    if ( filename != NULL ) {

      int readed = mcu->getFS()->readBinFile( filename, (uint8_t*)bpp_picturebuff, 1024, 0 );
      if ( readed < 0 ) { mcu->println("File not ready"); return; }

      int w = 128;
      int h = 64;

      if( w <= 0 || h <= 0 ) {
        return;
      }

      // int readed = f.readBytes( (char*)bpp_picturebuff, 1024);
      this->drawPictureBPP(bpp_picturebuff, x, y);
      // f.close();

    } else {
      // recall last MEM_RAST area
      this->drawPictureBPP(bpp_picturebuff, x, y);
    }

    // __blittIfNeeded();
  }

  void GenericMCU_SCREEN::drawPictureBPP( uint8_t* raster, int x, int y ) {
    if ( !ready ) { return; }

    int sx = screenOffsetX + x;
    int sy = screenOffsetY + y;

    int width = 128;
    int height = 64;

    unsigned char c;

    // TODO : lock blitt

    //this->drawRect(sx, sy, 128, 64, 1, 0);
    this->clear();
    //drawPixShadedRect(x, y, 128, 64, CLR_BLACK, SCREEN_MODE_128 );

    for (int yy = 0; yy < height; yy++) {
      for (int xx = 0; xx < width; xx++) {
        c = (raster[(yy * (width / 8)) + (xx / 8)] >> (7 - ((xx) % 8))) % 2;
        if (c == 0x00) {
        }
        else {
            // _oled_display->drawPixel(sx + xx, sy + yy, CLR_WHITE);
            drawPixShaded(xx, yy, CLR_WHITE, SCREEN_MODE_128 );
        }
      }
    }   
  }

  bool GenericMCU::getSystemMenuReqState()  { return this->btn( BTN_UP ) && this->btn( BTN_1 ) && this->btn( BTN_2 ); }
  bool GenericMCU::getSystemMenuReqEnd()    { return !this->btn( BTN_UP ) && !this->btn( BTN_1 ) && !this->btn( BTN_2 ); }
  bool GenericMCU::getSystemResetReqState() { return this->btn( BTN_DOWN ) && this->btn( BTN_1 ) && this->btn( BTN_2 ); }
  bool GenericMCU::getSystemBreakReqState() { return this->btn( BTN_1 ) && this->btn( BTN_2 ); }

// end of COMPUTER
#endif

#endif
