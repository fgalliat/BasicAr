/***********
 Xtase - fgalliat @Sep 2017
***********/

#ifndef __desk_dev_h_ 
#define __desk_dev_h_ 1

  // =======================================================

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
            // BEWARE W/ FULL DUPLEX -> use events
            return Serial.available(); 
          }
  
          int read() { 
              int tmpCh = Serial.read();
              if ( tmpCh == 10 || tmpCh==13 ) { tmpCh = PS2_ENTER; }
              return tmpCh;
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
  
//   class EEPROMClass {
//       private:
//           int content[1024];
  
//       public:
//           int read(int addr) { 
//               if ( addr >= 1024 ) {
//                printAt(1, 1, "\n\n\nERROR EEPROM ADDR");
//                return 0;
//               }
//               return this->content[ addr ]; 
//           }
  
//           void write(int addr, int value) { 
//               this->content[addr] = value; 
//               printAt(0, 5, "\n\n\nWRITING ON EEPROM");
//           }
//   };
  
//   static EEPROMClass EEPROM = EEPROMClass();


#endif