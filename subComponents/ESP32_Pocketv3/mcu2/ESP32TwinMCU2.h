#ifndef __ESP32T1_MCU_H__
#define __ESP32T1_MCU_H__ 1
/*
* Twin ESP32 #2 MCU header
* 
* designed for Xts-uBASIC (Xtase-fgalliat @Jan16)
*
* Xtase - fgalliat @ Apr. 2018
*/

  #include "ESP32TwinMCUSignals.h"

  // BUILTIN GPIO
  #define BUILTIN_LED 2
  #define BUILTIN_BTN 0


  // ======== MusicPlayer & Screen bridge UART ==========================================

  #include "HardwareSerial.h"
  #define UART2_NUM 1
  #define UART2_RX 16
  #define UART2_TX 4 // 17 does not seems to emit (TX2 labeled pin)

  #define UART3_NUM 2
  #define UART3_RX 26
  #define UART3_TX 14

  #ifdef MAIN_INO_FILE
   // the DFPlayerMini UART
   HardwareSerial mp3Serial(UART2_NUM);

   // the "next" MCU UART
   HardwareSerial mcuBridge(UART3_NUM);
  #else 
   extern HardwareSerial mp3Serial;
   extern HardwareSerial mcuBridge;
  #endif

  void setupAdditionalUARTs() {
    mp3Serial.begin(9600, SERIAL_8N1, UART2_RX, UART2_TX, false);
    mcuBridge.begin(115200, SERIAL_8N1, UART3_RX, UART3_TX, false);
  }

  // ===========================================================
  static bool __mcuBridgeReady = false;

  void GenericMCU::reset() { 
    println("Reset...");
    // if bridged to MCU#2
    // have to reset it

    // mcuBridge.write( SIG_MCU_RESET );
    // mcuBridge.flush();
    __mcuBridgeReady = false;

    ESP.restart();
  }

  // called by constructor
  void GenericMCU::init() {
    gpio = NULL;
    buzzer = NULL;
    fs = new GenericMCU_FS(this);

    musicPlayer = new GenericMCU_MUSIC_PLAYER(this);
    screen = new GenericMCU_SCREEN(this);
  }

  // called by setup()
  void GenericMCU::setupPreInternal() { 
    // BEWARE w/ multiple calls
    Serial.begin(115200);

    println("init");

    // Builtin GPIO
    pinMode( BUILTIN_LED, OUTPUT );
    digitalWrite(BUILTIN_LED, LOW);
    pinMode( BUILTIN_BTN, INPUT_PULLUP );

    setupAdditionalUARTs();

    println("init done...");

    // ===== Bridge Sync. Routine ======================

    __mcuBridgeReady = false;
    println("sync");

    int t0 = millis();
    int timeout = 3 * 1000;

    #ifdef MCU_MASTER
      const int signalToRead = SIG_MCU_SLAVE_SYNC;
      const int signalToSend = SIG_MCU_MASTER_SYNC;
    #else
      const int signalToRead = SIG_MCU_MASTER_SYNC;
      const int signalToSend = SIG_MCU_SLAVE_SYNC;
    #endif

    while(true) {
      led(0, true);
      //println(" Waiting a 1st byte");
      //Serial.println( mcuBridge.available() );

      if ( mcuBridge.available() > 0 ) {
        println(" Found a 1st byte");

        if ( mcuBridge.read() == 0xFF ) {
          println(" Found the 1st byte");

          while ( mcuBridge.available() <= 0 ) { 
            if ( millis() - t0 >= timeout ) { break; }
            delay(10);
          }
          if ( millis() - t0 >= timeout ) { break; }
          println(" Found a 2nd byte");
          if ( mcuBridge.read() == signalToRead ) {
            println(" Found the 2nd byte");
            __mcuBridgeReady = true;
          }
        }
      }
      if ( millis() - t0 >= timeout ) { break; }
      delay(100);
      println(" Send sync seq.");
      mcuBridge.write( 0xFF );
      mcuBridge.write( signalToSend );
      mcuBridge.flush();
      led(0, false);
      delay(100);
    }
    println(" Exited");
    led(0, false);

    if ( __mcuBridgeReady ) {
      println("sync done...");
    } else {
      println("sync failed...");
    }
  }

  // called by setup()
  void GenericMCU::setupPostInternal() { 
  }

  void GenericMCU::setupISR() { 
    // TODO
  }

  void GenericMCU::builtinLED(bool state) { digitalWrite( BUILTIN_LED, state ? HIGH : LOW ); }
  bool GenericMCU::builtinBTN() { return digitalRead( BUILTIN_BTN ) == LOW; }

  
  // ======== Ext GPIO ==================================================================
  void GenericMCU_GPIO::setup() {
  }

  // 0-based
  void GenericMCU_GPIO::led(uint8_t ledNum, bool state) { ; }
  // 0-based
  bool GenericMCU_GPIO::btn(uint8_t btnNum) {
    return false;
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

  // ======== MusicPlayer ===============================================================
  // uses Bridge

  // ==== [ TODO SECTION ] ====

  void GenericMCU_MUSIC_PLAYER::setup() {
    // if ( ! __mcuBridgeReady ) { 
    //   mcu->println("Bridged Music Player NOT ready !");
    //   this->ready = false; return; 
    // }
    // this->ready = true;
    this->ready = false;
  }

  void GenericMCU_MUSIC_PLAYER::playTrack(int trckNum) { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }

    // uint8_t d0 = trckNum / 256; // up to 64K files
    // uint8_t d1 = trckNum % 256;

    // mcuBridge.write( SIG_MP3_PLAY );
    // mcuBridge.write( d0 );
    // mcuBridge.write( d1 );
    // mcuBridge.flush();
  }

  void GenericMCU_MUSIC_PLAYER::pause() { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    // mcuBridge.write( SIG_MP3_PAUSE );
    // mcuBridge.flush();
  }

  void GenericMCU_MUSIC_PLAYER::next() { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    // mcuBridge.write( SIG_MP3_NEXT );
    // mcuBridge.flush();
  }

  void GenericMCU_MUSIC_PLAYER::prev() { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    // mcuBridge.write( SIG_MP3_PREV );
    // mcuBridge.flush();
  }

  void GenericMCU_MUSIC_PLAYER::setVolume(uint8_t volume) { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    // mcuBridge.write( SIG_MP3_VOL );
    // mcuBridge.write( volume );
    // mcuBridge.flush();
  }

  bool GenericMCU_MUSIC_PLAYER::isPlaying() {
    if ( !this->ready ) { return false; }
    // TODO : better -> leads to GPIO pin #0
    // return mcu->getGPIO()->btn( -8 );
  }

  // ======== Screen ====================================================================
  // uses Bridge
  void GenericMCU_SCREEN::setup() {
    // if ( ! __mcuBridgeReady ) { 
    //   mcu->println("Bridged Screen NOT ready !");
    //   this->ready = false; return; 
    // }
    // this->ready = true;
    // mcu->println("Temp. Screen ready !");
    this->ready = false;
  }

  void GenericMCU_SCREEN::print(char* str) { 
    if ( !this->ready ) { Serial.print(str); return; }
    // mcuBridge.write( SIG_SCR_PRINT_STR );
    // mcuBridge.print( str );
    // mcuBridge.write( 0x00 );
    // mcuBridge.flush(); 
  }

  void GenericMCU_SCREEN::print(char ch) {
    if ( !this->ready ) { Serial.print(ch); return; }
    // mcuBridge.write( SIG_SCR_PRINT_CH );
    // mcuBridge.write( ch );
    // mcuBridge.flush();
  }

  void GenericMCU_SCREEN::print(int   val) {
    if ( !this->ready ) { Serial.print(val); return; }
    // // in my BASIC int(s) are float(s)
    // // from mem_utils.h
    // const int tsize = getSizeOfFloat();
    // unsigned char memSeg[ tsize ];
    // copyFloatToBytes(memSeg, 0, (float)val);

    // mcuBridge.write( SIG_SCR_PRINT_INT );
    // mcuBridge.write( memSeg, tsize );
    // mcuBridge.flush();
  }

  void GenericMCU_SCREEN::print(float val) {
    if ( !this->ready ) { Serial.print(val); return; }
    // // from mem_utils.h
    // const int tsize = getSizeOfFloat();
    // unsigned char memSeg[ tsize ];
    // copyFloatToBytes(memSeg, 0, val);

    // mcuBridge.write( SIG_SCR_PRINT_NUM );
    // mcuBridge.write( memSeg, tsize );
    // mcuBridge.flush();
  }

  // cursor in nbof chars (so max is 256x256)
  void GenericMCU_SCREEN::setCursor(int x, int y) {
    if ( !this->ready ) { return; }
    // mcuBridge.write( SIG_SCR_CURSOR );
    // mcuBridge.write( x );
    // mcuBridge.write( y );
    // mcuBridge.flush();
  }

  void GenericMCU_SCREEN::setColor(uint16_t color) {
    if ( !this->ready ) { return; }
    // uint8_t d0 = color / 256; // up to 64K colors
    // uint8_t d1 = color % 256;

    // mcuBridge.write( SIG_SCR_COLOR );
    // mcuBridge.write( d0 );
    // mcuBridge.write( d1 );
    // mcuBridge.flush();
  }

  void GenericMCU_SCREEN::setMode(uint8_t mode) {
    if ( !this->ready ) { return; }
    // mcuBridge.write( SIG_SCR_MODE );
    // mcuBridge.write( mode );
    // mcuBridge.flush();
  }

  void GenericMCU_SCREEN::blitt(uint8_t mode) {
    if ( !this->ready ) { return; }
    // mcuBridge.write( SIG_SCR_BLITT );
    // mcuBridge.write( mode );
    // mcuBridge.flush();
  }




#endif