/***************
* Xtase - fgalliat @ Sept2017
* Hardware Layer
****************/

#ifndef COMPUTER
  #include <Arduino.h>
#else 
  #include "computer.h"
#endif

#include "xts_arch.h"
#include "xts_io.h"


#define BASIC_ASCII_FILE_EXT ".BAS"

#ifdef FS_SUPPORT
// // BEWARE : THIS IS REGULAR Arduino SD LIB
// #include <SD.h>
// #include <SPI.h>
// File curFile;

  char SDentryName[13];

  #ifdef USE_SDFAT_LIB
  // for teensy 3.6
  // > https://github.com/greiman/SdFat-beta
    #include "SdFat.h"
    SdFatSdio sd;
    SdFile file;
    SdFile dirFile;
  #elif USE_FS_LEGACY
    // ex. regular computer
    SDClass sd;
    SdFile file;
    SdFile dirFile;
  #endif


  // BEWARE : uses & modifies : SDentryName[]
  // ex. autocomplete_fileExt(filename, ".BAS") => SDentryName[] contains full file-name
  // assumes that ext is stricly 3 char long
  char* autocomplete_fileExt(char* filename, const char* defFileExt) {
    int flen = strlen(filename);
    memset(SDentryName, 0x00, 8+1+3+1); // 13 char long
    memcpy(SDentryName, filename, flen );
    if ( flen < 4 || filename[ flen-3 ] != '.' ) {
      strcat( SDentryName, defFileExt );
    }
    return SDentryName;
  }

#else
  char* autocomplete_fileExt(char* filename, const char* defFileExt) {
    // TODO
    Serial.println("autocomplete_fileExt NYI.");
    return "TOTO.BAS";
  }
#endif


#include "host_xts.h"

#include "basic.h"

extern int SCREEN_HEIGHT;
extern unsigned char tokenBuf[];

extern int OUTPUT_DEVICE;
extern int GFX_DEVICE;
extern int INPUT_DEVICE;

extern char codeLine[];
void cleanCodeLine() {
  memset(codeLine, 0x00, ASCII_CODELINE_SIZE);
}

extern unsigned char audiobuff[];
void cleanAudioBuff() { memset(audiobuff, 0x00, AUDIO_BUFF_SIZE); } 

extern unsigned char picturebuff[];
void cleanPictureBuff() { memset(picturebuff, 0x00, PICTURE_BUFF_SIZE); } 

extern int curToken;

// ============ Tmp Compatibility Code ===============
extern bool isWriting;

void lcd_println(char* text) { isWriting = true; Serial.print("LCD:"); Serial.println(text); isWriting = false; }

bool checkbreak() { return false; }

// ============ Tmp Compatibility Code =============== 



#ifdef BOARD_VGA
  #include "dev_screen_VGATEXT.h"
#endif


#ifdef BOARD_RPID
  #include "dev_screen_RPIGFX.h"
#endif

#ifdef BUILTIN_KBD
  #include "dev_kbd.h"
#endif

#ifdef BOARD_SND
  #include "dev_sound_dfplayer.h"
#endif


 // external forward decl.
 char charUpCase(char ch);
 bool endsWith(char* str, char* what);

 void host_outputString(char* str);
 int host_outputInt(long v);
 void host_outputChar(char c);
 void host_showBuffer();

// ===============================

bool STORAGE_OK = false;


#ifdef FS_SUPPORT

 void setupSD() {
   #ifdef USE_SDFAT_LIB
     if (!sd.begin()) {
   #else
     if (!SD.begin(BUILTIN_SDCARD)) { // Teensy3.6 initialization
   #endif
        led3(true);   delay(500);
        led3(false);  delay(500);
        led3(true);   delay(500);
        led3(false);  delay(500);
       return;
     }
   led1(true);   delay(500);
   led1(false);  delay(500);
 
   #ifdef USE_SDFAT_LIB
     sd.chvol();
   #endif
   STORAGE_OK = true;
 }
#endif // FS_SUPPORT

// ====== GPIO initialization ======

void setupGPIO() {
 if ( BUZZER_PIN > -1 ) { pinMode(BUZZER_PIN, OUTPUT); digitalWrite(BUZZER_PIN, LOW);  }

 if ( LED1_PIN > -1 ) { pinMode(LED1_PIN, OUTPUT); digitalWrite(LED1_PIN, LOW); }
 if ( LED2_PIN > -1 ) { pinMode(LED2_PIN, OUTPUT); digitalWrite(LED2_PIN, LOW); }
 if ( LED3_PIN > -1 ) { pinMode(LED3_PIN, OUTPUT); digitalWrite(LED3_PIN, LOW); }

 if ( BTN1_PIN > -1 ) { pinMode(BTN1_PIN, INPUT_PULLUP); }
 if ( BTN2_PIN > -1 ) { pinMode(BTN2_PIN, INPUT_PULLUP); }
 if ( BTN3_PIN > -1 ) { pinMode(BTN3_PIN, INPUT_PULLUP); }
}

// ====== HARD initialization ======

#ifdef BUILTIN_LCD
 // Teensy3.6 XtsuBasic hardware config

#include <SPI.h>
#include <i2c_t3.h>
#include <Adafruit_GFX.h>
#include "dev_screen_Adafruit_SSD1306.h"
//default is 4 that is an I2C pin on t3.6
#define OLED_RESET 6
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
 #error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setupLCD() {
  Wire2.begin(I2C_MASTER, 0x00, I2C_PINS_3_4, I2C_PULLUP_EXT, 400000);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  
  // tmp try
  display.setTextColor(WHITE);
  //display.setTextColor(INVERSE);
  // tmp try

  // avoid firware LOGO display
  display.clearDisplay();

  display.setCursor(0,0);
  display.println( "LCD Ready" );

  display.display();
}

