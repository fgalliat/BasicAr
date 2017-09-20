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
 void host_outputString(char* str);

 // internal forward decl.
 static void activityLed(bool state);


#ifdef FS_SUPPORT
 // static File SD_myFile;
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

static void setupHardware() {
 setupGPIO();
 #ifdef FS_SUPPORT
   setupSD();
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

static void activityLed(bool state) {
  led(1, state);
}

 // ======= Sound Sub System =======
 #include "xtase_tone.h"

 //static bool BUZZER_MUTE = false;
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






#endif