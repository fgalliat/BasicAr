#if defined(ARDUINO_ARCH_ESP32) and defined(ESP32PCKv2)

 /**************
 * Esp32 Devkit R1 Board w/ External SPI LCD SST7735
 * Xts-uBasic Pocket v2
 * Xtase - fgalliat @ Jan 2018
 ***************/

  #ifndef _ESP32PckV2_H
  #define _ESP32PckV2_H 1

    // #define DBUG_ESP32 1

#ifdef COLOR_64K
extern unsigned short color_picturebuff[];
#endif
    
    // need to be defined in TFT_eSPI too (User_Setup.h)
    // for 160x128 Screens
    #define MY_160 1

    #define COLOR_SCREEN 1

    // use the Lexibook Junior JG1010 Hacked Console cross-pad
    // 3 out / 2 in PINS
    #define USE_JG1010_PAD 1

    // ==============================

    // TFT_eSPI Lib is pretty faster than Adafruit one
    #include <TFT_eSPI.h> // Hardware-specific library

    #ifdef MY_160
      #define ROTATE_SCREEN 2
      #define TFT_WIDTH 160
      #define TFT_HEIGHT 128 
    #else
      #define ROTATE_SCREEN 2
      #define TFT_WIDTH 128
      #define TFT_HEIGHT 128
    #endif

    #define ST7735_BLACK TFT_BLACK
    #define ST7735_WHITE TFT_WHITE
    #define ST7735_CYAN TFT_CYAN
    #define ST7735_BLUE TFT_BLUE
    #define ST7735_YELLOW TFT_YELLOW

    // color565(uint8_t r, uint8_t g, uint8_t b)
    /*
    uint16_t TFT_eSPI::color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}



** Function name:           pushImage
** Description:             plot 16 bit colour sprite or image onto TFT
void TFT_eSPI::pushImage(int32_t x, int32_t y, uint32_t w, uint32_t h, uint16_t *data)

    */

    #define CLR_BLACK       TFT_BLACK
    #define CLR_WHITE       TFT_WHITE
    #define CLR_LIGHTGREY   TFT_LIGHTGREY
    #define CLR_GREY        0x8410 /* 128,128,128 */
    #define CLR_DARKGREY    TFT_DARKGREY

    #include <SPI.h>

    /*
      https://github.com/me-no-dev/arduino-esp32fs-plugin
    */
    #include <SPIFFS.h>
    #include <FS.h>

    #define BLACK 0x00
    #define WHITE 0x01

    // =========== GPIO =========

    // -- SPI SCreen - values to copy in TFT_eSPI/User_Select.h
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
      #define ACT_PCT    0x08

      #define ACT_MODE_DRAW 0x00
      #define ACT_MODE_FILL 0x01

      #define ACT_MODE_PCT_64K 0x01

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

            uint16_t screenOffsetX = 0; // MY_160
            uint16_t screenOffsetY = 0;

