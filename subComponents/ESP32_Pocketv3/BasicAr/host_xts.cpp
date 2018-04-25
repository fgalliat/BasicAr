/***************
* Xtase - fgalliat @ Sept2017
* Hardware Layer
****************/

#ifndef COMPUTER
  #include <Arduino.h>
#else 
  #include "computer.h"
#endif

#define ANOTHER_CPP 1
#include "xts_arch.h"

#include "xts_io.h"

#define BASIC_ASCII_FILE_EXT ".BAS"

// ***********************************
// TODO : move in GenericMCU impl.
char* SDentryName = NULL;

  // BEWARE : uses & modifies : SDentryName[]
  // ex. autocomplete_fileExt(filename, ".BAS") => SDentryName[] contains full file-name
  // assumes that ext is stricly 3 char long
  // + 1 for leading '/'
  char* autocomplete_fileExt(char* filename, const char* defFileExt) {
    if ( SDentryName == NULL ) {
      SDentryName = (char*)malloc(13+1);
    }
    int flen = strlen(filename);
    memset(SDentryName, 0x00, 8+1+3+1+1); // 13+1 char long
    SDentryName[0] = '/';
    int l = strlen( filename );
    char ch;
    bool foundAPoint = false;
    for(int i=0; i < l; i++) {
      ch = filename[i];
      if ( ch >= 'a' && ch <= 'z' ) {
        ch = ch - 'a' + 'A';
      } else if ( ch == '.' ) {
        foundAPoint = true;
      }
      SDentryName[1+i] = ch;
    }
    if ( !foundAPoint ) {
      strcat( SDentryName, defFileExt );
    }
    Serial.println( SDentryName );
    return (char*)SDentryName;
  }
// ***********************************

#include "host_xts.h"
#include "basic.h"
extern bool isGfxAutoBlitt();

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

 // external forward decl.
 char charUpCase(char ch);
 bool endsWith(char* str, char* what);

 void host_outputString(char* str);
 int host_outputInt(long v);
 void host_outputChar(char c);
 void host_showBuffer();

// ===============================

bool STORAGE_OK = false;



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
 bool btn1() {
  return mcu.btn(BTN_1);
 }

 bool btn2() {
  return mcu.btn(BTN_2);
 }

 bool btn3() {
  return mcu.btn(BTN_3);
 }

bool anyBtn() { return btn1() || btn2() || btn3(); }


 // ========= Led Sub System ========

void led1(bool state) { mcu.led(1, state); }
void led2(bool state) { mcu.led(2, state); }
void led3(bool state) { mcu.led(3, state); }

void activityLed(bool state) {
  mcu.led(1, state);
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

  mcu.noTone();
  mcu.tone( note_freq, duration*50 );
  delay(duration*50);
  mcu.noTone(); // MANDATORY for ESP32Oled
}

void playTuneString(char* strTune) {
  mcu.noTone();

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

   if (!BUZZER_MUTE) mcu.tone(pitch, defDuration);
   delay(defDuration);

  }
  mcu.noTone(); // MANDATORY for ESP32Oled
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

  #ifdef ESP32_FS
    char ftuneStreamName[8+1+3+1];
    strcpy(ftuneStreamName, "/");
    strcat(ftuneStreamName, tuneStreamName);
  #else
    char ftuneStreamName[8+1+3];
    strcpy(ftuneStreamName, tuneStreamName);
  #endif

  
  

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

  #ifdef ESP32_FS
    // host_outputString("ZIK NYI for esp32\n");
    // host_showBuffer();
    unsigned char preBuff[2];
    int n = esp32.getFs()->readBinFile(ftuneStreamName, preBuff, 2);
    if ( n <= 0 ) {
      host_outputString( ftuneStreamName );
      host_outputString("\n");
      host_outputString("ZIK File not ready\n");
      host_showBuffer();
      return;
    }

    int nbNotes = (preBuff[0]<<8)|preBuff[1];

    int fileLen = (nbNotes*sizeof(Note))+2+16+2;
    if ( format == AUDIO_FORMAT_T53 ) {
      fileLen = (nbNotes*(3+3+3))+2+16+2;
    }

    //file.read( audiobuff, fileLen );
    n = esp32.getFs()->readBinFile(ftuneStreamName, audiobuff, fileLen);

    led3(false);

    if ( format == AUDIO_FORMAT_T5K ) {
      __playTune( audiobuff, btnStop );  
    } else {
      __playTuneT53( audiobuff, btnStop );  
    }

  #elif defined(TOTO_TEENSY)
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
  
  mcu.noTone(); // MANDATORY for ESP32Oled
 }
 
