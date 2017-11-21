/***********
 Xtase - fgalliat @Sep 2017
***********/

#ifndef __desk_dev_h_ 
#define __desk_dev_h_ 1

#ifdef COMPUTER
  #include "computer.h"
#endif

  // =======================================================

  #define PS2_DELETE 8
  #define PS2_ENTER 13
  #define PS2_ESC 27

// =========/ Serial Event /==============
// extern String inputString;
// extern volatile boolean stringComplete;
extern volatile bool isWriting;
// =========/ Serial Event /==============

  class PS2Keyboard {
      private:
        // N.B. : unsigned long  is very important
        unsigned long lastTime = 0;
      public:
          void begin(int dataPin, int irqPin) {

            // "Retour Chariot" + 115200bps
            Serial.begin(115200);
            //while( !Serial ) { delay(200); } // beware with that !!!

            this->lastTime = millis();
          }
          
          int available() { 
            // BEWARE W/ FULL DUPLEX -> use events
            // delay(5);
            return Serial.available(); 

            //return inputString.length();

          }
  
          int read() { 
              int tmpCh = Serial.read();
              if ( tmpCh == 10 || tmpCh==13 ) { tmpCh = PS2_ENTER; }
              delay(5);
              return tmpCh;

            // if ( inputString.length() > 0 ) {
            //   char ch = inputString.charAt(0);
            //   inputString.remove(0,1);
            //   return ch;
            // }
            // return -1;
            // return Serial.read();
          }
  
  };
  
  // static void gen_clearscreen() {
  //     //clear();
  //     isWriting = true;
  //     Serial.println("\r\n\r\n\r\n\r\n\r\n\r\n");
  //     isWriting = false;
  // }
  
  // static void printAt(int x, int y, char* str) {
  //     //mvprintw(y, x, str);
  //     //refresh();
  //     isWriting = true;
  //     Serial.print( str );
  //     isWriting = false;
  // }
  
  #define SSD1306_SWITCHCAPVCC 0x2
  class SSD1306ASCII {
      int x = 0;
      int y = 0; int lastY = 0;
      char chs[2] = { 0x00, 0x00 };
      public:
          SSD1306ASCII(int _OLED_DATA, int _OLED_CLK, int _OLED_DC, int _OLED_RST, int _OLED_CS) {
          };
          
          void ssd1306_init(int mode) {};
  
          // void print(char ch) { 
          //     this->chs[0] = ch;
          //     /*
          //     if ( ch == 13 ) {
          //         return;
          //     }
  
          //     if ( ch == 10 ) {
          //         this->y++; this->x=0;
          //         return;
          //     }
  
          //     printAt(this->x, this->y, this->chs);
          //     this->x++;
          //     */
          //     isWriting = true;
          //     Serial.write( ch );
          //     isWriting = false;
          // };
  
          // void setCursor(int x, int y) { 
          //   isWriting = true;
          //   //this->x=x; this->y=y; 
          //   if ( y == 0 ) { 
          //     lastY = y;
          //     //Serial.print("\r\n\r\n\r\n");
          //     Serial.write('\n');
          //   } else if ( y > lastY ) { 
          //     lastY = y;
          //     //Serial.print("\r\n");
          //     Serial.write('\n');
          //   }
          //   isWriting = false;
          // };
  
          // void clear() { 
          //     //gen_clearscreen();
          //     //this->setCursor(0,0);

          //     display.clearDisplay();
          // };
  };
  
#ifdef COMPUTER
  class EEPROMClass {
      private:
          int content[1024];
  
      public:
          int read(int addr) { 
              if ( addr >= 1024 ) {
               Serial.println("\n\n\nERROR EEPROM ADDR\n");
               return 0;
              }
              return this->content[ addr ]; 
          }
  
          void write(int addr, int value) { 
              this->content[addr] = value; 
              Serial.println("\n\n\nWRITING ON EEPROM\n");
          }
  };
  
  static EEPROMClass EEPROM = EEPROMClass();
#endif

#endif