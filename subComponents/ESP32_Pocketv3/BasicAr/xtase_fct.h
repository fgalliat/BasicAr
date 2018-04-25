/*******************
* Xtase routines
*
* Xtase - fgalliat @Sept17
*******************/

#include "basic.h"

#ifndef COMPUTER
  #include <Arduino.h>
#else 
  #include "computer.h"
#endif

#ifdef ESP32_WIFI_SUPPORT
  extern Esp32WifiServer telnet;
#endif


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
int xts_fs_dir(bool sendToArray=false) {
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
    lsStorage(filter, sendToArray);
  }

  return 0;
}

// ========= BEWARE : SimpleCmd =============================

void xts_mcu_reset() {
  if ( executeMode ) {
    MCU_reset();
  }
}

void xts_mcu_halt() {
  if ( executeMode ) {
    MCU_halt();
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
// 4 left / 5 right / 6 up / 7 down / 8 L / 9 R / 10 MP3Busy
int xts_buttonRead(int btnNum) {
  if ( btnNum == 0 ) { return (btn1() ? 1 : 0)+(btn2() ? 2 : 0)+(btn3() ? 4 : 0); }

  // button mapping Hard <-> BASIC
  // BEWARE : here, we do not read builtin btn
  const uint8_t map[] = { -1, 5, 6, 7, 3, 4, 1, 2, -1, -1, -1 };

  if ( btnNum < 1 || btnNum > 10 ) { return 0; }

  return mcu.btn( map[ btnNum ] ) ? 1 : 0;
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

    mcu.led(ledID,state >= 1);
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
      mcu.tone(note_freq, duration);
      mcu.noTone(); // TODO : CHECK THAT
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

int xts_dispPCT() {
  getNextToken();

  int val = parseExpression();
  if (val & _ERROR_MASK) return val;
  if (!_IS_TYPE_STR(val))
      return _ERROR_EXPR_EXPECTED_STR;

  getNextToken();
  val = expectNumber();  // X1
  if (val) return val;	// error
  
  getNextToken();
  val = expectNumber();  // Y1
  if (val) return val;	// error

      
  if ( executeMode ) {
    int y = stackPopNum();
    int x = stackPopNum();
    //int x = 0, y = 0;
    char* pictStr = stackPopStr();
    if ( drawPCTfile( pictStr, x, y ) ) {
      return 0;
    } else {
      return ERROR_UNEXPECTED_TOKEN;
    }
  }

  return 0;
}

int xts_blittMode() {
  getNextToken();

  int val = parseExpression();
  if (val & _ERROR_MASK) return val;
  if (!_IS_TYPE_NUM(val))
      return ERROR_EXPR_EXPECTED_NUM;

  if ( executeMode ) {
    int mode = (int)stackPopNum();
    if ( mode < BLITT_LOCKED || mode > BLITT_AUTO ) {
      return ERROR_BAD_PARAMETER;
    }

    mcu.getScreen()->blitt( mode );

    // if ( mode == BLITT_DIRECT ) {
    //   draw_blitt();
    // } else {
    //   // stores only 0 & 2
    //   BLITT_MODE = mode;   
    //   if ( BLITT_AUTO ){
    //     draw_blitt();
    //   }
    // }

  }

  return 0;
}

int xts_screenMode() {
  getNextToken();

  int val = parseExpression();
  if (val & _ERROR_MASK) return val;
  if (!_IS_TYPE_NUM(val))
      return ERROR_EXPR_EXPECTED_NUM;

  if ( executeMode ) {
    int mode = (int)stackPopNum();
    if ( mode < BLITT_LOCKED || mode > BLITT_AUTO ) {
      return ERROR_BAD_PARAMETER;
    }

    mcu.getScreen()->setMode( mode );
  }

  return 0;
}


// N.B. @ this time all params are mandatory
int xts_dispRect() {
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

  getNextToken();
  val = expectNumber();  // COLOR
  if (val) return val;	// error

  getNextToken();
  val = expectNumber();  // MODE
  if (val) return val;	// error

  if ( executeMode ) {
    int mode = (int)stackPopNum();
    int color = (int)stackPopNum();
    int h = (int)stackPopNum();
    int w = (int)stackPopNum();
    int y = (int)stackPopNum();
    int x = (int)stackPopNum();
    
    drawRect(x,y,w,h,color,mode);
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
    #ifdef FS_SUPPORT
      deleteBasFile( optFilename );
    #endif
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


float xts_abs(float arg0) {
  float ret = arg0 < 0 ? -arg0 : arg0;
  return ret;
}

#define degToRag (3.141596 / 180.0)

// in radian
float xts_cos(float arg0) {
  float ret = cos(arg0 /* * degToRag */);
  return ret;
}

// in radian
float xts_sin(float arg0) {
  float ret = sin(arg0 /* * degToRag */);
  return ret;
}

float xts_sqrt(float arg0) {
  float ret = sqrt(arg0);
  return ret;
}

float xts_pow(float arg0, float arg1) {
  float ret = pow(arg0, arg1);
  return ret;
}

float xts_min(float arg0, float arg1) {
  float ret = arg0 < arg1 ? arg0 : arg1;
  return ret;
}

float xts_max(float arg0, float arg1) {
  float ret = arg0 > arg1 ? arg0 : arg1;
  return ret;
}


// ===================================================================
// === Extended Commands
// ===================================================================

// EXEC "WIFI","PARCEL"
// EXEC "MP3","PAUSE"
// @ this time : only support for String args....

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
  
  if ( executeMode ) {
      if ( argc > 0 ) {
        if ( strcmp( args[0], "MP3" ) == 0 ) {
          if ( argc > 1 ) {
            if ( strcmp( args[1], "PLAY" ) == 0 ) {    // ex. exec "mp3","play"
              int trackNum = 1;
              if ( argci > 0 ) { trackNum = argi[0]; } // ex. exec "mp3","play",2
              mcu.getMusicPlayer()->playTrack(trackNum);
            } else if ( strcmp( args[1], "VOL" ) == 0 ) {    // ex. exec "mp3","vol"
              int volume = 10;
              if ( argci > 0 ) { volume = argi[0]; } // ex. exec "mp3","vol",30
              mcu.getMusicPlayer()->setVolume(volume);
            } else if ( strcmp( args[1], "PAUSE" ) == 0 ) {
              mcu.getMusicPlayer()->pause();
            } else if ( strcmp( args[1], "NEXT" ) == 0 ) {
              mcu.getMusicPlayer()->next();
            } else {
              free( args[0] );
              free( args[1] );
              return ERROR_BAD_PARAMETER;
            }
            free( args[1] );
          } // end of argc > 1
        } 
        else if ( strcmp( args[0], "FS" ) == 0 ) {
          if ( argc > 1 ) {
            if ( strcmp( args[1], "FORMAT" ) == 0 ) {
              mcu.lockISR();
              mcu.getFS()->format();
              mcu.unlockISR();
            } 
            else {
              free( args[0] );
              free( args[1] );
              return ERROR_BAD_PARAMETER;
            }
            free( args[1] );
          } // end of argc > 1
        } 
        #ifdef ESP32_WIFI_SUPPORT
        else if ( strcmp( args[0], "WIFI" ) == 0 ) {
          if ( argc > 1 ) {
            if ( strcmp( args[1], "CONNECT" ) == 0 ) {
              telnet.connectWifi();
            } else if ( strcmp( args[1], "DISCONNECT" ) == 0 ) {
              telnet.disconnectWifi();
            } else if ( strcmp( args[1], "STARTAP" ) == 0 ) {
              telnet.startAP();
            } else if ( strcmp( args[1], "SRVSTART" ) == 0 ) {
              telnet.open();
            } else if ( strcmp( args[1], "SRVSTOP" ) == 0 ) {
              telnet.close();
            } else if ( strcmp( args[1], "UPLOAD" ) == 0 ) {
              telnet.uploadFile();
            }
            else {
              free( args[0] );
              free( args[1] );
              return ERROR_BAD_PARAMETER;
            }
            free( args[1] );
          } // end of argc > 1
        } 
        #endif
        else {
          free( args[0] );
          return ERROR_BAD_PARAMETER;
        }
        free( args[0] );
      } // end of argc > 0

  } // execMode

  return 0;
}



// ===================================================================

  // _________ TODO ______________________
  bool fopenTextFile(char* filename, bool forRead=true) {
    return mcu.getFS()->openCurrentTextFile(filename, forRead);
  }
  
  char* freadTextLine() {
    return mcu.getFS()->readCurrentTextLine();
  }

  void fwriteText(char* str, bool autoflush=true) {
    mcu.getFS()->writeCurrentText(str, autoflush);
  }

  void fcloseFile() {
    mcu.getFS()->closeCurrentTextFile();
  }
  // _________ TODO ______________________

  // remember to free() result ....
  char* str_trim(char* str) {
    if ( str == NULL ) { return NULL; }
    //int start = 0, stop = strlen( str )-1;
    int start = 0, stop = strlen( str );

    for(int i=0; i <= stop; i++) {
      if ( str[ i ] > 32 ) {
        start = i;
        break;
      }
    }

    if ( start == stop ) { char* empty = (char*)malloc(0+1); empty[0]=0x00; return empty; }

    for(int i=stop; i >= 0; i--) {
      if ( str[ i ] > 32 ) {
        stop = i;
        break;
      }
    }

    // printf("start=%d stop=%d \n", start, stop);

    char* result = (char*)malloc( (stop-start) +1);
    int cpt=0;
    for(int i=start; i <= stop; i++) {
      result[cpt++] = str[i];
    }
    result[ cpt ] = 0x00;

    return result;
  }


  char* nextSplit( char* remaining, int fullLen, char delim, bool treatEscapes ) {
    // reads remaining, fill w/ 0x00 readed chars then copy those to result; 
    int start = -1, stop = -1;
    //DBUG("1");
    for(int i=0; i < fullLen; i++) {
      if ( remaining[i] == 0x00 ) { continue; }
      if ( start == -1 ) { start = i; }

      if ( remaining[i] == '\\' && i+1 < fullLen && remaining[i+1] == delim && treatEscapes ) {
        i++;
        continue;
      }

      if ( remaining[i] == delim ) {
        stop = i+1;
        break;
      }
    }
    // reached end of line ?
    if ( start == -1 ) { return NULL; }

    //DBUG("2");
    if ( stop == -1 ) { stop = fullLen; }
    //printf("start=%d stop=%d \n", start, stop);

    // TODO : check '<= 0' instead of '< 0'
    if ( stop - start <= 0 ) { /*DBUG("eject 3");*/ return NULL; }
    else {
      //DBUG("4");

      char* result = (char*)malloc( stop-start-1+1 );

      //DBUG("5");
      //printf("start=%d stop=%d \n", start, stop);

      int cpt=0;
      for(int i=start; i <= stop; i++) {
        //printf(" 5.1 > %d [%c] \n", i, remaining[i]);

        if ( remaining[i] == '\\' && i+1 < fullLen && remaining[i+1] == delim && treatEscapes ) {
          // nothing
          //DBUG(" 5.2");
        } else if ( remaining[i] == delim && i == stop-1 ) {
          //DBUG(" 5.3");
          remaining[i] = 0x00; // erase readed chars
          break;
        } else {
          //DBUG(" 5.4");
          result[ cpt++ ] = remaining[i]; 
        }
        //DBUG(" 5.5");
        remaining[i] = 0x00; // erase readed chars
        //DBUG(" 5.6");
      }

      //DBUG("6");
      result[ cpt ] = 0x00;

      //DBUG("7");

      return result;
    }

    //DBUG("eject 8");

    return NULL;
  }

extern int storeNumVariable(char* name, float val);
extern int setNumArrayElem(char* name, float val);
extern char* autocomplete_fileExt(char*, const char* defExt);

// DATAF "TOTO", "SZ", "A$", "HP", "FO", "DE"
// reads "TOTO.BAD" from Fs
// feeds "SZ" with nb of lines
// feeds args given array names with line content
// ~~ CSV content
int xts_dataf_cmd() {
  getNextToken();
  
  const int MAX_ARGS = 12; // <file>, <sizeVar>, <10 arrays>
  char* args[MAX_ARGS]; // string args
  int   argc = 0;

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
        for(int i=0; i < stlen; i++) { tmp[i] = charUpCase( tt[i] ); }
        tmp[ stlen ] = 0x00;
        args[argc++] = tmp;
      }
    } else {
      return ERROR_BAD_PARAMETER;
    }

    if ( curToken == TOKEN_COMMA ) {
      getNextToken();
    }
  }

  if ( !executeMode ) {
    if (val & _ERROR_MASK) return val;
  }
  
  if ( executeMode ) {
      if ( argc >= 2 ) {
        // args[0] => filename str INPUT
        // args[1] => rowCount int OUTPUT
        // 10 useable typed arrays

        // DATAF "TOTO", "SZ", "A$", "HP", "FO", "DE"

        // [3
        // [# optionally remmark line, can be col. names (not counted)
        // [Rolph;20;20;15
        // [Mula;15;15;20
        // [Grumph;50;10;30

        if ( ! fopenTextFile( autocomplete_fileExt(args[0], ".BAD") ) ) {
          host_outputString("Failed to open file\n");
          host_showBuffer();

          return ERROR_VARIABLE_NOT_FOUND;
        }

        char* line, *oline; int cpt=0,total=-1;
        //Serial.println("DATAF 1");
        while( (oline = freadTextLine()) != NULL ) {
          //Serial.println("DATAF 2");
          line = str_trim( oline );
          //Serial.println("DATAF 3");
          // free(oline); ---> make crash .....
          //Serial.println("DATAF 4");

          if ( strlen(line) == 0 || line[0] == '#' ) {
            continue;
          }

          //Serial.println("DATAF 5");
          Serial.println( line );

          if ( total == -1 ) {
            // RowCount line

            total = atoi(line);
            cpt = 1;
            Serial.print("DATAF 6 Row Count : "); Serial.println(total);

            storeNumVariable(args[1], (float) total);
            //Serial.println("DATAF 7");

            for(int i=2; i < argc; i++) {
              int llen = strlen(args[i]);
              bool isStrArray = llen > 0 && args[i][ llen-1 ] == '$';

              if ( ! xts_createArray( args[i] , isStrArray ? 1 : 0, total) ) {
                host_outputString("Could not create ");
                host_outputString( args[i] );
                host_outputString(" column\n");
                host_showBuffer();
                free(line);
                fcloseFile();
                return ERROR_OUT_OF_MEMORY;
              }
            }
          } else {
            // regular line

            //char* remaining = copyOf( line );
            char* remaining = line;
            int fullLen = strlen( remaining );
            for(int i=2; i < argc; i++) {
              int llen = strlen(args[i]);
              bool isStrArray = llen > 0 && args[i][ llen-1 ] == '$';
              bool col_ok = false; int err=ERROR_NONE;
              // HAVE TO make my own split() routine
              // able to escape '\;' sequence
              char* token = nextSplit( remaining, fullLen, ';', true );
              if ( !isStrArray ) {
                float val = atof( token );
                Serial.print("DATAF 14bis >");Serial.print(val);Serial.println("<");
                col_ok = (err= xts_setNumArrayElem( args[i], cpt, val )) == ERROR_NONE;
              } else {
                col_ok = (err= xts_setStrArrayElem( args[i], cpt, token )) == ERROR_NONE;
              } 

              if ( !col_ok ) {
                host_outputString("Could not fill ");
                host_outputString( args[i] );
                host_outputString(" column @row=");
                host_outputInt( cpt );
                host_outputString(" cause : ");
                host_outputInt( err );
                host_outputString("\n");
                host_showBuffer();

                // if (token != NULL) free( token );
                // free( line );

                fcloseFile();
                return ERROR_IN_VAL_INPUT;
              }
              //if (token != NULL) free(token);
            }
            cpt++;
            //if (line != NULL) free( line );
            if ( cpt > total ) {
              host_outputString("file truncated !");
              break;
            }
          }

        }
        //if (line != NULL) free(line);
        fcloseFile();
      } // end of argc > 0
      else {
        // missing args
        return ERROR_BAD_PARAMETER;
      }

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




