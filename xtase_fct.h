/*******************
* Xtase routines
*
* Xtase - fgalliat @Sept17
*******************/

#include "basic.h"
//#include "host.h"

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

// == File System

// beware : SimpleCmd
void xts_fs_dir() {
  //getNextToken(); ==> NO (SimpleCmd : spe case)

  //host_outputString("IN XTS DIR !!!\n");
  if ( executeMode ) {
    lsStorage();
  }

  //return 0; ==> NO (SimpleCmd)
}

// =========================================================

int __xts_playSpeakerTune(int format) {
  getNextToken();
  int val = parseExpression();
    if (val & _ERROR_MASK) return val;
    if (!_IS_TYPE_STR(val))
        return _ERROR_EXPR_EXPECTED_STR;
  
  char* tuneName = stackPopStr();
  bool btnBreakMusic = false;

  // TODO : get the boolean breakButtons ...

  // getNextToken();
  // if (curToken != TOKEN_EOL && curToken != TOKEN_CMD_SEP) {
  //   val = parseExpression();
  //   if (val & _ERROR_MASK) return val;
    
  //     if (!_IS_TYPE_NUM(val))
  //       return ERROR_EXPR_EXPECTED_NUM;
      
  //     btnBreakMusic = stackPopNum() > 0;
  // }

  if (executeMode) {
    host_outputString("I will play : ");
    host_outputString( tuneName );
    host_outputString("\n");
    playTuneFromStorage( (const char*)tuneName, format, btnBreakMusic );
  }

  return 0;
}

int xts_playT5K() {
  return __xts_playSpeakerTune(AUDIO_FORMAT_T5K);
}


int xts_playT53() {
  return __xts_playSpeakerTune(AUDIO_FORMAT_T53);
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
  getNextToken();
  if ( executeMode ) {
    BUZZER_MUTE = !BUZZER_MUTE;
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
    playTuneString( tuneStr );
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
      playNote(note_freq, duration);
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




