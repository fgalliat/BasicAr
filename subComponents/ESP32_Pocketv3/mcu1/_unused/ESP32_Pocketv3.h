#if defined(ARDUINO_ARCH_ESP32) and defined(ESP32PCKv3)

 /**************
 * Esp32 Devkit R1 Board w/ External SPI LCD ILI9341 2"8 320x240
 * Xts-uBasic Pocket v3
 * Xtase - fgalliat @ Mar 2018
 ***************/

  #ifndef _ESP32PckV3_H
  #define _ESP32PckV3_H 1

    // #define DBUG_ESP32 1

// TODO : check if kept
//#ifdef COLOR_64K
extern unsigned short color_picturebuff[];
//#endif
    
    // for 320x240 Screens
    #define MY_320 1
    #define COLOR_SCREEN 1

    // use the Sega GameGear Hacked Console cross-pad
    #define USE_GAMEGEAR_PAD 1

    // ==============================
    #ifdef ESP32_I_TFT_eSPI
        // TFT_eSPI Lib is pretty faster than Adafruit one
        #include <TFT_eSPI.h> // Hardware-specific library
    #else
        #include "SPI.h"
        #include "Adafruit_GFX.h"
        #include "Adafruit_ILI9341.h"
    #endif
    

    #ifdef MY_320
      #define ROTATE_SCREEN 0
      #define TFT_WIDTH 320
      #define TFT_HEIGHT 240 
    #endif


    /* -- impl. sample --
    uint16_t TFT_eSPI::color565(uint8_t r, uint8_t g, uint8_t b) {
      return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
    */

// #define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
// #define ILI9341_DARKGREY 0x7BEF /* 128, 128, 128 */ !!!! NOT SAME VALUE !!!!

    #define CLR_BLACK       ILI9341_BLACK
    #define CLR_WHITE       ILI9341_WHITE
    //#define CLR_LIGHTGREY   TFT_LIGHTGREY // ILI9341_BLACK, ILI9341_RED, ILI9341_CYAN
    #define CLR_LIGHTGREY   ILI9341_RED
    #define CLR_GREY        0x8410 /* 128,128,128 */
    //#define CLR_DARKGREY    TFT_DARKGREY
    #define CLR_DARKGREY    ILI9341_CYAN
    #define CLR_PINK        ILI9341_PINK

    /*
      https://github.com/me-no-dev/arduino-esp32fs-plugin
      --> BEWARE NO MORE WORKING @ this time !!!!!!
    */
    #include <SPIFFS.h>
    #include <FS.h>

    #define BLACK 0x00
    #define WHITE 0x01

#ifndef min
 #define min(a,b) (a < b ? a : b)
#endif

    // =========== GPIO =========

    // -- SPI SCreen - values to copy in TFT_eSPI/User_Select.h
    #define TFT_MISO 19
    #define TFT_MOSI 23
    #define TFT_CLK 18
    #define TFT_CS    5  // Chip select control pin
    #define TFT_DC    15  // Data Command control pin
    // connected to EN pin
    #define TFT_RST   -1    // 4 is now used as TX2 ('cause pin labeled TX2 doesn't send anything)

    // ________________________

// [SECTION TO CHECK]-----------------
    // driven by xtase_fct.h

    // #define BTN1 26
    // // BEWARE 25 is used as LED on XtsPocket v1
    // // Oups mis-wired BT2 & TR-A !!
    // // #define BTN2 25
    // #define BTN2 32
    // #define BTN3 33
    #define BTN1 -1
    #define BTN2 -1
    #define BTN3 -1


    #ifdef USE_JG1010_PAD
     // Oups mis-wired BT2 & TR-A !!
     // #define BTN_TRIGGER_A 32
     #define BTN_TRIGGER_A 25
    #endif

