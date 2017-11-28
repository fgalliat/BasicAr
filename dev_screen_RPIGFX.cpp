/**
  RPI0W : Serial HDMI Controller
  Control routines

  @Xtase - fgalliat Nov2017
*/

#ifndef COMPUTER
  #include <Arduino.h>
  #include <SoftwareSerial.h>
#else
  #include "computer.h"
  #define max(a,b) a > b ? a : b
#endif

// on Teensy 3.6 : alt pins for Serial1 (aka Serial)
#define RPID_RX 27
#define RPID_TX 26
SoftwareSerial SoftSerial1( RPID_RX, RPID_TX );
#define SerialRPID SoftSerial1

bool yetLayoutGUI=false;

void rpid_reboot(bool wait);
void rpid_cls();

void setup_rpid(bool wait) {
    SerialRPID.begin(115200);
    SerialRPID.flush();

    //// BEWARE if don't wait reboot
    //vgat_reboot(wait);
    yetLayoutGUI=false;
}

void _rpid_cmd(const char* cmd, long delayToWait) {
    SerialRPID.print("^[");   // send escape sequence
    SerialRPID.println(cmd);  // send Command string

    delay( delayToWait );
}

void rpid_reboot(bool wait=true) {
  _rpid_cmd("r", wait?2000:500);

  yetLayoutGUI=false;
}

// 1 to 9
char _rpidFocus[] = {'f',',','?',0x00};
void rpid_focuswin(int winNum) {
  _rpidFocus[2] = (char)('0'+winNum);
  _rpid_cmd( (char*)_rpidFocus, 2 );
}

void rpid_clearwin() {
    _rpid_cmd( (char*)"e", 2 );
}

void rpid_cls() {
    rpid_clearwin();
}

char* rpidLocateSeq = NULL;
void rpid_locate(int left, int top) {
    if (rpidLocateSeq == NULL) { rpidLocateSeq = (char*)malloc( 1+1+3+1+3+1 ); }
    sprintf( rpidLocateSeq, "p,%d,%d", left, top );
    _rpid_cmd( rpidLocateSeq, 20 );
}

// winNum 1 to 9
#define MAX_TITLE_LEN 80
char* _rpidWinDesc = NULL;
void rpid_createwin(int winNum, int left, int top, int width, int height, bool border, const char* title) {
    if (_rpidWinDesc == NULL) { _rpidWinDesc = (char*)malloc( 1+1+1+3+1+3+1+3+1+3+1+1+ ( MAX_TITLE_LEN )+1 ); }
    // char* str = (char*)malloc( 1+1+1+3+1+3+1+3+1+3+1+1+ ( title == NULL ? 0 : strlen(title) )+1 );
    sprintf( _rpidWinDesc, "w%d,%d,%d,%d,%d,%d,%s", winNum, left, top, width, height, (border ? 1 : 0), ( title == NULL ? "" : title ) );
    _rpid_cmd( _rpidWinDesc, 20 );
    // free(str);
}


char* _rpidLine = NULL;
void rpid_print(char* str, int v) {
    // TODO : better Cf va args
    if ( _rpidLine == NULL ) { _rpidLine = (char*)malloc(128+1); }
    //memset(_vgatLine, 0x00, 128+1);

    //int nbLF = strcount(str, '\n');
    int llen = strlen(str);

// TODO : loop in substring
if ( llen > 128 ) { llen = 128; }

    //char* line = (char*)malloc( nbLF+llen+1 );
    int d=0;
    for(int i=0; i<llen; i++) {
        if ( str[i] == '\n' ) { _rpidLine[d++] = '\r'; }
        _rpidLine[d++] = str[i];
    }
    _rpidLine[d++] = 0x00;

    SerialRPID.printf( _rpidLine, v );
    SerialRPID.flush();
}

void rpid_print(char* str) {
    rpid_print( str, -1 );
}


void rpid_print(const char* str, int v) {
   rpid_print((char*) str, v);
}

void rpid_print(const char* str) {
    rpid_print((char*) str);
}

void rpid_printCh(char ch) {
    if ( ch == '\n' && true ) { SerialRPID.print( '\r' ); }
    SerialRPID.print( (char)ch );
    SerialRPID.flush();
}

/** may not use it to send a command */
void rpid_print(char ch, bool autoReplaceLF = true) {
    if ( ch == '\n' && autoReplaceLF ) { SerialRPID.print( '\r' ); }
    SerialRPID.print( ch );
}

// =============================================

void rpid_drawPixel(char px, int left, int top) {
    if ( left < 0 || top < 0 || left > 79 || top > 25 ) { return; }
    rpid_locate( left, top );
    rpid_print(px, false);
}

void rpid_Vline(int x1, int y1, int y2) {
    int yStart = min(y1,y2);
    int yStop  = max(y1,y2);
    for(int i=yStart; i <= yStop; i++) { rpid_drawPixel('|', x1, i); }
}

void rpid_Hline(int x1, int y1, int x2) {
    int xStart = min(x1,x2);
    int xStop  = max(x1,x2);
    for(int i=xStart; i <= xStop; i++) { rpid_drawPixel('-', i, y1); }
}


void rpid_line(int x1, int y1, int x2, int y2) {
  if ( x1 == x2 ) { rpid_Vline(x1, y1, y2); }
  else if ( y1 == y2 ) { rpid_Hline(x1, y1, x2); }
  else {
    char ch;
    int X1=x1,X2=x2,Y1=y1,Y2=y2;
    if ( x1 > x2 ) {
        X1=x2; Y1=y2; X2=x1; Y2=y1;
        if ( y1 > y2 ) { ch = '\\'; }
        else { ch = '/'; }
    } else {
        if ( y1 > y2 ) { ch = '/'; }
        else { ch = '\\'; }
    }
    
    int dx = X2 - X1;
    int dy = Y2 - Y1;
    int y;
    for (int x=X1; x < X2; x++) {
        y = Y1 + dy * (x - X1) / dx;
        rpid_drawPixel(ch, x, y);
    }

  }
}

// =============================================

void rpid_startScreen() {
    if (yetLayoutGUI) { return; }
    yetLayoutGUI = true;

    // +2 & +3 cf borders
    rpid_createwin(1, 10-1, 10-5-2, 100-10-10+2, 50-10-10+3, true, "Main Window");

    rpid_createwin(2, 10, 0, 100-10-10, 1, false, NULL);
    _rpid_cmd("l,0,1,222,101", 2);    // White text on purple lines 0-1
    rpid_cls();
    rpid_print("Xtase uBasic v1.0");

    rpid_createwin(3, 10, 50-1, 100-10-10, 1, false, NULL);
    _rpid_cmd("l,49,49,000,111", 2); // black text on grey line 49
    rpid_cls();
    rpid_print("Xtase - fgalliat @ 01/2017");

    rpid_createwin(4, 10-1, 50-10-10+6+1, 100-10-10+2, 10+1, true, "Status");
    _rpid_cmd("l,37,47,222,001", 2);  // white text on blue lines 36-46

    // char window4_colour[] = {"l,35,48,222,001"};  // white text on blue lines 35-48
    // char footer_colour[]  = {"l,49,49,000,111"};  // black text on grey line 49

    rpid_focuswin(1);
    rpid_cls();
}


void rpid_switchTo() {
    //rpid_reboot();
    rpid_startScreen();

    rpid_focuswin(4); // clear status/err win
    rpid_cls();
    rpid_focuswin(1); // clear main win
    rpid_cls();
}

