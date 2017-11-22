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

  #define O_READ 1
  #define FILE_READ O_READ
  #define O_WRITE 2
  #define FILE_WRITE O_WRITE
  #define BUILTIN_SDCARD 1

  class SdFile {
      private:
        char* name;
      public:
        SdFile() {}
        ~SdFile() {}

        int open(char* filename, int mode) {
            return 0;
        }

        void close() {
        }

        void flush() {
        }

        void print( char* str ) {
        }

        void print( int v ) {
        }

        int fgets(char* dest, int maxLen) {
            dest[0] = '?';
            return 1;
        }

        int read(void* dest, int maxLen) {
            //(*dest[0]) = '?';
            return 1;
        }
        
        int read() {
            return -1;
        }

        void seekSet(int offset) {

        }

        bool openNext( SdFile* entry, int mode ) {
            return false;
        }

        bool isSubDir() {
            return false;
        }

        bool isDir() {
            return false;
        }

        bool isHidden() {
            return false;
        }

        void getName(char* buff, int maxLen) {
            buff[0] = '@';
        }

  };

  class SDClass {
      private:
      public:
        SDClass() {}
        ~SDClass() {}

        bool begin(int pin) {
            return true;
        }

        int remove(char* filename) {
            return 0;
        }

  };

  static SDClass SD; 

  // ===================================

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