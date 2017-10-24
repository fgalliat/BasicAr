/***************
* Xtase - fgalliat @ Sept2017
* Hardware Layer
****************/

#include <Arduino.h>

#include "xts_arch.h"


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


#endif


#include "host_xts.h"

#include "basic.h"
extern char codeLine[];
void cleanCodeLine() {
  memset(codeLine, 0x00, ASCII_CODELINE_SIZE);
}



#ifdef BOARD_VGA
  #include "dev_screen_VGATEXT.h"
#endif

extern int SCREEN_HEIGHT;

extern unsigned char tokenBuf[];


 // external forward decl.
 char charUpCase(char ch);
 bool endsWith(char* str, char* what);

 void host_outputString(char* str);
 int host_outputInt(long v);
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
  setup_vgat(false);
  //vgat_reboot(false);
  vgat_startScreen();
}
#endif


#ifdef BUT_TEENSY
  //#include <TimerOne.h> // for Teensy 2 & 2++

  IntervalTimer myTimer;

  // code in host.cpp
  extern void _ISR_emul();
#endif


void setupHardware() {
 setupGPIO();

 #ifdef BUILTIN_LCD
   setupLCD();
 #endif

 #ifdef BOARD_VGA
   setupVGASerial();
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

// ============ Tmp Compatibility Code ===============
extern boolean isWriting;

void lcd_println(char* text) { isWriting = true; Serial.print("LCD:"); Serial.println(text); isWriting = false; }
bool checkbreak() { return false; }

// ============ Tmp Compatibility Code =============== 


// T5K Format
void __playTune(unsigned char* tuneStream, bool btnStop);
// T53
void __playTuneT53(unsigned char* tuneStream, bool btnStop);

// //#define AUDIO_BUFF_SIZE (5*1024)
// #define AUDIO_BUFF_SIZE 256
// unsigned char audiobuff[AUDIO_BUFF_SIZE];
extern unsigned char audiobuff[];
void cleanAudioBuff() { for(int i=0; i < AUDIO_BUFF_SIZE; i++) { audiobuff[i]=0x00; } } 

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
  
  host_outputString("   -= Playing =-\n");
  host_outputString( songname );
  host_outputString("\n");
  host_showBuffer();

  #if ((defined SCREEN_SUPPORT) && (SCREEN_SUPPORT > 0))
    //lcd_cls();
    //           12345678901234567890
    lcd_println("   -= Playing =-\n");
    lcd_println("-=%s=-", (const char*)songname);
  #endif

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
  host_outputString("   -= Playing =-");
  host_outputString( songname );
  host_outputString("\n");
  host_showBuffer();
  
  #if ((defined SCREEN_SUPPORT) && (SCREEN_SUPPORT > 0))
    //lcd_cls();
    //           12345678901234567890
    lcd_println("   -= Playing =-");
    lcd_println("-=%s=-", (const char*)songname);
  #endif

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
// = FileSystem Ops.
// =
// ==============================================

 #ifndef FS_SUPPORT
  void lsStorage() {
    host_outputString("ERR : NO Storage support\n");
  }
 #else
  extern int curY;
  bool _lsStorage(SdFile dirFile, int numTabs, bool recurse, char* filter);

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



  void lsStorage() {
    bool recurse = false;
    char* filter = NULL;

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

    _lsStorage(dirFile, 0, recurse, filter);
    //_lsStorage2(root, 0, recurse, filter);

// root.close();

    dirFile.close();
  }


//  #ifndef SCREEN_HEIGHT
//   #define SCREEN_HEIGHT       8
//  #endif

bool _lsStorage(SdFile dirFile, int numTabs, bool recurse, char* filter) {
    SdFile file;

    int cpt = 0;
    while (file.openNext(&dirFile, O_READ)) {
      if (!file.isSubDir() && !file.isHidden() ) {
        // //file.printFileSize(&Serial);
        // //file.printModifyDateTime(&Serial);
        // //file.printName(&Serial);

        memset(SDentryName, 0x00, 13);
        file.getName( SDentryName, 13 );

        host_outputInt( (cpt+1) );
        host_outputString("\t");

        host_outputString(SDentryName);

        if ( file.isDir() ) {
          // Indicate a directory.
          host_outputString("/");
        }
        host_outputString("\n");

// TMP - DIRTY ----- begin
//if ( curY % SCREEN_HEIGHT == SCREEN_HEIGHT-1 ) {
if ( cpt % SCREEN_HEIGHT == SCREEN_HEIGHT-1 ) {
  host_showBuffer();
}
// TMP - DIRTY ----- end

        cpt++;

      }
      file.close();
    }

    host_outputString("nb files : ");
    host_outputInt( cpt );
    host_outputString("\n");

    
host_showBuffer();

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

  cleanCodeLine();
  while( ( n = file.fgets(codeLine, ASCII_CODELINE_SIZE) ) > 0 ) {
    // TODO : interpret line
    host_outputString( codeLine );
    if ( codeLine[n-1] != '\n' ) {
      host_outputString( "\n" );
    }
    host_showBuffer();

    int ret = tokenize((unsigned char*)codeLine, tokenBuf, TOKEN_BUF_SIZE); ret = processInput(tokenBuf);
    if ( ret > 0 ) { host_outputString((char *)errorTable[ret]); host_showBuffer(); }
    //ret = ERROR_NONE;

  }
  host_outputString( "-EOF-\n" );
  host_showBuffer();

  file.close();
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

//extern int __heap_start, *__brkval; 
extern int *__brkval; 
int v; 
//int fr = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
int fr = (int) &v - ((int) __brkval); 
   Serial.print("Free ram: ");
   Serial.println(fr);
return fr;
}


void MCU_reset() {
  // _restart_Teensyduino_();
  host_outputString("\nRebooting\n");
  host_showBuffer();
  #ifdef BUT_TEENSY
    SCB_AIRCR = 0x05FA0004; // software reset
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
  sysPROGEND = strlen(TEST_PRGM);
  for (int i=0; i<sysPROGEND; i++)
      mem[i] = TEST_PRGM[i];
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

