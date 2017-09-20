/*******************
* Xtase routines
*
* Xtase - fgalliat @Sept17
*******************/

// BEWARE w/ that
// #define BUZZER_PIN 5

#include "basic.h"

#include "xtase_hardw.h"
#include "xtase_tone.h"

extern int sysVARSTART, sysPROGEND;
extern int stackPushNum(float val);
extern char *stackPopStr(); 
extern int getNextToken();
extern bool BUZZER_MUTE;
extern int expectNumber();
extern float stackPopNum();

extern int parseExpression();
extern int parseStringExpr();

// from basic.cpp
#define _ERROR_MASK						0x0FFF
#define _ERROR_EXPR_EXPECTED_STR			8
#define _TYPE_MASK						0xF000
#define _TYPE_NUMBER					0x0000
#define _TYPE_STRING					0x1000

#define _IS_TYPE_NUM(x) ((x & _TYPE_MASK) == _TYPE_NUMBER)
#define _IS_TYPE_STR(x) ((x & _TYPE_MASK) == _TYPE_STRING)

// ======================== BASIC FCTS ================================

int getMemFree() {
    // sysVARSTART - sysPROGEND
    getNextToken();
    if (executeMode && !stackPushNum((float) (sysVARSTART - sysPROGEND) ))
        return ERROR_OUT_OF_MEMORY;
    return _TYPE_NUMBER;	
}

// == Screen 
int xts_locate() {
  getNextToken();
  int val = expectNumber();
  if (val) return val;	// error
  uint16_t row = (uint16_t)stackPopNum();

  getNextToken();
  val = expectNumber();
  if (val) return val;	// error
  uint16_t col = (uint16_t)stackPopNum();

  if ( executeMode ) {
    host_moveCursor(col,row);
  }

  return 0;
}


// == Led
int xts_led() {
  getNextToken();
  int val = expectNumber();
  if (val) return val;	// error
  uint16_t ledID = (uint16_t)stackPopNum(); // 1 based

  getNextToken();
  val = expectNumber();
  if (val) return val;	// error
  uint16_t state = (uint16_t)stackPopNum(); // 1 or 0

  if ( executeMode ) {
    led(ledID,state >= 1);
  }

  return 0;
}

// == Sound 

int xts_mute() {
  // sysVARSTART - sysPROGEND
  getNextToken();
  if ( executeMode ) {
    BUZZER_MUTE = !BUZZER_MUTE;
  }
  return 0;	
}

int xts_tone() {
    getNextToken();
    int freq = expectNumber();
    if (freq) return freq;	// error
    uint16_t note_freq = (uint16_t)stackPopNum();

    getNextToken();
    int dur = expectNumber();
    if (dur) return dur;	// error
    uint16_t duration = (uint16_t)stackPopNum();

    if ( executeMode ) {
      if ( BUZZER_MUTE ) { return 0; }
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

  return 0;    
}

int xts_play() {
  getNextToken();

  int val = parseExpression();
  if (val & _ERROR_MASK) return val;
  if (!_IS_TYPE_STR(val))
      return _ERROR_EXPR_EXPECTED_STR;

  char* tuneStr = stackPopStr();

  if ( executeMode ) {
    noTone(BUZZER_PIN);
    playTuneString( tuneStr );
  }

  return 0;
}


// ===================================================================

char charUpCase(char ch) {
    if ( ch >= 'a' && ch <= 'z' ) {
        ch = ch - 'a' + 'A';
    }
    return ch;
}
