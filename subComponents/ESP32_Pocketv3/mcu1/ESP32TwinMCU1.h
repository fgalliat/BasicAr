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
    // if bridged to MCU#2
    // have to reset it
    ESP.restart();
  }

  // called by constructor
  void GenericMCU::init() {
    gpio = new GenericMCU_GPIO();
    buzzer = new GenericMCU_BUZZER();
    fs = new GenericMCU_FS();
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

  
  // ======== Ext GPIO ==================================================================
  #include <Wire.h> // Include the I2C library (required)
  #include <SparkFunSX1509.h> // Include SX1509 library

  #ifdef MAIN_INO_FILE
   SX1509 _ext_GPIO; // Create an SX1509 object
  #else 
   extern SX1509 _ext_GPIO;
  #endif

  #define SX1509_BTN_PIN 0 // 1st Button connected to 0 (active-low)

  void GenericMCU_GPIO::setup(GenericMCU* _mcu) {
    // Call io.begin(<I2C address>) to initialize the I/O
    // expander. It'll return 1 on success, 0 on fail.
    if (!_ext_GPIO.begin(0x3E)) {
      // If we failed to communicate, turn the pin 13 LED on
      _mcu->led(0,true);
      // while (1) { ; } // And loop forever.
      this->ready = false;
      _mcu->println("Ext GPIO not ready !");
      return;
    }

    // BEWARE : MP3 trigger isn't PULLUP !!!
    for(int btn=0; btn < 7; btn++) {
      // + 8 -> cf rewiring
      _ext_GPIO.pinMode(SX1509_BTN_PIN + 8 + btn, INPUT_PULLUP);
    }

    this->ready = true;
    _mcu->println("Ext GPIO ready !");
  }

  // 0-based
  void GenericMCU_GPIO::led(uint8_t ledNum) { ; }
  // 0-based
  bool GenericMCU_GPIO::btn(uint8_t btnNum) {
    if ( !this->ready ) { return false; }
    // + 8 -> cf rewiring
    return _ext_GPIO.digitalRead(SX1509_BTN_PIN + 8 + btnNum) == LOW;
  }

  // ======== Buzzer ====================================================================

  #define BUZZER1 27
  #define BUZ_channel 0
  #define BUZ_resolution 8

  void GenericMCU_BUZZER::setup(GenericMCU* _mcu) {
    int freq = 2000;
    ledcSetup(BUZ_channel, freq, BUZ_resolution);
    ledcAttachPin(BUZZER1, BUZ_channel);

    // sets the volume
    ledcWrite(BUZ_channel, 0);

    this->ready = true;
  }
  
  void GenericMCU_BUZZER::playTone(int freq, int duration) {
    ledcWrite(BUZ_channel, 125); // volume
    ledcWriteTone(BUZ_channel, freq); // freq
  }

  void GenericMCU_BUZZER::noTone() {
    ledcWrite(BUZ_channel, 0); // volume
  }

  // ======== FileSystem ================================================================

  #include <SPIFFS.h>
  #include <FS.h>

  void GenericMCU_FS::setup(GenericMCU* _mcu) {
    SPIFFS.begin();
    this->ready = true;
    _mcu->println("FS ready !");
  }

  // ======== MusicPlayer ===============================================================

  void GenericMCU_MUSIC_PLAYER::setup(GenericMCU* _mcu) { ; }

  // ======== Screen ====================================================================

  void GenericMCU_SCREEN::setup(GenericMCU* _mcu) {
    this->ready = true;
    _mcu->println("Temp. Screen ready !");
  }

  void GenericMCU_SCREEN::print(char* str) { Serial.print(str); }
  void GenericMCU_SCREEN::print(char ch) { Serial.print(ch); }

#endif