#if defined(ARDUINO_ARCH_ESP32) and defined(ESP32PCKv2)

 /**************
 * Esp32 Devkit R1 Board w/ External SPI LCD SST7735
 * Xts-uBasic Pocket v2
 * Xtase - fgalliat @ Jan 2018
 ***************/

  #ifndef _ESP32PckV2_H
  #define _ESP32PckV2_H 1

    #define DBUG_ESP32 1

    #include <Adafruit_GFX.h>    // Core graphics library
    #include "dev_screen_Adafruit_ST7735.h"
    #include <SPI.h>

    /*
      https://github.com/me-no-dev/arduino-esp32fs-plugin
    */
    #include <SPIFFS.h>
    #include <FS.h>

    #define BLACK 0x00
    #define WHITE 0x01

    // =========== GPIO =========

    // TODO : check these Screen PINS !!

    // For the breakout, you can use any 2 or 3 pins
    // These pins will also work for the 1.8" TFT shield
    #define TFT_CS     10
    #define TFT_RST    9  // you can also connect this to the Arduino reset
                          // in which case, set this #define pin to -1!
    #define TFT_DC 8
    // ________________________

    #define BTN1 16
    #define BTN2 14

    // #define AXIS_INV 1
    #ifndef AXIS_INV
        #define X_AXIS 39
        #define Y_AXIS 36
    #else
        #define X_AXIS 36
        #define Y_AXIS 39
    #endif

    //#define LED1 25
    // builtin led
    #define LED1 2

    #define BUZZER1 12
    #define BUZ_channel 0
    #define BUZ_resolution 8

    // NEVER DO that mistake !!!!!
    // static SSD1306 _oled_display(0x3C, 5, 4);
    // static int _oled_ttyY=0;


    class Esp32Pocketv2Screen {
        private:
            Adafruit_ST7735* _oled_display;
            int _oled_ttyY = 0;
            uint16_t drawColor = ST7735_WHITE;
        public:
            Esp32Pocketv2Screen() {
            }
            ~Esp32Pocketv2Screen() {}

            void init() {
                // TODO : check for MOSI 11 / CLK 13 pins.....
                _oled_display = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

                // Use this initializer if you're using a 1.8" TFT
                _oled_display->initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

                // Use this initializer (uncomment) if you're using a 1.44" TFT
                //_oled_display->initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab

                _oled_display->fillScreen(ST7735_BLACK);

                _oled_display->setTextColor(drawColor);
                _oled_display->setTextSize(1);
            }

            void blitt() {
                // no dbl buff ??
                // _oled_display->display();
            }

            void drawString(int x, int y, char* str) {
                _oled_display->setCursor(x, y);
                _oled_display->print(str);

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
                _oled_display->drawLine(x1,y1,x2,y2, drawColor);
            }

            void drawCircle(int x1, int y1, int r) {
                _oled_display->drawCircle(x1,y1,r, drawColor);
            }

            void setColor(int color) {
                //_oled_display->setColor(color==1?WHITE:BLACK);
                drawColor = color==1 ? ST7735_WHITE : ST7735_BLACK;
                _oled_display->setTextColor( drawColor );
            }

            void setPixel(int x,int y) {
                _oled_display->drawPixel(x,y, drawColor);
            }

            void drawImg(int x,int y, int width, int height, unsigned char* picBuff) {
                unsigned char c;
                for (int yy = 0; yy < height; yy++) {
                    for (int xx = 0; xx < width; xx++) {
                        c = (picBuff[(yy * (width / 8)) + (xx / 8)] >> (7 - ((xx) % 8))) % 2;
                        if (c == 0x00) {
                            setColor(BLACK);
                            setPixel(x + xx, y + yy);
                        }
                        else {
                            setColor(WHITE);
                            setPixel(x + xx, y + yy);
                        }
                    }
                }
            }

            // slow impl.
            // but supports gray
            void drawRect(int x,int y, int width, int height, int color, int mode) {
                unsigned char c; int cpt = 0;

                if ( color == 0 ) { setColor(BLACK); }
                else if ( color == 1 ) { setColor(WHITE); }

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

                        setPixel(x + xx, y + yy);
                        cpt++;
                    }
                }
            }

            void clear() {
                _oled_ttyY = 0;
                _oled_display->fillScreen( ST7735_BLACK );
            }
    };

    class Esp32Pocketv2Fs {
        private:
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

    class Esp32Pocketv2 {
        private:
          Esp32Pocketv2Screen* screen = new Esp32Pocketv2Screen();
          Esp32Pocketv2Fs* fs = new Esp32Pocketv2Fs();
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

            pinMode( X_AXIS, INPUT );
            pinMode( Y_AXIS, INPUT );

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

        int readBtns() {
            return readBtn(0);
        }

        int readBtn(int btnID) {
            // TODO : include pad state as btn
            if ( btnID == 0 ) {
                // TODO : better canonicalize
                bool b1 = BTN1 > -1 && digitalRead( BTN1 ) == LOW;
                bool b2 = BTN2 > -1 &&digitalRead( BTN2 ) == LOW;
                return (b1 ? 1 : 0) + (b2 ? 2 : 0);
            } else if (btnID == 1 && BTN1 > -1) {
                return digitalRead( BTN1 ) == LOW ? 1 : 0;
            } else if (btnID == 2 && BTN2 > -1) {
                return digitalRead( BTN2 ) == LOW ? 1 : 0;
            }
            return 0;
        }

        int readPadXaxis() {
            int v = analogRead( X_AXIS );
            // Serial.print("x:");Serial.println(v);
            if ( v <= 800 ) { return 1; }
            // VALUES CHANGED since use LIPO Battery cell
            // else if ( v >= 2800 ) { return -1; }
            else if ( v >= 2200 ) { return -1; }
            else { v = 0; }
            return v;
        }

        int readPadYaxis() {
            // BEWARE w/ LiPo battery cell voltage...
            int v = analogRead( Y_AXIS );
            //Serial.print("y:");Serial.println(v);
            if ( v <= 800 ) { return -1; }
            // VALUES CHANGED since use LIPO Battery cell
            // else if ( v >= 2800 ) { return 1; }
            else if ( v >= 2200 ) { return 1; }
            else { v = 0; }
            return v;
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

  #endif


#endif