// where tuneStream is the audio buffer content
void __playTune(unsigned char* tuneStream, bool btnStop = false) {
  mcu.noTone();

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
      if (!BUZZER_MUTE) mcu.tone(notes[ note-1 ], duration);

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
    mcu.noTone();

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
  mcu.noTone();
  
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
      if (!BUZZER_MUTE) mcu.tone(note, duration);
    }

    delay(wait*tempo);
    // stop the tone playing:
    mcu.noTone();

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

#define RPID_BLACK -16777216
#define RPID_WHITE -1
#define RPID_RED   -65536
#define RPID_GREEN -16711936
#define RPID_BLUE  -16776961

// // manually triggered
// void draw_blitt() {
//   if ( GFX_DEVICE == GFX_DEV_LCD_MINI ) {

//     mcu.getScreen()->blitt( SCREEN_BLITT_AUTO ); // TODO : CHECK THAT

//   } else if (GFX_DEVICE == GFX_DEV_RPID_SERIAL) {
//     #ifdef BOARD_RPID
//     #endif
//   }  
// }

void drawLine(int x1, int y1, int x2, int y2) {
  if ( GFX_DEVICE == GFX_DEV_LCD_MINI ) {
    mcu.getScreen()->drawLine(x1, y1, x2, y2, WHITE);
  } else if (GFX_DEVICE == GFX_DEV_RPID_SERIAL) {
    #ifdef BOARD_RPID
      rpid_gfx_line(x1, y1, x2, y2, RPID_WHITE);
    #endif
  }
}

void drawCircle(int x1, int y1, int radius) {
  if ( GFX_DEVICE == GFX_DEV_LCD_MINI ) {
    mcu.getScreen()->drawCircle(x1, y1, radius, WHITE);
  } else if (GFX_DEVICE == GFX_DEV_RPID_SERIAL) {
    #ifdef BOARD_RPID
      rpid_gfx_circle(x1, y1, radius, RPID_WHITE);
    #endif
  }
}

// 0: black else :white
void drawPixel(int x1, int y1, int color) {
  if ( GFX_DEVICE == GFX_DEV_LCD_MINI ) {
      mcu.getScreen()->drawPixel(x1, y1, color == 1 ? WHITE : BLACK);
  } else if (GFX_DEVICE == GFX_DEV_RPID_SERIAL) {
    #ifdef BOARD_RPID
      rpid_gfx_drawPixel(x1, y1, RPID_WHITE);
    #endif
  }
}

// color : 0 black / 1 white / 2 ~gray
// mode  : 0 draw / 1 fill
void drawRect(int x, int y, int w, int h, int color, int mode) {
  if ( GFX_DEVICE == GFX_DEV_LCD_MINI ) {
      // BEWARE : BASIC args order is #than one used
      // in Generic MCU impl. !!!
      mcu.getScreen()->drawRect(x, y, w, h, mode, color);
  } else if (GFX_DEVICE == GFX_DEV_RPID_SERIAL) {
    #ifdef BOARD_RPID
      // no gray support @ this time
      // no fill support @ this time
      unsigned int c = color == 0 ? RPID_BLACK : RPID_WHITE;
      rpid_gfx_line(x, y, x+w, y, c);
      rpid_gfx_line(x+w, y, x+w, y+h, c);
      rpid_gfx_line(x+w, y+h, x, y+h, c);
      rpid_gfx_line(x, y+h, x, y, c);
    #endif
  }
}

bool drawBPPfile(char* filename) {

  if ( filename == NULL || strlen(filename) <= 0 ) { 
    host_outputString("ERR : invalid file\n");
    return false;
  }

  autocomplete_fileExt(filename, ".BPP");
  
  // do something w/ these bytes ...
  if ( GFX_DEVICE == GFX_DEV_LCD_MINI ) {
        mcu.getScreen()->drawPictureBPP(SDentryName, 0, 0);
  } else if (GFX_DEVICE == GFX_DEV_RPID_SERIAL) {
    // #ifdef BUILTIN_LCD
    //   // TMP !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //   #ifdef BUT_ESP32
    //     esp32.getScreen()->clear();
    //     esp32.getScreen()->drawImg(0, 0, 128, 64, picturebuff);
    //     if ( isGfxAutoBlitt() ) esp32.getScreen()->blitt();
    //   #else
    //     display.clearDisplay();
    //     display.drawBitmap(0, 0, picturebuff, 128, 64, 0x01);
    //     if ( isGfxAutoBlitt() ) display.display();
    //   #endif
    // #endif
  }

  return true;
}

