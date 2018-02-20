/*******************
* Xtase - fgalliat : fgalliat@gmail.com @Sept2017
* redesigned for XtsuBasic boards & XtsuPocket boards
*
* Based on robin edwards work - (https://github.com/robinhedwards/ArduinoBASIC)
*
* if compile errors : (see error_log_win.txt)
* @01/31/2018 arduino ide 1.8.5 & esp32 core under Win10
*******************/

// Teensy's doesn't supports FS (SD, SDFat) & PROGMEM routines
#include "xts_arch.h"

#ifdef BUT_ESP32
 #ifdef ESP32PCKv2
   #include "HardwareSerial.h"
   //HardwareSerial Serial2(2);
   HardwareSerial Serial2(UART2_NUM);
   Esp32Pocketv2 esp32;

   #ifdef ESP32_WIFI_SUPPORT
     extern void host_outputString(char* str);
     extern int host_outputInt(long v);

     #define DBUG(a) { Serial.print(a); host_outputString(a); }
     #define DBUGi(a) { Serial.print(a); host_outputInt(a); }

     #include "Esp32WifiServer.h"
     Esp32WifiServer telnet;
     #undef DBUG
     #undef DBUGi
   #endif


 #else
  Esp32Oled esp32;
 #endif
  void noTone(int pin) { esp32.noTone(); }
  void tone(int pin, int freq, int duration) { esp32.tone(freq,duration); }
#endif


extern bool STORAGE_OK;
bool BUZZER_MUTE = false;

// NOT CONSTANT : Cf Output device
int SCREEN_WIDTH        = 21;
int SCREEN_HEIGHT       = 8;

#include "xts_io.h"
extern int OUTPUT_DEVICE;
extern int GFX_DEVICE;
extern int INPUT_DEVICE;


#ifdef BUT_TEENSY
    #include "desktop_devices.h"

    #ifndef COMPUTER
      #include <EEPROM.h>
    #endif
#else
    #include <font.h>
    #include <SSD1306ASCII.h>
    // ^ - modified for faster SPI
    #include <PS2Keyboard.h>
    
    #include <EEPROM.h>
#endif
#include "basic.h"
#include "host.h"

extern int xts_loadBas(char* optFilename=NULL);

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

// Keyboard --- to remove !!!
// NB Keyboard needs a seperate ground from the OLED
const int DataPin = 8;
const int IRQpin =  3;
PS2Keyboard keyboard;

#ifndef BUT_ESP32
 // OLED --- to remove !!!
 #define OLED_DATA 9
 #define OLED_CLK 10
 #define OLED_DC 11
 #define OLED_CS 12
 #define OLED_RST 13
 SSD1306ASCII oled(OLED_DATA, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);
#endif
// not to include for ESP32 OLED


// BASIC
unsigned char mem[MEMORY_SIZE];
unsigned char tokenBuf[TOKEN_BUF_SIZE];
char codeLine[ASCII_CODELINE_SIZE]; // !! if enought !! (BEWARE : LIGHT4.BAS)

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
unsigned char audiobuff[AUDIO_BUFF_SIZE];       // T5K & T53 file buffer
unsigned char picturebuff[PICTURE_BUFF_SIZE];   // BPP file buffer
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$



const char welcomeStr[]  = "Arduino BASIC (Xts)";

char autorun = 0;
bool selfRun = false; // for CHAIN "<...>" cmd

// =========/ Serial Event /==============
// String inputString = "";         // a String to hold incoming data
// volatile boolean stringComplete = false;  // whether the string is complete
volatile bool isWriting = false; // lock read when write
// =========/ Serial Event /==============

// WILL HAVE TO malloc(...)
// char screenBuffer[ (const int)(SCREEN_WIDTH*SCREEN_HEIGHT) ];
// char lineDirty[ (const int)(SCREEN_HEIGHT) ];

char* screenBuffer = NULL;
char* lineDirty = NULL;
char* line = NULL;

bool SCREEN_LOCKER = false;

