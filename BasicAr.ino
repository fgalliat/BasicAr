/*******************
* Xtase - fgalliat : fgalliat@gmail.com
* Based on robin edwards work - (https://github.com/robinhedwards/ArduinoBASIC)
*******************/
//#define REAL_ARDUINO 1
//#define ALL_DEVICES 1

#include "xts_arch.h"


// Teensy's doesn't supports FS (SD, SDFat) & PROGMEM routines


extern bool STORAGE_OK;
bool BUZZER_MUTE = false;


#ifdef BUT_TEENSY
    #include "desktop_devices.h"

    #include <EEPROM.h>
#else
    #include <font.h>
    #include <SSD1306ASCII.h>
    // ^ - modified for faster SPI
    #include <PS2Keyboard.h>
    
    #include <EEPROM.h>
#endif
#include "basic.h"
#include "host.h"

// Define in host.h if using an external EEPROM e.g. 24LC256
// Should be connected to the I2C pins
// SDA -> Analog Pin 4, SCL -> Analog Pin 5
// See e.g. http://www.hobbytronics.co.uk/arduino-external-eeprom

// If using an external EEPROM, you'll also have to initialise it by
// running once with the appropriate lines enabled in setup() - see below

#if EXTERNAL_EEPROM
    #include <I2cMaster.h>
    // Instance of class for hardware master with pullups enabled
    TwiMaster rtc(true);
#endif

// Keyboard
const int DataPin = 8;
const int IRQpin =  3;
PS2Keyboard keyboard;