#endif

#ifdef BOARD_VGA
void setupVGASerial() {
  host_outputString("Booting VGA\n");
  setup_vgat(false);
  //vgat_reboot(false);
  delay(300);
  vgat_startScreen();
  // host_outputString("Booted VGA\n");
  delay(200);
}
#endif

#ifdef BOARD_RPID
// TODO : better
void setupRPISerial() {
  host_outputString("Booting RPID\n");
  setup_rpid(false);
  //vgat_reboot(false);
  //delay(300);
  rpid_startScreen();
  // host_outputString("Booted VGA\n");
  delay(200);
}
#endif



#ifdef BUT_TEENSY
  //#include <TimerOne.h> // for Teensy 2 & 2++ ONLY

  //IntervalTimer myTimer;

  // code in host.cpp
  extern void _ISR_emul();
#endif


void setupHardware() {
 setupGPIO();

 #ifdef BUILTIN_LCD
   setupLCD();
 #endif

 #ifdef BUILTIN_KBD
   setup_kbd(); // must be done before VGASerial
 #endif

 #ifdef BOARD_VGA
   setupVGASerial();
 #endif

 #ifdef BOARD_RPID
   setupRPISerial();
 #endif

 #ifdef BOARD_SND
   setupSoundDFPlayer();
 #endif

 #ifdef FS_SUPPORT
   setupSD();
 #endif

 #ifdef BUT_TEENSY
   //for Teensy 2 & 2++
   //Timer1.initialize(350000); // 350ms 
   //Timer1.attachInterrupt( _ISR_emul );


  //  myTimer.begin(_ISR_emul, 150000); // 150ms
  //  myTimer.priority( 20 ); // 0 maximum priority
   
 #endif
}

#ifdef BUT_TEENSY
  void _noInterrupts() {
  //   myTimer.end();
  }

  void _interrupts() {
    // myTimer.begin(_ISR_emul, 150000); // 150ms
    // myTimer.priority( 20 ); // 0 maximum priority
  }
#endif




 // ========= Btns Sub System =======
 // as btns are plugged as INPUT_PULLUP
 // logic is inverted
 bool _btn(int pin) {
  if ( pin > -1 ) { return digitalRead(pin) == LOW; }
  return false;
 }

 bool btn1() {
  return _btn( BTN1_PIN );
 }

 bool btn2() {
  return _btn( BTN2_PIN );
 }

 bool btn3() {
  return _btn( BTN3_PIN );
 }

bool anyBtn() { return btn1() || btn2() || btn3(); }


 // ========= Led Sub System ========

// BEWARE : 1-based
void led(int ledID, bool state) {
  if ( ledID >= 1 && ledID <= 3 ) {
    if ( ledID == 1 && LED1_PIN > -1 ) { digitalWrite(LED1_PIN, state ? HIGH : LOW); }
    if ( ledID == 2 && LED2_PIN > -1 ) { digitalWrite(LED2_PIN, state ? HIGH : LOW); }
    if ( ledID == 3 && LED3_PIN > -1 ) { digitalWrite(LED3_PIN, state ? HIGH : LOW); }
  }
  // no error .... 
}

void led1(bool state) { led(1, state); }
void led2(bool state) { led(2, state); }
void led3(bool state) { led(3, state); }

void activityLed(bool state) {
  led(1, state);
}

 // ======= Sound Sub System =======
 #include "xtase_tone.h"
 typedef struct Note {
  unsigned char note;
  unsigned short duration;
 } Note;

