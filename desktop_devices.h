/***********
 Xtase - fgalliat @Sep 2017
***********/

#ifndef __desk_dev_h_ 
#define __desk_dev_h_ 1

//#include <Arduino.h>



#ifdef BUT_TEENSY
 // is really an AVR equiv....

  // =======================================================
  static char * dtostre(
    double __val,           // value
    char * __s,             // dest string 
    unsigned char __prec,   // precision
    unsigned char __flags) {    // ???
  
          sprintf( __s, "%f", __val );
          return __s;
  }
  
  static char * dtostrf(
    double __val,           // value
    signed char __width,    // width (? padding ?)
    unsigned char __prec,   // precision
      char * __s) {           // dest string
          
          // TODO : width
          sprintf( __s, "%f", __val );
          return __s;
  }
  
  
  // =======================================================

  // NOT FOR TEENSY
//   // code in host.cpp
//   extern void _ISR_emul();
  
//   // moa - fake
//   static void emulateInterrupt() {
//     // code in host.cpp
//     _ISR_emul();
//   }

  #define PS2_DELETE 8
  #define PS2_ENTER 13
  #define PS2_ESC 27

  class PS2Keyboard {
      private:
        // N.B. : unsigned long  is very important
        unsigned long lastTime = 0;
      public:
          void begin(int dataPin, int irqPin) {

            // "Retour Chariot" + 115200bps
            
            Serial.begin(115200);
            while( !Serial ) { delay(200); } // beware with that !!!
            this->lastTime = millis();
          }
          
          int available() { 

            // #ifndef BUT_TEENSY
            //     if ( millis() - this->lastTime > 4000 ) {
            //     emulateInterrupt();
            //     this->lastTime = millis();
            //     }
            //     delay(10);
            // #endif


            // EWARE W/ FULL DUPLEX -> use events
            return Serial.available(); 
            //return 0;
          }
  
          int read() { 
              return Serial.read();
              //return -1;
          }
          
  
  };
  
  static void gen_clearscreen() {
      //clear();
      Serial.println("\r\n\r\n\r\n\r\n\r\n\r\n");
  }
  
  static void printAt(int x, int y, char* str) {
      //mvprintw(y, x, str);
      //refresh();
      Serial.print( str );
  }
  
  #define SSD1306_SWITCHCAPVCC 1
  class SSD1306ASCII {
      int x = 0;
      int y = 0; int lastY = 0;
      char chs[2] = { 0x00, 0x00 };
      public:
          SSD1306ASCII(int _OLED_DATA, int _OLED_CLK, int _OLED_DC, int _OLED_RST, int _OLED_CS) {
          };
          
          void ssd1306_init(int mode) {};
  
          void print(char ch) { 
              // ncurses impl
              //static char chs[2]; chs[0] = ch; chs[1]=0x00;
              this->chs[0] = ch;
              /*
              if ( ch == 13 ) {
                  return;
              }
  
              if ( ch == 10 ) {
                  this->y++; this->x=0;
                  return;
              }
  
              printAt(this->x, this->y, this->chs);
              this->x++;
              */
              Serial.write( ch );
          };
  
          void setCursor(int x, int y) { 
            //this->x=x; this->y=y; 
            if ( y == 0 ) { 
              lastY = y;
              //Serial.print("\r\n\r\n\r\n");
              Serial.write('\n');
            } else if ( y > lastY ) { 
              lastY = y;
              //Serial.print("\r\n");
              Serial.write('\n');
            }
          };
  
          void clear() { 
              //gen_clearscreen();
              this->setCursor(0,0);
          };
  };
  
  class EEPROMClass {
      private:
          int content[255];
  
      public:
          int read(int addr) { 
              if ( addr >= 256 ) {
               printAt(1, 1, "ERROR EEPROM ADDR");
               return 0;
              }
              return this->content[ addr ]; 
          }
  
          void write(int addr, int value) { 
              this->content[addr] = value; 
              printAt(0, 5, "WRITING ON EEPROM");
          }
  };
  
  static EEPROMClass EEPROM = EEPROMClass();

