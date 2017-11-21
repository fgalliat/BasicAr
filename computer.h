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


  class _Serial {
      private:
        // N.B. : unsigned long  is very important
        unsigned long lastTime = 0;
      public:
          void begin(int speed) {
          }
          
          int available() { 
            return 0;
          }
  
          int read() { 
            return -1;
          }
  

          // TODO : use curses
          void print(int v) { printf("%d", v); }
          void print(const char* v) { printf("%s", v); }
          void println(int v) { printf("%d\n", v); }
          void println(const char* v) { printf("%s\n", v); }

          void flush() {}
  };


  static _Serial Serial = _Serial();


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