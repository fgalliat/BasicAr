#ifndef __ESP32T1_MCU_H__
#define __ESP32T1_MCU_H__ 1
/*
* Twin ESP32 #1 MCU header
* 
* designed for Xts-uBASIC (Xtase-fgalliat @Jan16)
*
* Xtase - fgalliat @ Apr. 2018
*/

  #define BUILTIN_LED 2
  #define BUILTIN_BTN 0

  void GenericMCU::reset() { 
    println("Reset...");
    ESP.restart();
  }

  // called by constructor
  void GenericMCU::init() {
    gpio = new GenericMCU_GPIO();
  }

  // called by setup()
  void GenericMCU::setupInternal() { 
    // BEWARE w/ multiple calls
    Serial.begin(115200);

    // Builtin GPIO
    pinMode( BUILTIN_LED, OUTPUT );
    digitalWrite(BUILTIN_LED, LOW);
    pinMode( BUILTIN_BTN, INPUT_PULLUP );

    println("init...");
    // do what needed ...
  }

  void GenericMCU::setupISR() { 
    // TODO
  }

  void GenericMCU::builtinLED(bool state) { digitalWrite( BUILTIN_LED, state ? HIGH : LOW ); }
  bool GenericMCU::builtinBTN() { return digitalRead( BUILTIN_BTN ) == LOW; }

  
  GenericMCU_BUZZER*       GenericMCU::getBUZZER()       { return NULL; }
  GenericMCU_SCREEN*       GenericMCU::getScreen()       { return NULL; }
  GenericMCU_FS*           GenericMCU::getFS()           { return NULL; }
  GenericMCU_MUSIC_PLAYER* GenericMCU::getMusicPlayer()  { return NULL; }

  // ============================================================================
  #include <Wire.h> // Include the I2C library (required)
  #include <SparkFunSX1509.h> // Include SX1509 library

  #ifdef MAIN_INO_FILE
   SX1509 io; // Create an SX1509 object
  #else 
   extern SX1509 io;
  #endif

  #define SX1509_BTN_PIN 0 // 1st Button connected to 0 (active-low)

  void GenericMCU_GPIO::setup(GenericMCU* _mcu) {
    // Call io.begin(<I2C address>) to initialize the I/O
    // expander. It'll return 1 on success, 0 on fail.
    if (!io.begin(0x3E)) {
      // If we failed to communicate, turn the pin 13 LED on
      _mcu->led(0,true); // need to be static ???
      while (1) { ; } // And loop forever.
    }

    // BEWARE : MP3 trigger isn't PULLUP !!!
    for(int btn=0; btn < 7; btn++) {
      // + 8 -> cf rewiring
      io.pinMode(SX1509_BTN_PIN + 8 + btn, INPUT_PULLUP);
    }
  }

  // 0-based
  void GenericMCU_GPIO::led(uint8_t ledNum) { ; }
  // 0-based
  bool GenericMCU_GPIO::btn(uint8_t btnNum) {
    // + 8 -> cf rewiring
    return io.digitalRead(SX1509_BTN_PIN + 8 + btnNum) == LOW;
  }

  // ============================================================================

  void GenericMCU_SCREEN::print(char* str) { Serial.print(str); }
  void GenericMCU_SCREEN::print(char ch) { Serial.print(ch); }

  #ifndef MAIN_INO_FILE
    extern GenericMCU mcu;
  #endif

#endif