// OLED
#define OLED_DATA 9
#define OLED_CLK 10
#define OLED_DC 11
#define OLED_CS 12
#define OLED_RST 13
SSD1306ASCII oled(OLED_DATA, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

// NB Keyboard needs a seperate ground from the OLED

// // buzzer pin, 0 = disabled/not present
// #define BUZZER_PIN    5

// BASIC
unsigned char mem[MEMORY_SIZE];

// NB OF TOKEN PER LINES
#define TOKEN_BUF_SIZE    64
unsigned char tokenBuf[TOKEN_BUF_SIZE];

const char welcomeStr[]  = "Arduino BASIC";
char autorun = 0;

// =========/ Serial Event /==============
// String inputString = "";         // a String to hold incoming data
// volatile boolean stringComplete = false;  // whether the string is complete
volatile boolean isWriting = false; // lock read when write
// =========/ Serial Event /==============

char screenBuffer[SCREEN_WIDTH*SCREEN_HEIGHT];
char lineDirty[SCREEN_HEIGHT];
// ============================================


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
unsigned char audiobuff[AUDIO_BUFF_SIZE];
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void setup() {

    BUZZER_MUTE = true;
    // inputString.reserve(200);
    
    setupHardware();

    
    keyboard.begin(DataPin, IRQpin);
    oled.ssd1306_init(SSD1306_SWITCHCAPVCC);


    reset();
    host_init(BUZZER_PIN);
    host_cls();
    //host_outputProgMemString(welcomeStr);
    host_outputString( (char*) welcomeStr);

    // show memory size
    host_outputFreeMem(sysVARSTART - sysPROGEND);
    host_outputString( "\n" );
    host_outputString( ARCH_TYPE );
    host_outputString( "\n" );

    #ifdef FS_SUPPORT
      if ( STORAGE_OK ) { host_outputString("SD : OK.\n"); }
      else              { host_outputString("SD : FAILED.\n"); }

      #ifdef USE_SDFAT_LIB
        host_outputString("SD : MODE FAT\n");
      #else
        host_outputString("SD : MODE LEG\n");
      #endif
    #endif

    host_showBuffer();
    
    // IF USING EXTERNAL EEPROM
    // The following line 'wipes' the external EEPROM and prepares
    // it for use. Uncomment it, upload the sketch, then comment it back
    // in again and upload again, if you use a new EEPROM.
    // writeExtEEPROM(0,0); writeExtEEPROM(1,0);

    if (EEPROM.read(0) == MAGIC_AUTORUN_NUMBER) {
        autorun = 1;
    }
    else {
        if ( !BUZZER_MUTE ) { host_startupTone(); }
    }

    //autorun = 1;
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void loop() {
    int ret = ERROR_NONE;

    if (!autorun) {
        // get a line from the user
        char *input = host_readLine();
        // special editor commands

        // 10 PRINT "Hello"      :: works
        // 10 PRINT "Hello toto" :: fails
        // 10 PRINT "12345678"   :: fails
        // 10 PRINT "12345"      :: works

        //if (input[0] == '?' && input[1] == 0) {
        if (input[0] == '*' && input[1] == 0) { // this is not the problem
            host_outputFreeMem(sysVARSTART - sysPROGEND);
            host_showBuffer();
            return;
        }
        // otherwise tokenize
        ret = tokenize((unsigned char*)input, tokenBuf, TOKEN_BUF_SIZE);
    }
    else {
        host_loadProgram();
        tokenBuf[0] = TOKEN_RUN;
        tokenBuf[1] = 0;
        autorun = 0;
    }
    // execute the token buffer
    if (ret == ERROR_NONE) {
        host_newLine();
        ret = processInput(tokenBuf);
    }
    if (ret != ERROR_NONE) {
        host_newLine();
        if (lineNumber !=0) {
            host_outputInt(lineNumber);
            host_outputChar('-');
        }
        //host_outputProgMemString((char *)pgm_read_word(&(errorTable[ret])));
        host_outputString((char *)errorTable[ret]);

        //host_showBuffer();
    }
    //host_showBuffer();
}



// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void loop2() {
    int ret = ERROR_NONE;

    if (!autorun) {
        // get a line from the user
        char *input = host_readLine();

        // special editor commands
        // Xtase modif
        //if (input[0] == '?' && input[1] == 0) {
        if (input[0] == '*' && input[1] == 0) {
            host_outputFreeMem(sysVARSTART - sysPROGEND);
            host_showBuffer();
            return;
        } else if (input[0] == '!' && input[1] == 0) {
            //input = (char*)"10 ? \"Coucou\"";
            input = (char*)"10 PRINT \"Coucou\"";
            ret = tokenize((unsigned char*)input, tokenBuf, TOKEN_BUF_SIZE); ret = processInput(tokenBuf);
            if ( ret > 0 ) { host_outputString((char *)errorTable[ret]); host_showBuffer(); }
            ret = ERROR_NONE;
            
            // input = (char*)"20 FOR I=1 TO 5 : ? \"coucou\";I : NEXT I";
            // ret = tokenize((unsigned char*)input, tokenBuf, TOKEN_BUF_SIZE); ret = processInput(tokenBuf);
            // if ( ret > 0 ) { host_outputString((char *)errorTable[ret]); host_showBuffer(); }
            // ret = ERROR_NONE;
            
            input = (char*)"30 GOTO 10";
            ret = tokenize((unsigned char*)input, tokenBuf, TOKEN_BUF_SIZE); ret = processInput(tokenBuf);
            if ( ret > 0 ) { host_outputString((char *)errorTable[ret]); host_showBuffer(); }
            ret = ERROR_NONE;
            
            input = (char*)"LIST";
            ret = tokenize((unsigned char*)input, tokenBuf, TOKEN_BUF_SIZE); ret = processInput(tokenBuf);
            if ( ret > 0 ) { host_outputString((char *)errorTable[ret]); host_showBuffer(); }
            ret = ERROR_NONE;
            
        }
        
        
        // if (input[0] == '!' && input[1] == 0) {
        //     xts_loadTestProgram();
        //     tokenBuf[0] = TOKEN_LIST;
        //     tokenBuf[1] = 0;
        //     autorun = 0;
        // } else {

            // otherwise tokenize
            ret = tokenize((unsigned char*)input, tokenBuf, TOKEN_BUF_SIZE);


            led2( ret > 0 );

            // moa
            host_showBuffer();
            //delay(50);
        //}
    }
    else {
        host_loadProgram();
        tokenBuf[0] = TOKEN_RUN;
        // xts_loadTestProgram();
        // tokenBuf[0] = TOKEN_LIST;
        tokenBuf[1] = 0;
        autorun = 0;
    }
    // execute the token buffer
    if (ret == ERROR_NONE) {
        host_newLine();
        ret = processInput(tokenBuf);

        led3( ret > 0 );

    }
    if (ret != ERROR_NONE) {
        host_newLine();
        if (lineNumber !=0) {
            host_outputInt(lineNumber);
            host_outputChar('-');
        }

        host_outputString( "RET = " );
        host_outputInt(ret);
        host_outputString( "\n" );
        //host_outputProgMemString((char *)pgm_read_word(&(errorTable[ret])));
        host_outputString((char *)errorTable[ret]);
        host_showBuffer();
    }

}

// ___________________________________________________________

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void xts_serialEvent() {
    // //while ( isWriting ) { delay(5); Serial.print('X'); }
    // Serial.flush();


    // while (Serial.available()) {
    //   // get the new byte:
    //   char inChar = (char)Serial.read();

    //   if ( inChar == '\n' ) { continue; }

    //   // add it to the inputString:
    //   inputString += inChar;
    //   // if the incoming character is a newline, set a flag so the main loop can
    //   // do something about it:
    //   if (inChar == '\n'|| inChar == '\r' ) {
    //     stringComplete = true;
    //   }
    // }
  }



// ___________________________________________________________
#ifdef DESKTOP_COMPUTER
  // ========================
  int main(int argc, char** argv) {
    setupComputer();
    setup();

    while(true) {
        loop();
        usleep( 10 * 1000 );
    }

    // to call.....
    closeComputer();
  }
#endif