//  enum {
//    AUDIO_FORMAT_T5K = 0,
//    AUDIO_FORMAT_T53,
//  };

extern bool BUZZER_MUTE;
#define BUZZER BUZZER_PIN

void playNote(int note_freq, int duration) {
  if ( BUZZER_MUTE ) { return; }
  if ( note_freq >= 1 && note_freq <= 48 ) {
    // 0..48 octave2 to 5
    note_freq = notes[ note_freq-1 ];
  } else if ( note_freq >= 49 && note_freq <= 4096 ) {
    // 49..4096 -> 19200/note in Hz
    note_freq *= 20;
  } else {
    note_freq = 0;
  }

  noTone(BUZZER_PIN);
  tone(BUZZER_PIN, note_freq, duration*50);
  delay(duration*50);
}

void playTuneString(char* strTune) {
  noTone(BUZZER_PIN);

  int defDuration = 200;
  int slen = strlen( strTune );
  for (int i=0; i < slen; i++) {
    char ch = strTune[i];
    ch = charUpCase(ch);
    bool sharp = false;
    if ( i+1 < slen && strTune[i+1] == '#' ) { 
      sharp = true; 
      i++; 
    }  

    int pitch = 0;
    switch (ch) {
        case 'C' :
          if ( sharp ) { pitch = notes[ NOTE_CS4 ]; }
          else pitch = notes[ NOTE_C4 ];
        break;
        case 'D' :
          if ( sharp ) { pitch = notes[ NOTE_DS4 ]; }
          else pitch = notes[ NOTE_D4 ];
        break;
        case 'E' :
          pitch = notes[ NOTE_E4 ];
        break;
        case 'F' :
          if ( sharp ) { pitch = notes[ NOTE_FS4 ]; }
          else pitch = notes[ NOTE_F4 ];
        break;
        case 'G' :
          if ( sharp ) { pitch = notes[ NOTE_GS4 ]; }
          else pitch = notes[ NOTE_G4 ];
        break;
        case 'A' :
          if ( sharp ) { pitch = notes[ NOTE_AS4 ]; }
          else pitch = notes[ NOTE_A4 ];
        break;
        case 'B' :
          pitch = notes[ NOTE_B4 ];
        break;
    }

   if (!BUZZER_MUTE) tone(BUZZER, pitch, defDuration);
   delay(defDuration);

  }
} // end of playTuneStreamSTring


// T5K Format
void __playTune(unsigned char* tuneStream, bool btnStop);
// T53
void __playTuneT53(unsigned char* tuneStream, bool btnStop);



//void playTuneFromStorage(const char* tuneStreamName, int format = AUDIO_FORMAT_T5K, bool btnStop = false) {
void playTuneFromStorage(char* tuneStreamName, int format = AUDIO_FORMAT_T5K, bool btnStop = false) {

  if ( !STORAGE_OK ) {
    host_outputString("ERR : Storage not ready\n");
    return;
  }

  cleanAudioBuff();

  #ifndef FS_SUPPORT
    host_outputString("ERR : NO Storage support\n");
  #else

  // SdFile zik("monkey.t5k", O_READ);

  led3(true);

  int tlen = strlen(tuneStreamName);
  if ( tlen < 1 ) {
    host_outputString("ERR : NO file provided\n");
    led3(false);
    return;
  }

  char lastCh = tuneStreamName[ tlen-1 ];
  if ( lastCh >= 'a' && lastCh <= 'z' ) { lastCh = lastCh - 'a' + 'A'; }
  tuneStreamName[ tlen-1 ] = lastCh;

  char ftuneStreamName[8+1+3];
  strcpy(ftuneStreamName, tuneStreamName);

  if ( !(( lastCh == 'K' || lastCh == '3') && (tlen >= 2 && tuneStreamName[ tlen-2 ] == '5') ) ) {
    if ( format == AUDIO_FORMAT_T5K ) {
      strcat(ftuneStreamName, ".T5K");
    } else {
      strcat(ftuneStreamName, ".T53");
    }
  }

  // regular SD lib
  // curFile = SD.open("monkey.t5k", FILE_READ);
  // if (!curFile) {
  //  led1(true);
  //  return;        
  // }

  // sdfat lib
  SdFile file;
  if (! file.open( ftuneStreamName , O_READ) ) {
    led1(true);
    return;        
  }

  //  host_outputString( "OK : Opening : " );
  //  host_outputString( (char*)tuneStreamName );
  //  host_outputString( "\n" );
  //zik.rewind();

  int nbNotes = (file.read()<<8)|file.read();
  file.seekSet(0);
  int fileLen = (nbNotes*sizeof(Note))+2+16+2;
  if ( format == AUDIO_FORMAT_T53 ) {
    fileLen = (nbNotes*(3+3+3))+2+16+2;
  }
  //file.readBytes( audiobuff, fileLen );
  file.read( audiobuff, fileLen );

  led3(false);
  file.close();

  if ( format == AUDIO_FORMAT_T5K ) {
    __playTune( audiobuff, btnStop );  
  } else {
    __playTuneT53( audiobuff, btnStop );  
  }

  #endif
  
 }
 
