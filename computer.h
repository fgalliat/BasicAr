/**************
 * Computer - Arduino equiv
 * Xtase - fgalliat @Nov 2017 
 *************/

#ifndef COMPUTER_H_
#define COMPUTER_H_ 1

 #include <cstdlib>
 #include <cstring>
 #include <stdio.h>


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
  };


  _Serial Serial = _Serial();


  void delay(long millis) {
      printf("TO IMPL. delay(%ld)\n", millis);
  }

  void tone(int BUZZPIN, int freq, int dur) {
      printf("TO IMPL. tone(%d, %d)\n", freq, dur);
  }

  void noTone(int BUZZPIN) {
      printf("TO IMPL. noTone()\n");
  }


  #define HIGH 255
  #define LOW    0

  #define OUTPUT       1
  #define INPUT        2
  #define INPUT_PULLUP 3

  void digitalWrite(int pin, int value) {
      printf("TO IMPL. digitalWrite(%d, %d)\n", pin, value);
  }


  int digitalRead(int pin) {
      printf("TO IMPL. digitalRead(%d)\n", pin);
      return LOW;
  }

  void pinMode(int pin, int mode) {
      printf("TO IMPL. pinMode(%d, %d)\n", pin, mode);
  }



#endif