// [SECTION TO CHECK]-----------------

    // the DFPlayer MP3 BUSY LINE
    // BEWARE : INPUT but NOT PULLUP
    #define BTN_TRIGGER_MP3_BUSY 35

    // [HERE HAVE TO DEFINE CONTROLLER/PAD PINS -OR- I2C]


    #define DBUG_PAD(a) { Serial.println(a); }

    // builtin led
    #define LED1 2

    #define BUZZER1 27
    #define BUZ_channel 0
    #define BUZ_resolution 8

    // #ifdef USE_JG1010_PAD
    // class Esp32Pocketv2JG1010DigitalPad {
    //     private:
    //       bool left  = false;
    //       bool right = false;
    //       bool up    = false;
    //       bool down  = false;
    //       bool btnB  = false;
    //     public:
    //       Esp32Pocketv2JG1010DigitalPad() {
    //       }
    //       ~Esp32Pocketv2JG1010DigitalPad() {
    //       }
    //       void initGPIO() {
    //           pinMode( JG1010_PAD_OUT_P1, OUTPUT );
    //           pinMode( JG1010_PAD_OUT_P2, OUTPUT );
    //           pinMode( JG1010_PAD_OUT_P4, OUTPUT );

    //           // BEWARE : not PULLUP inputs as usual
    //           pinMode( JG1010_PAD_IN_P3, INPUT );
    //           pinMode( JG1010_PAD_IN_P5, INPUT );

    //           digitalWrite( JG1010_PAD_OUT_P1, LOW );
    //           digitalWrite( JG1010_PAD_OUT_P2, LOW );
    //           digitalWrite( JG1010_PAD_OUT_P4, LOW );
    //       }

    //       void poll() {
    //           int result = 0;

    //           /* /!\ BEWARE : delay() in ISR makes 
    //           *  PLAY5K strange delays
    //           *  try to use yeld() instead
    //           */

    //           // a bit tricky ....
    //           pinMode( JG1010_PAD_IN_P3, INPUT );
    //           pinMode( JG1010_PAD_IN_P5, INPUT );
    //           digitalWrite( JG1010_PAD_OUT_P1, LOW );
    //           digitalWrite( JG1010_PAD_OUT_P2, LOW );
    //           digitalWrite( JG1010_PAD_OUT_P4, LOW );
    //           digitalWrite( JG1010_PAD_IN_P3, LOW );
    //           digitalWrite( JG1010_PAD_IN_P5, LOW );

    //           left = false;
    //           right = false;
    //           up = false;
    //           down = false;
    //           btnB = false;

    //           #define PAD_CONTACT_3 4070
    //           #define PAD_CONTACT_5 4070
    //           #define READ_DELAY 5

    //           digitalWrite( JG1010_PAD_OUT_P1, HIGH );
    //           result = analogRead( JG1010_PAD_IN_P3 );
    //           left = result >= PAD_CONTACT_3;
    //           result = analogRead( JG1010_PAD_IN_P5 );
    //           down = result >= PAD_CONTACT_5;
    //           digitalWrite( JG1010_PAD_OUT_P1, LOW );

    //           digitalWrite( JG1010_PAD_OUT_P2, HIGH );
    //           result = analogRead( JG1010_PAD_IN_P3 );
    //           //Serial.println(result);
    //           right = result >= PAD_CONTACT_3;
    //           digitalWrite( JG1010_PAD_OUT_P2, LOW );

    //           digitalWrite( JG1010_PAD_OUT_P4, HIGH );
    //           result = analogRead( JG1010_PAD_IN_P3 );
    //           up = result >= PAD_CONTACT_3;
    //           // Serial.print(result); Serial.print(" ");
    //           result = analogRead( JG1010_PAD_IN_P5 );
    //           btnB = result >= PAD_CONTACT_5;
    //           digitalWrite( JG1010_PAD_OUT_P4, LOW );
    //           // Serial.println(result);

    //         //   if (left) { DBUG_PAD("[LEFT]"); }
    //         //   if (right) { DBUG_PAD("[RIGHT]"); }
    //         //   if (up) { DBUG_PAD("[UP]"); }
    //         //   if (down) { DBUG_PAD("[DOWN]"); }
    //         //   if (btnB) { DBUG_PAD("[B]"); }

    //       }

    //       bool readLeft() {
    //           return left;
    //       }
    //       bool readRight() {
    //           return right;
    //       }
    //       bool readUp() {
    //           return up;
    //       }
    //       bool readDown() {
    //           return down;
    //       }
    //       bool readTriggerB() {
    //           return btnB;
    //       }
    // };
    // #endif


    class Esp32Pocketv3Screen {
        private:
            #ifdef ESP32_I_TFT_eSPI
              TFT_eSPI* _oled_display;
            #else
              Adafruit_ILI9341* _oled_display;
            #endif
            int _oled_ttyY = 0;
            uint16_t drawColor = CLR_WHITE;

            uint16_t screenOffsetX = 0; // MY_320
            uint16_t screenOffsetY = 0;
        public:
            Esp32Pocketv3Screen() {
                screenOffsetX = (320 - 128) / 2;
                screenOffsetY = (240 - 64) / 2;
            }

            ~Esp32Pocketv3Screen() {}

            void init() {
                #ifdef ESP32_I_TFT_eSPI
                 _oled_display = new TFT_eSPI();
                 _oled_display->init();
                #else
                  _oled_display = new Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
                  _oled_display->begin();
                #endif

                _oled_display->setRotation(1+ROTATE_SCREEN); // LANDSCAPE

                _oled_display->fillScreen(CLR_BLACK);

                _oled_display->setTextColor(drawColor);
                _oled_display->setTextSize(0); // if doesn't work --> try (1)
            }

            void blitt() {
                // ESP8266 specific impl. of yield() (Cf Wifi ops & deadlocks ....) (see AdaGFX samples)
                yield();
            }

            void drawString(int x, int y, char* str) {
                _oled_display->setCursor(x, y);
                _oled_display->print(str);

                #ifdef DBUG_ESP32
                  Serial.print( str );
                #endif
            }

            void println(char* str) {
                // could try :: _oled_display->println(str);

                this->drawString(0, this->_oled_ttyY*8, str);
                this->_oled_ttyY++;

                if ( this->_oled_ttyY >= 8 ) {
                    // auto scroll
                    this->_oled_ttyY = 8;
                }
            }

            void drawLine(int x1, int y1, int x2, int y2) {
                _oled_display->drawLine(x1,y1,x2,y2, drawColor);
            }

            void drawCircle(int x1, int y1, int r) {
                 _oled_display->drawCircle(x1,y1,r, drawColor);
            }

            void setColor(int color) {
                //_oled_display->setColor(color==1?WHITE:BLACK);
                drawColor = color==1 ? CLR_WHITE : CLR_BLACK;
                // _oled_display->setTextColor( drawColor );


                // ????? tft.color565(i*10, i*10, 0)); ==> seems to return an uint16_t
            }

            void setPixel(int x,int y) {
                #ifdef DBL_BUFF_ACTION
                 _scr_pushScreenAction(_oled_display, ACT_PIXEL,x, y, 0x00, 0x00, drawColor, ACT_MODE_DRAW);
                #else
                 _oled_display->drawPixel(x,y, drawColor);
                #endif
            }


            void drawImg(int x,int y, int width, int height, unsigned char* picBuff) {
                unsigned char c;
                _oled_display->fillScreen( CLR_BLACK );

                for (int yy = 0; yy < height; yy++) {
                    for (int xx = 0; xx < width; xx++) {
                        c = (picBuff[(yy * (width / 8)) + (xx / 8)] >> (7 - ((xx) % 8))) % 2;
                        if (c == 0x00) {
                        }
                        else {
                            _oled_display->drawPixel(x + xx, y + yy, CLR_WHITE);
                        }
                    }
                }
            }

            // not buffered
            bool drawPctFile(int x, int y, char* filename) {
                if (!SPIFFS.exists(filename) ) { return false; }

long t0,t1;
t0 = millis();

                File f = SPIFFS.open(filename, "r");
                if ( !f ) { return -1; }

                char header[7];
                int readed = f.readBytes( (char*)header, 7);

                 int mode = -1, w=-1, h=-1;
//Serial.println("A.1");
                if ( header[0] == '6' && header[1] == '4' && header[2] == 'K' ) {
                    // mode = ACT_MODE_PCT_64K;
                    w = ((int)header[3]*256) + ((int)header[4]);
                    h = ((int)header[5]*256) + ((int)header[6]);
                }
Serial.print("A.2 "); Serial.print(w); Serial.print('x');Serial.print(h);Serial.println("");

int yy = 0;
while( true ) { 
    // BEWARE : @ this time : h need to be 128
                readed = f.readBytes( (char*)color_picturebuff, w*h*2/8);
//Serial.print("A.2 bis"); Serial.print(readed); Serial.print(" of ");Serial.print(w*h*2);Serial.println("");

//Serial.println("A.3");
                #ifdef ESP32_I_TFT_eSPI
                  _oled_display->pushRect(x, y+yy, w, 128/8, color_picturebuff);
                #else
                  _oled_display->drawRGBBitmap(x, y+yy, color_picturebuff, w, 128/8);
                #endif
//Serial.println("A.4");

yy += 128/8;
if ( yy + (128/8) > h ) { break; }
}
                f.close();
Serial.println("A.5");

t1 = millis();
Serial.print(t1-t0);Serial.println("msec");

                return true;                
            }



            // but supports gray
            void drawRect(int x,int y, int width, int height, int color, int mode) {
                unsigned char c; int cpt = 0;

                uint16_t _color = CLR_PINK;
                if ( color == 0 ) { _color = CLR_BLACK; }
                else if ( color == 1 ) { _color = CLR_WHITE; }
                else if ( color == 2 ) { _color = CLR_DARKGREY; }
                else if ( color == 3 ) { _color = CLR_GREY; }
                else if ( color == 4 ) { _color = CLR_LIGHTGREY; }

                if ( mode == 0 ) {
                    _oled_display->drawRect(x,y,width,height, _color);
                } else {
                    _oled_display->fillRect(x,y,width,height, _color);
                }
            }

            void clear() {
                  _oled_ttyY = 0;
                  _oled_display->fillScreen( CLR_BLACK );
            }
    };

    class Esp32Pocketv3Fs {
        private:
          File currentFile;
          bool currentFileValid = false;
        public:
          Esp32Pocketv3Fs() {}
          ~Esp32Pocketv3Fs() {}

          void init() {
            SPIFFS.begin();
            //bool ff = SPIFFS.format();

/*
if (!SPIFFS.exists("/formatComplete.txt") && !SPIFFS.exists("/AUTORUN.BAS")) {
    Serial.println("Please wait 30 secs for SPIFFS to be formatted");
    SPIFFS.format();
    Serial.println("Spiffs formatted");
   

    File _f = SPIFFS.open("/AUTORUN.BAS", "w");
    _f.println("1 ' blank line");
    _f.println("10 ? \"Coucou\" ");
    _f.close();

    File f = SPIFFS.open("/formatComplete.txt", "w");
    if (!f) {
        Serial.println("file open failed");
    } else {
        f.println("Format Complete");
    }
    f.close();

    while(true) { delay(250); }
  } else {
    Serial.println("SPIFFS is formatted. Moving along...");
  }
*/
          }

          // BEWARE !!!!!!
          void format() {
            Serial.println("Formating");
            bool ff = SPIFFS.format();
            Serial.println("Formated");
          }

          void remove(char* filename) {
            // if (!SPIFFS.exists(filename) ) { return; }  
            SPIFFS.remove(filename);
            delay(100);
          }

          // filename = "/toto.txt"
          // returns nb of bytes readed
          int readBinFile(char* filename, uint8_t* dest, int maxLen) {
            if (!SPIFFS.exists(filename) ) { return -1; }

            File f = SPIFFS.open(filename, "r");
            if ( !f ) { return -1; }
            int readed = f.readBytes( (char*)dest, maxLen);
            f.close();
            return readed;
          }

          // filename = "/toto.txt"
          // returns nb of lines readed
          // callbacked lines does not contains trailing '\n'
          int readTextFile(char* filename, void (*callback)(char*) ) {

            if ( filename == NULL ) {
                Serial.println("NULL filename !!!!");
                return -1;
            }
            else {
                Serial.print("TRY TO OPEN : ");Serial.println(filename);
            }

            if (!SPIFFS.exists(filename) ) { Serial.print("NOT EXISTS : ");Serial.println(filename); return -1; }

            File f = SPIFFS.open(filename, "r");
            if ( !f ) { Serial.print("FAILED : ");Serial.println(filename); return -1; }
            int lineNb = 0;
            // BEWARE w/ these values
            char buff[256];
            char line[164]; 
            int i=0, cpt=0, max=0;
            bool pending = false;

            callback( "\n" ); // just to be sure ....

            while( pending || (!pending && (max = f.readBytes( buff, 256 )) > 0) ) {

                if ( max <= 0 ) { break; }

                pending = false;
                while( max > 0 && i < max ) {
                    if ( buff[i] == 0x00 ) { pending = false; break; }
                    if ( buff[i] == '\n' ) { pending = true; i++; break; }
                    line[cpt++] = buff[i++];
                }
                
                if ( pending ) {
                    Serial.println( line );
                    callback( line );
                    
                    lineNb++;
                    memset(line, 0x00, 164);
                    cpt=0; 
                }

                if ( max <= 0 || buff[i] == 0x00 ) { pending=false; break; }
                if ( i >= max-1 ) { i=0; pending = false; }
            }

            if ( cpt > 0 ) {
                callback( line );
            }

            f.close();
            callback( "\n" ); // just to be sure ....

            return lineNb;
          }

          // ================================
          // WORKS on a CURRENT file .....

          bool openCurrentTextFile(char* filename, bool readMode = true) {
            if ( filename == NULL ) { Serial.print("CAN'T OPEN NULL FILE\n"); return false; }
            Serial.print("opening "); Serial.println(filename);
            this->currentFileValid = false;
            if ( readMode && !SPIFFS.exists(filename) ) { return false; }
            delay(100);
            //this->currentFile = NULL; // free ?
            Serial.println("RIGHT HERE");
            this->currentFile = SPIFFS.open(filename, readMode ? "r" : "w");
            // File f = SPIFFS.open(filename, readMode ? "r" : "w");
            Serial.println("RIGHT NOW");
            // if ( !f ) { Serial.println("failed"); return false; }
            if ( !currentFile ) { Serial.println("failed"); return false; }

            if (readMode) {
                currentFile.seek(0);
                // UTF BOM(s)
                // UTF-8 	                EF BB BF
                // UTF-16 Big Endian 	    FE FF
                // UTF-16 Little Endian 	FF FE
                // UTF-32 Big Endian 	    00 00 FE FF
                // UTF-32 Little Endian 	FF FE 00 00
            } else {
                currentFile.seek(0);
            }

            // this->currentFile = &( f );
            delay(100);
            // if (!readMode) this->currentFile->seek(0);
            // if ( this->currentFile == NULL ) { Serial.println("failed"); return false; }
            Serial.println("RIGHT ...");
            this->currentFileValid = true;
            Serial.println("opened");
            return true;
          }

          void closeCurrentTextFile() {
            Serial.println("closing");
            // if ( this->currentFileValid && this->currentFile != NULL ) {
            if ( this->currentFileValid ) {
                // this->currentFile->close();
                currentFile.flush();
                currentFile.close();
                this->currentFileValid = false;
            }
            Serial.println("closed");
          }
        
          void writeCurrentTextByte(char b) {
              currentFile.write( (uint8_t*)b, 1 );
              //currentFile.flush();
          }

          void writeCurrentTextBytes(char* b, int len) {
              currentFile.write( (uint8_t*)b, len );
              currentFile.flush();
          }

          // have to provide "\n" @ end of line
          void writeCurrentTextLine(char* line) {
              if ( line == NULL ) { Serial.print("CANT WRITE NULL LINE\n"); return; }
              int len = strlen( line );
              currentFile.write( (uint8_t*)line, len );
              currentFile.flush();
          }

          // slow impl !!!!!!!
          char __myLine[256+1];

          char* readCurrentTextLine() {
            //   Serial.println("readLine.1");
            int MAX_LINE_LEN = 256;
            memset(__myLine, 0x00, MAX_LINE_LEN +1);

            if ( this->currentFile.available() <= 0 ) {
                return NULL;
            }

            int ch, cpt=0;
            for(int i=0; i < MAX_LINE_LEN; i++) {
                if ( this->currentFile.available() <= 0 ) { break; }
                ch = this->currentFile.read();
                if ( ch == -1 )   { break;    }
                if ( ch == '\r' ) { continue; }
                if ( ch == '\n' ) { break;    }
                __myLine[ cpt++ ] = (char)ch;
            }

            return __myLine;
          }

          // ================================

          // listDir("/") -> returns nb of file
          int listDir(char* dirName, void (*callback)(char*,uint32_t) ) {
            File dir = SPIFFS.open("/");
            File entry;
            int entryNb = 0;
            while( (entry = dir.openNextFile() ) ) {
                if ( entry.isDirectory() ) { continue; }
                callback( (char*)entry.name(), entry.size() );
                entryNb++;
            }
            return entryNb;
          }
    };

    // dirty ...
    static void IRAM_ATTR _doISR();

    class Esp32Pocketv3 {
        private:
          Esp32Pocketv3Screen* screen = new Esp32Pocketv3Screen();
          Esp32Pocketv3Fs* fs = new Esp32Pocketv3Fs();

        //   #ifdef USE_JG1010_PAD
        //    Esp32Pocketv2JG1010DigitalPad* digiCross = new Esp32Pocketv2JG1010DigitalPad();
        //   #endif

          bool _isISR_LOCKED = false;

        public:

        Esp32Pocketv3() {
        }

        ~Esp32Pocketv3() {
        }

        bool isISR_LOCKED() { return _isISR_LOCKED; }

        void lockISR() { _isISR_LOCKED = true; }
        void unlockISR() { _isISR_LOCKED = false; }

        void setup() {
            this->initGPIO();
            this->initBuzzer();
            this->initLCD();
            this->initFS();

            // #ifdef USE_JG1010_PAD
            //  this->digiCross->initGPIO();
            // #endif

            this->initISR();
        }
        // === IRQ emulation =

        void initISR() {
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

        void poll() {
            // #ifdef USE_JG1010_PAD
            //   this->digiCross->poll();
            // #endif
        }

        // === FileSystem ===
        void initFS() {
            fs->init();
        }

        Esp32Pocketv3Fs* getFs() {
            return fs;
        }

        // ==================
        // ===== Screen =====

        void initLCD() {
            screen->init();
        }

        Esp32Pocketv3Screen* getScreen() {
            return screen;
        }

        // ==================
        // ====== GPIO ======

        void initGPIO() {

// [SECTION TO CHECK] ------------------------
//             pinMode( BTN1, INPUT_PULLUP );
//             pinMode( BTN2, INPUT_PULLUP );
//             pinMode( BTN3, INPUT_PULLUP );

// #ifdef USE_JG1010_PAD
//             pinMode( BTN_TRIGGER_A, INPUT_PULLUP );
// #else
//             pinMode( X_AXIS, INPUT );
//             pinMode( Y_AXIS, INPUT );
// #endif
// [SECTION TO CHECK] ------------------------



#ifdef BTN_TRIGGER_MP3_BUSY
            // BEWARE : NOT PULLUP
            pinMode( BTN_TRIGGER_MP3_BUSY, INPUT );
#endif

            pinMode( LED1, OUTPUT );
            digitalWrite(LED1, LOW); 
        }

        void initBuzzer() {
            int freq = 2000;
            ledcSetup(BUZ_channel, freq, BUZ_resolution);
            ledcAttachPin(BUZZER1, BUZ_channel);

            // sets the volume 
            ledcWrite(BUZ_channel, 0);
        }

        // ==== Inputs =====

        // not used by external @ this time : see xtase_fct.h
        int readBtns() {
            return readBtn(0);
        }

        // not used by external @ this time : see xtase_fct.h
        int readBtn(int btnID) {
            // TODO : include pad state as btn
            if ( btnID == 0 ) {
                // TODO : better canonicalize
                bool b1 = BTN1 > -1 && digitalRead( BTN1 ) == LOW;
                bool b2 = BTN2 > -1 && digitalRead( BTN2 ) == LOW;
                bool b3 = BTN3 > -1 && digitalRead( BTN3 ) == LOW;
                return (b1 ? 1 : 0) + (b2 ? 2 : 0) + (b3 ? 4 : 0);
            } else if (btnID == 1 && BTN1 > -1) {
                return digitalRead( BTN1 ) == LOW ? 1 : 0;
            } else if (btnID == 2 && BTN2 > -1) {
                return digitalRead( BTN2 ) == LOW ? 1 : 0;
            } else if (btnID == 3 && BTN3 > -1) {
                return digitalRead( BTN3 ) == LOW ? 1 : 0;
            } 

            // #ifdef USE_JG1010_PAD
            // else if (btnID == 8 && BTN_TRIGGER_A > -1) {
            //     return digitalRead( BTN_TRIGGER_A ) == LOW ? 1 : 0;
            // }
            // else if (btnID == 9) {
            //     return this->digiCross->readTriggerB() ? 1 : 0;
            // }
            // #endif

            else if (btnID == 10 && BTN_TRIGGER_MP3_BUSY > -1) {
                int v = digitalRead( BTN_TRIGGER_MP3_BUSY );
                // Serial.print("BUSY ");Serial.println(v);
                return v == 0 ? 1 : 0;
            }

            return 0;
        }

        int readPadXaxis() {

// [SECTION TO CHECK] -------------------------------
return 0;

// #ifdef USE_JG1010_PAD
//   // maybe slow !!!
//   if ( this->digiCross->readLeft() ) return -1;
//   if ( this->digiCross->readRight() ) return  1;
//   return 0;
// // #elif defined(BTN_LEFT) and defined(BTN_RIGHT)
// //   // digital joypad
// //   if ( digitalRead( BTN_LEFT ) == LOW ) return -1;
// //   if ( digitalRead( BTN_RIGHT ) == LOW ) return 1;
// #else
//             int v = analogRead( X_AXIS );
//             // Serial.print("x:");Serial.println(v);
//             if ( v <= 800 ) { return -1; }
//             else if ( v >= 2800 ) { return 1; }
//             else { v = 0; }
//             return v;
// #endif

// [SECTION TO CHECK] -------------------------------
        }

        int readPadYaxis() {
return 0;

// [SECTION TO CHECK] -------------------------------
// #ifdef USE_JG1010_PAD
//   // maybe slow !!!
//   if ( this->digiCross->readUp() ) return 1;
//   if ( this->digiCross->readDown() ) return -1;
//   return 0;
// #else
//             int v = analogRead( Y_AXIS );
//             //Serial.print("y:");Serial.println(v);
//             if ( v <= 800 ) { return -1; }
//             else if ( v >= 2800 ) { return 1; }
//             else { v = 0; }
//             return v;
// #endif
// [SECTION TO CHECK] -------------------------------
        }


        // equiv of Ctrl-C / Break key
        bool getBreakSignal() {
            return readBtn(1) && readBtn(2);
        }

        // equiv of F1 (Up + A + B)
        bool getSystemSignal() {
            return readPadYaxis() > 0 && readBtn(1) && readBtn(2);
        }

        bool getEndSystemSignal() {
            return readPadYaxis() == 0 && !readBtn(1) && !readBtn(2);
        }

        // equiv of 'BYE' cmd (Down + A + B)
        bool getRebootSignal() {
            return readPadYaxis() < 0 && readBtn(1) && readBtn(2);
        }

        // ==== Sound ====

        void tone(int freq, int duration) {
            ledcWrite(BUZ_channel, 125); // volume
            ledcWriteTone(BUZ_channel, freq); // freq
        }

        void noTone() {
          ledcWrite(BUZ_channel, 0); // volume  
        }

        // ===== Leds =======

        void led(bool state) {
            digitalWrite( LED1, state ? HIGH : LOW );
        }

    };

    extern Esp32Pocketv3 esp32;
    static void IRAM_ATTR _doISR() {
        if ( esp32.isISR_LOCKED() ) { return; }

// [SECTION TO CHECK] -------------------------------
        // #ifdef USE_JG1010_PAD
        //     esp32.poll();
        // #endif
// [SECTION TO CHECK] -------------------------------
    }

  #endif


#endif