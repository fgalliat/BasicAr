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

#ifdef BUT_TEENSY
  #include "xts_teensy.h"
#endif

#include "xts_arch.h"
#ifdef BUT_ESP32
  extern Esp32Oled esp32;
#endif


#include "xts_io.h"
int OUTPUT_DEVICE;
int GFX_DEVICE;
int INPUT_DEVICE;


extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern char* line; // one TTY line
// char screenBuffer[SCREEN_WIDTH*SCREEN_HEIGHT];
// char lineDirty[SCREEN_HEIGHT];
extern char* screenBuffer;
extern char* lineDirty;
extern bool SCREEN_LOCKER;


 #include "desktop_devices.h"

 #ifndef COMPUTER
   #include <EEPROM.h>
 #else
   extern EEPROMClass EEPROM;
 #endif

// extern SSD1306ASCII oled;
extern PS2Keyboard keyboard;
//   #include <SSD1306ASCII.h>
//   #include <PS2Keyboard.h>


#include "host.h"
#include "basic.h"

// for dtostre & dtostrf
#include "xts_compat.h" 
#include <stdlib.h> 

extern bool BUZZER_MUTE;

#ifdef BUILTIN_LCD
  #if defined(BUT_ESP32)
   // ......
  #else
   #ifndef COMPUTER
   #include "dev_screen_Adafruit_SSD1306.h"
   extern Adafruit_SSD1306 display;
  #else 
   //#define display _displayOLED
  #endif
  #endif
#endif


#ifdef BUILTIN_KBD
  #include "dev_kbd.h"
#endif

#ifdef BOARD_VGA
  #include "dev_screen_VGATEXT.h"
#endif


#ifdef BOARD_RPID
  #include "dev_screen_RPIGFX.h"
#endif


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
//   #include <avr/io.h>
//   #include <avr/interrupt.h>
// #endif

#ifdef BUT_TEENSY

 // see : https://www.pjrc.com/teensy/td_timing_IntervalTimer.html

#else
    noInterrupts();           // disable all interrupts
    
    TCCR1A = 0;
    TCCR1B = 0;
    timer1_counter = 34286;   // preload timer 65536-16MHz/256/2Hz
    TCNT1 = timer1_counter;   // preload timer
    TCCR1B |= (1 << CS12);    // 256 prescaler 
    TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
    
    interrupts();             // enable all interrupts
#endif
    

}

// MOA - TO LOOK
#ifndef BUT_TEENSY
    ISR(TIMER1_OVF_vect)        // interrupt service routine 
    {
        TCNT1 = timer1_counter;   // preload timer
        //flash = !flash;
        _flash = !_flash;
        redraw = 1;
    }
#else

extern void xts_serialEvent();
bool firstTime = true;

    void _ISR_emul() // called from desktop_devices.h
    {

        if ( firstTime ) {
            firstTime = false;
        } else {
            return;
        }

        xts_serialEvent();
        

        // TCNT1 = timer1_counter;   // preload timer
        //flash = !flash;
        _flash = !_flash;
        redraw = 1;

        // test if inited !!!!!! 
        // setupLCD is called before interrupt init ..
        // display.display();

    }
#endif


void host_init(int buzzerPin) {
    buzPin = buzzerPin;
    
    //oled.clear();

    if (buzPin > 0) {
        pinMode(buzPin, OUTPUT);
    }

    initTimer();
}

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
    if ( buzPin <= 0 || BUZZER_MUTE) return;
    digitalWrite(buzPin, HIGH);
    delay(1);
    digitalWrite(buzPin, LOW);
}

void host_startupTone() {
    if ( buzPin <= 0 || BUZZER_MUTE) return;
    for (int i=1; i<=2; i++) {
        for (int j=0; j<50*i; j++) {
            digitalWrite(buzPin, HIGH);
            delay(3-i);
            digitalWrite(buzPin, LOW);
            delay(3-i);
        }
        delay(100);
    }    
}