void reconfigureScreen() {
    SCREEN_LOCKER = true;

    if ( screenBuffer != NULL ) { free(screenBuffer); } 
    if ( lineDirty != NULL ) { free(lineDirty); } 
    if ( line != NULL ) { free(line); } 

    screenBuffer = (char*)malloc( SCREEN_WIDTH*SCREEN_HEIGHT );
    lineDirty    = (char*)malloc( SCREEN_HEIGHT );
    line         = (char*)malloc( SCREEN_WIDTH+1 );

    memset( screenBuffer, 0x20, SCREEN_WIDTH*SCREEN_HEIGHT ); // 0x20 -> space
    memset( lineDirty, 0x00, SCREEN_HEIGHT );
    memset( line, 0x00, SCREEN_WIDTH+1 );

    SCREEN_LOCKER = false;

    //host_showBuffer();
}

// in tty characters
void setScreenSize(int cols, int rows) {
//   led1(true);
  SCREEN_LOCKER = true;

  SCREEN_WIDTH = cols; 
  SCREEN_HEIGHT = rows;
  reconfigureScreen();
//   led1(false);
}

// ============================================


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 

bool addAutorunFlag = false;

void setup() {

    // "Retour Chariot" + 115200bps
    Serial.begin(115200);

#ifdef BUT_ESP32
   esp32.setup();
   STORAGE_OK = true;
   // addAutorunFlag = true;
#endif

    // BUZZER_MUTE = true;
    // inputString.reserve(200);
    
    SCREEN_LOCKER = true;
    setConsoles(OUT_DEV_SERIAL, -1, -1);
    SCREEN_LOCKER = false;

    #ifdef BUILTIN_LCD
        setConsoles(OUT_DEV_LCD_MINI, -1, -1);
    #else
        // OUTPUT_DEVICE = OUT_DEV_SERIAL;
        // setScreenSize( SER_TEXT_WIDTH, SER_TEXT_HEIGHT );
    #endif

    setupHardware();

    // TO REMOVE...
    keyboard.begin(DataPin, IRQpin);
    //oled.ssd1306_init(SSD1306_SWITCHCAPVCC);

    reset();

#ifndef BUT_ESP32
    host_init(BUZZER_PIN);
#endif

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

    //   #ifdef USE_SDFAT_LIB
    //     host_outputString("SD : MODE FAT\n");
    //   #else
    //     host_outputString("SD : MODE LEG\n");
    //   #endif
    #endif

    host_showBuffer();

    // IF USING EXTERNAL EEPROM
    // The following line 'wipes' the external EEPROM and prepares
    // it for use. Uncomment it, upload the sketch, then comment it back
    // in again and upload again, if you use a new EEPROM.
    // writeExtEEPROM(0,0); writeExtEEPROM(1,0);

#ifndef BUT_ESP32
    if (EEPROM.read(0) == MAGIC_AUTORUN_NUMBER) {
        autorun = 1;
    }
    else {
        if ( !BUZZER_MUTE ) { host_startupTone(); }
    }
#else
    // addAutorunFlag = true;
#endif

}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

int doRun() {
  int ret;
  tokenBuf[0] = TOKEN_RUN;
  tokenBuf[1] = 0;
  selfRun = false;
  ret = processInput(tokenBuf);
  return ret;
}


bool MODE_EDITOR = false;
bool systemHalted = false;

bool wasNot = true;

