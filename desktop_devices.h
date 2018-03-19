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

            // no more .... / done in Basicar.ino now
            // "Retour Chariot" + 115200bps
            //Serial.begin(115200);
            //while( !Serial ) { delay(200); } // beware with that !!!

            this->lastTime = millis();
          }
          
          int available() { 
            return Serial.available(); 
          }
  
          int read() { 
              int tmpCh = Serial.read();
              if ( tmpCh == 10 || tmpCh==13 ) { tmpCh = PS2_ENTER; }
              delay(5);
              return tmpCh;
          }
  
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