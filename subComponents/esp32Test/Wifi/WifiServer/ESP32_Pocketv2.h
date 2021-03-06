#if defined(ARDUINO_ARCH_ESP32) and defined(ESP32PCKv2)

 /**************
 * Esp32 Devkit R1 Board w/ External SPI LCD SST7735
 * Xts-uBasic Pocket v2
 * Xtase - fgalliat @ Jan 2018
 ***************/

  #ifndef _ESP32PckV2_H
  #define _ESP32PckV2_H 1

    // #define DBUG_ESP32 1

    
    // need to be defined in TFT_eSPI too (User_Setup.h)
    // for 160x128 Screens
    #define MY_160 1

    #define COLOR_SCREEN 1
    #define COLOR_64K    1

    // use the Lexibook Junior JG1010 Hacked Console cross-pad
    // 3 out / 2 in PINS
    #define USE_JG1010_PAD 1

    // ==============================

    // TFT_eSPI Lib is pretty faster than Adafruit one
    #include <TFT_eSPI.h> // Hardware-specific library

    #ifdef MY_160
      #define ROTATE_SCREEN 2
    #else
      #define ROTATE_SCREEN 2
    #endif

    #define ST7735_BLACK TFT_BLACK
    #define ST7735_WHITE TFT_WHITE
    #define ST7735_CYAN TFT_CYAN
    #define ST7735_BLUE TFT_BLUE
    #define ST7735_YELLOW TFT_YELLOW
    #include <SPI.h>

    /*
      https://github.com/me-no-dev/arduino-esp32fs-plugin
    */
    #include <SPIFFS.h>
    #include <FS.h>

    #define BLACK 0x00
    #define WHITE 0x01

    // =========== GPIO =========

    // -- SPI SCreen
    #define TFT_MISO 19
    #define TFT_MOSI 23
    #define TFT_SCLK 18
    #define TFT_CS    5  // Chip select control pin
    #define TFT_DC    15  // Data Command control pin
    // #define TFT_RST   4  // Reset pin (could connect to RST pin)
    // connected to EN pin
    #define TFT_RST   -1    // 4 is now used as TX2 ('cause pin labeled TX2 doesn't send anything)

    #define TFT_SID TFT_MOSI
    #define TFT_CLK TFT_SCLK

    #define DBL_BUFF_ACTION 1
    #ifdef DBL_BUFF_ACTION

      #define TFT_CLR_BLACK TFT_BLACK
      #define TFT_CLR_WHITE TFT_WHITE


      // type(1) + bounds(4) + color(2), mode(1)
      // 1600Bytes action buffer
      #define DBL_BUFF_ACTION_MAX 200
      #define DBL_BUFF_ACTION_SIZE (1 + 4 + 2 + 1)

      // +8 is arbitrary safety measure (based on 128px screen width)
      // both must be < 256
      #define DBL_BUFF_TEXT_MAX_LEN (21+8)
      #define DBL_BUFF_TEXT_MAX 30


      
      // bpp isn't stored as action because of size of its datas
      #define ACT_CLS    0x01
      #define ACT_PIXEL  0x02
      #define ACT_LINE   0x03 
      #define ACT_CIRCLE 0x04
      #define ACT_RECT   0x05
      #define ACT_TEXT   0x06
      #define ACT_BPP    0x07

      #define ACT_MODE_DRAW 0x00
      #define ACT_MODE_FILL 0x01

    #endif


    // ________________________

    // driven by xtase_fct.h
    #define BTN1 26
    // BEWARE 25 is used as LED on XtsPocket v1
    // Oups mis-wired BT2 & TR-A !!
    // #define BTN2 25
    #define BTN2 32
    #define BTN3 33

    #ifdef USE_JG1010_PAD
     // Oups mis-wired BT2 & TR-A !!
     // #define BTN_TRIGGER_A 32
     #define BTN_TRIGGER_A 25
    #endif

    // the DFPlayer MP3 BUSY LINE
    // BEWARE : INPUT but NOT PULLUP
    #define BTN_TRIGGER_MP3_BUSY 35

    // #define AXIS_INV 1
    #ifndef AXIS_INV
        #define X_AXIS 36
        #define Y_AXIS 39
    #else
        #define X_AXIS 39
        #define Y_AXIS 36
    #endif

    #ifdef USE_JG1010_PAD
      // #define JG1010_PAD_OUT_P1 13 // cause it's SPIMOSI
      // #define JG1010_PAD_OUT_P2 12 // cause it's SPIMISO
      // #define JG1010_PAD_OUT_P4 14 // 'cause it's SPICLK (resets when DIR) 
      #define JG1010_PAD_OUT_P1 19
      #define JG1010_PAD_OUT_P2 21 // SDA condamned ;(
      #define JG1010_PAD_OUT_P4 22 // SCL condamned ;(
      // beware to not wire as INPUT_PULLUP @ same time
    //   #define JG1010_PAD_IN_P3 32
    //   #define JG1010_PAD_IN_P5 33
      #define JG1010_PAD_IN_P3 36
      #define JG1010_PAD_IN_P5 39 // analog pad pins
    #endif


    #define DBUG_PAD(a) { Serial.println(a); }

    //#define LED1 25
    // builtin led
    #define LED1 2

    //#define BUZZER1 12
    #define BUZZER1 27
    #define BUZ_channel 0
    #define BUZ_resolution 8

    #ifdef USE_JG1010_PAD
    class Esp32Pocketv2JG1010DigitalPad {
        private:
          bool left  = false;
          bool right = false;
          bool up    = false;
          bool down  = false;
          bool btnB  = false;
        public:
          Esp32Pocketv2JG1010DigitalPad() {
          }
          ~Esp32Pocketv2JG1010DigitalPad() {
          }
          void initGPIO() {
              pinMode( JG1010_PAD_OUT_P1, OUTPUT );
              pinMode( JG1010_PAD_OUT_P2, OUTPUT );
              pinMode( JG1010_PAD_OUT_P4, OUTPUT );

              // BEWARE : not PULLUP inputs as usual
              pinMode( JG1010_PAD_IN_P3, INPUT );
              pinMode( JG1010_PAD_IN_P5, INPUT );

              digitalWrite( JG1010_PAD_OUT_P1, LOW );
              digitalWrite( JG1010_PAD_OUT_P2, LOW );
              digitalWrite( JG1010_PAD_OUT_P4, LOW );
          }

          void poll() {
              int result = 0;

              /* /!\ BEWARE : delay() in ISR makes 
              *  PLAY5K strange delays
              *  try to use yeld() instead
              */

              // a bit tricky ....
              pinMode( JG1010_PAD_IN_P3, INPUT );
              pinMode( JG1010_PAD_IN_P5, INPUT );
              digitalWrite( JG1010_PAD_OUT_P1, LOW );
              digitalWrite( JG1010_PAD_OUT_P2, LOW );
              digitalWrite( JG1010_PAD_OUT_P4, LOW );
              digitalWrite( JG1010_PAD_IN_P3, LOW );
              digitalWrite( JG1010_PAD_IN_P5, LOW );

              left = false;
              right = false;
              up = false;
              down = false;
              btnB = false;

              #define PAD_CONTACT_3 4070
              #define PAD_CONTACT_5 4070
              #define READ_DELAY 5

              digitalWrite( JG1010_PAD_OUT_P1, HIGH );
              result = analogRead( JG1010_PAD_IN_P3 );
              left = result >= PAD_CONTACT_3;
              result = analogRead( JG1010_PAD_IN_P5 );
              down = result >= PAD_CONTACT_5;
              digitalWrite( JG1010_PAD_OUT_P1, LOW );

              digitalWrite( JG1010_PAD_OUT_P2, HIGH );
              result = analogRead( JG1010_PAD_IN_P3 );
              //Serial.println(result);
              right = result >= PAD_CONTACT_3;
              digitalWrite( JG1010_PAD_OUT_P2, LOW );

              digitalWrite( JG1010_PAD_OUT_P4, HIGH );
              result = analogRead( JG1010_PAD_IN_P3 );
              up = result >= PAD_CONTACT_3;
              // Serial.print(result); Serial.print(" ");
              result = analogRead( JG1010_PAD_IN_P5 );
              btnB = result >= PAD_CONTACT_5;
              digitalWrite( JG1010_PAD_OUT_P4, LOW );
              // Serial.println(result);

            //   if (left) { DBUG_PAD("[LEFT]"); }
            //   if (right) { DBUG_PAD("[RIGHT]"); }
            //   if (up) { DBUG_PAD("[UP]"); }
            //   if (down) { DBUG_PAD("[DOWN]"); }
            //   if (btnB) { DBUG_PAD("[B]"); }

          }

          bool readLeft() {
              return left;
          }
          bool readRight() {
              return right;
          }
          bool readUp() {
              return up;
          }
          bool readDown() {
              return down;
          }
          bool readTriggerB() {
              return btnB;
          }
    };
    #endif


    class Esp32Pocketv2Screen {
        private:
            TFT_eSPI* _oled_display;
            int _oled_ttyY = 0;
            uint16_t drawColor = ST7735_WHITE;

// ______________________________________________
            #ifdef DBL_BUFF_ACTION

      uint8_t bppBuff[1024];

      uint8_t scrActionBuff[ DBL_BUFF_ACTION_SIZE * DBL_BUFF_ACTION_MAX ];
      int scrActionCursor = 0;

      uint8_t scrTextBuff[ DBL_BUFF_TEXT_MAX_LEN * DBL_BUFF_TEXT_MAX ];
      // beware to not overflow it .....
      // later : add a % op to prevent it .... 
      int scrTextCursor = 0;


      //void _scr_blitt(TFT_eSPI* _oled_display);

      // DRAWING ACTIONS FIFO

      // BEWARE : for lines   w => x2 // h => y2
      //              circles w => radius
      //              text    w => scrTextCursor // h => strlen()
      void _scr_pushScreenAction(TFT_eSPI* _oled_display, uint8_t type, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color, uint8_t mode ) {
          if ( scrActionCursor >= DBL_BUFF_ACTION_MAX ) { _scr_blitt(_oled_display); }
          int addr = scrActionCursor * DBL_BUFF_ACTION_SIZE;
          scrActionBuff[ addr+0 ] = type;

          scrActionBuff[ addr+1 ] = x;
          scrActionBuff[ addr+2 ] = y;
          scrActionBuff[ addr+3 ] = w;
          scrActionBuff[ addr+4 ] = h;

          scrActionBuff[ addr+5 ] = color / 256;
          scrActionBuff[ addr+6 ] = color % 256;

          scrActionBuff[ addr+7 ] = mode;

          scrActionCursor++;
      }

      void _scr_pushScreenText(TFT_eSPI* _oled_display, uint8_t x, uint8_t y, char* str, uint16_t color ) {
          int txtAddr = scrTextCursor * DBL_BUFF_TEXT_MAX_LEN;

          int len = strlen( str );
          if ( len >= DBL_BUFF_TEXT_MAX_LEN ) { len = DBL_BUFF_TEXT_MAX_LEN; }
          int i=0;
          for(; i < len; i++ ) {
            scrTextBuff[txtAddr+i] = str[i];
          }
          for(; i < DBL_BUFF_TEXT_MAX_LEN; i++ ) {
              scrTextBuff[txtAddr+i] = 0x00;
          }

          _scr_pushScreenAction( _oled_display, ACT_TEXT, x, y, scrTextCursor, len, color, 0 );
          scrTextCursor++;
      }

      void _doScreenAction( TFT_eSPI* _oled_display, int actionCursor ) {
          if (actionCursor < 0) { actionCursor = 0; }
          int addr = (actionCursor) * DBL_BUFF_ACTION_SIZE;

          int type = scrActionBuff[ addr+0 ];

          int x    = scrActionBuff[ addr+1 ];
          int y    = scrActionBuff[ addr+2 ];
          int w    = scrActionBuff[ addr+3 ];
          int h    = scrActionBuff[ addr+4 ];

          uint16_t color = (uint16_t)((uint16_t)scrActionBuff[ addr+5 ] * 256 ) + scrActionBuff[ addr+6 ];

          int mode = scrActionBuff[ addr+7 ];

          if ( type == ACT_CLS ) { 
            //_oled_ttyY = 0;
            _oled_display->fillScreen( TFT_BLACK );
          }
          else if ( type == ACT_PIXEL ) { _oled_display->drawPixel( x, y, color ); }
          else if ( type == ACT_LINE ) { 
              // !!! lines   w => x2 // h => y2
              _oled_display->drawLine( x, y, w, h, color ); }
          else if ( type == ACT_CIRCLE ) { 
              // !!! circles w => radius
              if ( mode == ACT_MODE_DRAW ) _oled_display->drawCircle( x, y, w, color ); 
              else                         _oled_display->fillCircle( x, y, w, color ); 
          }
          else if ( type == ACT_RECT ) { 
              if ( mode == ACT_MODE_DRAW ) _oled_display->drawRect( x, y, w, h, color ); 
              else                         _oled_display->fillRect( x, y, w, h, color ); 
          } 
          else if ( type == ACT_TEXT ) {
              char str[ h+1 ];
              int txtAddr = w * DBL_BUFF_TEXT_MAX_LEN;
              for(int i=0; i < h; i++) { str[i] = scrTextBuff[txtAddr+i]; }
              str[h]=0;

              //_oled_display->setTextColor(color);
              _oled_display->setCursor(x, y);
              _oled_display->print(str);
          }
          else if ( type == ACT_BPP ) {
              unsigned char c;
                _oled_display->fillScreen( TFT_CLR_BLACK );
                int width = 128;
                int height = 64;
                for (int yy = 0; yy < height; yy++) {
                    for (int xx = 0; xx < width; xx++) {
                        c = (bppBuff[(yy * (width / 8)) + (xx / 8)] >> (7 - ((xx) % 8))) % 2;
                        if (c == 0x01) {
                            _oled_display->drawPixel(x + xx, y + yy, TFT_CLR_WHITE);
                        }
                    }
                }
          }

      }

      void _scr_blitt(TFT_eSPI* _oled_display) {
          int len = scrActionCursor;
          scrActionCursor = 0;
          for(int i=0; i < len; i++ ) {
              _doScreenAction( _oled_display, i );
          }
          scrActionCursor = 0;
          scrTextCursor = 0;
      }

            #endif

        public:
            Esp32Pocketv2Screen() {
            }
            ~Esp32Pocketv2Screen() {}

            void init() {
                _oled_display = new TFT_eSPI();
                _oled_display->init();

                _oled_display->setRotation(1+ROTATE_SCREEN); // LANDSCAPE

                _oled_display->fillScreen(ST7735_BLACK);

                _oled_display->setTextColor(drawColor);
                _oled_display->setTextSize(0);
            }

            void blitt() {
                #ifdef DBL_BUFF_ACTION
                  _scr_blitt( _oled_display );
                #endif
            }

            void drawString(int x, int y, char* str) {
                #ifdef DBL_BUFF_ACTION
                  _scr_pushScreenText(_oled_display, x, y, str, TFT_CLR_WHITE);
                #else
                _oled_display->setCursor(x, y);
                _oled_display->print(str);
                #endif

                #ifdef DBUG_ESP32
                  Serial.print( str );
                #endif
            }

            void println(char* str) {
                this->drawString(0, this->_oled_ttyY*8, str);
                this->_oled_ttyY++;

                if ( this->_oled_ttyY >= 8 ) {
                    // auto scroll
                    this->_oled_ttyY = 8;
                }
            }

            void drawLine(int x1, int y1, int x2, int y2) {
                #ifdef DBL_BUFF_ACTION
                 _scr_pushScreenAction(_oled_display, ACT_LINE,x1, y1, x2, y2, drawColor, ACT_MODE_DRAW);
                #else
                _oled_display->drawLine(x1,y1,x2,y2, drawColor);
                #endif
            }

            void drawCircle(int x1, int y1, int r) {
                #ifdef DBL_BUFF_ACTION
                 _scr_pushScreenAction(_oled_display, ACT_CIRCLE,x1, y1, r, 0x00, drawColor, ACT_MODE_DRAW);
                #else
                 _oled_display->drawCircle(x1,y1,r, drawColor);
                #endif
            }

            void setColor(int color) {
                //_oled_display->setColor(color==1?WHITE:BLACK);
                drawColor = color==1 ? ST7735_WHITE : ST7735_BLACK;
                // _oled_display->setTextColor( drawColor );
            }

            void setPixel(int x,int y) {
                #ifdef DBL_BUFF_ACTION
                 _scr_pushScreenAction(_oled_display, ACT_PIXEL,x, y, 0x00, 0x00, drawColor, ACT_MODE_DRAW);
                #else
                 _oled_display->drawPixel(x,y, drawColor);
                #endif
            }


            void drawImg(int x,int y, int width, int height, unsigned char* picBuff) {

                #ifdef DBL_BUFF_ACTION
                 memcpy(bppBuff, picBuff, 1024);
                 _scr_pushScreenAction(_oled_display, ACT_BPP,0x00, 0x00, 0x00, 0x00, TFT_CLR_WHITE, ACT_MODE_FILL);
                 return;
                #endif

                unsigned char c;
                _oled_display->fillScreen( ST7735_BLACK );

                for (int yy = 0; yy < height; yy++) {
                    for (int xx = 0; xx < width; xx++) {
                        c = (picBuff[(yy * (width / 8)) + (xx / 8)] >> (7 - ((xx) % 8))) % 2;
                        if (c == 0x00) {
                            // //setColor(BLACK);
                            // //setPixel(x + xx, y + yy);
                            // _oled_display->drawPixel(x + xx, y + yy, ST7735_BLACK);
                        }
                        else {
                            // setColor(WHITE);
                            // setPixel(x + xx, y + yy);
                            _oled_display->drawPixel(x + xx, y + yy, ST7735_WHITE);
                        }
                    }
                }
            }

            // but supports gray
            void drawRect(int x,int y, int width, int height, int color, int mode) {
                unsigned char c; int cpt = 0;

                // if ( color == 0 ) { setColor(BLACK); }
                // else if ( color == 1 ) { setColor(WHITE); }

    uint16_t _color = ST7735_BLACK;
    if ( color == 1 ) { _color = ST7735_WHITE; }
    else if ( color == 2 ) { _color = ST7735_CYAN; } // light grayshade
    else if ( color == 3 ) { _color = ST7735_BLUE; } // dark grayshade

    else if ( color == 4 ) { _color = ST7735_YELLOW; } // very dark grayshade

#ifdef DBL_BUFF_ACTION
  _scr_pushScreenAction(_oled_display, ACT_RECT,x, y, width, height, _color, mode == 0 ? ACT_MODE_DRAW : ACT_MODE_FILL);
#else
  if ( mode == 0 ) {
    _oled_display->drawRect(x,y,width,height, _color);
  } else {
    _oled_display->fillRect(x,y,width,height, _color);
  }
#endif

/*
                for (int yy = 0; yy < height; yy++) {
                    for (int xx = 0; xx < width; xx++) {
                        
                        if ( mode == 0 && !(xx == 0 || xx == width-1) ) {
                            if ( !(yy == 0 || yy == height-1) ) {
                                // just draw
                                continue;
                            }
                        }

                        if ( color >= 2 ) { 
                            c = cpt % color; 
                            if ( c == 1 ) setColor(WHITE);
                            else          setColor(BLACK);
                        }

                        //setPixel(x + xx, y + yy);
                        _oled_display->drawPixel(x + xx, y + yy, drawColor);
                        cpt++;
                    }
                }
*/
            }

            void clear() {
                

#ifdef DBL_BUFF_ACTION
  _oled_ttyY = 0; // BEWARE W/ACTTION BUFFER
  _scr_pushScreenAction(_oled_display, ACT_CLS,0, 0, 0, 0, ST7735_BLACK, ACT_MODE_FILL);
#else
                  _oled_ttyY = 0;
                  _oled_display->fillScreen( ST7735_BLACK );
#endif
            }
    };

    class Esp32Pocketv2Fs {
        private:
          File currentFile;
          bool currentFileValid = false;
        public:
          Esp32Pocketv2Fs() {}
          ~Esp32Pocketv2Fs() {}

          void init() {
            SPIFFS.begin();
            //bool ff = SPIFFS.format();
          }

          // filename = "/toto.txt"
          // returns nb of bytes readed
          int readBinFile(char* filename, uint8_t* dest, int maxLen) {
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
            File f = SPIFFS.open(filename, "r");
            if ( !f ) { return -1; }
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
            Serial.println("opening");
            this->currentFileValid = false;
            if ( !SPIFFS.exists(filename) ) { return false; }
            this->currentFile = SPIFFS.open(filename, readMode ? "r" : "w");
            if ( !this->currentFile ) { Serial.println("failed"); return false; }
            this->currentFileValid = true;
            Serial.println("opened");
            return true;
          }

          void closeCurrentTextFile() {
              Serial.println("closing");
            if ( !this->currentFileValid ) { return; }
            this->currentFile.close();
            this->currentFileValid = false;
            Serial.println("closed");
          }

          // slow impl !!!!!!!
          char* readCurrentTextLine() {
            //   Serial.println("readLine.1");

            // TODO see for single line instance allocation !!!!!!!
            int MAX_LINE_LEN = 128; // TO CHECK
            char* line = (char*)malloc( MAX_LINE_LEN+1 );
            memset(line, 0x00, MAX_LINE_LEN +1);

            // Serial.println("readLine.2");

            int cpt = 0;
            for( int i=0; i < MAX_LINE_LEN; i++ ) {

                if ( !this->currentFile.available() ) {
                    break;
                }

                char ch = (char)this->currentFile.read();
                if ( ch == -1 )   { break; }
                if ( ch == '\r' ) { continue; }
                if ( ch == '\n' ) { break; }

                line[cpt++] = ch;
                //Serial.println("readLine.3");
            }

// Serial.println("readLine.4");
            return line;
          }

          // ================================

          // listDir("/") -> returns nb of file
          int listDir(char* dirName, void (*callback)(char*) ) {
            File dir = SPIFFS.open("/");
            File entry;
            int entryNb = 0;
            while( (entry = dir.openNextFile() ) ) {
                if ( entry.isDirectory() ) { continue; }
                callback( (char*)entry.name() );
                entryNb++;
            }
            return entryNb;
          }
    };

    // dirty ...
    static void IRAM_ATTR _doISR();

    class Esp32Pocketv2 {
        private:
          Esp32Pocketv2Screen* screen = new Esp32Pocketv2Screen();
          Esp32Pocketv2Fs* fs = new Esp32Pocketv2Fs();
          #ifdef USE_JG1010_PAD
           Esp32Pocketv2JG1010DigitalPad* digiCross = new Esp32Pocketv2JG1010DigitalPad();
          #endif
        public:
        Esp32Pocketv2() {
        }

        ~Esp32Pocketv2() {
        }

        void setup() {
            this->initGPIO();
            this->initBuzzer();
            this->initLCD();
            this->initFS();
            #ifdef USE_JG1010_PAD
             this->digiCross->initGPIO();
            #endif
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
            #ifdef USE_JG1010_PAD
              this->digiCross->poll();
            #endif
        }

        // === FileSystem ===
        void initFS() {
            fs->init();
        }

        Esp32Pocketv2Fs* getFs() {
            return fs;
        }

        // ==================
        // ===== Screen =====

        void initLCD() {
            screen->init();
        }

        Esp32Pocketv2Screen* getScreen() {
            return screen;
        }

        // ==================
        // ====== GPIO ======

        void initGPIO() {
            pinMode( BTN1, INPUT_PULLUP );
            pinMode( BTN2, INPUT_PULLUP );
            pinMode( BTN3, INPUT_PULLUP );

#ifdef USE_JG1010_PAD
            pinMode( BTN_TRIGGER_A, INPUT_PULLUP );
#else
            pinMode( X_AXIS, INPUT );
            pinMode( Y_AXIS, INPUT );
#endif

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
            #ifdef USE_JG1010_PAD
            else if (btnID == 8 && BTN_TRIGGER_A > -1) {
                return digitalRead( BTN_TRIGGER_A ) == LOW ? 1 : 0;
            }
            else if (btnID == 9) {
                return this->digiCross->readTriggerB() ? 1 : 0;
            }
            #endif

            else if (btnID == 10 && BTN_TRIGGER_MP3_BUSY > -1) {
                int v = digitalRead( BTN_TRIGGER_MP3_BUSY );
                // Serial.print("BUSY ");Serial.println(v);
                return v == 0 ? 1 : 0;
            }

            return 0;
        }

        int readPadXaxis() {
#ifdef USE_JG1010_PAD
  // maybe slow !!!
  if ( this->digiCross->readLeft() ) return -1;
  if ( this->digiCross->readRight() ) return  1;
  return 0;
// #elif defined(BTN_LEFT) and defined(BTN_RIGHT)
//   // digital joypad
//   if ( digitalRead( BTN_LEFT ) == LOW ) return -1;
//   if ( digitalRead( BTN_RIGHT ) == LOW ) return 1;
#else

            int v = analogRead( X_AXIS );
            // Serial.print("x:");Serial.println(v);
            if ( v <= 800 ) { return -1; }
            else if ( v >= 2800 ) { return 1; }
            else { v = 0; }

            return v;
#endif
        }

        int readPadYaxis() {
#ifdef USE_JG1010_PAD
  // maybe slow !!!
  if ( this->digiCross->readUp() ) return 1;
  if ( this->digiCross->readDown() ) return -1;
  return 0;
#else
            int v = analogRead( Y_AXIS );
            //Serial.print("y:");Serial.println(v);
            if ( v <= 800 ) { return -1; }
            else if ( v >= 2800 ) { return 1; }
            else { v = 0; }
            return v;
#endif
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

            //delay(duration);
            //ledcWrite(BUZ_channel, 0); // volume    
        }

        void noTone() {
          ledcWrite(BUZ_channel, 0); // volume  
        }

        // ===== Leds =======

        void led(bool state) {
            digitalWrite( LED1, state ? HIGH : LOW );
        }

    };

    extern Esp32Pocketv2 esp32;
    static void IRAM_ATTR _doISR() {
        #ifdef USE_JG1010_PAD
            esp32.poll();
        #endif
    }

  #endif


#endif