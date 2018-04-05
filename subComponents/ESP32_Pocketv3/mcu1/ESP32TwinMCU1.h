#ifndef __ESP32T1_MCU_H__
#define __ESP32T1_MCU_H__ 1
/*
* Twin ESP32 #1 MCU header
* 
* designed for Xts-uBASIC (Xtase-fgalliat @Jan16)
*
* Xtase - fgalliat @ Apr. 2018
*/

  #include "ESP32TwinMCUSignals.h"

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
    gpio = new GenericMCU_GPIO(this);
    buzzer = new GenericMCU_BUZZER(this);
    fs = new GenericMCU_FS(this);

    musicPlayer = new GenericMCU_MUSIC_PLAYER(this);
  }

  void setupAdditionalUARTs();

  // called by setup()
  void GenericMCU::setupInternal() { 
    // BEWARE w/ multiple calls
    Serial.begin(115200);
    println("init");

    // Builtin GPIO
    pinMode( BUILTIN_LED, OUTPUT );
    digitalWrite(BUILTIN_LED, LOW);
    pinMode( BUILTIN_BTN, INPUT_PULLUP );

    setupAdditionalUARTs();

    println("init done...");
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

  void GenericMCU_GPIO::setup() {
    // Call io.begin(<I2C address>) to initialize the I/O
    // expander. It'll return 1 on success, 0 on fail.
    if (!_ext_GPIO.begin(0x3E)) {
      // If we failed to communicate, turn the pin 13 LED on
      mcu->led(0,true);
      // while (1) { ; } // And loop forever.
      this->ready = false;
      mcu->println("Ext GPIO not ready !");
      return;
    }

    // BEWARE : MP3 trigger isn't PULLUP !!!
    for(int btn=0; btn < 7; btn++) {
      // + 8 -> cf rewiring
      _ext_GPIO.pinMode(SX1509_BTN_PIN + 8 + btn, INPUT_PULLUP);
    }

    this->ready = true;
    mcu->println("Ext GPIO ready !");
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

  void GenericMCU_BUZZER::setup() {
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

  void GenericMCU_FS::setup() {
    SPIFFS.begin();
    this->ready = true;
    mcu->println("FS ready !");
  }

  // ======== MusicPlayer & Screen bridge UART ==========================================

  #include "HardwareSerial.h"
  #define UART2_NUM 1
  #define UART2_RX 16
  #define UART2_TX 4 // 17 does not seems to emit (TX2 labeled pin)

  #define UART3_NUM 2
  #define UART3_RX 26
  #define UART3_TX 14

  #ifdef MAIN_INO_FILE
   // // the DFPlayerMini UART / HT USBHOST-HID-Keyboard
   // HardwareSerial mp3Serial(UART2_NUM);

   // the "next" MCU UART
   HardwareSerial mcuBridge(UART3_NUM);
  #else 
   extern HardwareSerial mcuBridge;
  #endif

  void setupAdditionalUARTs() {
    // mp3Serial.begin(9600, SERIAL_8N1, UART2_RX, UART2_TX, false);
    mcuBridge.begin(115200, SERIAL_8N1, UART3_RX, UART3_TX, false);
  }

  // ======== MusicPlayer ===============================================================
  // uses Bridge
  void GenericMCU_MUSIC_PLAYER::setup() {
    // HAVE TO TEST if bridge synced ...
    this->ready = true;
  }

  void GenericMCU_MUSIC_PLAYER::playTrack(int trckNum) { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }

    uint8_t d0 = trckNum / 256; // up to 64K files
    uint8_t d1 = trckNum % 256;

    mcuBridge.print( SIG_MP3_PLAY );
    mcuBridge.print( d0 );
    mcuBridge.print( d1 );
    mcuBridge.println();
  }

  void GenericMCU_MUSIC_PLAYER::pause() { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    mcuBridge.print( SIG_MP3_PAUSE );
    mcuBridge.println();
  }

  void GenericMCU_MUSIC_PLAYER::next() { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    mcuBridge.print( SIG_MP3_NEXT );
    mcuBridge.println();
  }

  void GenericMCU_MUSIC_PLAYER::prev() { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    mcuBridge.print( SIG_MP3_PREV );
    mcuBridge.println();
  }

  void GenericMCU_MUSIC_PLAYER::setVolume(uint8_t volume) { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    mcuBridge.print( SIG_MP3_VOL );
    mcuBridge.print( volume );
    mcuBridge.println();
  }

  bool GenericMCU_MUSIC_PLAYER::isPlaying() {
    if ( !this->ready ) { return false; }
    // TODO : better -> leads to GPIO pin #0
    return mcu->getGPIO()->btn( -8 );
  }

  // ======== Screen ====================================================================
  // uses Bridge
  void GenericMCU_SCREEN::setup() {
    this->ready = true;
    mcu->println("Temp. Screen ready !");
  }

  void GenericMCU_SCREEN::print(char* str) { Serial.print(str); }
  void GenericMCU_SCREEN::print(char ch) { Serial.print(ch); }

#endif