void host_cls() {
    if ( SCREEN_LOCKER ) { return; }
    isWriting = true;
    memset(screenBuffer, 0x00, SCREEN_WIDTH*SCREEN_HEIGHT);
    memset(lineDirty, 0, SCREEN_HEIGHT);
    curX = 0;
    curY = 0;

    #ifdef BUILTIN_LCD
        if ( OUTPUT_DEVICE == OUT_DEV_LCD_MINI ) {
            #if defined(BUT_ESP32)
              esp32.getScreen()->clear();
              esp32.getScreen()->blitt();
            #else
              display.clearDisplay();
              display.display();
            #endif
        } else 
    #endif

    #ifdef BOARD_VGA
        if ( OUTPUT_DEVICE == OUT_DEV_VGA_SERIAL ) {
            vgat_cls();
        } else 
    #endif

    #ifdef BOARD_RPID
        if ( OUTPUT_DEVICE == OUT_DEV_RPID_SERIAL ) {
            rpid_cls(); 
        } else 
    #endif

      if ( OUTPUT_DEVICE == OUT_DEV_SERIAL ) {
          #ifndef COMPUTER
          Serial.print("\n\n\n\n------------\n\n\n");
          #endif
      }

    isWriting = false;
}

void host_moveCursor(int x, int y) {
    isWriting = true;
    if (x<0) x = 0;
    if (x>=SCREEN_WIDTH) x = SCREEN_WIDTH-1;
    if (y<0) y = 0;
    if (y>=SCREEN_HEIGHT) y = SCREEN_HEIGHT-1;
    curX = x;
    curY = y; 

    #ifdef BUILTIN_LCD
        if ( OUTPUT_DEVICE == OUT_DEV_LCD_MINI ) {
            #if defined(ARDUINO_ARCH_ESP32)
             // Oups what TODO
            #else
             display.setCursor(x*6,y*8);
            #endif
        }
    #endif
    #ifdef BOARD_VGA
        if ( OUTPUT_DEVICE == OUT_DEV_VGA_SERIAL ) {
            vgat_locate(x,y);
        }
    #endif
    #ifdef BOARD_RPID
        if ( OUTPUT_DEVICE == OUT_DEV_RPID_SERIAL ) {
            rpid_locate(x,y);
        }
    #endif
    isWriting = false;
}

int dirtyCOunter = 0;

