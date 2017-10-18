/**
  HobbyTronics : Serial VGA Monitor Driver Board v1.1
  Control routines

  @Xtase - fgalliat Jan2017
*/

#include <Arduino.h>

// Le croquis utilise 93 848 octets (8%) de l'espace de stockage de programmes. Le maximum est de 1 048 576 octets.
// Les variables globales utilisent 36 504 octets (13%) de mémoire dynamique, ce qui laisse 225 640 octets pour les variable
// s locales. Le maximum est de 262 144 octets.

// Le croquis utilise 94 512 octets (9%) de l'espace de stockage de programmes. Le maximum est de 1 048 576 octets.
// Les variables globales utilisent 36 504 octets (13%) de mémoire dynamique, ce qui laisse 225 640 octets pour les variable
// s locales. Le maximum est de 262 144 octets.


#define SerialVGA Serial3

bool yetLayoutVGA=false;

void vgat_reboot(bool wait);
void vgat_cls();

void setup_vgat(bool wait) {
    // BEWARE this port is shared w/ USB Keyboard driver
    SerialVGA.begin(9600);
    SerialVGA.flush();

    //// BEWARE if don't wait reboot
    //vgat_reboot(wait);
    yetLayoutVGA=false;
}

void _vga_cmd(const char* cmd, long delayToWait) {
    SerialVGA.print("^[");   // send escape sequence
    SerialVGA.println(cmd);  // send Command string

    delay( delayToWait );
}

void vgat_reboot(bool wait=true) {
  _vga_cmd("r", wait?2000:500);

  yetLayoutVGA=false;
}

// 1 to 9
char _vgatFocus[] = {'f',',','?',0x00};
void vgat_focuswin(int winNum) {
  _vgatFocus[2] = (char)('0'+winNum);
  _vga_cmd( (char*)_vgatFocus, 2 );
}

void vgat_clearwin() {
    _vga_cmd( (char*)"e", 2 );
}

void vgat_cls() {
    vgat_clearwin();
}

char* vgaLocateSeq = NULL;
void vgat_locate(int left, int top) {
    if (vgaLocateSeq == NULL) { vgaLocateSeq = (char*)malloc( 1+1+3+1+3+1 ); }
    sprintf( vgaLocateSeq, "p,%d,%d", left, top );
    _vga_cmd( vgaLocateSeq, 20 );
}

// winNum 1 to 9
#define MAX_TITLE_LEN 80
char* _vgaWinDesc = NULL;
void vgat_createwin(int winNum, int left, int top, int width, int height, bool border, const char* title) {
    if (_vgaWinDesc == NULL) { _vgaWinDesc = (char*)malloc( 1+1+1+3+1+3+1+3+1+3+1+1+ ( MAX_TITLE_LEN )+1 ); }
    // char* str = (char*)malloc( 1+1+1+3+1+3+1+3+1+3+1+1+ ( title == NULL ? 0 : strlen(title) )+1 );
    sprintf( _vgaWinDesc, "w%d,%d,%d,%d,%d,%d,%s", winNum, left, top, width, height, (border ? 1 : 0), ( title == NULL ? "" : title ) );
    _vga_cmd( _vgaWinDesc, 20 );
    // free(str);
}


char* _vgatLine = NULL;
void vgat_print(char* str, int v) {
    // TODO : better Cf va args
    if ( _vgatLine == NULL ) { _vgatLine = (char*)malloc(128+1); }
    //memset(_vgatLine, 0x00, 128+1);

    //int nbLF = strcount(str, '\n');
    int llen = strlen(str);

// TODO : loop in substring
if ( llen > 128 ) { llen = 128; }

    //char* line = (char*)malloc( nbLF+llen+1 );
    int d=0;
    for(int i=0; i<llen; i++) {
        if ( str[i] == '\n' ) { _vgatLine[d++] = '\r'; }
        _vgatLine[d++] = str[i];
    }
    _vgatLine[d++] = 0x00;

    SerialVGA.printf( _vgatLine, v );
    // free(line);
}

void vgat_print(char* str) {
    vgat_print( str, -1 );
}


void vgat_print(const char* str, int v) {
   vgat_print((char*) str, v);
}

void vgat_print(const char* str) {
    vgat_print((char*) str);
}


/** may not use it to send a command */
void vgat_print(char ch, bool autoReplaceLF = true) {
    if ( ch == '\n' && autoReplaceLF ) { SerialVGA.print( '\r' ); }
    SerialVGA.print( ch );
}

// =============================================

void vgat_drawPixel(char px, int left, int top) {
    if ( left < 0 || top < 0 || left > 79 || top > 25 ) { return; }
    vgat_locate( left, top );
    vgat_print(px, false);
}

void vgat_Vline(int x1, int y1, int y2) {
    int yStart = min(y1,y2);
    int yStop  = max(y1,y2);
    for(int i=yStart; i <= yStop; i++) { vgat_drawPixel('|', x1, i); }
}

void vgat_Hline(int x1, int y1, int x2) {
    int xStart = min(x1,x2);
    int xStop  = max(x1,x2);
    for(int i=xStart; i <= xStop; i++) { vgat_drawPixel('-', i, y1); }
}


void vgat_line(int x1, int y1, int x2, int y2) {
  if ( x1 == x2 ) { vgat_Vline(x1, y1, y2); }
  else if ( y1 == y2 ) { vgat_Hline(x1, y1, x2); }
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
        vgat_drawPixel(ch, x, y);
    }

  }
}

// =============================================

void vgat_startScreen() {
    if (yetLayoutVGA) { return; }
    yetLayoutVGA = true;

    // +2 & +3 cf borders
    vgat_createwin(1, 10-1, 10-5-2, 100-10-10+2, 50-10-10+3, true, "Main Window");

    vgat_createwin(2, 10, 0, 100-10-10, 1, false, NULL);
    _vga_cmd("l,0,1,222,101", 2);    // White text on purple lines 0-1
    vgat_cls();
    vgat_print("Xtase uBasic v1.0");

    vgat_createwin(3, 10, 50-1, 100-10-10, 1, false, NULL);
    _vga_cmd("l,49,49,000,111", 2); // black text on grey line 49
    vgat_cls();
    vgat_print("Xtase - fgalliat @ 01/2017");

    vgat_createwin(4, 10-1, 50-10-10+6+1, 100-10-10+2, 10+1, true, "Status");
    _vga_cmd("l,37,47,222,001", 2);  // white text on blue lines 36-46

    // char window4_colour[] = {"l,35,48,222,001"};  // white text on blue lines 35-48
    // char footer_colour[]  = {"l,49,49,000,111"};  // black text on grey line 49

    vgat_focuswin(1);
    vgat_cls();
}


extern int ext_OUTPUT_VGA_BOARD_TXT;
extern void setRawOutputType(int out); 

void vgat_switchTo() {
    //vgat_reboot();
    vgat_startScreen();

    vgat_focuswin(4); // clear status/err win
    vgat_cls();
    vgat_focuswin(1); // clear main win
    vgat_cls();

    setRawOutputType( ext_OUTPUT_VGA_BOARD_TXT );
}

