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
extern int GFX_DEVICE;
extern int INPUT_DEVICE;

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

char charUpCase(char ch);

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

int fct_getMillis() {
  getNextToken();
  if (executeMode && !stackPushNum((float)(millis())))
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
    uint16_t row = (uint16_t)stackPopNum(); // inv order
    uint16_t col = (uint16_t)stackPopNum();

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
      filter = stackPopStr();     // TODO : check exec mode
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
  
  
  // TODO : get the boolean breakButtons ...


  if (executeMode) {
    char* tuneName = stackPopStr();
    bool btnBreakMusic = true;
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


// PLAY "abc#"
int xts_play() {
  getNextToken();

  int val = parseExpression();
  if (val & _ERROR_MASK) return val;
  if (!_IS_TYPE_STR(val))
      return _ERROR_EXPR_EXPECTED_STR;
      
  if ( executeMode ) {
    char* tuneStr = stackPopStr();
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

int _last_outDev = -1; // undefined
int _last_inDev  = -1; // undefined
int _last_gfxDev = -1; // undefined


int xts_console() {

  if ( _last_outDev == -1 && _last_inDev == -1 && _last_gfxDev == -1 ) {
    _last_outDev = OUTPUT_DEVICE;
    _last_inDev  = INPUT_DEVICE;
    _last_gfxDev = GFX_DEVICE;
  }

  getNextToken();

  int nbArgs = 0;
  int outDev = -1; // undefined
  int inDev  = -1; // undefined
  int gfxDev = -1; // undefined

  // TODO : add & parse parameters to select devices
  if (curToken != TOKEN_EOL) {
    int val = expectNumber();  // OUTPUT
    if (val == 0){
      nbArgs++;
      getNextToken();
      if (curToken != TOKEN_EOL) {
        val = expectNumber();  // INPUT
        if (val == 0){
          nbArgs++;
          getNextToken();
          if (curToken != TOKEN_EOL) {
            val = expectNumber();  // GFX
            if (val == 0){
              nbArgs++;
            }
          }
        } 
      }
    }
  }

  if ( executeMode ) {
    if ( nbArgs == 3 ) {
      gfxDev = (uint16_t)stackPopNum();
      inDev  = (uint16_t)stackPopNum();
      outDev = (uint16_t)stackPopNum();
    } else if ( nbArgs == 2 ) {
      gfxDev = _last_gfxDev;
      inDev  = (uint16_t)stackPopNum();
      outDev = (uint16_t)stackPopNum();
    } else if ( nbArgs == 1 ) {
      gfxDev = _last_gfxDev;
      inDev  = _last_inDev;
      outDev = (uint16_t)stackPopNum();
    } else if ( nbArgs == 0 ) {
      gfxDev = _last_gfxDev;
      inDev  = _last_inDev;
      outDev = -1;
    }
  }

  if ( executeMode ) {

    gfxDev = setConsoles( outDev, inDev, gfxDev );

    host_outputString("OUT : ");
    host_outputInt(outDev);
    host_outputString(" IN : ");
    host_outputInt(inDev);
    host_outputString(" GFX : ");
    host_outputInt(gfxDev);
    host_outputString("\n");
    host_showBuffer();

    _last_outDev = outDev;
    _last_inDev  = inDev;
    _last_gfxDev = gfxDev;
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

// ===== Strings =====

// ''''''' to move in a generic string include ''''''
int strpos(char *hay, char *needle, int offset) {
   char haystack[strlen(hay)];
   strncpy(haystack, hay+offset, strlen(hay)-offset);
   char *p = strstr(haystack, needle);
   if (p)
      return p - haystack+offset;
   return -1;
}
// ''''''''''''''''''''''''''''''''''''''''''''''''''

int xts_str_asc(char* str) {
  if ( str == NULL || strlen(str) < 1 ) { return 0; }
  int ret = (int)str[0];

  return ret;
}

int xts_str_instr(char* str, char* search) {
  if ( str == NULL || search == NULL ) { return 0; }
  // char* ret = strstr(str, search);
  // // the result is 1-based
  // if ( ret == NULL ) { return 0; }
  // // BEWARE w/ that ?????
  // return (&ret - &str) + 1;
  return strpos(str, search, 0) + 1;
}

char* xts_str_string(int nbTimes, int chr) {
  // BEWARE w/ that ?????
  char* ret = (char*)malloc( nbTimes+1 );
  memset( ret, chr, nbTimes );
  ret[nbTimes] = 0x00;
  return ret;
}

char* xts_str_upper(char* str) {
  // BEWARE w/ that ?????
  int len = strlen( str );
  char* ret = (char*)malloc( len+1 );
  for(int i=0; i < len; i++) { ret[i] = charUpCase(str[i]); }
  ret[len] = 0x00;
  return ret;
}


// ===================================================================
// === Extended Commands
// ===================================================================

// EXEC "WIFI","PARCEL"
// EXEC "MP3","PAUSE"
// @ this time : only support for String args....

#ifdef BOARD_SND
  #include "dev_sound_dfplayer.h"
#endif


int xts_exec_cmd() {
  getNextToken();
  
  const int MAX_ARGS = 6;
  char* args[MAX_ARGS]; // string args
  int   argc = 0;
  int   argi[MAX_ARGS]; // int args
  int   argci = 0;

  int val = -1;
  while (curToken != TOKEN_EOL && curToken != TOKEN_CMD_SEP) {
    val = parseExpression();
    //if (val & _ERROR_MASK) return val;
    if (val & _ERROR_MASK) break;

    // STRING 1st arg is optional
    if (_IS_TYPE_STR(val)) {
      if ( executeMode && argc < MAX_ARGS) {
        char* tt = stackPopStr();
        int stlen = strlen(tt);
        char* tmp = (char*)malloc( stlen+1 ); // BEWARE w/ free()
        //memcpy( tmp, tt, strlen(tt) );
        for(int i=0; i < stlen; i++) { tmp[i] = charUpCase( tt[i] ); }
        tmp[ stlen ] = 0x00;
        args[argc++] = tmp;
        //host_outputString( tmp );host_outputString("\n");
      }
    } else if (_IS_TYPE_NUM(val)) {
      if ( executeMode && argci < MAX_ARGS) {
        int tmp = (int)stackPopNum();
        argi[argci++] = tmp;
        //host_outputInt( tmp );host_outputString("\n");
      }
    } else {
      return ERROR_BAD_PARAMETER;
    }

    // getNextToken();
    if ( curToken == TOKEN_COMMA ) {
      getNextToken();
    }
  }

  if ( !executeMode ) {
    if (val & _ERROR_MASK) return val;
  }
  
  // TODO : @ least 1 param
  // else {
  //   // @ least 1 param
  //   return ERROR_BAD_PARAMETER;
  // }

  if ( executeMode ) {
      if ( argc > 0 ) {
        if ( strcmp( args[0], "MP3" ) == 0 ) {
          if ( argc > 1 ) {
            if ( strcmp( args[1], "PLAY" ) == 0 ) {    // ex. exec "mp3","play"
              int trackNum = 1;
              if ( argci > 0 ) { trackNum = argi[0]; } // ex. exec "mp3","play",2
              //host_outputString("Will play #");host_outputInt(trackNum);
              #ifdef BOARD_SND
                snd_playTrack(trackNum);
              #endif
            } else if ( strcmp( args[1], "VOL" ) == 0 ) {    // ex. exec "mp3","vol"
              int volume = 10;
              if ( argci > 0 ) { volume = argi[0]; } // ex. exec "mp3","vol",30
              #ifdef BOARD_SND
                snd_setVolume(volume);
              #endif
            } else if ( strcmp( args[1], "PAUSE" ) == 0 ) {
              #ifdef BOARD_SND
                snd_pause();
              #endif
            } else if ( strcmp( args[1], "NEXT" ) == 0 ) {
              #ifdef BOARD_SND
                snd_next();
              #endif
            } else {
              free( args[0] );
              free( args[1] );
              return ERROR_BAD_PARAMETER;
            }
            free( args[1] );
          } // end of argc > 1
        } else {
          free( args[0] );
          return ERROR_BAD_PARAMETER;
        }
        free( args[0] );
      } // end of argc > 0

  } // execMode

  return 0;
}



// ===================================================================


char charUpCase(char ch) {
    if ( ch >= 'a' && ch <= 'z' ) {
        ch = ch - 'a' + 'A';
    }
    return ch;
}