bool drawPCTfile(char* filename, int x, int y) {
  if ( filename == NULL || strlen(filename) <= 0 ) { 
    host_outputString("ERR : invalid file\n");
    return false;
  }

  autocomplete_fileExt(filename, ".PCT");

  mcu.getScreen()->drawPicture565(SDentryName, x, y);

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

  #ifdef ESP32_FS

    void esp_ls_callback(char* entry,uint32_t size) {
      host_outputString(entry);
      host_outputString(" (");
      host_outputInt(size);
      host_outputString(")\n");
      host_showBuffer();
    }

    void lsStorage(char* filter=NULL, bool sendToArray=false) {
      if ( sendToArray ) {
        host_outputString("DIR2ARRAY NYI !");
        host_showBuffer();
        return;
      }

      esp32.lockISR();
      int cpt = esp32.getFs()->listDir("/", esp_ls_callback);
      esp32.unlockISR();

      host_outputString("nb files : ");
      host_outputInt( cpt );
      host_outputString("\n");
      host_showBuffer();
    }


  #else

    bool _lsStorage(SdFile dirFile, int numTabs, bool recurse, char* filter, bool sendToArray);

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
          host_outputString("Could not create DIR$\n");
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

  #endif // not ESP32-FS

 #endif // FS_SUPPORT


 // ==== Load/Save source code (ascii) from SDCard ====

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
          sprintf( tmp, "%d", (int)( *(long*)p ) ); 
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


  void loadCallback(char* codeLine) {

    if ( codeLine == NULL ) { Serial.println("CANT HANDLE NULL LINES"); return; }

    //Serial.print(">> ");Serial.println(codeLine);

    // interpret line
    int ret = tokenize((unsigned char*)codeLine, tokenBuf, TOKEN_BUF_SIZE); 
    if (ret == 0) { ret = processInput(tokenBuf); }
    if ( ret > 0 ) { 
      //host_outputInt( curToken );
      host_outputString((char *)codeLine);
      host_outputString((char *)" ->");
      host_outputString((char *)errorTable[ret]); 
      host_outputString((char *)" @");
      //host_outputInt( lineCpt );
      host_outputInt( 999 );
      host_outputString((char *)"\n");
      host_showBuffer(); 
    }
}


 void loadAsciiBas(char* filename) {
  if ( !STORAGE_OK ) {
    host_outputString("ERR : Storage not ready\n");
    host_showBuffer();
    return;
  }

  autocomplete_fileExt(filename, BASIC_ASCII_FILE_EXT);


    cleanCodeLine();
    memset( tokenBuf, 0x00, TOKEN_BUF_SIZE );

    // esp32.lockISR();
    bool ok = mcu.getFS()->openCurrentTextFile( SDentryName );
    if ( !ok ) {
      Serial.println( "-FAILED-" );
      host_outputString( "-FAILED-\n" );
      host_showBuffer();
    } else {
      char* codeLine; int cpt = 0;
      while( (codeLine = mcu.getFS()->readCurrentTextLine() ) != NULL ) {
        //if ( strlen(codeLine) == 0 ) { break; }
        //Serial.println( codeLine );
        loadCallback( codeLine );
      }
      mcu.getFS()->closeCurrentTextFile();
    }
    // esp32.unlockISR();

    host_outputString( "-EOF-\n" );
    host_showBuffer();

    return;
 }

// TODO : better
extern bool fopenTextFile(char* filename, bool forRead=true);
extern char* freadTextLine();
extern void fwriteText(char* str, bool autoflush=true);
extern void fcloseFile();


void saveAsciiBas(char* filename) {
  if ( !STORAGE_OK ) {
    host_outputString("ERR : Storage not ready\n");
    host_showBuffer();
    return;
  }

  autocomplete_fileExt(filename, BASIC_ASCII_FILE_EXT);

  // TO MOVE INSIDE fopen()
  mcu.getFS()->remove(SDentryName);

  fopenTextFile(SDentryName, false);
  // esp32.lockISR();

  fwriteText("1 ' blank\n");

  char lineNumStr[7];

  cleanCodeLine();
  unsigned char *p = &mem[0];
  while (p < &mem[sysPROGEND]) {
      uint16_t lineNum = *(uint16_t*)(p+2);
      sprintf(lineNumStr, "%d ", lineNum);
      fwriteText(lineNumStr, false);

      _serializeTokens(p+4, codeLine);
      fwriteText(codeLine, false);
      cleanCodeLine();

      fwriteText("\r\n");
      p+= *(uint16_t *)p;
  }
  fwriteText("\r\n");
  fcloseFile();

  //esp32.unlockISR();
  Serial.println("closed");

      
  host_outputString( "-EOF-\n" );
  host_showBuffer();
}




void llistAsciiBas(char* filename=NULL) {
  if ( filename == NULL ) {
    // just DUMP current PRGM to Serial
    
    #ifdef ESP32_WIFI_SUPPORT
      #define outPrint(a) { if ( telnet.isClientConnected() ) { telnet.print(a); } else { Serial.print(a); } }
    #else
      #define outPrint(a) { Serial.print(a); }
    #endif

    cleanCodeLine();
    unsigned char *p = &mem[0];
    while (p < &mem[sysPROGEND]) {
        uint16_t lineNum = *(uint16_t*)(p+2);
        outPrint(lineNum);
        outPrint(" ");
  
        _serializeTokens(p+4, codeLine);
        outPrint(codeLine);
        cleanCodeLine();
  
        outPrint("\n");
        p+= *(uint16_t *)p;
    }
    outPrint("-EOF-\n");
    //outSerial.flush();
    return;
  }

  // ==== ELSE : DUMP PLAIN ASCII BASIC FILE TO SERIAL ====

  if ( !STORAGE_OK ) {
    host_outputString("ERR : Storage not ready\n");
    host_showBuffer();
    return;
  }

  autocomplete_fileExt(filename, BASIC_ASCII_FILE_EXT);

host_outputString("LLIST NYI for esp32\n");
host_showBuffer();

  // #ifdef ESP32_FS
  //   host_outputString("LLIST NYI for esp32\n");
  //   host_showBuffer();
  // #else
  //   // SFATLIB mode -> have to switch for regular SD lib
  //   SdFile file;
  //   if (! file.open( SDentryName , O_READ) ) {
  //     led1(true);
  //     host_outputString("ERR : File not ready\n");
  //     host_showBuffer();
  //     return;        
  //   }

  //   file.seekSet(0);

  //   int n;

  //   cleanCodeLine();
  //   while( ( n = file.fgets(codeLine, ASCII_CODELINE_SIZE) ) > 0 ) {
  //     outSerial.print( codeLine );
  //     if ( codeLine[n-1] != '\n' ) {
  //       outSerial.print( "\n" );
  //     }
  //   }
  //   outSerial.print( "-EOF-\n" );
  //   outSerial.flush();

  //   file.close();
}



void deleteBasFile(char* filename) {
  if ( !STORAGE_OK ) {
    host_outputString("ERR : Storage not ready\n");
    host_showBuffer();
    return;
  }

  autocomplete_fileExt(filename, BASIC_ASCII_FILE_EXT);

  mcu.getFS()->remove( SDentryName );
}



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

  mcu.reset();

  // #ifdef BUT_TEENSY
  //   #ifdef COMPUTER
  //     closeComputer();
  //     exit(0);
  //   #elif defined (ARDUINO_ARCH_ESP32)
  //     ESP.restart();
  //   #else
  //     SCB_AIRCR = 0x05FA0004; // software reset
  //   #endif
  // #endif
  //     // void(*resetFunc)(void) = 0;
  //     // resetFunc();
  //     // for(;;) {}
}