// where tuneStream is the audio buffer content
void __playTune(unsigned char* tuneStream, bool btnStop = false) {
  noTone(BUZZER_PIN);

  short nbNotes = (*tuneStream++ << 8) | (*tuneStream++);
  char songname[16];
  for(int i=0; i < 16; i++) {
    songname[i] = *tuneStream++;
  }
  short tempoPercent = (*tuneStream++ << 8) | (*tuneStream++);

  //printfln("nbN:%d title:'%s' tmp:%d", nbNotes, (const char*)songname, tempoPercent);
  //if ( !true ) {
  #ifdef COMPUTER
    host_outputString("   -= Playing =-\n");
    host_outputString( songname );
    host_outputString("\n");
    host_showBuffer();
  #endif
  //}

  // #if ((defined SCREEN_SUPPORT) && (SCREEN_SUPPORT > 0))
  //   //lcd_cls();
  //   //           12345678901234567890
  //   lcd_println("   -= Playing =-\n");
  //   lcd_println("-=%s=-", (const char*)songname);
  // #endif

  float tempo = (float)tempoPercent / 100.0;
  // cf a bit too slow (Cf decoding)
  tempo *= 0.97;

  for (int thisNote = 0; thisNote < nbNotes; thisNote++) {
    int note = *tuneStream++;
    short duration = (*tuneStream++ << 8) | (*tuneStream++);
    // note 0 -> silence
    if ( note > 0 ) {
      if (!BUZZER_MUTE) tone(BUZZER, notes[ note-1 ], duration);

      // just a try
      led2( note > 30 );
      led3( note > 36 );
      // just a try


    }
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 10% seems to work well => 1.10:
    int pauseBetweenNotes = duration * tempo;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(BUZZER);

    if (btnStop && anyBtn() ) {
      led2(false);
      led3(false);
      return;
    }

    if ( checkbreak() ) { 
      #if ((defined SCREEN_SUPPORT) && (SCREEN_SUPPORT > 0))
        lcd_println("STOP"); 
      #endif
      led2(false);
      led3(false);
      return; 
    }
  }
   led2(false);
   led3(false);
 }

 // T53 Format
 // where tuneStream is the audio buffer content
void __playTuneT53(unsigned char* tuneStream, bool btnStop = false) {
  noTone(BUZZER_PIN);
  
  short nbNotes = (*tuneStream++ << 8) | (*tuneStream++);
  char songname[16];
  for(int i=0; i < 16; i++) {
    songname[i] = *tuneStream++;
  }
  short tempoPercent = (*tuneStream++ << 8) | (*tuneStream++);

  //printfln("nbN:%d title:'%s' tmp:%d", nbNotes, (const char*)songname, tempoPercent);
  if ( !true ) {
    host_outputString("   -= Playing =-");
    host_outputString( songname );
    host_outputString("\n");
    host_showBuffer();
  }
  
  // #if ((defined SCREEN_SUPPORT) && (SCREEN_SUPPORT > 0))
  //   //lcd_cls();
  //   //           12345678901234567890
  //   lcd_println("   -= Playing =-");
  //   lcd_println("-=%s=-", (const char*)songname);
  // #endif

  float tempo = (float)tempoPercent / 100.0;
  // cf a bit too slow (Cf decoding)
  tempo *= 0.97;

  for (int thisNote = 0; thisNote < nbNotes; thisNote++) {
    short note = (*tuneStream++ << 8) | (*tuneStream++);
    short duration = (*tuneStream++ << 8) | (*tuneStream++);
    short wait = (*tuneStream++ << 8) | (*tuneStream++);
    
    // note 0 -> silence
    if ( note > 0 ) {
      if (!BUZZER_MUTE) tone(BUZZER, note, duration);
    }

    delay(wait*tempo);
    // stop the tone playing:
    noTone(BUZZER);

    if (btnStop && anyBtn() ) {
      return;
    }

    if ( checkbreak() ) { 
      #if ((defined SCREEN_SUPPORT) && (SCREEN_SUPPORT > 0))
        lcd_println("STOP"); 
      #endif
      return; 
    }
  }
   
 }

// ==============================================
// =
// = Graphical Ops.
// =
// ==============================================
//#define WHITE 1
//#define BLACK 0

