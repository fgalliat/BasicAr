/*******************
* Xtase routines
*
* Xtase - fgalliat @Sept17
*******************/

#include "basic.h"
//#include "host.h"

#include <Arduino.h>

#include "xts_io.h"
extern int OUTPUT_DEVICE;

extern int sysVARSTART, sysPROGEND;
extern int stackPushNum(float val);
extern char *stackPopStr(); 
extern int getNextToken();
extern bool BUZZER_MUTE;
extern int expectNumber();
extern float stackPopNum();

extern int parseExpression();
extern int parseStringExpr();

extern unsigned char tokenBuf[TOKEN_BUF_SIZE];

extern bool selfRun; // for CHAIN "<...>" cmd

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

// == Clock ==
int fct_getSecs() {
  getNextToken();
  if (executeMode && !stackPushNum((float)(millis()/1000)))
      return ERROR_OUT_OF_MEMORY;
  return _TYPE_NUMBER;	
}


// == Screen 
int xts_locate() {
  getNextToken();
  int val = expectNumber();  // ROW
  if (val) return val;	// error
  
  getNextToken();
  val = expectNumber();     // COL
  if (val) return val;	// error
  
  if ( executeMode ) {
    uint16_t col = (uint16_t)stackPopNum();
    uint16_t row = (uint16_t)stackPopNum();

    host_moveCursor(col,row);
  }

  return 0;
}

// == File System


// BEWARE : no more a "SimpleCmd"
int xts_fs_dir() {
  getNextToken(); //==> NO (SimpleCmd : spe case) => NO MORE

  char* filter = NULL;

  if (curToken != TOKEN_EOL && curToken != TOKEN_CMD_SEP) {
    int val = parseExpression();
    // STRING 1st arg is optional
    if (_IS_TYPE_STR(val)) {
      filter = stackPopStr();
    } else {
      return ERROR_BAD_PARAMETER;
    }
  }


  if ( executeMode ) {
    lsStorage(filter);
  }

  return 0;
}

// ========= BEWARE : SimpleCmd =============================

void xts_mcu_reset() {
  if ( executeMode ) {
    MCU_reset();
  }
}

// =========================================================

int __xts_playSpeakerTune(int format) {
  getNextToken();
  int val = parseExpression();
    if (val & _ERROR_MASK) return val;
    if (!_IS_TYPE_STR(val))
        return _ERROR_EXPR_EXPECTED_STR;
  
  char* tuneName = stackPopStr();
  bool btnBreakMusic = true;

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
    // host_outputString("I will play : ");
    // host_outputString( tuneName );
    // host_outputString("\n");
    //playTuneFromStorage( (const char*)tuneName, format, btnBreakMusic );
    playTuneFromStorage( tuneName, format, btnBreakMusic );
  }

  return 0;
}

int xts_playT5K() {
  return __xts_playSpeakerTune(AUDIO_FORMAT_T5K);
}


int xts_playT53() {
  return __xts_playSpeakerTune(AUDIO_FORMAT_T53);
}

// == Console

int xts_echo() {
  getNextToken();
  int val = expectNumber();
  if (val) return val;	// error
  
  if (executeMode) {
    uint16_t echoValue = (uint16_t)stackPopNum();

    LOCAL_ECHO = ( echoValue != 0 );
  }

  return 0;
}

// == Clock
int xts_delay() {
  getNextToken();
  int val = expectNumber();
  if (val) return val;	// error
  
  if (executeMode) {
    uint16_t delayValue = (uint16_t)stackPopNum();

    //noInterrupts();
    host_sleep(delayValue);
    //interrupts();
  }
  
  return 0;
}


// == Btns

// BEWARE : Simple Function
// 1-based // 0 for read whole mask in 1 time
int xts_buttonRead(int btnNum) {
  if ( btnNum == 0 ) { return (btn1() ? 1 : 0)+(btn2() ? 2 : 0)+(btn3() ? 4 : 0); }
  else if ( btnNum == 1 ) { return btn1() ? 1 : 0; }
  else if ( btnNum == 2 ) { return btn2() ? 1 : 0; }
  else if ( btnNum == 3 ) { return btn3() ? 1 : 0; }

  return 0;
}