#else

  
  #include <cmath>
  
  #include <stdio.h>
  #include <ncurses.h> // for console work....
  #include <string.h>
  
  // =======================================================
  static char * dtostre(
  	double __val,           // value
  	char * __s,             // dest string 
  	unsigned char __prec,   // precision
  	unsigned char __flags) {    // ???
  
          sprintf( __s, "%f", __val );
          return __s;
  }
  
  static char * dtostrf(
  	double __val,           // value
  	signed char __width,    // width (? padding ?)
  	unsigned char __prec,   // precision
      char * __s) {           // dest string
          
          // TODO : width
          sprintf( __s, "%f", __val );
          return __s;
  }
  
  
  // =======================================================
  
  #define PROGMEM 
  
  static unsigned char pgm_read_byte_near(const void* mem_addr) {
      char t = *((char *) mem_addr);
      if ( t < 0 ) { t += 256; }
      return (unsigned char)t;
  }
  
  static unsigned char pgm_read_byte(const void* mem_addr) {
      return pgm_read_byte_near(mem_addr);
  }
  
  //static char* pgm_read_word(const void* mem_addr) {
  static void* pgm_read_word(const void* mem_addr) {
      //return (char*)mem_addr;
      return (void*)mem_addr;
  }
  // ========================================
  // float floor(float f) { return (float)(int)f; }
  // ========================================
  
  #include <unistd.h>
  static void delay(int millis) {
    usleep( millis * 1000 );
  }
  // ========================================
  
  #define LOW 0
  #define HIGH 255
  
  #define INPUT 1
  #define INPUT_PULLUP 2
  #define OUTPUT 3
  
  static void digitalWrite(int PIN, int STATE) {
      // TODO : DBUG
  }
  
  static void pinMode(int PIN, int MODE) {
      
  }
  
  static int digitalRead(int PIN) {
      return 0;
  }
  
  static int analogRead(int PIN) {
      return 0;
  }
  
  // ========================================
  
  static int TIMSK1 = 0;
  static int TCCR1B = 0;
  static int TCNT1 = 0;
  static int TCCR1A = 0;
  #define TOIE1 1
  #define CS12 2
  
  static void interrupts() {}
  static void noInterrupts() {}
  
  // ========================================
  
  static bool kbInited = false;
  
  #define PS2_DELETE 8
  #define PS2_ENTER 13
  #define PS2_ESC 27
  
  static int lastChar = -1;
  static bool prevAvailable = false;
  
  // NOT FOR TEENSY !!!!!!!!
  // code in host.cpp
  extern void _ISR_emul();
  
  // moa - fake
  static void emulateInterrupt() {
    // code in host.cpp
    _ISR_emul();
  }
  
  
  
  class PS2Keyboard {
  
      public:
          void begin(int dataPin, int irqPin) {}
  
          int __getch() {
              int i = getch();
              if ( i == 10 ) { i = PS2_ENTER; }
              
              #ifndef BUT_TEENSY
                emulateInterrupt();
              #endif
  
              return i;
          }
  
          // ncurses impl.
          int available() { prevAvailable=true; lastChar = this->__getch(); delay(10); return lastChar > -1 ? 1 : 0; }
  
          int read() { 
              // test if available has been called
              if ( prevAvailable ) { prevAvailable = false; return lastChar; }
              else { lastChar = this->__getch(); delay(10); return lastChar; }
          }
          
  
  };
  
  static void gen_clearscreen() {
      clear();
  }
  
  static void printAt(int x, int y, char* str) {
      mvprintw(y, x, str);
      refresh();
  }
  
  #define SSD1306_SWITCHCAPVCC 1
  class SSD1306ASCII {
      int x = 0;
      int y = 0;
      char chs[2] = { 0x00, 0x00; }
      public:
          SSD1306ASCII(int _OLED_DATA, int _OLED_CLK, int _OLED_DC, int _OLED_RST, int _OLED_CS) {
          }
          
          void ssd1306_init(int mode) {}
  
          void print(char ch) { 
              // ncurses impl
              //static char chs[2]; chs[0] = ch; chs[1]=0x00;
              this->chs[0] = ch;
  
              if ( ch == 13 ) {
                  return;
              }
  
              if ( ch == 10 ) {
                  this->y++; this->x=0;
                  return;
              }
  
              printAt(this->x, this->y, this->chs);
              this->x++;
          }
  
          void setCursor(int x, int y) { this->x=x; this->y=y; }
  
          void clear() { 
              //printf("\n\n\n\n\n\n===== CLS ===\n"); 
              //mvprintw(0,0,"\n\n\n\n\n\n===== CLS ===\n"); 
              //clear(); // ncurses -> loop segfault
              gen_clearscreen();
              this->setCursor(0,0);
          }
  };
  
  class EEPROMClass {
      private:
          int content[2048];
  
      public:
          int read(int addr) { 
              if ( addr >= 2048 ) {
               printAt(1, 1, "ERROR EEPROM ADDR");
               return 0;
              }
              return this->content[ addr ]; 
          }
  
          void write(int addr, int value) { 
              this->content[addr] = value; 
              printAt(0, 5, "WRITING ON EEPROM");
          }
  };
  
  static EEPROMClass EEPROM = EEPROMClass();
  
  
  static void setupComputer() {
      WINDOW *w = initscr();
      cbreak();
      noecho();
      nodelay(w, TRUE); // prevent (ncurses) getch() from blocking !!!!!!
  }
  
  static void closeComputer() {
      printAt(0,0, "Bye");
      clrtoeol();
      refresh();
      endwin();
  }

// of BUT_TEENSY
#endif

#endif