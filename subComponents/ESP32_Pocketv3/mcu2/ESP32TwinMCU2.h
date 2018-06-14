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
        t0 = millis();

        if ( mcuBridge.read() == 0xFF ) {
          println(" Found the 1st byte");
          t0 = millis();

          while ( mcuBridge.available() <= 0 ) { 
            if ( millis() - t0 >= timeout ) { break; }
            delay(10);
          }
          if ( millis() - t0 >= timeout ) { break; }
          println(" Found a 2nd byte");
          if ( mcuBridge.read() == signalToRead ) {
            println(" Found the 2nd byte");
            __mcuBridgeReady = true;
            break;
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

  void GenericMCU::setupISR() {}
  void GenericMCU::lockISR() {}
  void GenericMCU::unlockISR() {}

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

  #define BRIDGE_UPL_PACKET_SIZE 32

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

      if ( ch == -1 ) { return cpt; }
      if ( ch == '\n' ) { return cpt; }
      dest[cpt++] = (char)ch;

      if ( cpt > destSize ) { break; }

      while( mcuBridge.available() == 0 ) { 
        yield(); delay(50); 
        if ( millis() - t0 > 3000 ) {
          return cpt;
        }
      }
    }
    return cpt;
  }

  static int _readSerialLine(char* dest, int destSize, int timeout=3000) {
    int t0 = millis();
    while( Serial.available() == 0 ) { 
      yield(); delay(50); 
      if ( millis() - t0 > timeout ) {
        return -1;
      }
    }
    int cpt = 0;
    while(true) {
      int ch = Serial.read();

      if ( ch == -1 ) { return cpt; }
      if ( ch == '\n' ) { return cpt; }
      dest[cpt++] = (char)ch;

      if ( cpt > destSize ) { break; }

      while( Serial.available() == 0 ) { 
        yield(); delay(50); 
        if ( millis() - t0 > 3000 ) {
          return cpt;
        }
      }
    }
    return cpt;
  }

  void GenericMCU_FS::uploadViaSerial() {
    Serial.write(0xFF); //mcuBridge.flush();

    int t0 = millis();

    static char entryName[1+8+1+3+1 +1];
    int readed = _readSerialLine(entryName, 1+8+1+3+0 +1);
    if ( readed < 0 ) { mcu->println("name timeout !"); return; }
    if ( readed == 0 ) { mcu->println("name empty !"); return; }
    for (int i=readed; i < 1+8+1+3+1; i++) { entryName[i]=0x00; }
    mcu->println("writing ");mcu->println(entryName);

    delay(100);

    static char entrySizeS[11+1 +1];
    readed = _readSerialLine(entrySizeS, 11+0 +1);
    if ( readed < 0 ) { mcu->println("size timeout !"); return; }
    if ( readed == 0 ) { mcu->println("size empty !"); return; }
    for (int i=readed; i < 11+1; i++) { entrySizeS[i]=0x00; }

    // TODO : remove
    mcu->println("long (STR) ");mcu->println(entrySizeS);
    for(int i=0; i < readed; i++) { mcu->print( (int)entrySizeS[i] ); mcu->print(", "); }
    mcu->println("");

    int entrySize = atoi( entrySizeS );
    mcu->println("long ");mcu->print(entrySize);mcu->println("");

    static char content[BRIDGE_UPL_PACKET_SIZE];

    File f = SPIFFS.open( entryName, "w" );
    if ( !f ) {
      mcu->println("Failed to open file !");
      return;
    }

    f.seek(0);

    int total = 0;
    readed = 0;

    while(true) {
      t0 = millis();
      while( Serial.available() <= 0 ) { 
        yield(); delay(10); 
        if ( millis() - t0 > 10000 ) {
          mcu->print( total );
          mcu->println(" timeout !");
          f.flush(); f.close();
          return;
        }
      }
      readed = Serial.readBytes(content, BRIDGE_UPL_PACKET_SIZE);

      if ( readed > 0 ) {
        mcu->print( (int)content[0] );
        mcu->print( ' ' );
        mcu->print( (char)content[0] );
        mcu->print( '/' );
      }

      if ( readed <= 0 ) {
          mcu->println("Oups !");
          f.flush(); f.close();
          return;
      }
      f.write( (const uint8_t*)content, readed);
      f.flush();

      Serial.write(0xFE); // mcuBridge.flush();
      total += readed;
      if ( total >= entrySize ) { break; }
      delay(2); yield();
    }
    f.flush(); f.close();
    Serial.write(0xFF); //mcuBridge.flush();
    mcu->println("DONE !");
  }


  void GenericMCU_FS::uploadViaBridge() {
    mcuBridge.write(0xFF); //mcuBridge.flush();

    int t0 = millis();

    static char entryName[1+8+1+3+1 +1];
    int readed = _readBridgeLine(entryName, 1+8+1+3+0 +1);
    if ( readed < 0 ) { mcu->println("name timeout !"); return; }
    if ( readed == 0 ) { mcu->println("name empty !"); return; }
    for (int i=readed; i < 1+8+1+3+1; i++) { entryName[i]=0x00; }
    mcu->println("writing ");mcu->println(entryName);

    delay(100);

    static char entrySizeS[11+1 +1];
    readed = _readBridgeLine(entrySizeS, 11+0 +1);
    if ( readed < 0 ) { mcu->println("size timeout !"); return; }
    if ( readed == 0 ) { mcu->println("size empty !"); return; }
    for (int i=readed; i < 11+1; i++) { entrySizeS[i]=0x00; }

    // TODO : remove
    mcu->println("long (STR) ");mcu->println(entrySizeS);
    for(int i=0; i < readed; i++) { mcu->print( (int)entrySizeS[i] ); mcu->print(", "); }
    mcu->println("");

    int entrySize = atoi( entrySizeS );
    mcu->println("long ");mcu->print(entrySize);mcu->println("");

    static char content[BRIDGE_UPL_PACKET_SIZE];

    File f = SPIFFS.open( entryName, "w" );
    if ( !f ) {
      mcu->println("Failed to open file !");
      return;
    }

    f.seek(0);

    int total = 0;
    readed = 0;

    while(true) {
      t0 = millis();
      while( mcuBridge.available() <= 0 ) { 
        yield(); delay(10); 
        if ( millis() - t0 > 10000 ) {
          mcu->print( total );
          mcu->println(" timeout !");
          f.flush(); f.close();
          return;
        }
      }
      readed = mcuBridge.readBytes(content, BRIDGE_UPL_PACKET_SIZE);

      if ( readed > 0 ) {
        mcu->print( (int)content[0] );
        mcu->print( ' ' );
        mcu->print( (char)content[0] );
        mcu->print( '/' );
      }

      if ( readed <= 0 ) {
          mcu->println("Oups !");
          f.flush(); f.close();
          return;
      }
      f.write( (const uint8_t*)content, readed);
      f.flush();

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


  void GenericMCU_FS::ls(char* filter, void (*callback)(char*, int, uint8_t, int) ) {
    mcu->lockISR();
    File dir = SPIFFS.open("/");
    File entry;
    int entryNb = 0;
    uint8_t type;
    static char entName[13+1];
    while( (entry = dir.openNextFile() ) ) {
      if ( entry.isDirectory() ) { type = FS_TYPE_DIR; }
      else                       { type = FS_TYPE_FILE; }

      memset( entName, 0x00, 13+1 );
      int len = strlen( entry.name() ), cpt=0;
      // skip leading '/'
      // skip trailing strange chars... (if any)
      for(int i=1; i < len; i++) {
        char ch = entry.name()[i]; 
        if ( ch > ' ' && ch < (char)128 ) {
          entName[cpt++] = ch;
        } else {
          break;
        }
      }

      if ( filter != NULL ) {
        int ll = strlen(filter);
        if ( ll > 1 ) {
          // check ONLY endsWithPattern @ this time
          // ex. ".BAS"
          if ( filter[ ll-1 ] != '*' ) {
            if ( cpt >= ll ) {
              bool found = true;
              int e=0;
              for(int i=cpt-ll; i < cpt; i++) {
                if ( entName[i] != filter[e++] ) { found = false; break; }
              }
              // ignore entry
              if ( !found ) { continue; }
            }
          }
        }
      }

      callback( entName, (int)entry.size(), type, entryNb );
      entryNb++;
    }
    callback( "-EOD-", entryNb, FS_TYPE_EOF, entryNb );
    // return entryNb;
    mcu->unlockISR();
  }



  // ======== MusicPlayer ===============================================================
  #include "DFRobotDFPlayerMini.h"

  DFRobotDFPlayerMini myDFPlayer;

  void GenericMCU_MUSIC_PLAYER::setup() {
    // needs up to 3secs
    if (!myDFPlayer.begin(mp3Serial)) {
      mcu->println("Unable to begin:\n1.Please recheck the connection!\n2.Please insert the SD card!");
      this->ready = false;
      // delay(500);
      return;
    }
    mcu->println("DFPlayer Mini online.");
    delay(100);
    myDFPlayer.volume(20);  //Set volume value. From 0 to 30

    this->ready = true;
  }

  // from 1 to 2999
  void GenericMCU_MUSIC_PLAYER::playTrack(int trckNum) { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }

    // positionate only the current file ???
    myDFPlayer.playMp3Folder(trckNum); // plays SD:/MP3/0001.mp3 - @ least 1st song of /MP3/
    delay(100);
    myDFPlayer.play(trckNum); // ??????
  }

  static bool __mp3Paused = false;

  void GenericMCU_MUSIC_PLAYER::pause() { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    if ( !__mp3Paused ) {
      myDFPlayer.pause();
      __mp3Paused = true;
    } else {
      myDFPlayer.start();
      __mp3Paused = false;
    }
  }

  void GenericMCU_MUSIC_PLAYER::next() { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    myDFPlayer.next();
  }

  void GenericMCU_MUSIC_PLAYER::prev() { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    myDFPlayer.previous();
  }

  /* from 0-30 */
  void GenericMCU_MUSIC_PLAYER::setVolume(uint8_t volume) { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    myDFPlayer.volume(volume);
  }

  bool GenericMCU_MUSIC_PLAYER::isPlaying() {
    if ( !this->ready ) { return false; }
    
    // WIRED to #1 MCU !!!
    return false;
  }

  void GenericMCU_MUSIC_PLAYER::stop() {
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    myDFPlayer.stop();
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
    // #define CLR_LIGHTGREY   0xC618 /* 192x3 */
    // #define CLR_GREY        0x8410 /* 128,128,128 */
    // #define CLR_DARKGREY    0x7BEF /* ? 128x3 ? */

    #define CLR_LIGHTGREY   0xCE59 /* 200x3 */
    #define CLR_GREY        0x8410 /* 128x3 */
    #define CLR_DARKGREY    0x3186 /* 50x3 */

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
  static uint16_t txtBgColor = CLR_BLACK;

  static uint16_t screenOffsetX = 0; // MY_320
  static uint16_t screenOffsetY = 0;
  static uint16_t __screenWidth = 0; // MY_320
  static uint16_t __screenHeight = 0;

  // TODO
  static uint8_t __screenMode = -1;
  static uint8_t __screenBlittMode = -1;

  // ==== Color routine ====
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

    // BG is transparent by default
    _oled_display->setTextColor(drawColor, txtBgColor);
    _oled_display->setTextSize(0); // if doesn't work --> try (1)

    setMode( SCREEN_MODE_320 );

    clear();
    blitt( SCREEN_BLITT_AUTO );

    // hosno effect for now
    // _oled_display->setWindow(0,0,160,128);
    _oled_display->setTextWrap(true, true);

    this->ready = true;
  }

  // ------------------------------
  static void __doBlitt() {
#ifdef SCREEEN_BUFFER
    // _b_doBlitt();
#else
    // ESP8266 specific impl. of yield() (Cf Wifi ops & deadlocks ....) (see AdaGFX samples)
    yield();
#endif
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
    _oled_display->setCursor((x*FONT_WIDTH)+screenOffsetX, (y*FONT_HEIGHT)+screenOffsetY );

    // !! TO remember if use ACTION_BUFFER !!
  }

  void GenericMCU_SCREEN::setColor(uint16_t color) {
    if ( !this->ready ) { return; }
    
    drawColor = color;
    // BG is transparent by default
    _oled_display->setTextColor(drawColor, txtBgColor);

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

  // mode : TEXT_OVERWRITE / TEXT_INCRUST
  void GenericMCU_SCREEN::setTextMode(uint8_t mode, uint16_t fg, uint16_t bg) {
    if ( !this->ready ) { return; }

    uint16_t _fg = CLR_WHITE;
    uint16_t _bg = CLR_BLACK;

    _fg = __getColor(fg);
    _bg = __getColor(bg);

    if ( mode == TEXT_OVERWRITE ) {
      _oled_display->setTextColor(_fg,_bg);
    } else if ( mode == TEXT_INCRUST ) {
      _bg = TFT_TRANSPARENT;
      _oled_display->setTextColor(_fg,_bg);
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

  void GenericMCU_SCREEN::drawTriangle(int x, int y, int x2, int y2, int x3, int y3, uint8_t mode, uint16_t color) {
    if ( !this->ready ) { return; }

    uint16_t usedColor = __getColor(color);
    if (mode == 0) {
      _oled_display->drawTriangle(screenOffsetX+x,screenOffsetY+y,screenOffsetX+x2,screenOffsetY+y2, 
                                  screenOffsetX+x3,screenOffsetY+y3, usedColor);
    } else {
      _oled_display->fillTriangle(screenOffsetX+x,screenOffsetY+y,screenOffsetX+x2,screenOffsetY+y2, 
                                  screenOffsetX+x3,screenOffsetY+y3, usedColor);
    }

    __blittIfNeeded();
  }

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
      f.seek(0);
      int readed = f.readBytes( (char*)header, PCT_HEADER_LEN);
      
      if ( header[0] == '6' && header[1] == '4' && header[2] == 'K' ) {
          w = ((int)header[3]*256) + ((int)header[4]);
          h = ((int)header[5]*256) + ((int)header[6]);
      } else {
          mcu->println( "Wrong PCT header" );
          mcu->print( (int)header[0] );
          mcu->print( ',' );
          mcu->print( (int)header[1] );
          mcu->print( ',' );
          mcu->print( (int)header[2] );
          mcu->println( "");
          // mcu->println( header );
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

  // == just a try @ this time ==

  // destMode is SCREEN_MODE_320
  void drawPixShaded(int x, int y, uint16_t color, int fromMode ) {
    int sx = 0; // screenOffsetX;
    int sy = 0; // screenOffsetY;

    // REFACTO that code
    // if ( fromMode == SCREEN_MODE_128 ) {
    //   int b__screenWidth = 128;
    //   int b__screenHeight = 64;

    //   sx = (320 - b__screenWidth) / 2;
    //   sy = (240 - b__screenHeight) / 2;
    // } else if ( fromMode == SCREEN_MODE_160 ) {
    //   int b__screenWidth = 160;
    //   int b__screenHeight = 128;

    //   sx = (320 - b__screenWidth) / 2;
    //   sy = (240 - b__screenHeight) / 2;
    // } else if ( fromMode == SCREEN_MODE_320 ) {
    //   int b__screenWidth = 320;
    //   int b__screenHeight = 240;

    //   sx = 0;
    //   sy = 0;
    // } 



    if ( __screenMode != SCREEN_MODE_320 ) {
      _oled_display->drawPixel(screenOffsetX+x, screenOffsetY+y, color);
      return;
    }

    if ( fromMode == SCREEN_MODE_128 ) {
      sx = (320-(128*2))/2;
      sy = (240-(64*3))/2;
      x*=2; int w=2;
      y*=3; int h=3;
      _oled_display->fillRect(sx+x, sy+y, w, h, color);
    } else if ( fromMode == SCREEN_MODE_160 ) {
      sx = (320-(160*2))/2;
      sy = (240-(128*2))/2; // check if not overflow (256 Vs 240)
      x*=2; int w=2;
      y*=2; int h=2;
      _oled_display->fillRect(sx+x, sy+y, w, h, color);
    } else if ( fromMode == SCREEN_MODE_320 ) {
      _oled_display->fillRect(sx+x, sy+y, 1, 1, color);
    } 

  }
  // ============================

  void GenericMCU_SCREEN::drawPictureBPP( uint8_t* raster, int x, int y ) {
    if ( !ready ) { return; }

    int sx = screenOffsetX + x;
    int sy = screenOffsetY + y;

    int width = 128;
    int height = 64;

    unsigned char c;

    // TODO : lock blitt

    //this->drawRect(sx, sy, 128, 64, 1, 0);
    this->clear();
    //drawPixShadedRect(x, y, 128, 64, CLR_BLACK, SCREEN_MODE_128 );

    for (int yy = 0; yy < height; yy++) {
      for (int xx = 0; xx < width; xx++) {
        c = (raster[(yy * (width / 8)) + (xx / 8)] >> (7 - ((xx) % 8))) % 2;
        if (c == 0x00) {
        }
        else {
            // _oled_display->drawPixel(sx + xx, sy + yy, CLR_WHITE);
            drawPixShaded(xx, yy, CLR_WHITE, SCREEN_MODE_128 );
        }
      }
    }    

    // NEED to blitt ??????
  }

  // void drawPictureIndexed( uint8_t* raster, int x, int y, int w=-1, int h=-1, bool includesPalette=false );

void GenericMCU_SCREEN::drawPicture565Sprite( uint16_t* raster, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh ) {

  static int lastOffset=-1,lastW=-1,lastH=-1;

  // BEWARE : mem overflow !!!!!
  // 32x64 seems to be the max @ this time
  // static uint16_t subImage[ dw*dh ];
  static uint16_t subImage[ 64*64 ];

  int startOffset = ( sy * dw ) + sx;
  int offset = startOffset;

  if (! ( lastOffset == offset && lastW == dw && lastH == dh ) ) {
    for(int yy=0; yy < dh; yy++) {
      // // TODO : find faster way !
      // for(int xx=0; xx < dw; xx++) {
      //   subImage[(yy*dw)+xx] = color_picturebuff[offset+xx];
      // }

      memcpy( &subImage[(yy*dw)], &color_picturebuff[offset], dw*sizeof(uint16_t) );

      offset = ( (sy+yy) * sw ) + sx;
    }
  }

  dx += screenOffsetX;
  dy += screenOffsetY;

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