// == Led
int xts_led() {
  getNextToken();
  int val = expectNumber();
  if (val) return val;	// error
  
  getNextToken();
  val = expectNumber();
  if (val) return val;	// error
  
  if ( executeMode ) {
    uint16_t state = (uint16_t)stackPopNum(); // 1 or 0
    uint16_t ledID = (uint16_t)stackPopNum(); // 1 based

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
    // uint16_t note_freq = (uint16_t)stackPopNum();

    getNextToken();
    int dur = expectNumber();
    if (dur) return dur;	// error
    // uint16_t duration = (uint16_t)stackPopNum();

    if ( executeMode ) {

      // inexecute mode + inv. order
      uint16_t duration = (uint16_t)stackPopNum();
      uint16_t note_freq = (uint16_t)stackPopNum();

      if ( BUZZER_MUTE ) { return 0; }
      playNote(note_freq, duration);
    }

  return 0;    
}

// ================================================
// GFX

int xts_dispLine() {
  getNextToken();
  int val = expectNumber();  // X1
  if (val) return val;	// error
  
  getNextToken();
  val = expectNumber();  // Y1
  if (val) return val;	// error

  getNextToken();
  val = expectNumber();  // X2
  if (val) return val;	// error

  getNextToken();
  val = expectNumber();  // Y2
  if (val) return val;	// error

  if (executeMode) {
    uint16_t y2 = (uint16_t)stackPopNum();
    uint16_t x2 = (uint16_t)stackPopNum();
    uint16_t y1 = (uint16_t)stackPopNum();
    uint16_t x1 = (uint16_t)stackPopNum();

    drawLine(x1, y1, x2, y2);
  }
  
return 0;
}

int xts_dispCircle() {
  getNextToken();
  int val = expectNumber();  // X1
  if (val) return val;	// error
  
  getNextToken();
  val = expectNumber();  // Y1
  if (val) return val;	// error

  getNextToken();
  val = expectNumber();  // RADIUS
  if (val) return val;	// error

  if (executeMode) {
    uint16_t radius = (uint16_t)stackPopNum();
    uint16_t y1 = (uint16_t)stackPopNum();
    uint16_t x1 = (uint16_t)stackPopNum();

    drawCircle(x1, y1, radius);
  }
  
return 0;
}

int xts_pset() {
  getNextToken();
  int val = expectNumber();  // X1
  if (val) return val;	// error
  
  getNextToken();
  val = expectNumber();  // Y1
  if (val) return val;	// error

  if (executeMode) {
    uint16_t y1 = (uint16_t)stackPopNum();
    uint16_t x1 = (uint16_t)stackPopNum();

    drawPixel(x1, y1, 0x01);
  }
  
return 0;
}

int xts_preset() {
  getNextToken();
  int val = expectNumber();  // X1
  if (val) return val;	// error
  
  getNextToken();
  val = expectNumber();  // Y1
  if (val) return val;	// error

  if (executeMode) {
    uint16_t y1 = (uint16_t)stackPopNum();
    uint16_t x1 = (uint16_t)stackPopNum();

    drawPixel(x1, y1, 0x00);
  }
  
return 0;
}


int xts_dispBPP() {
  getNextToken();

  int val = parseExpression();
  if (val & _ERROR_MASK) return val;
  if (!_IS_TYPE_STR(val))
      return _ERROR_EXPR_EXPECTED_STR;

      
  if ( executeMode ) {
    char* pictStr = stackPopStr();
    if ( drawBPPfile( pictStr ) ) {
      return 0;
    } else {
      return ERROR_UNEXPECTED_TOKEN;
    }
  }

  return 0;
}


// ================================================
// I/O Console

extern void setScreenSize(int cols, int rows);

int xts_console() {
  getNextToken();

  // TODO : add & parse parameters to select devices

  // @ this time : ONLY switch to VGAText
  if ( executeMode ) {
    if (OUTPUT_DEVICE != OUT_DEV_VGA_SERIAL) { 
      OUTPUT_DEVICE = OUT_DEV_VGA_SERIAL; 
      setScreenSize(VGA_TEXT_WIDTH, VGA_TEXT_HEIGHT);
    }
    else { 
      OUTPUT_DEVICE = OUT_DEV_LCD_MINI; 
      setScreenSize(LCD_TEXT_WIDTH, LCD_TEXT_HEIGHT);
    }

  }

  return 0;
}

// ======= Load / Save .BAS files ======

int xts_loadBas(char* optFilename=NULL) {
  bool woFileMode = false;
  if ( optFilename == NULL ) {
    woFileMode = true;
    // if a filename is provided => the whole token check has already been done
    getNextToken();

    int val = parseExpression();
    if (val & _ERROR_MASK) return val;
    if (!_IS_TYPE_STR(val))
        return _ERROR_EXPR_EXPECTED_STR;

    if ( executeMode ) {
      optFilename = stackPopStr();
    }
  }

  if ( executeMode ) {
    loadAsciiBas( optFilename );
  }

  return woFileMode ? 0 : 1; // 1 for true when use in saleVloadCmd(..)
}

// BEWARE : SaveLoadCmd !!!!!
bool xts_chain(char* filename) {
  bool tmpExec = executeMode;
  if ( xts_loadBas(filename) == true ) {
    if ( tmpExec ) {
      // doRunPrg();
      // unsigned char tkb[2];
      // tkb[0] = TOKEN_RUN;
      // tkb[1] = 0x00;
      // int ret = processInput( tkb );
      selfRun = true; // the only way that works : can't run inside run ...
      return true;
    }
  } else {
    // TODO : better
    return false;
  }

  return true;
}


int xts_saveBas(char* optFilename=NULL) {
  bool woFileMode = false;
  if ( optFilename == NULL ) {
    woFileMode = true;
    // if a filename is provided => the whole token check has already been done
    getNextToken();

    int val = parseExpression();
    if (val & _ERROR_MASK) return val;
    if (!_IS_TYPE_STR(val))
        return _ERROR_EXPR_EXPECTED_STR;

    if ( executeMode ) {
      optFilename = stackPopStr();
    }
  }

  if ( executeMode ) {
    saveAsciiBas( optFilename );
  }

  return woFileMode ? 0 : 1; // 1 for true when use in saleVloadCmd(..)
}

int xts_llist() {
  char* optFilename=NULL;

  getNextToken();

  if (curToken != TOKEN_EOL && curToken != TOKEN_CMD_SEP) {
    int val = parseExpression();
    // STRING 1st arg is optional
    if (_IS_TYPE_STR(val)) {
      optFilename = stackPopStr();
    } else {
      return ERROR_BAD_PARAMETER;
    }
  }

  if ( executeMode ) {
    llistAsciiBas( optFilename );
  }

  return 0;
}


int xts_delBas(char* optFilename=NULL) {
  bool woFileMode = false;
  if ( optFilename == NULL ) {
    woFileMode = true;
    // if a filename is provided => the whole token check has already been done
    getNextToken();

    int val = parseExpression();
    if (val & _ERROR_MASK) return val;
    if (!_IS_TYPE_STR(val))
        return _ERROR_EXPR_EXPECTED_STR;

    optFilename = stackPopStr();
  }

  if ( executeMode ) {
    deleteBasFile( optFilename );
  }

  return woFileMode ? 0 : 1; // 1 for true when use in saleVloadCmd(..)
}

// ===================================================================

char charUpCase(char ch) {
    if ( ch >= 'a' && ch <= 'z' ) {
        ch = ch - 'a' + 'A';
    }
    return ch;
}




