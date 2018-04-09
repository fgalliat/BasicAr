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
  
  // TFT_eSPI Lib is pretty faster than Adafruit one
  #include <TFT_eSPI.h> // Hardware-specific library

  /* -- impl. sample --
    uint16_t TFT_eSPI::color565(uint8_t r, uint8_t g, uint8_t b) {
      return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
  */

    // #define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
    // #define ILI9341_DARKGREY 0x7BEF /* 128, 128, 128 */ !!!! NOT SAME VALUE !!!!

  #define ROTATE_SCREEN 0

    #define CLR_BLACK       ILI9341_BLACK
    #define CLR_WHITE       ILI9341_WHITE
    //#define CLR_LIGHTGREY   TFT_LIGHTGREY // ILI9341_BLACK, ILI9341_RED, ILI9341_CYAN
    #define CLR_LIGHTGREY   ILI9341_RED
    #define CLR_GREY        0x8410 /* 128,128,128 */
    //#define CLR_DARKGREY    TFT_DARKGREY
    #define CLR_DARKGREY    ILI9341_CYAN
    #define CLR_PINK        ILI9341_PINK

    #define BLACK 0x00
    #define WHITE 0x01

    // -- SPI SCreen - values to copy in TFT_eSPI/User_Select.h
    #define TFT_MISO 19
    #define TFT_MOSI 23
    #define TFT_CLK  18
    #define TFT_CS    5  // Chip select control pin
    #define TFT_DC   15  // Data Command control pin
    // connected to EN pin
    #define TFT_RST   -1

  // beware w/ that !!!
  static TFT_eSPI* _oled_display;
  static int _oled_ttyY = 0;
  static uint16_t drawColor = CLR_WHITE;

  static uint16_t screenOffsetX = 0; // MY_320
  static uint16_t screenOffsetY = 0;
  static uint16_t __screenWidth = 0; // MY_320
  static uint16_t __screenHeight = 0;

  // TODO
  static uint8_t __screenMode = -1;
  static uint8_t __screenBlittMode = -1;

  void GenericMCU_SCREEN::setup() {
    screenOffsetX = (320 - 128) / 2;
    screenOffsetY = (240 - 64) / 2;

    _oled_display = new TFT_eSPI();
    _oled_display->init();

    _oled_display->setRotation(1+ROTATE_SCREEN); // LANDSCAPE

    _oled_display->fillScreen(CLR_BLACK);

    _oled_display->setTextColor(drawColor);
    _oled_display->setTextSize(0); // if doesn't work --> try (1)

    setMode( SCREEN_MODE_320 );

    clear();
    blitt( SCREEN_BLITT_AUTO );

    this->ready = true;
  }

  // ------------------------------
  static void __doBlitt() {
    // SEE LATER when a screenBuffer strategy will be defined

    // ESP8266 specific impl. of yield() (Cf Wifi ops & deadlocks ....) (see AdaGFX samples)
    yield();
  }

  static void __blittIfNeeded() {
    if ( __screenBlittMode == SCREEN_BLITT_AUTO ) {
      __doBlitt();
    }
  }

  static void __drawString(int x, int y, char* str) {
      _oled_display->setCursor(x, y);
      _oled_display->print(str);

      #ifdef DBUG_ESP32
        Serial.print( str );
      #endif
  }
  // ------------------------------

  void GenericMCU_SCREEN::print(char* str) { 
    if ( !this->ready ) { Serial.print(str); return; }
    _oled_display->print(str);
    __blittIfNeeded();

    // ?? do I need to manage scrolling TTY ??
  }

  void GenericMCU_SCREEN::print(char ch) {
    if ( !this->ready ) { Serial.print(ch); return; }
    _oled_display->print(ch);
    __blittIfNeeded();

    // ?? do I need to manage scrolling TTY ??
  }

  void GenericMCU_SCREEN::print(int   val) {
    if ( !this->ready ) { Serial.print(val); return; }
    _oled_display->print(val);
    __blittIfNeeded();

    // ?? do I need to manage scrolling TTY ??
  }

  void GenericMCU_SCREEN::print(float val) {
    if ( !this->ready ) { Serial.print(val); return; }
    _oled_display->print(val);
    __blittIfNeeded();

    // ?? do I need to manage scrolling TTY ??
  }

  // cursor in nb of chars (so max is 256x256)
  void GenericMCU_SCREEN::setCursor(int x, int y) {
    if ( !this->ready ) { return; }
    #define FONT_WIDTH (6+1)
    #define FONT_HEIGHT (8)
    _oled_display->setCursor(x*FONT_WIDTH, y*FONT_HEIGHT);

    // !! TO remember if use ACTION_BUFFER !!
  }

  void GenericMCU_SCREEN::setColor(uint16_t color) {
    if ( !this->ready ) { return; }
    
    drawColor = color;

    // !! TO remember if use ACTION_BUFFER !!
  }

  void GenericMCU_SCREEN::setMode(uint8_t mode) {
    if ( !this->ready ) { return; }
    
    if ( mode == SCREEN_MODE_128 ) {
      __screenWidth = 128;
      __screenHeight = 64;

      screenOffsetX = (320 - __screenWidth) / 2;
      screenOffsetY = (240 - __screenHeight) / 2;
      __screenMode = mode;
    } else if ( mode == SCREEN_MODE_160 ) {
      __screenWidth = 160;
      __screenHeight = 128;

      screenOffsetX = (320 - __screenWidth) / 2;
      screenOffsetY = (240 - __screenHeight) / 2;
      __screenMode = mode;
    } else if ( mode == SCREEN_MODE_320 ) {
      __screenWidth = 320;
      __screenHeight = 240;

      screenOffsetX = 0;
      screenOffsetY = 0;
      __screenMode = mode;
    } 
  }

  void GenericMCU_SCREEN::blitt(uint8_t mode) {
    if ( !this->ready ) { return; }
    __screenBlittMode = mode;
    if ( mode >= SCREEN_BLITT_REQUEST ) {
      __doBlitt();
    }
  }

  uint8_t getBlittMode() {
    return __screenBlittMode;
  }

  // === Shapes routines ===
  // 1 is always white
  // 0 is always black

  static uint16_t __getColor(uint16_t color) {
    uint16_t usedColor = color;
    if ( usedColor == BLACK ) { usedColor = CLR_BLACK; }
    else if ( usedColor == WHITE ) { usedColor = CLR_WHITE; }
    else if ( usedColor == 2 ) { usedColor = CLR_LIGHTGREY; }
    else if ( usedColor == 3 ) { usedColor = CLR_GREY; }
    else if ( usedColor == 4 ) { usedColor = CLR_DARKGREY; }
    else if ( usedColor == 5 ) { usedColor = CLR_PINK; }
    // else direct color value
    // ..... use palette impl. !!!
    return usedColor;
  }

  int GenericMCU_SCREEN::getWidth() {
    return __screenWidth;
  }
  int GenericMCU_SCREEN::getHeight() {
    return __screenHeight;
  }

  void GenericMCU_SCREEN::clear() {
    if ( !this->ready ) { return; }

    _oled_display->setCursor( screenOffsetX, screenOffsetY );

    if ( __screenMode == SCREEN_MODE_128 ) {
      _oled_display->fillRect( screenOffsetX, screenOffsetY, 128, 64, CLR_BLACK );
    } else if ( __screenMode == SCREEN_MODE_160 ) {
      _oled_display->fillRect( screenOffsetX, screenOffsetY, 160, 128, CLR_BLACK );
    } else if ( __screenMode == SCREEN_MODE_320 ) {
      _oled_display->fillScreen( CLR_BLACK );
    } 

    __blittIfNeeded();
  }

  // mode = 1 : fill // mode = 0 : draw
  void GenericMCU_SCREEN::drawRect(int x, int y, int w, int h, uint8_t mode, uint16_t color) {
    if ( !this->ready ) { return; }

    uint16_t usedColor = __getColor(color);
    if ( mode == 0 ) {
      _oled_display->drawRect(screenOffsetX+x,screenOffsetY+y,w,h, usedColor);
    } else {
      _oled_display->fillRect(screenOffsetX+x,screenOffsetY+y,w,h, usedColor);
    }

    __blittIfNeeded();
  }

  void GenericMCU_SCREEN::drawCircle(int x, int y, int radius, uint8_t mode, uint16_t color) {
    if ( !this->ready ) { return; }

    uint16_t usedColor = __getColor(color);
    if ( mode == 0 ) {
      _oled_display->drawCircle(x,y,radius, usedColor);
    } else {
      _oled_display->fillCircle(x,y,radius, usedColor);
    }

    __blittIfNeeded();
  }

  void GenericMCU_SCREEN::drawLine(int x, int y, int x2, int y2, uint16_t color){
    if ( !this->ready ) { return; }

    //_oled_display->drawLine(x1,y1,x2,y2, drawColor);
    uint16_t usedColor = __getColor(color);
    _oled_display->drawLine(x,y,x2,y2, usedColor);

    __blittIfNeeded();
  }

  void GenericMCU_SCREEN::drawPixel(int x, int y, uint16_t color){
    if ( !this->ready ) { return; }

    uint16_t usedColor = __getColor(color);
    _oled_display->drawPixel(x,y, usedColor);

    __blittIfNeeded();
  }

  // // see if keep that
  // void GenericMCU_SCREEN::drawTriangle(int x, int y, int x2, int y2, int x3, int y3, uint8_t mode=0, uint16_t color=1);
  // // see if keep that
  // void GenericMCU_SCREEN::drawPolyline(int* x, int* y, int nbPoints);

  // === Picture routines ===
  // void setPalette( uint16_t* pal, int length, int bpp=4 );

  // // for coords : keeps int(s) instead of uint16_t(s) because coords could be negative during transforms

  // // see if need to contains header : Cf fileWidth Vs drawWidth
  // void drawPicture565( uint16_t* raster, int x, int y, int w=-1, int h=-1 );
  // void drawPictureIndexed( uint8_t* raster, int x, int y, int w=-1, int h=-1, bool includesPalette=false );
  // void drawPictureBPP( uint8_t* raster, int x, int y, int w=-1, int h=-1 );

#endif