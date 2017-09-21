/***************
* Xtase - fgalliat @ Sept2017
* Hardware Layer
****************/

#ifndef xtase_hardw_h_
#define xtase_hardw_h_ 1

#include <Arduino.h>

#include "xts_arch.h"


 // external forward decl.
 char charUpCase(char ch);
 bool endsWith(char* str, char* what);

 void host_outputString(char* str);
 int host_outputInt(long v);
 void host_showBuffer();

 // internal forward decl.
 static void activityLed(bool state);


#ifdef FS_SUPPORT
 //static File SD_myFile;
 static bool STORAGE_OK = false;
 
 static void setupSD() {
   #ifdef USE_SDFAT_LIB
     if (!SD.begin()) {
   #else
     if (!SD.begin(BUILTIN_SDCARD)) { // Teensy3.6 initialization
   #endif
       //outprint("initialization failed!\n");
       //host_outputString("SD : initialization failed!\n");
       //Serial.print(F("SD : initialization failed!\n"));
       activityLed(true);
       delay(500);
       activityLed(false);
       delay(500);
       activityLed(true);
     
       return;
     }
     //lcd_println("SD : initialization done.");
     //host_outputString("SD : initialization done.\n");
     //Serial.print(F("SD : initialization done.\n"));
 
   #ifdef USE_SDFAT_LIB
     SD.chvol();
   #endif
   STORAGE_OK = true;
 }
#endif // FS_SUPPORT

// ====== GPIO initialization ======

static void setupGPIO() {
 if ( BUZZER_PIN > -1 ) { pinMode(BUZZER_PIN, OUTPUT); digitalWrite(BUZZER_PIN, LOW);  }

 if ( LED1_PIN > -1 ) { pinMode(LED1_PIN, OUTPUT); digitalWrite(LED1_PIN, LOW); }
 if ( LED2_PIN > -1 ) { pinMode(LED2_PIN, OUTPUT); digitalWrite(LED2_PIN, LOW); }
 if ( LED3_PIN > -1 ) { pinMode(LED3_PIN, OUTPUT); digitalWrite(LED3_PIN, LOW); }
}

// ====== HARD initialization ======

#ifdef BUT_TEENSY
  #include <TimerOne.h>
  // code in host.cpp
  extern void _ISR_emul();
#endif

static void setupHardware() {
 setupGPIO();
 #ifdef FS_SUPPORT
   setupSD();
 #endif

 #ifdef BUT_TEENSY
   Timer1.initialize(350000); // 350ms 
   Timer1.attachInterrupt( _ISR_emul );
 #endif
}



 // ========= Led Sub System ========

// BEWARE : 1-based
static void led(int ledID, bool state) {
  if ( ledID >= 1 && ledID <= 3 ) {
    if ( ledID == 1 && LED1_PIN > -1 ) { digitalWrite(LED1_PIN, state ? HIGH : LOW); }
    if ( ledID == 2 && LED2_PIN > -1 ) { digitalWrite(LED2_PIN, state ? HIGH : LOW); }
    if ( ledID == 3 && LED3_PIN > -1 ) { digitalWrite(LED3_PIN, state ? HIGH : LOW); }
  }
  // no error .... 
}

static void led1(bool state) { led(1, state); }
static void led2(bool state) { led(2, state); }
static void led3(bool state) { led(3, state); }

static void activityLed(bool state) {
  led(1, state);
}

 // ======= Sound Sub System =======
 #include "xtase_tone.h"

 extern bool BUZZER_MUTE;
 #define BUZZER BUZZER_PIN

 #define AUDIO_BUFF_SIZE (5*1024)
 static unsigned char audiobuff[AUDIO_BUFF_SIZE];
 static void cleanAudioBuff() { for(int i=0; i < AUDIO_BUFF_SIZE; i++) { audiobuff[i]=0x00; } } 
 
 typedef struct Note {
  unsigned char note;
  unsigned short duration;
 } Note;

 enum {
   AUDIO_FORMAT_T5K = 0,
   AUDIO_FORMAT_T53,
 };


static void playTuneString(char* strTune) {
  int defDuration = 200;
  int slen = strlen( strTune );
  for (int i=0; i < slen; i++) {
    char ch = strTune[i];
    ch = charUpCase(ch);
    bool sharp = false;
    if ( i < slen-1 && strTune[i] == '#' ) { sharp = true; i++; }  

    int pitch = 0;
    switch (ch) {
        case 'C' :
          if ( sharp ) { pitch = notes[ NOTE_CS4 ]; }
          pitch = notes[ NOTE_C4 ];
        break;
        case 'D' :
          if ( sharp ) { pitch = notes[ NOTE_DS4 ]; }
          pitch = notes[ NOTE_D4 ];
        break;
        case 'E' :
          pitch = notes[ NOTE_E4 ];
        break;
        case 'F' :
          if ( sharp ) { pitch = notes[ NOTE_FS4 ]; }
          pitch = notes[ NOTE_F4 ];
        break;
        case 'G' :
          if ( sharp ) { pitch = notes[ NOTE_GS4 ]; }
          pitch = notes[ NOTE_G4 ];
        break;
        case 'A' :
          if ( sharp ) { pitch = notes[ NOTE_AS4 ]; }
          pitch = notes[ NOTE_A4 ];
        break;
        case 'B' :
          pitch = notes[ NOTE_B4 ];
        break;
    }

   if (!BUZZER_MUTE) tone(BUZZER, pitch, defDuration);
   delay(defDuration);

  }
} // end of playTuneSTring

// ============ Tmp Compatibility Code ===============
static void lcd_println(char* text) { Serial.print("LCD:"); Serial.println(text); }
static bool checkbreak() { return false; }
static bool anyBtn() { return false; }
// ============ Tmp Compatibility Code =============== 


// T5K Format
static void __playTune(unsigned char* tune, bool btnStop);
// T53
static void __playTuneT53(unsigned char* tune, bool btnStop);

static long t0,t1;

#if ((defined FS_SUPPORT) && (FS_SUPPORT > 0))
static void playTuneFromStorage(const char* tuneName, int format = AUDIO_FORMAT_T5K, bool btnStop = false) {
  cleanAudioBuff();

  // t0 = millis();
  // File zik = SD.open(tuneName);
  // int nbNotes = (zik.read()<<8)|zik.read();
  // zik.seek(0);
  // int fileLen = (nbNotes*sizeof(Note))+2+16+2;
  // if ( format == AUDIO_FORMAT_T53 ) {
  //   fileLen = (nbNotes*(3+3+3))+2+16+2;
  // }
  // zik.readBytes( audiobuff, fileLen );
  // zik.close();
  // t1 = millis();
  // //printfln("load:%d msec", (t1-t0) );
  // host_outputInt( fileLen ); host_outputString( "bytes\n" );

  // if ( format == AUDIO_FORMAT_T5K ) {
  //   __playTune( audiobuff, btnStop );  
  // } else {
  //   __playTuneT53( audiobuff, btnStop );  
  // }
  
 }
#endif
 
// where tune is the audio buffer content
static void __playTune(unsigned char* tune, bool btnStop = false) {
  short nbNotes = (*tune++ << 8) | (*tune++);
  char songname[16];
  for(int i=0; i < 16; i++) {
    songname[i] = *tune++;
  }
  short tempoPercent = (*tune++ << 8) | (*tune++);

  //printfln("nbN:%d title:'%s' tmp:%d", nbNotes, (const char*)songname, tempoPercent);
  
  host_outputString("PLAYING :");
  host_outputString( songname );
  host_outputString("\n");

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
    int note = *tune++;
    short duration = (*tune++ << 8) | (*tune++);
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
 // where tune is the audio buffer content
 static void __playTuneT53(unsigned char* tune, bool btnStop = false) {
  short nbNotes = (*tune++ << 8) | (*tune++);
  char songname[16];
  for(int i=0; i < 16; i++) {
    songname[i] = *tune++;
  }
  short tempoPercent = (*tune++ << 8) | (*tune++);

  //printfln("nbN:%d title:'%s' tmp:%d", nbNotes, (const char*)songname, tempoPercent);
  
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
    short note = (*tune++ << 8) | (*tune++);
    short duration = (*tune++ << 8) | (*tune++);
    short wait = (*tune++ << 8) | (*tune++);
    
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

 #ifdef FS_SUPPORT
    #ifdef USE_SDFAT_LIB
      static SdFile file;
      static SdFile dirFile;
    #endif

  extern int curY;

  static bool _lsStorage(File dir, int numTabs, bool recurse, char* filter) {
     if (!dirFile.open("/", O_READ)) {
       host_outputString("ERR opening SD root\n");
     }
    
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
#ifndef SCREEN_HEIGHT
 #define SCREEN_HEIGHT       8
#endif

if ( curY % SCREEN_HEIGHT == SCREEN_HEIGHT-1 ) {
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

    dirFile.close();
host_showBuffer();

    return true;
  }

  // NO FOWARD DECLARATION WHEN USE 'static'

  static void lsStorageR(bool recurse, char* filter) {
    File SD_myFile; // IGNORED
    _lsStorage(SD_myFile, 0, recurse, filter);
  }

  static void lsStorage() {
    lsStorageR(false, NULL);
  }


 #endif // FS_SUPPORT



#endif