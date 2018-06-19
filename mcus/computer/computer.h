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

    // exit(0);
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


  void GenericMCU::init() { }
  void GenericMCU::setupPreInternal() {  }
  void GenericMCU::setupPostInternal() {  }

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

  void GenericMCU_GPIO::setup() { }
  void GenericMCU_GPIO::led(uint8_t ledNum, bool state) { }
  bool GenericMCU_GPIO::btn(uint8_t btnNum) { return false;  }


  void GenericMCU_BUZZER::setup() { }
  void GenericMCU_BUZZER::playTone(int freq, int duration) { }
  void GenericMCU_BUZZER::noTone() { }

  void GenericMCU_FS::setup() { }
  void GenericMCU_FS::format() { mcu->println("> FS FORMAT REQUEST"); }

  void GenericMCU_FS::remove(char* filename) {
    mcu->println("> remove()");
  }
  bool  GenericMCU_FS::openCurrentTextFile(char* filename, bool readMode) {
    mcu->println("> openFile()");
    return false;
  }

  void  GenericMCU_FS::closeCurrentTextFile() { }
  void GenericMCU_FS::writeCurrentText(char* line, bool autoflush) { }
  void GenericMCU_FS::writeCurrentTextBytes(char* line, int len) { }

  char* GenericMCU_FS::readCurrentTextLine() { return NULL; }
  int GenericMCU_FS::readBinFile(char* filename, uint8_t* dest, int maxLen, int start) {
    mcu->println("readBin()");
    return 0;
  }

  void GenericMCU_FS::lsMCU2() { }
  void GenericMCU_FS::ls(char* filter, void (*callback)(char*, int, uint8_t, int) ) { }

  void GenericMCU_FS::copyToBridge(char* filename) { }

  void GenericMCU_MUSIC_PLAYER::setup() { }
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
  }

  void GenericMCU_SCREEN::clear()          { Serial.print("\n\n\n\n\n\n\n\n\n"); }
  void GenericMCU_SCREEN::print(char* str) { Serial.print(str);  }
  void GenericMCU_SCREEN::print(char ch)   { Serial.print(ch); }
  void GenericMCU_SCREEN::print(int   val) { Serial.print(val); }
  void GenericMCU_SCREEN::print(float val) { Serial.print(val); }

  void GenericMCU_SCREEN::setCursor(int x, int y) { }
  void GenericMCU_SCREEN::setColor(uint16_t color) { }

  void GenericMCU_SCREEN::setMode(uint8_t mode) { }
  void GenericMCU_SCREEN::setTextMode(uint8_t mode, uint16_t fg, uint16_t bg) { }
  void GenericMCU_SCREEN::blitt(uint8_t mode) { }
  void GenericMCU_SCREEN::drawRect(int x, int y, int w, int h, uint8_t mode, uint16_t color) { }
  void GenericMCU_SCREEN::drawTriangle(int x, int y, int x2, int y2, int x3, int y3, uint8_t mode, uint16_t color) { }
  void GenericMCU_SCREEN::drawCircle(int x, int y, int radius, uint8_t mode, uint16_t color) { }
  void GenericMCU_SCREEN::drawLine(int x, int y, int x2, int y2, uint16_t color) { }
  void GenericMCU_SCREEN::drawPixel(int x, int y, uint16_t color) { }
  void GenericMCU_SCREEN::drawPicture565( char* filename, int x, int y, int _w, int _h ) { }
  void GenericMCU_SCREEN::drawPicture565Sprite( char* filename, int x, int y, int w, int h, int sx, int sy ) { }
  void GenericMCU_SCREEN::drawPictureBPP( char* filename, int x, int y ) { }

  bool GenericMCU::getSystemMenuReqState()  { return this->btn( BTN_UP ) && this->btn( BTN_1 ) && this->btn( BTN_2 ); }
  bool GenericMCU::getSystemMenuReqEnd()    { return !this->btn( BTN_UP ) && !this->btn( BTN_1 ) && !this->btn( BTN_2 ); }
  bool GenericMCU::getSystemResetReqState() { return this->btn( BTN_DOWN ) && this->btn( BTN_1 ) && this->btn( BTN_2 ); }
  bool GenericMCU::getSystemBreakReqState() { return this->btn( BTN_1 ) && this->btn( BTN_2 ); }

// end of COMPUTER
#endif

#endif