void drawLine(int x1, int y1, int x2, int y2) {
  if ( GFX_DEVICE == GFX_DEV_LCD_MINI ) {
    #ifdef BUILTIN_LCD
    display.drawLine(x1, y1, x2, y2, WHITE);
    display.display();
    #endif
  }
}

void drawCircle(int x1, int y1, int radius) {
  if ( GFX_DEVICE == GFX_DEV_LCD_MINI ) {
    #ifdef BUILTIN_LCD
    display.drawCircle(x1, y1, radius, WHITE);
    display.display();
    #endif
  }
}

// 0: black else :white
void drawPixel(int x1, int y1, int color) {
  if ( GFX_DEVICE == GFX_DEV_LCD_MINI ) {
    #ifdef BUILTIN_LCD
    display.drawPixel(x1, y1, color);
    display.display(); // see if fast enought .... else use interrupts.
    #endif 
  }
}



bool drawBPPfile(char* filename) {

  #ifndef FS_SUPPORT
    host_outputString("ERR : storage not supported\n");
    return false;
  #endif

  if ( ! STORAGE_OK ) {
    host_outputString("ERR : storage not ready\n");
    return false;
  }

  if ( filename == NULL || strlen(filename) <= 0 ) { 
    host_outputString("ERR : invalid file\n");
    return false;
  }

  autocomplete_fileExt(filename, ".BPP");
  
  #ifdef FS_SUPPORT
  // SFATLIB mode -> have to switch for regular SD lib
  SdFile file;
  if (! file.open( SDentryName , FILE_READ) ) {
    host_outputString("ERR : File not ready\n");
    return false;        
  }

  file.seekSet(0);
  int n;
  if ( (n = file.read(&picturebuff, PICTURE_BUFF_SIZE)) != PICTURE_BUFF_SIZE ) {
    host_outputString("ERR : File not valid\n");
    host_outputInt( n );
    host_outputString(" bytes read\n");
    file.close();
    return false;
  }
  file.close();
  #endif

  // do something w/ these bytes ...
  if ( GFX_DEVICE == GFX_DEV_LCD_MINI ) {
    #ifdef BUILTIN_LCD
      display.clearDisplay();
      display.drawBitmap(0, 0, picturebuff, 128, 64, 0x01);
      display.display();
    #endif
  }

  return true;
}



// ==============================================
// =
// = FileSystem Ops.
// =
// ==============================================

 #ifndef FS_SUPPORT
  void lsStorage(char* filter=NULL, bool sendToArray=false) {
    host_outputString("ERR : NO Storage support\n");
  }
 #else
  extern int curY;
  bool _lsStorage(SdFile dirFile, int numTabs, bool recurse, char* filter, bool sendToArray);

  // bool _lsStorage2(File dir, int numTabs, bool recurse, char* filter) {
  //   while(true) {
      
  //     File entry =  dir.openNextFile();
  //     if (! entry) {
  //       // no more files
  //       //Serial.println("**nomorefiles**");
  //       break;
  //     }
  //     for (uint8_t i=0; i<numTabs; i++) {
  //       //Serial.print('\t');
  //       host_outputString(" ");
  //     }
  //     //Serial.print(entry.name());
  //     host_outputString("entry name");
  //     if (entry.isDirectory()) {
  //       //Serial.println("/");
  //       host_outputString("/\n");
  //       //printDirectory(entry, numTabs+1);
  //     } else {
  //       // files have sizes, directories do not
  //       // Serial.print("\t\t");
  //       // Serial.println(entry.size(), DEC);
  //       host_outputString("*\n");
  //     }
  //     entry.close();
  //   }
  //   host_showBuffer();
  // }


  // filter can be "*.BAS"
  // sendToArray == true -> create "DIR$" array & fill it
  //   instead of sending to console display
  void lsStorage(char* filter=NULL, bool sendToArray=false) {
    bool recurse = false;

    // SdFile dirFile;

    if ( !STORAGE_OK ) {
      host_outputString("ERR : Storage not ready\n");
      return;
    }

    if (!dirFile.open("/", O_READ)) {
      host_outputString("ERR : opening SD root failed\n");
      return;
    }

//root = SD.open("/");

    _lsStorage(dirFile, 0, recurse, filter, sendToArray);
    //_lsStorage2(root, 0, recurse, filter);

// root.close();

    dirFile.close();
  }


//  #ifndef SCREEN_HEIGHT
//   #define SCREEN_HEIGHT       8
//  #endif

