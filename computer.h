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

 #include <ncurses.h>

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


  static void closeComputer() {
    clrtoeol();
	refresh();
	endwin();
  }


  static void interrupts() {
      printf("TO IMPL. interrupts()\n");
  }
  static void noInterrupts() {
      printf("TO IMPL. noInterrupts()\n");
  }



  static void delay(long millis) {
      usleep( millis * 1000 );
  }

  static long millis() {
      printf("TO IMPL. millis()\n");
    return 500;
  }

  static void tone(int BUZZPIN, int freq, int dur) {
      printf("TO IMPL. tone(%d, %d)\n", freq, dur);
  }

  static void noTone(int BUZZPIN) {
      printf("TO IMPL. noTone()\n");
  }


  #define HIGH 255
  #define LOW    0

  #define OUTPUT       1
  #define INPUT        2
  #define INPUT_PULLUP 3

  static void digitalWrite(int pin, int value) {
      //printf("TO IMPL. digitalWrite(%d, %d)\n", pin, value);
  }


  static int digitalRead(int pin) {
      printf("TO IMPL. digitalRead(%d)\n", pin);
      return LOW;
  }

  static int analogRead(int pin) {
      printf("TO IMPL. analogRead(%d)\n", pin);
      return 512;
  }

  static void pinMode(int pin, int mode) {
      printf("TO IMPL. pinMode(%d, %d)\n", pin, mode);
  }



#endif