// ______________________________________________
            #ifdef DBL_BUFF_ACTION

      uint8_t bppBuff[1024]; // (for 128x64 1bpp)

      #ifdef COLOR_64K
       // BEWARE : this is uint16_t -> 40960 bytes (for 160x128)
       //uint16_t pct64KBuff[ TFT_WIDTH * TFT_HEIGHT ];
       uint16_t* pct64KBuff = NULL;
      #endif

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

        char _myStr[256+1];

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
          else if ( type == ACT_PIXEL ) { _oled_display->drawPixel( screenOffsetX+x, screenOffsetY+y, color ); }
          else if ( type == ACT_LINE ) { 
              // !!! lines   w => x2 // h => y2
              _oled_display->drawLine( screenOffsetX+x, screenOffsetY+y, screenOffsetX+w, screenOffsetY+h, color ); }
          else if ( type == ACT_CIRCLE ) { 
              // !!! circles w => radius
              if ( mode == ACT_MODE_DRAW ) _oled_display->drawCircle( screenOffsetX+x, screenOffsetY+y, w, color ); 
              else                         _oled_display->fillCircle( screenOffsetX+x, screenOffsetY+y, w, color ); 
          }
          else if ( type == ACT_RECT ) { 
              if ( mode == ACT_MODE_DRAW ) _oled_display->drawRect( screenOffsetX+x, screenOffsetY+y, w, h, color ); 
              else                         _oled_display->fillRect( screenOffsetX+x, screenOffsetY+y, w, h, color ); 
          } 
          else if ( type == ACT_TEXT ) {
              //char str[ h+1 ];
              if ( h > 256 ) { h = 256; }
              memset(_myStr, 0x00, h+1);
              int txtAddr = w * DBL_BUFF_TEXT_MAX_LEN;
              for(int i=0; i < h; i++) { _myStr[i] = scrTextBuff[txtAddr+i]; }
              _myStr[h]=0;

              //_oled_display->setTextColor(color);
              _oled_display->setCursor(screenOffsetX+x, screenOffsetY+y);

              #ifdef LCD_LOCKED
                Serial.print(_myStr);
              #else
                // there is a bug w/ that impl....
                //_oled_display->print(_myStr);

                int l = strlen( _myStr );
                for(int i=0; i < l; i++) { 
                    if ( _myStr[i] < ' ') { _myStr[i] = ' '; }
                    else if ( _myStr[i] >= (char)127 ) { _myStr[i] = ' '; } // that made ESP crash by LoadProhibed
                    _oled_display->write( _myStr[i] ); 
                }
              #endif
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
                            _oled_display->drawPixel(screenOffsetX+x + xx, screenOffsetY+y + yy, TFT_CLR_WHITE);
                        }
                    }
                }
          } 
          else if ( type == ACT_PCT ) {
              if ( mode == ACT_MODE_PCT_64K ) {
                #ifdef COLOR_64K
                if ( pct64KBuff != NULL ) {
                    _oled_display->pushImage( x, y, w, h, pct64KBuff);
                }
                #else
                _oled_display->print( "NO COLOR 64K !" ); 
                #endif
              } else {
                _oled_display->print( "PCT MODE NOT HANDLED !" ); 
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
                #ifdef MY_160
                  screenOffsetX = (160 - 128) / 2;
                #else
                  screenOffsetX = (160 - 128) / 2;
                #endif

                screenOffsetY = (128 - 64) / 2;
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
                    mode = ACT_MODE_PCT_64K;
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
                _oled_display->pushRect(x, y+yy, w, 128/8, color_picturebuff);
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


            // fails !!!!!!!
            void drawPct(int x, int y, unsigned char* picFileBuff) {


// not using actionBuff !!!!

                 int mode = -1, w=-1, h=-1;
Serial.println("A.1");
                if ( picFileBuff[0] == '6' && picFileBuff[1] == '4' && picFileBuff[2] == 'K' ) {
                    mode = ACT_MODE_PCT_64K;
                    w = ((int)picFileBuff[3]*256) + ((int)picFileBuff[4]);
                    h = ((int)picFileBuff[5]*256) + ((int)picFileBuff[6]);
                }

Serial.println("A.2 "); Serial.print(w); Serial.print('x');Serial.print(h);Serial.println("");

int hh = 4;

uint16_t picture[ w * hh ];
Serial.println("A.2 bis");

for(int i=0; i < (w*h*2); i++) {
    picFileBuff[i] = picFileBuff[i+7];
}

Serial.println("A.2 ter");

for(int i=0; i < (w*hh); i++) {
    picture[i] = ((uint16_t)picFileBuff[(i*2)+0]) + ((uint16_t)picFileBuff[(i*2)+1]);
}

Serial.println("A.3");
//_oled_display->pushImage(x, y, w, h, (uint16_t*) picFileBuff);
//_oled_display->pushImage(x, y, w, hh, picture);
_oled_display->pushRect(x, y, w, hh, picture);
Serial.println("A.4");


//                 #ifdef DBL_BUFF_ACTION
//                 #ifdef COLOR_64K
//                  //memcpy(bppBuff, picBuff, 1024);

// #define REDUCE_MOD 1
// /*
//                  if ( pct64KBuff == NULL ) {
//                      Serial.println("INIT OF PCT64KBUFF"); Serial.flush();
//                      pct64KBuff = ( uint16_t* )malloc( sizeof(uint16_t*) * TFT_WIDTH * TFT_HEIGHT / REDUCE_MOD );
//                      Serial.println("DONE"); Serial.flush();
//                  } else {
//                      Serial.println("PCT64KBUFF AREADY INIT !"); Serial.flush();
//                  }
// */                 

// pct64KBuff = (uint16_t*)picFileBuff;

// Serial.println("A.3");
//                  for(int i=0; i < w *h/REDUCE_MOD; i++) { 
//                      pct64KBuff[i] = ((uint16_t)picFileBuff[7+(i*2)+0]*256)+picFileBuff[7+(i*2)+1]; 
//                  }
// Serial.println("A.4");
//                  // BEWARE if img is smaller than buff array
//                  _scr_pushScreenAction(_oled_display, ACT_PCT, x, y, (uint8_t)w, (uint8_t)h/REDUCE_MOD, TFT_CLR_WHITE, mode);
// Serial.println("A.5");
//                  return;
//                 #endif
//                 #endif
            }

            // but supports gray
            void drawRect(int x,int y, int width, int height, int color, int mode) {
                unsigned char c; int cpt = 0;

                // if ( color == 0 ) { setColor(BLACK); }
                // else if ( color == 1 ) { setColor(WHITE); }

    uint16_t _color = ST7735_BLACK;
    if ( color == 1 ) { _color = ST7735_WHITE; }
    else if ( color == 2 ) { _color = CLR_DARKGREY; }
    else if ( color == 3 ) { _color = CLR_GREY; }
    else if ( color == 4 ) { _color = CLR_LIGHTGREY; }

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
        //   File* currentFile = NULL;
          File currentFile;
          //int currentFileSeek = 0;
          bool currentFileValid = false;
        public:
          Esp32Pocketv2Fs() {}
          ~Esp32Pocketv2Fs() {}

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
            //   this->currentFile->write( (uint8_t*)line, len );
            //   this->currentFile->flush();
              currentFile.write( (uint8_t*)line, len );
              currentFile.flush();
          }

          // slow impl !!!!!!!
          char __myLine[256+1];

#ifndef min
 #define min(a,b) (a < b ? a : b)
#endif

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

    class Esp32Pocketv2 {
        private:
          Esp32Pocketv2Screen* screen = new Esp32Pocketv2Screen();
          Esp32Pocketv2Fs* fs = new Esp32Pocketv2Fs();
          #ifdef USE_JG1010_PAD
           Esp32Pocketv2JG1010DigitalPad* digiCross = new Esp32Pocketv2JG1010DigitalPad();
          #endif
          bool _isISR_LOCKED = false;

        public:

        Esp32Pocketv2() {
        }

        ~Esp32Pocketv2() {
        }

        bool isISR_LOCKED() { return _isISR_LOCKED; }

        void lockISR() { _isISR_LOCKED = true; }
        void unlockISR() { _isISR_LOCKED = false; }

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
        if ( esp32.isISR_LOCKED() ) { return; }
        #ifdef USE_JG1010_PAD
            esp32.poll();
        #endif
    }

  #endif


#endif