/*
* Fake MCU (computer) Arduino header
* 
* designed for Xts-uBASIC (Xtase-fgalliat @Jan16)
*
* Xtase - fgalliat @ Jun. 2018
*/


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

#ifndef COMPARDUINO 
#define COMPARDUINO 1

  #define LOW 0
  #define HIGH 255

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)
#define abs(a) (a < 0 ? -a : a)
static void swap(int& a, int& b) { int swp=a; a=b; b=swp; }

#define icos(a) cos((double)a / 3.141596 * 180.0)
#define isin(a) sin((double)a / 3.141596 * 180.0)

#define WHITE 0x01
#define BLACK 0x00
#define SSD1306_LCDHEIGHT 64
#define SSD1306_SWITCHCAPVCC 1111

  // arduino compat. functions
  static void delay(long millis) {
    // usleep( millis * 1000 );
    printf("delay(%d)\n", millis);
    SDL_Delay(millis);
  }

  static long millis() {
    std::printf("TO IMPL. millis()\n");
    return 500;
  }

// void tone(int BUZZPIN, int freq, int dur) {
//     Beep(freq, dur);
//     //buzzer.wait();
// }
// void noTone(int BUZZPIN){
// }
  static void digitalWrite(int pin, int value) {
      //printf("TO IMPL. digitalWrite(%d, %d)\n", pin, value);
  }
  static int digitalRead(int pin) {
      //printf("TO IMPL. digitalRead(%d)\n", pin);
      //return LOW;
      // Cf INPUT_PULLUP btns
      return HIGH;
  }
  static int analogRead(int pin) {
      std::printf("TO IMPL. analogRead(%d)\n", pin);
      return 512;
  }
  static void pinMode(int pin, int mode) {
      std::printf("TO IMPL. pinMode(%d, %d)\n", pin, mode);
  }

#endif
#endif