bool _lsStorage(SdFile dirFile, int numTabs, bool recurse, char* filter, bool sendToArray) {
    SdFile file;

    if ( sendToArray ) {
      // can't use createArray because it use expr stack
      #define MAX_FILE_IN_ARRAY 128
      if ( ! xts_createArray("DIR$", 1, MAX_FILE_IN_ARRAY) ) {
        host_outputString("Could not create DIR$");
        return false;
      }
    }

    int cpt = 0;
    while (file.openNext(&dirFile, O_READ)) {
      if (!file.isSubDir() && !file.isHidden() ) {
        // //file.printFileSize(&Serial);
        // //file.printModifyDateTime(&Serial);
        // //file.printName(&Serial);

        memset(SDentryName, 0x00, 13);
        file.getName( SDentryName, 13 );

        if ( filter != NULL ) {
          // TODO : optimize !!!!!
          int strt = 0;
          if ( filter[strt] == '*' ) { strt++; }
          if ( filter[strt] == '.' ) { strt++; } // see 'else' case

          int tlen = strlen(SDentryName);
          int flen = strlen(filter);
          if ( tlen > 4 ) {
            if ( SDentryName[ tlen-1-3 ] == '.' ) {
              bool valid = true;
              for(int i=0; i < flen-strt; i++) {

                //host_outputChar( SDentryName[ tlen-3+i ] ); host_outputChar(' ');host_outputChar(filter[strt+i]);host_outputChar('\n');

                if ( charUpCase( SDentryName[ tlen-3+i ] ) != charUpCase(filter[strt+i]) ) {
                  valid = false;
                  break;
                }
              }
              if ( !valid ) { file.close(); continue; }
            } else {
              file.close(); 
              continue;
            }
          } else {
            file.close(); 
            continue;
          }
        }

        if ( !sendToArray ) {
          host_outputInt( (cpt+1) );
          host_outputString("\t");
          host_outputString(SDentryName);
        } else {
          // SDentryName @ (cpt+1)

          // to check
          if ( file.isDir() ) { SDentryName[ 13-1 ] = '/'; }
          else                { SDentryName[ 13-1 ] = 0x00; }

          if ( xts_setStrArrayElem( "DIR$", (cpt+1), SDentryName ) != ERROR_NONE ) {
            host_outputInt( (cpt+1) );
            host_outputString("\t");
            host_outputString(SDentryName);
            host_outputString(" : ERROR\n");

            break;
          }
        }

        if ( file.isDir() ) {
          // Indicate a directory.
          if ( !sendToArray ) {
            host_outputString("/");
          }
        }
        if ( !sendToArray ) {
          host_outputString("\n");

          // TMP - DIRTY ----- begin
          if ( cpt % SCREEN_HEIGHT == SCREEN_HEIGHT-1 ) {
            host_showBuffer();
          }
          // TMP - DIRTY ----- end
        }

        cpt++;

      }
      file.close();
    }

    if ( sendToArray ) {
      xts_setStrArrayElem( "DIR$", (cpt+1), "-EOF-" );
    } else {
      host_outputString("nb files : ");
      host_outputInt( cpt );
      host_outputString("\n");
      host_showBuffer();
    }

    return true;
  }

 #endif // FS_SUPPORT


 // ==== Load/Save source code (ascii) from SDCard ====

 #ifndef FS_SUPPORT
  void loadAsciiBas(char* filename) {
    host_outputString("ERR : NO Storage support\n");
    host_showBuffer();
  }

  void saveAsciiBas(char* filename) {
    host_outputString("ERR : NO Storage support\n");
    host_showBuffer();
  }

  void llistAsciiBas(char* filename) {
    if ( filename == NULL ) {
      // TODO
      host_outputString("TODO: dump PRGM to Serial\n");
      host_showBuffer();
      return;
    }
    host_outputString("ERR : NO Storage support\n");
    host_showBuffer();
  }


 #else
 void loadAsciiBas(char* filename) {
  if ( !STORAGE_OK ) {
    host_outputString("ERR : Storage not ready\n");
    host_showBuffer();
    return;
  }

  autocomplete_fileExt(filename, BASIC_ASCII_FILE_EXT);

  // SFATLIB mode -> have to switch for regular SD lib
  SdFile file;
  if (! file.open( SDentryName , O_READ) ) {
    led1(true);
    host_outputString("ERR : File not ready\n");
    host_showBuffer();
    return;        
  }

  file.seekSet(0);

  int n;

  //reset(); // aka NEW -- no more Cf saveLoadCmd() call

  cleanCodeLine();
  memset( tokenBuf, 0x00, TOKEN_BUF_SIZE );
  while( ( n = file.fgets(codeLine, ASCII_CODELINE_SIZE) ) > 0 ) {
    // // show line
    // host_outputString( codeLine );
    // if ( codeLine[n-1] != '\n' ) {
    //   host_outputString( "\n" );
    // }
    // host_showBuffer();

    // interpret line
    int ret = tokenize((unsigned char*)codeLine, tokenBuf, TOKEN_BUF_SIZE); 
    if (ret == 0) { ret = processInput(tokenBuf); }
    if ( ret > 0 ) { 
      //host_outputInt( curToken );
      host_outputString((char *)codeLine);
      host_outputString((char *)" ->");
      host_outputString((char *)errorTable[ret]); 
      host_outputString((char *)"\n");
      host_showBuffer(); 
    }
    //ret = ERROR_NONE;
    cleanCodeLine();
    memset( tokenBuf, 0x00, TOKEN_BUF_SIZE );
  }
  file.close();

  host_outputString( "-EOF-\n" );
  host_showBuffer();
}