void loop() {
    int ret = ERROR_NONE;

    if ( systemHalted ) {
        delay(200);
        return;
    }

    if (!autorun) {

        if ( addAutorunFlag ) {
          addAutorunFlag = false;
          reset();
        //   host_outputString("autorun ?\n");
        //   host_showBuffer();

            //   if( xts_loadBas("AUTORUN") == true ) {
            //     selfRun = true;
            //   } ....
            const char* _input = "CHAIN \"AUTORUN\" \n";

            ret = tokenize((unsigned char*)_input, tokenBuf, TOKEN_BUF_SIZE); 
            ret = processInput(tokenBuf);
            if ( ret > 0 ) { host_outputString((char *)errorTable[ret]); host_showBuffer(); }
            ret = ERROR_NONE;

            #ifdef BUT_ESP32 
             ret = doRun();
             if ( ret > 0 ) { host_outputString((char *)errorTable[ret]); host_showBuffer(); }
             ret = ERROR_NONE;
            #endif
        }

        // get a line from the user
        MODE_EDITOR = true;
        #ifdef ESP32_WIFI_SUPPORT

            char *input = NULL; 
            if ( telnet.isServerStarted() ) {
                telnet.runServerTick();

                if ( telnet.isClientConnected() ) {

                    if (wasNot) {
                        wasNot = false;
                        telnet.print("> Please press Enter");    
                        while( ( input = telnet.readLine() ) == NULL ) { delay(150); }
                    }

                    telnet.print("> ");
                    while( ( input = telnet.readLine() ) == NULL ) { delay(150); }
                    if ( strcmp( input, "/quit" ) == 0) { wasNot = true; telnet.print("Bye \n"); telnet.close(); }
                    else {
                        telnet.print(input);
                        telnet.print("\n");
                    }
                } else {
                    delay(500);
                    // BEWARE !!!!!!!
                    return;
                }

            } else {
                input = host_readLine();
            }

        #else
            char *input = host_readLine();
        #endif
        MODE_EDITOR = false;

        // special editor commands
        // Xtase modif
        //if (input[0] == '?' && input[1] == 0) {
        if (input[0] == '*' && input[1] == 0) {
            host_outputFreeMem(sysVARSTART - sysPROGEND);
            host_showBuffer();
            return;
        } else if (input[0] == '!' && input[1] == 0) {
            //input = (char*)"10 ? \"Coucou\"";
            // input = (char*)"10 PRINT \"12345\"";  // 12345  works
            input = (char*)"10 PRINT \"123456\""; // 123456 fails

            ret = tokenize((unsigned char*)input, tokenBuf, TOKEN_BUF_SIZE); ret = processInput(tokenBuf);
            if ( ret > 0 ) { host_outputString((char *)errorTable[ret]); host_showBuffer(); }
            ret = ERROR_NONE;
            
            //input = (char*)"20 FOR I=1 TO 5 : PRINT \"coucou\";I : NEXT I";
            input = (char*)"20 FOR I=1 TO 5 : PRINT INT( RND * 10 );\"  \";I : NEXT I";
            ret = tokenize((unsigned char*)input, tokenBuf, TOKEN_BUF_SIZE); ret = processInput(tokenBuf);
            if ( ret > 0 ) { host_outputString((char *)errorTable[ret]); host_showBuffer(); }
            ret = ERROR_NONE;
            
            input = (char*)"30 GOTO 10";
            ret = tokenize((unsigned char*)input, tokenBuf, TOKEN_BUF_SIZE); ret = processInput(tokenBuf);
            if ( ret > 0 ) { host_outputString((char *)errorTable[ret]); host_showBuffer(); }
            ret = ERROR_NONE;
            
            input = (char*)"LIST";
            ret = tokenize((unsigned char*)input, tokenBuf, TOKEN_BUF_SIZE); ret = processInput(tokenBuf);
            if ( ret > 0 ) { host_outputString((char *)errorTable[ret]); host_showBuffer(); }
            ret = ERROR_NONE;
            
        }

            // otherwise tokenize
            ret = tokenize((unsigned char*)input, tokenBuf, TOKEN_BUF_SIZE);


            led2( ret > 0 );

            // moa
            host_showBuffer();
            //delay(50);
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


        if ( selfRun ) { // for CHAIN "<...>" cmd
            // tokenBuf[0] = TOKEN_RUN;
            // tokenBuf[1] = 0;
            // selfRun = false;
            // ret = processInput(tokenBuf);
            ret = doRun();
        }

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
#ifdef COMPUTER
  // ========================

  int main(int argc, char** argv) {
    setupComputer();
    // system ("/bin/stty raw");
    setup();

    SDL_Event event;
    while(true) {
        loop();

		if ( SDL_PollEvent( &event ) ) {
			if ( event.type == SDL_QUIT ) {
                break;
            }
        }

PC_ISR(); // by SDL call

        delay( 10 );
    }

    // to call.....
    closeComputer();
    // system ("/bin/stty cooked");
  }
#endif