void host_showBuffer() {
  if ( SCREEN_LOCKER ) { return; }

  // en plus de la lecture de ligne ....
  if ( !LOCAL_ECHO ) { isWriting = false; return; }

//#ifdef BUT_TEENSY
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

            if (lineDirty[y] || (inputMode && y==curY)) {
                //display.setCursor(0,y);

#if defined(BUT_ESP32)
 // TODO : BETTER
 char line[SCREEN_WIDTH+1];
 for (int x=0; x<SCREEN_WIDTH; x++) {
   char c = screenBuffer[y*SCREEN_WIDTH+x];
   if (c<32) c = ' ';
   line[x] = c;
 }
 line[SCREEN_WIDTH] = 0x00;
 esp32.getScreen()->drawString( 0, y*8, line );
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
#ifdef BUT_ESP32
        esp32.getScreen()->blitt();
#else
        display.display(); // to place in an interrupt
#endif
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

    #ifdef BUILTIN_LCD
        if ( OUTPUT_DEVICE == OUT_DEV_LCD_MINI ) {
            #if defined(BUT_ESP32)
             esp32.getScreen()->clear();
            #else
             display.clearDisplay();
            #endif
            for(int i=0; i < SCREEN_HEIGHT; i++) {
                lineDirty[i] = SCREEN_WIDTH;
            }
            host_showBuffer();// just a test
        }
    #endif
    #ifdef BOARD_VGA
        if ( OUTPUT_DEVICE == OUT_DEV_VGA_SERIAL ) {
            //vgat_cls();
        }
    #endif
    #ifdef BOARD_RPID
        if ( OUTPUT_DEVICE == OUT_DEV_RPID_SERIAL ) {
            //vgat_cls();
        }
    #endif
    isWriting = false;
}

void host_outputString(char *str);

void host_outputString(const char *str) {
    host_outputString((char*)str);
}

void host_outputString(char *str) {
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

int host_outputInt(long num) {
    isWriting = true;
    // returns len
    long i = num, xx = 1;
    int c = 0;
    do {
        c++;
        xx *= 10;
        i /= 10;
    } 
    while (i);

    for (int i=0; i<c; i++) {
        xx /= 10;
        char digit = ((num/xx) % 10) + '0';
        host_outputChar(digit);
    }
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
    static char buf[16];
    host_outputString(host_floatToStr(f, buf));
}

void host_newLine() {
    if ( SCREEN_LOCKER ) { return; }
    isWriting = true;
    curX = 0;
    curY++;
    if (curY == SCREEN_HEIGHT)
        scrollBuffer();
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

        #ifdef BUILTIN_KBD
          printable = false;
          while (keyboard.available() || (kc = read_kbd(&printable)) > -1 ) {
              if ( MODE_EDITOR && kc == KBD_F1 ) {
                host_system_menu();
                // to trigger end-of-line
                // & execute selfRun
                kc = PS2_ENTER;
              }
        #else
          while (keyboard.available() ) {
        #endif
            host_click();
            // read the next key
            // Optim try
            //lineDirty[pos / SCREEN_WIDTH] = 1;
            //lineDirty[pos / SCREEN_WIDTH]++;
            lineDirty[pos / SCREEN_WIDTH] = pos % SCREEN_WIDTH; // Cf VGAT + Kbd display bug
            
            char c = (kc > -1) ? kc : keyboard.read();
            if (c>=32 && c<=126) {
                screenBuffer[pos++] = c;
                
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
            }
            else if (c==PS2_DELETE && pos > startPos) {
                screenBuffer[--pos] = 0;
                #ifdef BOARD_VGA
                    if ( OUTPUT_DEVICE == OUT_DEV_VGA_SERIAL ) {
                        vgat_printCh( '\b' );
                    }
                #endif
                #ifdef BOARD_RPID
                    if ( OUTPUT_DEVICE == OUT_DEV_RPID_SERIAL ) {
                        rpid_printCh( '\b' );
                    }
                #endif
            }
            else if (c==PS2_ENTER) {
                done = true;
                //screenBuffer[pos] = 0;
                #ifdef BOARD_VGA
                    if ( OUTPUT_DEVICE == OUT_DEV_VGA_SERIAL ) {
                        vgat_printCh( '\n' );
                    }
                #endif
                #ifdef BOARD_RPID
                    if ( OUTPUT_DEVICE == OUT_DEV_RPID_SERIAL ) {
                        rpid_printCh( '\n' );
                    }
                #endif
            }
            curX = pos % SCREEN_WIDTH;
            curY = pos / SCREEN_WIDTH;
            // scroll if we need to
            if (curY == SCREEN_HEIGHT) {
                if (startPos >= SCREEN_WIDTH) {
                    startPos -= SCREEN_WIDTH;
                    pos -= SCREEN_WIDTH;
                    scrollBuffer();
                }
                else
                {
                    screenBuffer[--pos] = 0;
                    curX = pos % SCREEN_WIDTH;
                    curY = pos / SCREEN_WIDTH;
                }
            }
            redraw = 1;
        }
        if (redraw) {
            if (LOCAL_ECHO) { host_showBuffer(); }
        }

        #ifdef COMPUTER
        delay(50);
        #endif

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
    #else
      return anyBtn();
    #endif

    // while (keyboard.available()) {
    //     // read the next key
    //     inkeyChar = keyboard.read();
    //     if (inkeyChar == PS2_ESC || inkeyChar == '!')
    //         return true;
    // }
    //return false;
    //host_sleep(50);
    
    //return anyBtn();
}

void host_outputFreeMem(unsigned int val)
{
    host_newLine();
    host_outputInt(val);
    host_outputChar(' ');
    //host_outputProgMemString(bytesFreeStr);      
    host_outputString( (char*) bytesFreeStr);      
}

void host_saveProgram(bool autoexec) {
    EEPROM.write(0, autoexec ? MAGIC_AUTORUN_NUMBER : 0x00);
    EEPROM.write(1, sysPROGEND & 0xFF);
    EEPROM.write(2, (sysPROGEND >> 8) & 0xFF);
    for (int i=0; i<sysPROGEND; i++)
        EEPROM.write(3+i, mem[i]);
}

void host_loadProgram() {
    // skip the autorun byte
    sysPROGEND = EEPROM.read(1) | (EEPROM.read(2) << 8);
    for (int i=0; i<sysPROGEND; i++)
        mem[i] = EEPROM.read(i+3);
}

#if EXTERNAL_EEPROM
#include <I2cMaster.h>
extern TwiMaster rtc;

void writeExtEEPROM(unsigned int address, byte data) 
{
  if (address % 32 == 0) host_click();
  rtc.start((EXTERNAL_EEPROM_ADDR<<1)|I2C_WRITE);
  rtc.write((int)(address >> 8));   // MSB
  rtc.write((int)(address & 0xFF)); // LSB
  rtc.write(data);
  rtc.stop();
  delay(5);
}
 
byte readExtEEPROM(unsigned int address) 
{
  rtc.start((EXTERNAL_EEPROM_ADDR<<1)|I2C_WRITE);
  rtc.write((int)(address >> 8));   // MSB
  rtc.write((int)(address & 0xFF)); // LSB
  rtc.restart((EXTERNAL_EEPROM_ADDR<<1)|I2C_READ);
  byte b = rtc.read(true);
  rtc.stop();
  return b;
}

// get the EEPROM address of a file, or the end if fileName is null
unsigned int getExtEEPROMAddr(char *fileName) {
    unsigned int addr = 0;
    while (1) {
        unsigned int len = readExtEEPROM(addr) | (readExtEEPROM(addr+1) << 8);
        if (len == 0) break;
        
        if (fileName) {
            bool found = true;
            for (int i=0; i<=strlen(fileName); i++) {
                if (fileName[i] != readExtEEPROM(addr+2+i)) {
                    found = false;
                    break;
                }
            }
            if (found) return addr;
        }
        addr += len;
    }
    return fileName ? EXTERNAL_EEPROM_SIZE : addr;
}

void host_directoryExtEEPROM() {
    unsigned int addr = 0;
    while (1) {
        unsigned int len = readExtEEPROM(addr) | (readExtEEPROM(addr+1) << 8);
        if (len == 0) break;
        int i = 0;
        while (1) {
            char ch = readExtEEPROM(addr+2+i);
            if (!ch) break;
            host_outputChar(readExtEEPROM(addr+2+i));
            i++;
        }
        addr += len;
        host_outputChar(' ');
    }
    host_outputFreeMem(EXTERNAL_EEPROM_SIZE - addr - 2);
}

bool host_removeExtEEPROM(char *fileName) {
    unsigned int addr = getExtEEPROMAddr(fileName);
    if (addr == EXTERNAL_EEPROM_SIZE) return false;
    unsigned int len = readExtEEPROM(addr) | (readExtEEPROM(addr+1) << 8);
    unsigned int last = getExtEEPROMAddr(NULL);
    unsigned int count = 2 + last - (addr + len);
    while (count--) {
        byte b = readExtEEPROM(addr+len);
        writeExtEEPROM(addr, b);
        addr++;
    }
    return true;    
}

bool host_loadExtEEPROM(char *fileName) {
    unsigned int addr = getExtEEPROMAddr(fileName);
    if (addr == EXTERNAL_EEPROM_SIZE) return false;
    // skip filename
    addr += 2;
    while (readExtEEPROM(addr++)) ;
    sysPROGEND = readExtEEPROM(addr) | (readExtEEPROM(addr+1) << 8);
    for (int i=0; i<sysPROGEND; i++)
        mem[i] = readExtEEPROM(addr+2+i);
}

bool host_saveExtEEPROM(char *fileName) {
    unsigned int addr = getExtEEPROMAddr(fileName);
    if (addr != EXTERNAL_EEPROM_SIZE)
        host_removeExtEEPROM(fileName);
    addr = getExtEEPROMAddr(NULL);
    unsigned int fileNameLen = strlen(fileName);
    unsigned int len = 2 + fileNameLen + 1 + 2 + sysPROGEND;
    if ((long)EXTERNAL_EEPROM_SIZE - addr - len - 2 < 0)
        return false;
    // write overall length
    writeExtEEPROM(addr++, len & 0xFF);
    writeExtEEPROM(addr++, (len >> 8) & 0xFF);
    // write filename
    for (int i=0; i<strlen(fileName); i++)
        writeExtEEPROM(addr++, fileName[i]);
    writeExtEEPROM(addr++, 0);
    // write length & program    
    writeExtEEPROM(addr++, sysPROGEND & 0xFF);
    writeExtEEPROM(addr++, (sysPROGEND >> 8) & 0xFF);
    for (int i=0; i<sysPROGEND; i++)
        writeExtEEPROM(addr++, mem[i]);
    // 0 length marks end
    writeExtEEPROM(addr++, 0);
    writeExtEEPROM(addr++, 0);
    return true;
}

#endif

