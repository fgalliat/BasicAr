/**************
 * Computer - Arduino equiv
 * Xtase - fgalliat @Nov 2017 
 *************/

#ifndef COMPUTER_H_
#define COMPUTER_H_ 1

 #include <cstdlib>
 #include <cstring>
 #include <stdio.h>
 #include <unistd.h>
 #include <iostream>

 #include <ncurses.h>
 #include <sys/ioctl.h>
 #include <fcntl.h>
 #include <linux/kd.h>

 #include <SDL2/SDL.h>
 #include "dev_sound_pcspeaker.h"

  static SDL_Window *window;

  class _Serial {
      private:
        // N.B. : unsigned long  is very important
        unsigned long lastTime = 0;
        int lastKC = -1;
      public:
          void begin(int speed) {
             WINDOW *w = initscr();
             cbreak();
             noecho();
             nodelay(w, TRUE); // prevent getch() from blocking !!!!!!
          }
          
          int available() { 
            lastKC = getch(); // ncurses version
            return lastKC > -1 ? 1 : 0;
          }
  
          int read() { 
            int ret = lastKC;
            lastKC = -1;
            return ret;
          }
  
          void _printAt(int x, int y, char* str) {
            mvprintw(y, x, str);
          }

          // TODO : use curses
          void print(int v) { printf("%d", v); }
          void println(int v) { printf("%d\n", v); }
          
          void print(const char* v) { 
              int l = strlen(v);
              char* vv = (char*)malloc(l+1);
              for ( int i=0; i < l; i++ ) {
                  if ( vv[i] == '\r' ) { vv[i] = '\n'; }
                  else vv[i] = v[i];
              }
              vv[l] = 0x00;
              printf("%s", vv); 
              free(vv);        
              refresh();      
          }

          void println(const char* v) { 
              int l = strlen(v);
              char* vv = (char*)malloc(l+1);
              for ( int i=0; i < l; i++ ) {
                  if ( vv[i] == '\r' ) { vv[i] = 0x00; }
                  else vv[i] = v[i];
              }
              vv[l] = 0x00;
              printf("%s\n", vv); 
              free(vv);
              refresh();
          }

          void flush() {}
  };


  static _Serial Serial = _Serial();



  void setupComputer();


  void closeComputer();


  void interrupts();
  void noInterrupts();

  void delay(long millis);

  long millis();

  void tone(int BUZZPIN, int freq, int dur);

  void noTone(int BUZZPIN);


  #define HIGH 255
  #define LOW    0

  #define OUTPUT       1
  #define INPUT        2
  #define INPUT_PULLUP 3

  void digitalWrite(int pin, int value);


  int digitalRead(int pin);

  int analogRead(int pin);

  void pinMode(int pin, int mode);



#endif