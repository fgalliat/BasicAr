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

  // ===========================================================
  static bool __mcuBridgeReady = false;

  void GenericMCU::reset() { 
    println("Reset...");
    // if bridged to MCU#2
    // have to reset it

    mcuBridge.write( SIG_MCU_RESET );
    // flushBridgeRX();
    __mcuBridgeReady = false;

    delay(500);
    yield();

    ESP.restart();
  }

  // called by constructor
  void GenericMCU::init() {
    gpio = new GenericMCU_GPIO(this);
    buzzer = new GenericMCU_BUZZER(this);
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

    //println("init done...");
    Serial.println("init done...");

    // ===== Bridge Sync. Routine ======================

    __mcuBridgeReady = false;
    //println("sync");
    Serial.println("sync");

    int t0 = millis();
    int timeout = 3 * 1000;

    #ifdef MCU_MASTER
      const int signalToRead = SIG_MCU_SLAVE_SYNC;
      const int signalToSend = SIG_MCU_MASTER_SYNC;
    #else
      const int signalToRead = SIG_MCU_MASTER_SYNC;
      const int signalToSend = SIG_MCU_SLAVE_SYNC;
    #endif

    __mcuBridgeReady = false;
    int cpt = 0;
    while(true) {
      led(0, true);
      // on MCU#1 -> use Serial.println() until 
      // bridge is not ready

      if ( mcuBridge.available() > 0 ) {
        Serial.println(" Found a 1st byte");
        t0 = millis();

        if ( mcuBridge.read() == 0xFF ) {
          Serial.println(" Found the 1st byte");
          t0 = millis();

          while ( mcuBridge.available() <= 0 ) { 
            if ( millis() - t0 >= timeout ) { break; }
            delay(10);
          }
          if ( millis() - t0 >= timeout ) { break; }
          Serial.println(" Found a 2nd byte");
          if ( mcuBridge.read() == signalToRead ) {
            Serial.println(" Found the 2nd byte");
            __mcuBridgeReady = true;
            break;
          }
        }
      }
      if ( millis() - t0 >= timeout ) { break; }
      delay(100);
      Serial.println(" Send sync seq.");
      mcuBridge.write( 0xFF );
      mcuBridge.write( signalToSend );
      led(0, false);
      cpt++;
      // failed : no connected MCU 
      if ( cpt >= 100 ) { break; }
      delay(100);
    }
    println(" Exited");
    led(0, false);

    if ( __mcuBridgeReady ) {
      println("sync done...");
    } else {
      Serial.println("sync failed...");
    }
  }

  // called by setup()
  void GenericMCU::setupPostInternal() { 
  }

  // ISR managment further ....

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
    // MP3 TRIGGER
    _ext_GPIO.pinMode(SX1509_BTN_PIN + 0, INPUT);

    this->ready = true;
    mcu->println("Ext GPIO ready !");
  }

  // 0-based
  void GenericMCU_GPIO::led(uint8_t ledNum, bool state) {
    // tmp code
    this->mcu->led(0, state);
  }

  static bool btns[11];

  // 0-based
  bool GenericMCU_GPIO::btn(uint8_t btnNum) {
    // if ( !this->ready ) { return false; }
    // // + 8 -> cf rewiring
    // return _ext_GPIO.digitalRead(SX1509_BTN_PIN + 8 + btnNum) == LOW;

    // Cf btns[0] is builtinBtn
    return btns[btnNum+1];
  }


  // ===============================
  static bool isISRLOCKED = false;
  static bool MASTER_LOCK_ISR = false; // beware w/ that

  static void IRAM_ATTR _doISR();

  // static void IRAM_ATTR _doISR() {
  //     if ( isISRLOCKED ) { return; }
  //     mcu.doISR();
  // }

  void GenericMCU::setupISR() { 
    /* create a hardware timer */
    hw_timer_t * timer = NULL;

    /* Use 1st timer of 4 */
    /* 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up */
    timer = timerBegin(0, 80, true);

    /* Attach onTimer function to our timer */
    timerAttachInterrupt(timer, &(_doISR), true);

    /* Set alarm to call onTimer function every second 1 tick is 1us
    => 1 second is 1000000us */
    /* Repeat the alarm (third parameter) */
    uint64_t tVal = 1000000;
    tVal = 50000; // 50millis
    tVal = 100000; // 100 millis
    timerAlarmWrite(timer, tVal, true);

    /* Start an alarm */
    timerAlarmEnable(timer);
  }

  void GenericMCU::lockISR() { isISRLOCKED = true; }
  void GenericMCU::unlockISR() { isISRLOCKED = false; }

  void GenericMCU::MASTERlockISR() { MASTER_LOCK_ISR = true; }
  void GenericMCU::MASTERunlockISR() { MASTER_LOCK_ISR = false; }


  static bool inISR = false;

  // fcts called by an IRAM_ATTR must be IRAM_ATTR too !!!!
  void IRAM_ATTR GenericMCU::doISR() { 
    if ( isISRLOCKED || MASTER_LOCK_ISR ) { 
      // yield(); 
      inISR = false;
      return; 
    }

    if ( inISR ) { return; }  
    inISR = true;
    btns[0] = builtinBTN();
    if ( !this->gpio->isReady() ) { return; }

    for(int i=0; i < 7; i++) {
      // 0-based
      // + 8 -> cf rewiring
      btns[1+i] = _ext_GPIO.digitalRead(SX1509_BTN_PIN + 8 + i) == LOW;
    }
    for(int i=8; i < 11; i++) {
      btns[i] = false;
    }
    // really low : inv. logic
    btns[10] = _ext_GPIO.digitalRead(SX1509_BTN_PIN + 0) == LOW;

    inISR = false;
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

  void GenericMCU_FS::format() {
    Serial.println("Formating");
    bool ff = SPIFFS.format();
    Serial.println("Formated");
  }

  void GenericMCU_FS::remove(char* filename) {
    // if (!SPIFFS.exists(filename) ) { return; }
    mcu->lockISR();
    SPIFFS.remove(filename);
    mcu->unlockISR();
    delay(100);
  }

  #ifdef MAIN_INO_FILE
    File currentFile;
    bool currentFileValid = false;

    char __myLine[256+1];

#ifndef min
 #define min(a,b) (a < b ? a : b)
#endif

  #endif

  bool  GenericMCU_FS::openCurrentTextFile(char* filename, bool readMode) {
    mcu->lockISR();
    if ( filename == NULL ) { Serial.print("CAN'T OPEN NULL FILE\n"); return false; }
    Serial.print("opening "); Serial.println(filename);
    currentFileValid = false;
    if ( readMode && !SPIFFS.exists(filename) ) { return false; }
    delay(100);
    // Serial.println("RIGHT HERE");
    currentFile = SPIFFS.open(filename, readMode ? "r" : "w");
    // Serial.println("RIGHT NOW");
    if ( !currentFile ) { Serial.println("failed"); mcu->unlockISR(); return false; }

    currentFile.seek(0);
    delay(50);
    // Serial.println("RIGHT ...");
    currentFileValid = true;
    // Serial.println("opened");
    mcu->unlockISR();
    return true;
  }

  void  GenericMCU_FS::closeCurrentTextFile() {
    mcu->lockISR();
    if ( currentFileValid ) {
        currentFile.flush();
        currentFile.close();
        currentFileValid = false;
    }
    mcu->unlockISR();
  }

  // have to provide "\n" @ end of line
  void GenericMCU_FS::writeCurrentText(char* line, bool autoflush) {
      if ( line == NULL ) { Serial.print("CANT WRITE NULL LINE\n"); return; }
      mcu->lockISR();
      int len = strlen( line );
      currentFile.write( (uint8_t*)line, len );
      if (autoflush) currentFile.flush();
      mcu->unlockISR();
  }

  void GenericMCU_FS::writeCurrentTextBytes(char* line, int len) {
      bool autoflush = true;
      if ( line == NULL ) { Serial.print("CANT WRITE NULL BYTES\n"); return; }
      mcu->lockISR();
      currentFile.write( (uint8_t*)line, len );
      if (autoflush) currentFile.flush();
      mcu->unlockISR();
  }

  char* GenericMCU_FS::readCurrentTextLine() {
    const int MAX_LINE_LEN = 256;
    memset(__myLine, 0x00, MAX_LINE_LEN +1);
    mcu->lockISR();
    if ( currentFile.available() <= 0 ) {
        mcu->unlockISR();
        return NULL;
    }

    int ch, cpt=0;
    for(int i=0; i < MAX_LINE_LEN; i++) {
        if ( currentFile.available() <= 0 ) { break; }
        ch = currentFile.read();
        if ( ch == -1 )   { break;    }
        if ( ch == '\r' ) { continue; }
        if ( ch == '\n' ) { break;    }
        __myLine[ cpt++ ] = (char)ch;
    }

    mcu->unlockISR();
    return __myLine;
  }

  // filename = "/toto.txt"
  // returns nb of bytes readed
  // start = 0 by default
  int GenericMCU_FS::readBinFile(char* filename, uint8_t* dest, int maxLen, int start) {
    mcu->lockISR();
    if (!SPIFFS.exists(filename) ) { mcu->unlockISR(); return -1; }

    File f = SPIFFS.open(filename, "r");
    if ( !f ) { mcu->unlockISR(); return -1; }

    f.seek( start );

    int readed = f.readBytes( (char*)dest, maxLen);
    f.close();

    mcu->unlockISR();
    return readed;
  }

  static bool flushBridgeRX(int valueToWait=0xFF);

  void GenericMCU_FS::lsMCU2() {
    mcuBridge.write( SIG_MCU_DIR2 );
    flushBridgeRX();
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

  // ======== Bridge ====================================================================
  static bool waitBridgeSIG(int valueToWait=0xFF) {
    while( mcuBridge.available() <= 0 ) {
      delay(10); yield();
    }
    // 0xFE if failed
    bool ok = mcuBridge.read() == valueToWait;
    return ok;
  }

  static bool flushBridgeRX(int valueToWait) {
    return waitBridgeSIG(valueToWait);
  }

  static bool writeBridgeU16(int val) {
    //uint8_t d0 = val / 256; // up to 64K value
    uint8_t d0 = val >> 8;
    uint8_t d1 = val % 256;

    mcuBridge.write( d0 );
    mcuBridge.write( d1 );
  }

  static bool writeBridgeU16Coords(int val) {
    if ( val < 0 )     { val = 0; }
    if ( val > 65535 ) { val = 65535; }
    return writeBridgeU16(val);
  }

  void GenericMCU_FS::copyToBridge(char* filename) {
    mcu->lockISR();
    // DO NOT OUTPUT TO SCREEN
    // DO NOT USE mcu->print(...)
    
    File f = SPIFFS.open( filename, "r" );
    if ( !f ) {
      Serial.println("File not ready !");
      mcu->unlockISR();
      return;
    }

    f.seek(0);

    Serial.println("Upload starts");
    mcuBridge.write( SIG_MCU_UPLOAD_BDG );
    Serial.println(" Upload waits for ACK");
    // just wait for an 0xFF ACK
    waitBridgeSIG(0xFF);

    Serial.println(" Upload sending filename");
    mcuBridge.println(filename);
    // delay(10);
    delay(200);

    int fileSize = f.size(); // TODO chech that

    Serial.println(" Upload sending filesize");
    mcuBridge.println( fileSize );
    delay(500);

    #define BRIDGE_UPL_PACKET_SIZE 32
    static char content[BRIDGE_UPL_PACKET_SIZE];

    int readed,total=0;
    while(true) {
      readed = f.readBytes( content, BRIDGE_UPL_PACKET_SIZE );

      mcuBridge.write( (uint8_t*)content, readed );
      // delay(10);
      if ( readed > 2 ) {
        Serial.print( (int)content[0] );
        Serial.print( ' ' );
        Serial.print( (char)content[0] );
        Serial.print( '/' );
        Serial.print( (int)content[1] );
        Serial.print( ' ' );
        Serial.print( (char)content[1] );
      }

      total += readed;

      // just wait for an 0xFE HandShake
      Serial.println(" Upload waiting handshake");
      waitBridgeSIG(0xFE);

      if (total >= fileSize) { break; }
    }

    f.close();
    // just wait for an 0xFF HandShake
    Serial.println(" Upload waiting -EOF-");
    waitBridgeSIG(0xFF);

    Serial.println("Upload complete");
    flushBridgeRX();

    mcu->unlockISR();
  }


  // ======== MusicPlayer ===============================================================
  // uses Bridge
  void GenericMCU_MUSIC_PLAYER::setup() {
    if ( ! __mcuBridgeReady ) { 
      mcu->println("Bridged Music Player NOT ready !");
      this->ready = false; return; 
    }
    this->ready = true;
  }

  void GenericMCU_MUSIC_PLAYER::playTrack(int trckNum) { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }

    mcuBridge.write( SIG_MP3_PLAY );
    writeBridgeU16( trckNum );
    flushBridgeRX();
  }

  void GenericMCU_MUSIC_PLAYER::pause() { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    mcuBridge.write( SIG_MP3_PAUSE );
    flushBridgeRX();
  }

  void GenericMCU_MUSIC_PLAYER::stop() { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    mcuBridge.write( SIG_MP3_STOP );
    flushBridgeRX();
  }

  void GenericMCU_MUSIC_PLAYER::next() { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    mcuBridge.write( SIG_MP3_NEXT );
    flushBridgeRX();
  }

  void GenericMCU_MUSIC_PLAYER::prev() { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    mcuBridge.write( SIG_MP3_PREV );
    flushBridgeRX();
  }

  void GenericMCU_MUSIC_PLAYER::setVolume(uint8_t volume) { 
    if ( !this->ready ) { mcu->println("Music Player not ready !"); return; }
    mcuBridge.write( SIG_MP3_VOL );
    mcuBridge.write( volume );
    flushBridgeRX();
  }

  bool GenericMCU_MUSIC_PLAYER::isPlaying() {
    if ( !this->ready ) { return false; }
    // TODO : better -> leads to GPIO pin #0
    return mcu->btn( 10 );
  }

  // ======== Screen ====================================================================
  // uses Bridge
  void GenericMCU_SCREEN::setup() {
    if ( ! __mcuBridgeReady ) { 
      mcu->println("Bridged Screen NOT ready !");
      this->ready = false; return; 
    }
    this->ready = true;
    mcu->println("Temp. Screen ready !");
  }

  void GenericMCU_SCREEN::print(char* str) { 
    if ( !this->ready ) { Serial.print(str); return; }
    mcuBridge.write( SIG_SCR_PRINT_STR );
    mcuBridge.print( str );
    mcuBridge.write( 0x00 );
    flushBridgeRX();
  }

  void GenericMCU_SCREEN::print(char ch) {
    if ( !this->ready ) { Serial.print(ch); return; }
    mcuBridge.write( SIG_SCR_PRINT_CH );
    mcuBridge.write( ch );
    flushBridgeRX();
  }

  void GenericMCU_SCREEN::print(int   val) {
    if ( !this->ready ) { Serial.print(val); return; }
    // in my BASIC int(s) are float(s)
    // from mem_utils.h
    const int tsize = getSizeOfFloat();
    unsigned char memSeg[ tsize ];
    copyFloatToBytes(memSeg, 0, (float)val);

    mcuBridge.write( SIG_SCR_PRINT_INT );
    mcuBridge.write( memSeg, tsize );
    flushBridgeRX();
  }

  void GenericMCU_SCREEN::print(float val) {
    if ( !this->ready ) { Serial.print(val); return; }
    // from mem_utils.h
    const int tsize = getSizeOfFloat();
    unsigned char memSeg[ tsize ];
    copyFloatToBytes(memSeg, 0, val);

    mcuBridge.write( SIG_SCR_PRINT_NUM );
    mcuBridge.write( memSeg, tsize );
    flushBridgeRX();
  }

  // cursor in nbof chars (so max is 256x256)
  void GenericMCU_SCREEN::setCursor(int x, int y) {
    if ( !this->ready ) { return; }
    mcuBridge.write( SIG_SCR_CURSOR );
    mcuBridge.write( x ); // in nb of chars
    mcuBridge.write( y );
    flushBridgeRX();
  }

  void GenericMCU_SCREEN::setColor(uint16_t color) {
    if ( !this->ready ) { return; }

    mcuBridge.write( SIG_SCR_COLOR );
    writeBridgeU16( color );
    flushBridgeRX();
  }

  void GenericMCU_SCREEN::setMode(uint8_t mode) {
    if ( !this->ready ) { return; }
    mcuBridge.write( SIG_SCR_MODE );
    mcuBridge.write( mode );
    flushBridgeRX();
  }

  // mode : TEXT_OVERWRITE / TEXT_INCRUST
  void GenericMCU_SCREEN::setTextMode(uint8_t mode, uint16_t fg, uint16_t bg) {
    if ( !this->ready ) { return; }
    mcuBridge.write( SIG_SCR_TXTMODE );
    mcuBridge.write( mode );
    writeBridgeU16( fg );
    writeBridgeU16( bg );
    flushBridgeRX();
  }

  void GenericMCU_SCREEN::blitt(uint8_t mode) {
    if ( !this->ready ) { return; }
    mcuBridge.write( SIG_SCR_BLITT );
    mcuBridge.write( mode );
    flushBridgeRX();
  }

  void GenericMCU_SCREEN::clear() {
    if ( !this->ready ) { return; }
    mcuBridge.write( SIG_SCR_CLEAR );
    flushBridgeRX();
  }

  // mode = 1 : fill // mode = 0 : draw
  void GenericMCU_SCREEN::drawRect(int x, int y, int w, int h, uint8_t mode, uint16_t color) {
    if ( !this->ready ) { return; }

    mcuBridge.write( SIG_SCR_DRAW_RECT );
    writeBridgeU16Coords( x );
    writeBridgeU16Coords( y );
    writeBridgeU16Coords( w );
    writeBridgeU16Coords( h );
    mcuBridge.write( mode ); // MODE
    writeBridgeU16( color );

    flushBridgeRX();
  }

  // mode = 1 : fill // mode = 0 : draw
  void GenericMCU_SCREEN::drawTriangle(int x, int y, int x2, int y2, int x3, int y3, uint8_t mode, uint16_t color) {
    if ( !this->ready ) { return; }

    mcuBridge.write( SIG_SCR_DRAW_TRIANGLE );
    writeBridgeU16Coords( x );
    writeBridgeU16Coords( y );
    writeBridgeU16Coords( x2 );
    writeBridgeU16Coords( y2 );
    writeBridgeU16Coords( x3 );
    writeBridgeU16Coords( y3 );
    mcuBridge.write( mode ); // MODE
    writeBridgeU16( color );

    flushBridgeRX();
  }

  // mode = 1 : fill // mode = 0 : draw
  void GenericMCU_SCREEN::drawCircle(int x, int y, int radius, uint8_t mode, uint16_t color) {
    if ( !this->ready ) { return; }

    mcuBridge.write( SIG_SCR_DRAW_CIRCLE );
    writeBridgeU16Coords( x );
    writeBridgeU16Coords( y );
    writeBridgeU16Coords( radius );
    mcuBridge.write( mode ); // MODE
    writeBridgeU16( color );
    
    flushBridgeRX();
  }

  void GenericMCU_SCREEN::drawLine(int x, int y, int x2, int y2, uint16_t color) {
    if ( !this->ready ) { return; }

    mcuBridge.write( SIG_SCR_DRAW_LINE );
    writeBridgeU16Coords( x );
    writeBridgeU16Coords( y );
    writeBridgeU16Coords( x2 );
    writeBridgeU16Coords( y2 );
    writeBridgeU16( color );

    flushBridgeRX();
  }

  void GenericMCU_SCREEN::drawPixel(int x, int y, uint16_t color) {
    if ( !this->ready ) { return; }

    mcuBridge.write( SIG_SCR_DRAW_PIX );
    writeBridgeU16Coords( x );
    writeBridgeU16Coords( y );
    writeBridgeU16( color );

    flushBridgeRX();
  }

  void GenericMCU_SCREEN::drawPicture565( char* filename, int x, int y, int _w, int _h ) {
    if ( !this->ready ) { return; }

    mcuBridge.write( SIG_SCR_DRAW_PCT );
    writeBridgeU16Coords( x );
    writeBridgeU16Coords( y );
    mcuBridge.print( filename );
    mcuBridge.write( 0x00 );

    flushBridgeRX();
  }

  void GenericMCU_SCREEN::drawPicture565Sprite( char* filename, int x, int y, int w, int h, int sx, int sy ) {
    if ( !this->ready ) { return; }

    mcuBridge.write( SIG_SCR_DRAW_PCT_SPRITE );
    writeBridgeU16Coords( x );
    writeBridgeU16Coords( y );
    writeBridgeU16Coords( w );
    writeBridgeU16Coords( h );
    writeBridgeU16Coords( sx );
    writeBridgeU16Coords( sy );
    mcuBridge.print( filename );
    mcuBridge.write( 0x00 );

    flushBridgeRX();
  }

  void GenericMCU_SCREEN::drawPictureBPP( char* filename, int x, int y ) {
    if ( !this->ready ) { return; }

    mcuBridge.write( SIG_SCR_DRAW_BPP );
    writeBridgeU16Coords( x );
    writeBridgeU16Coords( y );
    mcuBridge.print( filename );
    mcuBridge.write( 0x00 );

    flushBridgeRX();
  }

  #define BTN_UP   1
  #define BTN_DOWN 2

  #define BTN_1    7
  #define BTN_2    6
  #define BTN_3    5

  bool GenericMCU::getSystemMenuReqState()  { return this->btn( BTN_UP ) && this->btn( BTN_1 ) && this->btn( BTN_2 ); }
  bool GenericMCU::getSystemMenuReqEnd()    { return !this->btn( BTN_UP ) && !this->btn( BTN_1 ) && !this->btn( BTN_2 ); }
  bool GenericMCU::getSystemResetReqState() { return this->btn( BTN_DOWN ) && this->btn( BTN_1 ) && this->btn( BTN_2 ); }
  bool GenericMCU::getSystemBreakReqState() { return this->btn( BTN_1 ) && this->btn( BTN_2 ); }


#endif