/***************************
* Host.cpp
*
* Xtase - fgalliat @ Sept2017
* Original Code by Robin Edwards @2014
***************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <limits.h>

bool LOCAL_ECHO = true;
extern bool MODE_EDITOR;
extern void host_system_menu();

#ifndef COMPUTER
  #include <Arduino.h>
#else
  #include "computer.h"
#endif

  #define PS2_DELETE 8
  #define PS2_ENTER 13
  #define PS2_ESC 27

#define ANOTHER_CPP 1
#include "xts_arch.h"

#include "xts_io.h"
int OUTPUT_DEVICE;
int GFX_DEVICE;
int INPUT_DEVICE;


extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern char* line; // one TTY line
extern char* screenBuffer;
extern char* lineDirty;
extern bool SCREEN_LOCKER;

#include "host.h"
#include "basic.h"

extern bool isWriting;
int BLITT_MODE = BLITT_AUTO;
bool isGfxAutoBlitt() { return BLITT_MODE == BLITT_AUTO; }

// for dtostre & dtostrf
#include "xts_compat.h" 
#include <stdlib.h> 

extern bool selfRun;
extern bool BUZZER_MUTE;

int timer1_counter;

int curX = 0, curY = 0;

// because of ncurses
//volatile char flash = 0, redraw = 0;
volatile char _flash = 0, redraw = 0;

char inputMode = 0;
char inkeyChar = 0;
char buzPin = 0;

//const char bytesFreeStr[] PROGMEM = "bytes free";
const char bytesFreeStr[] = "bytes free";

void initTimer() {
// #ifdef BUT_TEENSY
//  // see : https://www.pjrc.com/teensy/td_timing_IntervalTimer.html
// #else
//     noInterrupts();           // disable all interrupts
//     TCCR1A = 0;
//     TCCR1B = 0;
//     timer1_counter = 34286;   // preload timer 65536-16MHz/256/2Hz
//     TCNT1 = timer1_counter;   // preload timer
//     TCCR1B |= (1 << CS12);    // 256 prescaler 
//     TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
//     interrupts();             // enable all interrupts
// #endif
}

// // MOA - TO LOOK
// #ifndef BUT_TEENSY
//     ISR(TIMER1_OVF_vect)        // interrupt service routine 
//     {
//         TCNT1 = timer1_counter;   // preload timer
//         //flash = !flash;
//         _flash = !_flash;
//         redraw = 1;
//     }
// #else

// extern void xts_serialEvent();
// bool firstTime = true;

//     void _ISR_emul() // called from desktop_devices.h
//     {

//         if ( firstTime ) {
//             firstTime = false;
//         } else {
//             return;
//         }

//         xts_serialEvent();
        

//         // TCNT1 = timer1_counter;   // preload timer
//         //flash = !flash;
//         _flash = !_flash;
//         redraw = 1;

//         // test if inited !!!!!! 
//         // setupLCD is called before interrupt init ..
//         // display.display();

//     }
// #endif


// void host_init(int buzzerPin) {
//     buzPin = buzzerPin;
//     //oled.clear();
//     if (buzPin > 0) {
//         pinMode(buzPin, OUTPUT);
//     }
//     initTimer();
// }

void host_sleep(long ms) {
    delay(ms);
}

void host_digitalWrite(int pin,int state) {
    digitalWrite(pin, state ? HIGH : LOW);
}

int host_digitalRead(int pin) {
    return digitalRead(pin);
}

int host_analogRead(int pin) {
    return analogRead(pin);
}

void host_pinMode(int pin,int mode) {
    pinMode(pin, mode);
}

void host_click() {
    mcu.tone(100, 20);
    delay(20);
    mcu.noTone();
}

void host_startupTone() {
    // if ( buzPin <= 0 || BUZZER_MUTE) return;
    // for (int i=1; i<=2; i++) {
    //     for (int j=0; j<50*i; j++) {
    //         digitalWrite(buzPin, HIGH);
    //         delay(3-i);
    //         digitalWrite(buzPin, LOW);
    //         delay(3-i);
    //     }
    //     delay(100);
    // }    
}

void host_cls() {

#ifdef SCREEN_BRIDGED
  mcu.getScreen()->clear();
  return;
#endif


    if ( SCREEN_LOCKER ) { return; }
    isWriting = true;
    memset(screenBuffer, 0x00, SCREEN_WIDTH*SCREEN_HEIGHT);
    memset(lineDirty, 0, SCREEN_HEIGHT);
    curX = 0;
    curY = 0;

    if ( OUTPUT_DEVICE == OUT_DEV_LCD_MINI ) {
        mcu.getScreen()->clear();
    }
    else if ( OUTPUT_DEVICE == OUT_DEV_SERIAL ) {
        #ifndef COMPUTER
        Serial.print("\n\n\n\n------------\n\n\n");
        #endif
    }

    isWriting = false;
}

void host_moveCursor(int x, int y) {
#ifdef SCREEN_BRIDGED
  mcu.getScreen()->setCursor(x,y);
  return;
#endif

    isWriting = true;
    if (x<0) x = 0;
    if (x>=SCREEN_WIDTH) x = SCREEN_WIDTH-1;
    if (y<0) y = 0;
    if (y>=SCREEN_HEIGHT) y = SCREEN_HEIGHT-1;
    curX = x;
    curY = y; 

    mcu.getScreen()->setCursor(x,y);

    isWriting = false;
}

int dirtyCOunter = 0;

#ifdef BUT_ESP32
  int lastScreenWidth = -1;
  char* espScreenline = NULL;
#endif


void host_showBuffer() {

#ifdef SCREEN_BRIDGED
  return;
#endif


  if ( SCREEN_LOCKER ) { return; }

  // en plus de la lecture de ligne ....
  if ( !LOCAL_ECHO ) { isWriting = false; return; }

#ifndef BUILTIN_LCD

        isWriting = true;
        bool dirty = false;
        for (int y=0; y<SCREEN_HEIGHT; y++) {
          if ( SCREEN_LOCKER ) { return; }
          if ( lineDirty[y] != 0 ) {
            dirty = true;
            break;
          }
        }
    
        if ( !dirty ) { isWriting = false; return; }
        //Serial.print( "\n\n----------\n" );
        Serial.flush();
        for (int y=0; y<SCREEN_HEIGHT; y++) {
            if ( SCREEN_LOCKER ) { return; }
            for (int x=0; x<SCREEN_WIDTH; x++) {
              char c = screenBuffer[y*SCREEN_WIDTH+x];
              if (c<32) c = ' ';
              line[x] = c;
            }
            line[SCREEN_WIDTH] = 0x00;
            //Serial.println( line );
            Serial._printAt( 0, y, line );
            Serial.flush();
            
            //if (lineDirty[y] || (inputMode && y==curY)) {
              lineDirty[y] = 0;
            //}
        }
        isWriting = false;

        Serial._printAt(0, SCREEN_HEIGHT-1, "Show buffer on regular TTY : ");
        //Serial._printAt(80-2, y+1, dirtyCOunter);
        dirtyCOunter++;

#else    

    // noInterrupts();
    // _noInterrupts();

    // BEWARE W/ COMPILLER CONDITION !!!!!!

    if ( OUTPUT_DEVICE == OUT_DEV_LCD_MINI ) {
        for (int y=0; y<SCREEN_HEIGHT; y++) {
            if ( SCREEN_LOCKER ) { return; }

            // BEWARE : removed since 21/02/2018 
            //if (lineDirty[y] || (inputMode && y==curY)) {
            if (lineDirty[y] ) {

                //display.setCursor(0,y);

#if defined(BUT_ESP32)
 // TODO : BETTER
 if ( lastScreenWidth != SCREEN_WIDTH ) {
     if ( espScreenline != NULL ) { free(espScreenline); }
     espScreenline = (char*)malloc( SCREEN_WIDTH+1 );
     lastScreenWidth = SCREEN_WIDTH;
 }
 char c;
 for (int x=0; x<SCREEN_WIDTH; x++) {
   c = screenBuffer[y*SCREEN_WIDTH+x];
   if (c<32) c = ' ';
   espScreenline[x] = c;
 }
 espScreenline[SCREEN_WIDTH] = 0x00;
 mcu.getScreen()->setCursor(0, y);
 mcu.getScreen()->print( espScreenline );
#else
                display.setCursor(0,y*8);
                display.setTextColor( BLACK );
                display.drawFastHLine( 0, (y+0)*8, 128, BLACK );
                display.drawFastHLine( 0, (y+1)*8, 128, BLACK );
                display.drawFastHLine( 0, (y+2)*8, 128, BLACK );
                display.drawFastHLine( 0, (y+3)*8, 128, BLACK );
                display.drawFastHLine( 0, (y+4)*8, 128, BLACK );
                display.drawFastHLine( 0, (y+5)*8, 128, BLACK );
                display.drawFastHLine( 0, (y+6)*8, 128, BLACK );
                display.drawFastHLine( 0, (y+7)*8, 128, BLACK );
                display.setTextColor( WHITE );

                for (int x=0; x<SCREEN_WIDTH; x++) {
                    char c = screenBuffer[y*SCREEN_WIDTH+x];
                    if (c<32) c = ' ';
                    //if (x==curX && y==curY && inputMode && flash) c = 127;
                    if (x==curX && y==curY && inputMode && _flash) c = 127;
                    display.print(c);
                }
#endif
                lineDirty[y] = 0;
            }
        }

        // Xtase
    } else if ( OUTPUT_DEVICE == OUT_DEV_VGA_SERIAL ) {

        #ifdef BOARD_VGA

            // no more done since 16/11/2017 - 'cause direct print
            isWriting = true;
            // bool dirty = false;
            // for (int y=0; y<SCREEN_HEIGHT; y++) {
            //     if ( SCREEN_LOCKER ) { return; }
            //     if ( lineDirty[y] != 0 ) {
            //         dirty = true;
            //         break;
            //     }
            // }
        
            // if ( !dirty ) { isWriting = false; return; }

            // for (int y=0; y<SCREEN_HEIGHT; y++) {
            //     if ( SCREEN_LOCKER ) { return; }

            //     if ( lineDirty[y] == 0 ) { continue; }

            //     vgat_locate(0,y);
            //     for (int x=0; x<SCREEN_WIDTH; x++) {
            //       char c = screenBuffer[y*SCREEN_WIDTH+x];
            //       if (c<32) c = ' ';
            //       line[x] = c;
            //       if ( x > lineDirty[y] ) { line[x] = 0x00; break; }
            //     }
            //     line[SCREEN_WIDTH] = 0x00;

            //     vgat_print( line );
                
            //     //if (lineDirty[y] || (inputMode && y==curY)) {
            //     lineDirty[y] = 0;
            //     //}
            // }
            isWriting = false;

        #else
            Serial.println("Show buffer on VGA TEXT Serial");
        #endif

    }  else if ( OUTPUT_DEVICE == OUT_DEV_RPID_SERIAL ) {

        #ifdef BOARD_RPID

            // no more done since 16/11/2017 - 'cause direct print
            isWriting = true;
            isWriting = false;

        #else
            Serial.println("Show buffer on RPI Display Serial");
        #endif

    } else if ( OUTPUT_DEVICE == OUT_DEV_SERIAL ) {
        isWriting = true;
        bool dirty = false;
        for (int y=0; y<SCREEN_HEIGHT; y++) {
          if ( SCREEN_LOCKER ) { return; }
          if ( lineDirty[y] != 0 ) {
            dirty = true;
            break;
          }
        }
    
        if ( !dirty ) { isWriting = false; return; }
        Serial.print( "\n\n----------\n" );
        Serial.flush();
        for (int y=0; y<SCREEN_HEIGHT; y++) {
            if ( SCREEN_LOCKER ) { return; }
            for (int x=0; x<SCREEN_WIDTH; x++) {
              char c = screenBuffer[y*SCREEN_WIDTH+x];
              if (c<32) c = ' ';
              line[x] = c;
            }
            line[SCREEN_WIDTH] = 0x00;
            Serial.println( line );
            Serial.flush();
            
            //if (lineDirty[y] || (inputMode && y==curY)) {
              lineDirty[y] = 0;
            //}
        }
        isWriting = false;

        Serial.println("Show buffer on regular Serial");
    }

    // interrupts();
    // _interrupts();

#endif
}

void scrollBuffer() {

// TODO : check if TFT_eSPI lib can scroll vertically
// TO-LOOK : (https://github.com/Bodmer/TFT_eSPI/blob/master/TFT_eSPI.h)
// 
// void setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
// setTextWrap(boolean wrapX, boolean wrapY = false)
// 
#ifdef SCREEN_BRIDGED
  return;
#endif


    if ( SCREEN_LOCKER ) { return; }
    isWriting = true;
    memcpy(screenBuffer, screenBuffer + SCREEN_WIDTH, SCREEN_WIDTH*(SCREEN_HEIGHT-1));
    memset(screenBuffer + SCREEN_WIDTH*(SCREEN_HEIGHT-1), 32, SCREEN_WIDTH);

    //Optim try
    //memset(lineDirty, 1, SCREEN_HEIGHT);
    for(int i=0; i < SCREEN_HEIGHT-1; i++) {
        lineDirty[i] = lineDirty[i+1];
    }
    lineDirty[SCREEN_HEIGHT-1] = 1;

    curY--;

    if ( OUTPUT_DEVICE == OUT_DEV_LCD_MINI ) {
        mcu.getScreen()->clear();
        for(int i=0; i < SCREEN_HEIGHT; i++) {
            lineDirty[i] = SCREEN_WIDTH;
        }
        host_showBuffer();// just a test
    }
    isWriting = false;
}

void host_outputString(char *str);

void host_outputString(const char *str) {
    host_outputString((char*)str);
}

void host_outputString(char *str) {
#ifdef SCREEN_BRIDGED
  mcu.getScreen()->print(str);
  return;
#endif

    if ( SCREEN_LOCKER ) { return; }
    isWriting = true;
    int pos = curY*SCREEN_WIDTH+curX;
    char ch;

    // @ top else need to 'rewind' str
    #ifdef BOARD_VGA
    if ( OUTPUT_DEVICE == OUT_DEV_VGA_SERIAL ) {
        vgat_print( (const char*)str );
    }
    #endif
    #ifdef BOARD_RPID
    if ( OUTPUT_DEVICE == OUT_DEV_RPID_SERIAL ) {
        rpid_print( (const char*)str );
    }
    #endif

    while (*str) {
        // Optim try
        //lineDirty[pos / SCREEN_WIDTH] = 1;
        lineDirty[pos / SCREEN_WIDTH]++;

        ch = *str++;
        screenBuffer[pos++] = ch;

        // moa
        if ( ch == '\n' ) { 
            // Optim try
            int tmpY = pos / SCREEN_WIDTH;
            if (pos % SCREEN_WIDTH > lineDirty[tmpY] ) { lineDirty[tmpY] = pos%SCREEN_WIDTH; }

            pos = (( pos/SCREEN_WIDTH )+1)*SCREEN_WIDTH; 
        }

        if (pos >= SCREEN_WIDTH*SCREEN_HEIGHT) {
            scrollBuffer();
            pos -= SCREEN_WIDTH;
        }
    }
    curX = pos % SCREEN_WIDTH;
    curY = pos / SCREEN_WIDTH;

    // Optim try
    if (curX > lineDirty[curY] ) { lineDirty[curY] = curX; }
    
    isWriting = false;
}

// void host_outputProgMemString(const char *p) {
//     isWriting = true;
//     while (1) {
//         unsigned char c = pgm_read_byte(p++);
//         if (c == 0) break;
//         host_outputChar(c);
//     }
//     isWriting = false;
// }

void host_outputChar(char c) {

#ifdef SCREEN_BRIDGED
  mcu.getScreen()->print(c);
  return;
#endif

    if ( SCREEN_LOCKER ) { return; }
    isWriting = true;
    int pos = curY*SCREEN_WIDTH+curX;
    //lineDirty[pos / SCREEN_WIDTH] = 1;
    screenBuffer[pos++] = c;
    if (pos >= SCREEN_WIDTH*SCREEN_HEIGHT) {
        scrollBuffer();
        pos -= SCREEN_WIDTH;
    }
    curX = pos % SCREEN_WIDTH;
    curY = pos / SCREEN_WIDTH;

    // Optim try
    if ( curX > lineDirty[curY] ) { lineDirty[curY] = curX; }

    #ifdef BOARD_VGA
    if ( OUTPUT_DEVICE == OUT_DEV_VGA_SERIAL ) {
        vgat_printCh( c );
    }
    #endif

    #ifdef BOARD_RPID
    if ( OUTPUT_DEVICE == OUT_DEV_RPID_SERIAL ) {
        rpid_printCh( c );
    }
    #endif

    isWriting = false;
}

// TODO : faster !!
// see if really need longs (instead of ints)
int host_outputInt(long num) {
    isWriting = true;

    // TODO : check that cast !!!
    mcu.getScreen()->print( (int)num );

    // returns len
    int c = 7; // check that !!!!!

    // // returns len
    // long i = num, xx = 1;
    // int c = 0;
    // do {
    //     c++;
    //     xx *= 10;
    //     i /= 10;
    // } 
    // while (i);

    // for (int i=0; i<c; i++) {
    //     xx /= 10;
    //     char digit = ((num/xx) % 10) + '0';
    //     host_outputChar(digit);
    // }
    isWriting = false;
    return c;
}

char *host_floatToStr(float f, char *buf) {
    // floats have approx 7 sig figs
    float a = fabs(f);
    if (f == 0.0f) {
        buf[0] = '0'; 
        buf[1] = 0;
    }
    else if (a<0.0001 || a>1000000) {
        // this will output -1.123456E99 = 13 characters max including trailing nul
        dtostre(f, buf, 6, 0);
    }
    else {
        int decPos = 7 - (int)(floor(log10(a))+1.0f);
        dtostrf(f, 1, decPos, buf);
        if (decPos) {
            // remove trailing 0s
            char *p = buf;
            while (*p) p++;
            p--;
            while (*p == '0') {
                *p-- = 0;
            }
            if (*p == '.') *p = 0;
        }   
    }
    return buf;
}

void host_outputFloat(float f) {

#ifdef SCREEN_BRIDGED
  mcu.getScreen()->print(f);
  return;
#endif

    static char buf[16];
    host_outputString(host_floatToStr(f, buf));
}

void host_newLine() {

#ifdef SCREEN_BRIDGED
  mcu.getScreen()->println("");
  return;
#endif

    if ( SCREEN_LOCKER ) { return; }
    isWriting = true;
    curX = 0;
    curY++;
    if (curY == SCREEN_HEIGHT) {
        scrollBuffer();
    }
    memset(screenBuffer + SCREEN_WIDTH*(curY), 32, SCREEN_WIDTH);
    
    // Optim try
    //lineDirty[curY] = 1;

// ==== moa =
//host_showBuffer();
// ==========

    #ifdef BOARD_VGA
    if ( OUTPUT_DEVICE == OUT_DEV_VGA_SERIAL ) {
        vgat_print( "\n" );
    }
    #endif
    #ifdef BOARD_RPID
    if ( OUTPUT_DEVICE == OUT_DEV_RPID_SERIAL ) {
        rpid_print( "\n" );
    }
    #endif

    isWriting = false;
}

extern int doRun();
extern int xts_buttonRead(int btnNum);

char *host_readLine() {
    inputMode = 1;

    if (curX == 0) memset(screenBuffer + SCREEN_WIDTH*(curY), 32, SCREEN_WIDTH);
    else host_newLine();

    int startPos = curY*SCREEN_WIDTH+curX;
    int pos = startPos;

    bool done = false;
    int kc = -1;
    bool printable = false;
    while (!done) {

          if ( MODE_EDITOR && mcu.getSystemMenuReqState() ) {
            while( !mcu.getSystemMenuReqEnd() ) {
                delay(100);
            }
            host_click();
            host_system_menu();
            
            // to trigger end-of-line
            // & execute selfRun
            kc = PS2_ENTER;

            //screenBuffer[pos++] = '\n';
            host_newLine();

            done = true;
          }
          else if ( MODE_EDITOR && mcu.getSystemResetReqState() ) {
            while( mcu.getSystemResetReqState() ) {
                delay(100);
            }
            // to trigger end-of-line
            host_click();
            kc = PS2_ENTER;

            //screenBuffer[pos++] = '\n';
            host_newLine();

            MCU_reset();
            done = true;
          }
          else if ( MODE_EDITOR && xts_buttonRead(2) > 0 ) {
            while( xts_buttonRead(2) > 0 ) {
                delay(100);
            }
            // to trigger end-of-line
            host_click();
            kc = PS2_ENTER;

            // screenBuffer[pos++] = '\n';
            host_newLine();

            done = true;
          }
          else if ( MODE_EDITOR && selfRun ) {
            // BUGFIX : to remove mandatory ENTER key pressing
            //          when selfRun flag activated
            host_click();
            kc = PS2_ENTER;
            host_newLine();
            done = true;
          }
          while ( Serial.available() ) {
            host_click();
            lineDirty[pos / SCREEN_WIDTH] = pos % SCREEN_WIDTH; // Cf VGAT + Kbd display bug
            
            char c = (kc > -1) ? kc : Serial.read();
            if (c>=32 && c<=126) {
                screenBuffer[pos++] = c; // kept cf readLine !!
mcu.print(c);
            }
            else if (c==PS2_DELETE && pos > startPos) {
mcu.print( '\b' );
                screenBuffer[--pos] = 0; // kept cf readLine !!
            }
            else if (c==PS2_ENTER) {
                done = true;
mcu.print( '\n' );
            }
            // curX = pos % SCREEN_WIDTH;
            // curY = pos / SCREEN_WIDTH;
            // // scroll if we need to
            // if (curY == SCREEN_HEIGHT) {
            //     if (startPos >= SCREEN_WIDTH) {
            //         startPos -= SCREEN_WIDTH;
            //         pos -= SCREEN_WIDTH;
            //         scrollBuffer();
            //     }
            //     else
            //     {
            //         screenBuffer[--pos] = 0;
            //         curX = pos % SCREEN_WIDTH;
            //         curY = pos / SCREEN_WIDTH;
            //     }
            // }
            redraw = 1;
        }
        if (redraw) {
            if (LOCAL_ECHO) { host_showBuffer(); }
        }

        // #ifdef COMPUTER
        // delay(50);
        // #endif

    } // end of while(done)
    screenBuffer[pos] = 0;
    inputMode = 0;
    
    // remove the cursor
    //lineDirty[curY] = 1;
    if (LOCAL_ECHO) { host_showBuffer(); }

    return &screenBuffer[startPos];
}

char host_getKey() {
      #ifdef COMPUTER
        Serial.available(); // just to poll ncurses kbd
        int c = Serial.read();
        if (c >= 32 && c <= 126)
            return c;
        else return 0;
      #else
        char c = inkeyChar;
        inkeyChar = 0;
        if (c >= 32 && c <= 126)
            return c;
        else return 0;
    #endif
}

bool host_ESCPressed() {

    #ifdef BUILTIN_KBD
      bool printable = false;
      int kc = -1;
      while ( (kc = read_kbd(&printable) ) != -1 ) {
          inkeyChar = kc;
          if ( inkeyChar == PS2_ESC || inkeyChar == KBD_CTRL_C || inkeyChar == KBD_BREAK ) {
              return true;
          }
      }
    #elif BUT_ESP32
      if ( mcu.getSystemBreakReqState() ) {
          mcu.noTone();
          while( mcu.getSystemBreakReqState() ) {
              delay(100);
          }
          return true;
      }
      return false;
    #else
      return anyBtn();
    #endif
}

void host_outputFreeMem(unsigned int val)
{
    host_newLine();
    host_outputInt(val);
    host_outputChar(' ');
    //host_outputProgMemString(bytesFreeStr);      
    host_outputString( (char*) bytesFreeStr);      
}

// void host_saveProgram(bool autoexec) {
//     EEPROM.write(0, autoexec ? MAGIC_AUTORUN_NUMBER : 0x00);
//     EEPROM.write(1, sysPROGEND & 0xFF);
//     EEPROM.write(2, (sysPROGEND >> 8) & 0xFF);
//     for (int i=0; i<sysPROGEND; i++)
//         EEPROM.write(3+i, mem[i]);
// }

// void host_loadProgram() {
//     // skip the autorun byte
//     sysPROGEND = EEPROM.read(1) | (EEPROM.read(2) << 8);
//     for (int i=0; i<sysPROGEND; i++)
//         mem[i] = EEPROM.read(i+3);
// }