void _serializeTokens(unsigned char *p, char* destLine) {
  int modeREM = 0;
  while (*p != TOKEN_EOL) {
      if (*p == TOKEN_IDENT) {
          p++;
          while (*p < 0x80) {
              //host_outputChar(*p++);
              (*destLine++) = (*p++);
          }
          //host_outputChar((*p++)-0x80);
          (*destLine++) = ((*p++)-0x80);
      }
      else if (*p == TOKEN_NUMBER) {
          p++;
          static char tmp[15];
          //host_outputFloat(*(float*)p);
          sprintf( tmp, "%f", *(float*)p ); 
          strcat( destLine, tmp );
          destLine+=strlen(tmp); // BEWARE !!!!+ optim 
          p+=4;
      }
      else if (*p == TOKEN_INTEGER) {
          p++;
          // host_outputInt(*(long*)p);
          static char tmp[15];
          sprintf( tmp, "%d", *(long*)p ); 
          strcat( destLine, tmp );
          destLine+=strlen(tmp); // BEWARE !!!!+ optim 
          p+=4;
      }
      else if (*p == TOKEN_STRING) {
          p++;
          if (modeREM) {
              //host_outputString((char*)p);
              (*destLine) = (*p);
              p+=1 + strlen((char*)p);
              destLine+=1 + strlen((char*)p);//BEWARE!!!!+ optim 
          }
          else {
              //host_outputChar('\"');
              (*destLine++) = ('\"');
              while (*p) {
                  if (*p == '\"') {
                    //host_outputChar('\"');
                    (*destLine++) = ('\"');
                  }
                  //host_outputChar(*p++);
                  (*destLine++) = (*p++);
              }
              // host_outputChar('\"');
              (*destLine++) = ('\"');
              p++;
          }
      }
      else {
          //uint8_t fmt = pgm_read_byte_near(&tokenTable[*p].format);
          uint8_t fmt = tokenTable[*p].format;

          if (fmt & TKN_FMT_PRE) {
              //host_outputChar(' ');
              (*destLine++) = (' ');
          }

          //host_outputString((char *)pgm_read_word(&tokenTable[*p].token));
          //host_outputString((char *)tokenTable[*p].token);
          strcat(destLine, (char *)tokenTable[*p].token );
          destLine += strlen( (char *)tokenTable[*p].token ); // BEWARE + optim !!!

          if (fmt & TKN_FMT_POST) {
              //host_outputChar(' ');
              (*destLine++) = (' ');
          }
          if (*p==TOKEN_REM || *p==TOKEN_REM_EM) {
              modeREM = 1;
          }
          p++;
      }
  }
}





void saveAsciiBas(char* filename) {
  if ( !STORAGE_OK ) {
    host_outputString("ERR : Storage not ready\n");
    host_showBuffer();
    return;
  }

  autocomplete_fileExt(filename, BASIC_ASCII_FILE_EXT);

  // SFATLIB mode -> have to switch for regular SD lib
  sd.remove( SDentryName );
  SdFile file;

  if (! file.open( SDentryName , FILE_WRITE) ) {
    led1(true);
    host_outputString("ERR : File not ready\n");
    host_showBuffer();
    return;        
  }

  file.seekSet(0);

  // file.print("coucou1"); file.print("\n");
  // file.flush();

  cleanCodeLine();
  unsigned char *p = &mem[0];
  while (p < &mem[sysPROGEND]) {
      uint16_t lineNum = *(uint16_t*)(p+2);
          file.print(lineNum);
          file.print(" ");

          //printTokens(p+4);
          _serializeTokens(p+4, codeLine);
          file.print(codeLine);
          cleanCodeLine();

          file.print("\n");
      p+= *(uint16_t *)p;
  }
  file.flush();

  
  host_outputString( "-EOF-\n" );
  host_showBuffer();

  file.close();
}

