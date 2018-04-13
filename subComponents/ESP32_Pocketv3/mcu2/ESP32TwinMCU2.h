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
      //mcuBridge.flush();
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

  void GenericMCU_FS::uploadViaSerial() {
    mcu->println("NYI !");
  }

  static int _readBridgeLine(char* dest, int destSize, int timeout=3000) {
    int t0 = millis();
    while( mcuBridge.available() == 0 ) { 
      yield(); delay(50); 
      if ( millis() - t0 > timeout ) {
        return -1;
      }
    }
    int cpt = 0;
    while(true) {
      int ch = mcuBridge.read();
      // Serial.print(cpt);Serial.print(" ");Serial.print((char)ch);Serial.print(" ");Serial.print(ch);Serial.println("");

      if ( ch == -1 ) { return cpt; }
      if ( ch == '\n' ) { return cpt; }
      dest[cpt++] = (char)ch;

      if ( cpt >= destSize ) { break; }

      while( mcuBridge.available() == 0 ) { 
        yield(); delay(50); 
        if ( millis() - t0 > 3000 ) {
          return cpt;
        }
      }
    }
    return cpt;
  }

  void GenericMCU_FS::uploadViaBridge() {
    // mcu->getScreen()->lock();

    // dirties the UART input
    //mcuBridge.flush();

    mcuBridge.write(0xFF); //mcuBridge.flush();

    int t0 = millis();

    char entryName[1+8+1+3+1];
    int readed = _readBridgeLine(entryName, 1+8+1+3+0);
    if ( readed < 0 ) { mcu->println("name timeout !"); return; }
    if ( readed == 0 ) { mcu->println("name empty !"); return; }
    for (int i=readed; i < 1+8+1+3+1; i++) { entryName[i]=0x00; }
    mcu->println("writing ");mcu->println(entryName);

    char entrySizeS[11+1];
    readed = _readBridgeLine(entrySizeS, 11+0);
    if ( readed < 0 ) { mcu->println("size timeout !"); return; }
    if ( readed == 0 ) { mcu->println("size empty !"); return; }
    for (int i=readed; i < 11+1; i++) { entrySizeS[i]=0x00; }
    // mcu->println("long ");mcu->println(entrySizeS);
    // for(int i=0; i < readed; i++) { Serial.print( (int)entrySizeS[i] ); Serial.print(", "); }
    // Serial.println("");

    int entrySize = atoi( entrySizeS );
    mcu->println("long ");Serial.println(entrySize);

    #define BRIDGE_UPL_PACKET_SIZE 32
    char content[BRIDGE_UPL_PACKET_SIZE];

    File f = SPIFFS.open( entryName, "w" );
    if ( !f ) {
      mcu->println("Failed to open file !");
      return;
    }

    int total = 0;
    readed = 0;

    while(true) {
      t0 = millis();
      while( mcuBridge.available() <= 0 ) { 
        yield(); delay(10); 
        if ( millis() - t0 > 3000 ) {
          Serial.print( total );
          mcu->println("timeout !");
          f.flush(); f.close();
          return;
        }
      }
      readed = mcuBridge.readBytes(content, BRIDGE_UPL_PACKET_SIZE);
      if ( readed <= 0 ) {
          mcu->println("Oups !");
          f.flush(); f.close();
          return;
      }
      f.write( (const uint8_t*)content, readed);
      //mcu->print('.'); Serial.flush();
      mcuBridge.write(0xFE); // mcuBridge.flush();
      total += readed;
      if ( total >= entrySize ) { break; }

      delay(2); yield();
    }
    f.flush(); f.close();
    mcuBridge.write(0xFF); //mcuBridge.flush();
    mcu->println("DONE !");
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
    #define CLR_LIGHTGREY   0xC618 /* 192x3 */
    #define CLR_GREY        0x8410 /* 128,128,128 */
    #define CLR_DARKGREY    0x7BEF /* ? 128x3 ? */
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

  void GenericMCU_SCREEN::lock() {
    this->ready = false;
  }

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
      _oled_display->drawCircle(screenOffsetX+x,screenOffsetY+y,radius, usedColor);
    } else {
      _oled_display->fillCircle(screenOffsetX+x,screenOffsetY+y,radius, usedColor);
    }

    __blittIfNeeded();
  }

  void GenericMCU_SCREEN::drawLine(int x, int y, int x2, int y2, uint16_t color){
    if ( !this->ready ) { return; }

    uint16_t usedColor = __getColor(color);
    _oled_display->drawLine(screenOffsetX+x,screenOffsetY+y,screenOffsetX+x2,screenOffsetY+y2, usedColor);

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
  void GenericMCU_SCREEN::drawPicture565( uint16_t* raster, int x, int y, int w, int h ) {
    if ( !ready ) { return; }
    __blittIfNeeded();
  }

  //#define MEM_RAST_HEIGHT  16
  #define MEM_RAST_HEIGHT  128
  #define MEM_RAST_WIDTH   160
  #define MEM_RAST_LEN_u16 MEM_RAST_WIDTH * MEM_RAST_HEIGHT
  #define MEM_RAST_LEN_u8  MEM_RAST_LEN_u16 * 2

  #define PCT_HEADER_LEN 7

  #ifdef MAIN_INO_FILE
    uint16_t color_picturebuff[ MEM_RAST_LEN_u16 ];
  #else
    extern uint16_t color_picturebuff[];
  #endif

  // _w & _h to clip a zone...
  void GenericMCU_SCREEN::drawPicture565( char* filename, int x, int y, int _w, int _h ) {
    static int w=-1, h=-1; // img size
    static char header[PCT_HEADER_LEN];


    if ( !ready ) { return; }
    uint8_t prevBlittMode = __screenBlittMode;
    __screenBlittMode = SCREEN_BLITT_LOCKED;

    if ( filename != NULL ) {
      File f = SPIFFS.open(filename, "r");
      if ( !f ) { mcu->println("File not ready"); return; }
      int readed = f.readBytes( (char*)header, PCT_HEADER_LEN);
      
      if ( header[0] == '6' && header[1] == '4' && header[2] == 'K' ) {
          w = ((int)header[3]*256) + ((int)header[4]);
          h = ((int)header[5]*256) + ((int)header[6]);
      } else {
          mcu->println( "Wrong PCT header" );
          mcu->println( header );
      }
      // Serial.print("A.2 "); Serial.print(w); Serial.print('x');Serial.print(h);Serial.println("");
      if( w <= 0 || h <= 0 ) {
        f.close();
        return;
      }

      int scanZoneSize = w*MEM_RAST_HEIGHT*2; // *2 -> u16
      int startX = screenOffsetX+x;
      int startY = screenOffsetY+y;

      int yy = 0;
      while( true ) { 
        // BEWARE : @ this time : h need to be 128
        readed = f.readBytes( (char*)color_picturebuff, scanZoneSize);
        //Serial.print("A.2 bis"); Serial.print(readed); Serial.print(" of ");Serial.print(w*h*2);Serial.println("");

        //_oled_display->pushRect(screenOffsetX+x, screenOffsetY+y+yy, w, MEM_RAST_HEIGHT, color_picturebuff);
        _oled_display->pushImage(startX, startY+yy, w, MEM_RAST_HEIGHT, color_picturebuff);
        yy += MEM_RAST_HEIGHT;
        if ( yy + (MEM_RAST_HEIGHT) > h ) { break; }
      }
      f.close();


    } else {
      // recall last MEM_RAST area
      int scanZoneSize = w*MEM_RAST_HEIGHT*2; // *2 -> u16
      int startX = screenOffsetX+x;
      int startY = screenOffsetY+y;
      _oled_display->pushImage(startX, startY, w, MEM_RAST_HEIGHT, color_picturebuff);
    }


    // Set TFT address window to clipped image bounds
    //_oled_display->startWrite(); // Requires start/end transaction now
    // _oled_display->setAddrWindow(screenOffsetX+x, screenOffsetY+y, w, h);

    
    //_oled_display->endWrite();

 
    __screenBlittMode = prevBlittMode;
    __blittIfNeeded();
  }

  #ifdef MAIN_INO_FILE
    uint8_t bpp_picturebuff[ 1024 ];
  #else
    extern uint8_t bpp_picturebuff[];
  #endif

  void GenericMCU_SCREEN::drawPictureBPP( char* filename, int x, int y ) {
    if ( !ready ) { return; }

    if ( filename != NULL ) {
      File f = SPIFFS.open(filename, "r");
      if ( !f ) { mcu->println("File not ready"); return; }

      int w = 128;
      int h = 64;

      if( w <= 0 || h <= 0 ) {
        f.close();
        return;
      }

      int readed = f.readBytes( (char*)bpp_picturebuff, 1024);
      this->drawPictureBPP(bpp_picturebuff, x, y);
      f.close();

    } else {
      // recall last MEM_RAST area
      this->drawPictureBPP(bpp_picturebuff, x, y);
    }

    __blittIfNeeded();
  }

  void GenericMCU_SCREEN::drawPictureBPP( uint8_t* raster, int x, int y ) {
    if ( !ready ) { return; }

    int sx = screenOffsetX + x;
    int sy = screenOffsetY + y;

    int width = 128;
    int height = 64;

    unsigned char c;

    // TODO : lock blitt


    // TODO : impl. pixel shaders
    
    // this->clear();
    this->drawRect(sx, sy, 128, 64, 1, 0);

    for (int yy = 0; yy < height; yy++) {
      for (int xx = 0; xx < width; xx++) {
        c = (raster[(yy * (width / 8)) + (xx / 8)] >> (7 - ((xx) % 8))) % 2;
        if (c == 0x00) {
        }
        else {
            // TODO : impl. pixel shaders
            _oled_display->drawPixel(sx + xx, sy + yy, CLR_WHITE);
        }
      }
    }    

    // NEED to blitt ??????
  }

  // void drawPictureIndexed( uint8_t* raster, int x, int y, int w=-1, int h=-1, bool includesPalette=false );

void GenericMCU_SCREEN::drawPicture565Sprite( uint16_t* raster, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh ) {
  int startOffset = ( sy * dw ) + sx;
  int offset = startOffset;

  dx += screenOffsetX;
  dy += screenOffsetY;

  // BEWARE : mem overflow !!!!!
  // 32x64 seems to be the max @ this time
  uint16_t subImage[ dw*dh ];

  for(int yy=0; yy < dh; yy++) {
    // TODO : find faster way !
    for(int xx=0; xx < dw; xx++) {
      subImage[(yy*dw)+xx] = color_picturebuff[offset+xx];
    }
    offset = ( (sy+yy) * sw ) + sx;
  }
  _oled_display->pushImage(dx, dy, dw, dh, subImage);

}

void GenericMCU_SCREEN::drawPicture565Sprite( char* filename, int dx, int dy, int dw, int dh, int sx, int sy ) {
  static int pctW=-1, pctH=-1;
  static char header[PCT_HEADER_LEN];

  if ( sx < 0 ) { sx = 0; }
  if ( sy < 0 ) { sy = 0; }

  if ( filename != NULL ) {
      File f = SPIFFS.open(filename, "r");

      if ( !f ) { mcu->println("File not ready"); return; }
      int readed = f.readBytes( (char*)header, PCT_HEADER_LEN);
      
      if ( header[0] == '6' && header[1] == '4' && header[2] == 'K' ) {
          pctW = ((int)header[3]*256) + ((int)header[4]);
          pctH = ((int)header[5]*256) + ((int)header[6]);
      } else {
          mcu->println( "Wrong PCT header" );
          mcu->println( header );

          f.close();
          return;
      }

      // need to store the whole sprite in mem.
      // in order that # subSprites to be recalled

      int toRead = (pctW * pctH)*2; // x2 Cf U16

      readed = f.readBytes( (char*)color_picturebuff, toRead);
      f.close();

      if ( dw < 0 ) { dw = pctW; }
      if ( dh < 0 ) { dh = pctH; }
      this->drawPicture565Sprite(color_picturebuff, dx, dy, dw, dh, sx, sy, pctW, pctH);

  } else {
      if ( dw < 0 ) { dw = pctW; }
      if ( dh < 0 ) { dh = pctH; }
      this->drawPicture565Sprite(color_picturebuff, dx, dy, dw, dh, sx, sy, pctW, pctH);
  }
  
}


#endif