extern bool systemHalted;

void MCU_halt() {
  host_outputString("\nHalting system\n");
  host_showBuffer();
  #ifdef BUT_TEENSY

    #ifdef BOARD_RPID
      rpid_haltGPU(false);
    #endif

    #ifdef BOARD_SND
      snd_pause();
    #endif

    #ifdef COMPUTER
      closeComputer();
      exit(0);
    #else
      systemHalted = true;
    #endif
    
  #endif
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


#ifndef DBUG_NOLN
  void DBUG(int v) { Serial.println(v); }
  void DBUG(float v) { Serial.println(v); }
  void DBUG(char* v) { Serial.println(v); }
  void DBUG(const char* v) { Serial.println(v); }
  void DBUG(const char* v, int v2) { Serial.print(v); Serial.println(v2); }

  void DBUG_NOLN(char* v) { Serial.print(v); }
  void DBUG_NOLN(const char* v) { Serial.print(v); }
  void DBUG_NOLN(const char* v, int v2) { Serial.print(v); Serial.print(v2); }
  void DBUG_NOLN(int v) { Serial.print(v); }
#endif

// ========= Console Ops =====================
/*
void onKeyReceived(struct KeyEvent ke) {
  Serial.println("a key was pressed");
}
*/


// ======== System Configuration =============
extern bool selfRun; // for CHAIN "<...>" cmd
extern int xts_loadBas(char * optFilename);
extern int doRun();

void host_system_menu() {
  reset();
  // == true Cf used w/ parameters => bool result
  if ( xts_loadBas("SYSMENU") == true) {
    selfRun = true;
    #ifdef BUT_ESP32
      doRun();
    #endif
  } else {
    host_outputString("no SYSMENU PRGM");
  }
}

