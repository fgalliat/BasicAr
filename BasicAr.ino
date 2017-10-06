/*******************
* Xtase - fgalliat : fgalliat@gmail.com
* Based on robin edwards work - (https://github.com/robinhedwards/ArduinoBASIC)
*******************/
//#define REAL_ARDUINO 1
//#define ALL_DEVICES 1

#include "xts_arch.h"


// Teensy's doesn't supports FS (SD, SDFat) & PROGMEM routines

#ifdef FS_SUPPORT
    // // BEWARE : THIS IS NOT SDFAT_LIB
    // #include <SD.h>
    // #include <SPI.h>
    // File curFile;

    // for teensy 3.6
    // > https://github.com/greiman/SdFat-beta
    #include "SdFat.h"
    SdFatSdio sd;
    // SdFile file;
    // SdFile dirFile;
#endif


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
String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete
boolean isWriting = false; // lock read when write
// =========/ Serial Event /==============

char screenBuffer[SCREEN_WIDTH*SCREEN_HEIGHT];
char lineDirty[SCREEN_HEIGHT];


// ============================================


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//#define AUDIO_BUFF_SIZE (5*1024)
unsigned char audiobuff[AUDIO_BUFF_SIZE];
// void cleanAudioBuff() { for(int i=0; i < AUDIO_BUFF_SIZE; i++) { audiobuff[i]=0x00; } } 

// //void playTuneFromStorage(const char* tuneStreamName, int format = AUDIO_FORMAT_T5K, bool btnStop = false) {
// void playTuneFromStorage(char* tuneStreamName, int format = AUDIO_FORMAT_T5K, bool btnStop = false) {

//   if ( !STORAGE_OK ) {
//     host_outputString("ERR : Storage not ready\n");
//     return;
//   }

//   cleanAudioBuff();

  
  
//   #ifndef FS_SUPPORT
//     host_outputString("ERR : NO Storage support\n");
//   #else

//   led3(true);

//   SdFile curFile;

//   if (! curFile.open("monkey.t5k", O_READ) ) {
//   //  host_outputString( "ERR : Opening : " );
//   //  host_outputString( (char*)tuneStreamName );
//   //  host_outputString( "\n" );
//    led1(true);
//    return;        
//   }

//   curFile.close();

//    led2(true);
//    #endif
// }
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void setup() {

    #ifdef FS_SUPPORT
         //if (!SD.begin(BUILTIN_SDCARD)) { // Teensy3.6 initialization
         if (!sd.begin()) { // Teensy3.6 initialization
            led3(true);   delay(500);
            led3(false);  delay(500);
            led3(true);   delay(500);
            led3(false);  delay(500);
           return;
         }
       led1(true);   delay(500);
       led1(false);  delay(500);
       STORAGE_OK = true;

    // while (!Serial) {
    //     SysCall::yield();
    //   }
    //   Serial.println("Type any character to start");
    //   while (!Serial.available()) {
    //     SysCall::yield();
    //   }
    //   if (!sd.begin()) {
    //     sd.initErrorHalt("sd.begin() failed");
    //   }
    //   if (!dirFile.open("/", O_READ)) {
    //     sd.errorHalt("dirFile.open failed");
    //   }
    //   while (file.openNext(&dirFile, O_READ)) {
    //     if (!file.isSubDir() && !file.isHidden()) {
    //       file.printFileSize(&Serial);
    //       Serial.write(' ');
    //       file.printModifyDateTime(&Serial);
    //       Serial.write(' ');
    //       file.printName(&Serial);
    //       if (file.isDir()) {
    //         // Indicate a directory.
    //         Serial.write('/');
    //       }
    //       Serial.println();
    //     }
    //     file.close();
    //   }
    //   Serial.println("Done!");


    #endif // FS_SUPPORT

    BUZZER_MUTE = true;
    inputString.reserve(200);
    
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

    
}




void loop() {
    int ret = ERROR_NONE;

// // print the string when a newline arrives:
// if (stringComplete) {
//     Serial.println("Ya typed : ");
//     Serial.println(inputString);
//     // clear the string:
//     inputString = "";
//     stringComplete = false;
//   }

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

        host_outputString("RET = ");
        host_outputInt(ret);
        host_outputString("\n");
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
    //while ( isWriting ) { delay(5); Serial.print('X'); }
    Serial.flush();


    while (Serial.available()) {
      // get the new byte:
      char inChar = (char)Serial.read();
      // add it to the inputString:
      inputString += inChar;
      // if the incoming character is a newline, set a flag so the main loop can
      // do something about it:
      if (inChar == '\n'|| inChar == '\r' ) {
        stringComplete = true;
      }
    }
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