// ====== LLIST ==================
#define outSerial Serial

void llistAsciiBas(char* filename=NULL) {
  if ( filename == NULL ) {
    // just DUMP current PRGM to Serial
    cleanCodeLine();
    unsigned char *p = &mem[0];
    while (p < &mem[sysPROGEND]) {
        uint16_t lineNum = *(uint16_t*)(p+2);
        outSerial.print(lineNum);
        outSerial.print(" ");
  
        _serializeTokens(p+4, codeLine);
        outSerial.print(codeLine);
        cleanCodeLine();
  
        outSerial.print("\n");
        p+= *(uint16_t *)p;
    }
    outSerial.print("-EOF-\n");
    outSerial.flush();
    return;
  }

  // ==== ELSE : DUMP PLAIN ASCII BASIC FILE TO SERIAL ====

  if ( !STORAGE_OK ) {
    host_outputString("ERR : Storage not ready\n");
    host_showBuffer();
    return;
  }

  autocomplete_fileExt(filename, BASIC_ASCII_FILE_EXT);

  // SFATLIB mode -> have to switch for regular SD lib
  SdFile file;
  if (! file.open( SDentryName , O_READ) ) {
    led1(true);
    host_outputString("ERR : File not ready\n");
    host_showBuffer();
    return;        
  }

  file.seekSet(0);

  int n;

  cleanCodeLine();
  while( ( n = file.fgets(codeLine, ASCII_CODELINE_SIZE) ) > 0 ) {
    outSerial.print( codeLine );
    if ( codeLine[n-1] != '\n' ) {
      outSerial.print( "\n" );
    }
  }
  outSerial.print( "-EOF-\n" );
  outSerial.flush();

  file.close();
}



void deleteBasFile(char* filename) {
  if ( !STORAGE_OK ) {
    host_outputString("ERR : Storage not ready\n");
    host_showBuffer();
    return;
  }

  autocomplete_fileExt(filename, BASIC_ASCII_FILE_EXT);

  // SFATLIB mode -> have to switch for regular SD lib
  sd.remove( SDentryName );
}


 #endif




int MCU_freeRam() {
  // THIS IMPL : just return the space used by the sketch
  // we have to substract varMem & prgmMmem

#ifdef COMPUTER
  int fr = 16*1024*1024; // 16MB
#else
  //extern int __heap_start, *__brkval; 
  extern int *__brkval; 
  int v; 
  //int fr = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
  int fr = (int) &v - ((int) __brkval); 
#endif
   Serial.print("Free ram: ");
   Serial.println(fr);
return fr;
}


void MCU_reset() {
  // _restart_Teensyduino_();
  host_outputString("\nRebooting\n");
  host_showBuffer();
  #ifdef BUT_TEENSY
    #ifdef COMPUTER
      closeComputer();
      exit(0);
    #else
      SCB_AIRCR = 0x05FA0004; // software reset
    #endif
  #endif
      // void(*resetFunc)(void) = 0;
      // resetFunc();
      // for(;;) {}
}



extern unsigned char mem[];
extern int sysPROGEND;

// il faudrait que le contenu soit tokenized pour que cela fonctionne
const char* TEST_PRGM = "10 ? \"Coucou\"\r20 GOTO 10\r";
void xts_loadTestProgram() {
  // skip the autorun byte
  // sysPROGEND = strlen(TEST_PRGM);
  // for (int i=0; i<sysPROGEND; i++)
  //     mem[i] = TEST_PRGM[i];
}


// =================================



void DBUG(int v) { Serial.println(v); }
void DBUG(float v) { Serial.println(v); }
void DBUG(char* v) { Serial.println(v); }
void DBUG(const char* v) { Serial.println(v); }
void DBUG(const char* v, int v2) { Serial.print(v); Serial.println(v2); }

void DBUG_NOLN(char* v) { Serial.print(v); }
void DBUG_NOLN(const char* v) { Serial.print(v); }
void DBUG_NOLN(const char* v, int v2) { Serial.print(v); Serial.print(v2); }
void DBUG_NOLN(int v) { Serial.print(v); }

// ========= Console Ops =====================
/*
void onKeyReceived(struct KeyEvent ke) {
  Serial.println("a key was pressed");
}
*/


// ======== System Configuration =============
extern bool selfRun; // for CHAIN "<...>" cmd
extern int xts_loadBas(char * optFilename);

void host_system_menu() {
  // == true Cf used w/ parameters => bool result
  if ( xts_loadBas("SYSMENU") == true) {
    selfRun = true;
  } else {
    host_outputString("no SYSMENU PRGM");
  }
}

