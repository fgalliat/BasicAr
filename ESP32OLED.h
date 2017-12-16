#ifdef ARDUINO_ARCH_ESP32

 /**************
 * Esp32 w/ OLED (makerhawk version)
 * Wemos LOLIN32 module
 *
 * Xtase - fgalliat @ Dec 2017
 ***************/

  #ifndef _ESP32OLED_H
  #define _ESP32OLED_H 1


    #include <SSD1306.h>

    /*
      https://github.com/me-no-dev/arduino-esp32fs-plugin
    */
    #include <SPIFFS.h>
    #include <FS.h>

    // =========== GPIO =========

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

    #define LED1 25

    #define BUZZER1 12
    #define BUZ_channel 0
    #define BUZ_resolution 8

    // NEVER DO that mistake !!!!!
    // static SSD1306 _oled_display(0x3C, 5, 4);
    // static int _oled_ttyY=0;


    class Esp32OledScreen {
        private:
            //SSD1306 _oled_display(0x3C, 5, 4);
            SSD1306* _oled_display;
            int _oled_ttyY = 0;
        public:
            Esp32OledScreen() {
            }
            ~Esp32OledScreen() {}

            void init() {
                _oled_display = new SSD1306(0x3C, 5, 4);
                _oled_display->init();
                _oled_display->flipScreenVertically();
                _oled_display->setContrast(255);
                _oled_display->clear();
            }

            void blitt() {
                _oled_display->display();
            }

            void drawString(int x, int y, char* str) {
                _oled_display->drawString(x,y,str);
            }

            void println(char* str) {
                //display.println("Hardware test");
                //display.drawLogBuffer(0, 0);

                this->drawString(0, _oled_ttyY*8, str);
                _oled_ttyY++;

                if ( _oled_ttyY >= 8 ) {
                    // auto scroll
                    _oled_ttyY = 8;
                }
            }

            void drawLine(int x1, int y1, int x2, int y2) {
                _oled_display->drawLine(x1,y1,x2,y2);
            }

            void drawCircle(int x1, int y1, int r) {
                _oled_display->drawCircle(x1,y1,r);
            }

            void setColor(int color) {
                _oled_display->setColor(color==1?WHITE:BLACK);
            }

            void setPixel(int x,int y) {
                _oled_display->setPixel(x,y);
            }

            void drawImg(int x,int y, int w, int h, unsigned char* img) {
                _oled_display->drawFastImage(x,y, w, h, (const char*)img);
            }

            void clear() {
                _oled_ttyY = 0;
                _oled_display->clear();
            }
    };

    class Esp32Oled {
        private:
          Esp32OledScreen screen;
        public:
        Esp32Oled() {
        }

        ~Esp32Oled() {
        }

        void setup() {
            this->initGPIO();
            this->initBuzzer();
            this->initLCD();
            this->initFS();
        }
        // === FileSystem ===
        void initFS() {
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
            f.close();
            return lineNb;
        }

        // ==================
        // ===== Screen =====

        void initLCD() {
            screen.init();
        }

        Esp32OledScreen getScreen() {
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

        void tone(int freq, int duration) {
            ledcWrite(BUZ_channel, 125); // volume
            ledcWriteTone(BUZ_channel, freq); // freq

            //delay(duration);
            //ledcWrite(BUZ_channel, 0); // volume    
        }

        void noTone() {
          ledcWrite(BUZ_channel, 0); // volume  
        }

        void led(bool state) {
            digitalWrite( LED1, state ? HIGH : LOW );
        }

        int readPadXaxis() {
            int v = analogRead( X_AXIS );
            if ( v <= 800 ) { return 1; }
            else if ( v >= 2800 ) { return -1; }
            else { v = 0; }
            return v;
        }

        int readPadYaxis() {
            int v = analogRead( Y_AXIS );
            if ( v <= 800 ) { return -1; }
            else if ( v >= 2800 ) { return 1; }
            else { v = 0; }
            return v;
        }

    };

    // static Esp32Oled esp32;
    // static void noTone(int pin) { esp32.noTone(); }
    // static void tone(int pin, int freq, int duration) { esp32.tone(freq,duration); }

  #endif





#endif