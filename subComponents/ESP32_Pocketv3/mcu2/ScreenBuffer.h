#ifndef __SCREEN_BUFFER__
#define __SCREEN_BUFFER__ 1

 // TYPE+COORDS+FILENAME
 #define ACTION_REC_SIZE  (1+12+13)
 // 50x26 -> 1300 bytes
 #define ACTION_BUFF_SIZE (50)

 // NO TEXT BUFFER 'cause of TTY BUFFER
 // #define TEXT_REC_SIZE   (128)

 // 1350 bytes -- max size
 #define TEXT_TTY_SIZE   (45*30)

 // not final !!! Cf screen mode
 int ttyWidth  = 45;
 int ttyHeight = 30;

 char tty_memseg[TEXT_TTY_SIZE];
 unsigned char act_memseg[ACTION_BUFF_SIZE * ACTION_REC_SIZE];
 
 int act_cursor = 0;
 int tty_cursorX = 0;
 int tty_cursorY = 0;

 void ttyCLS() {
     memset( tty_memseg, 0x00, ttyWidth*ttyHeight );
     tty_cursorX = 0;
     tty_cursorY = 0;
 }

 void ttyBR() {
     tty_cursorX = 0;
     tty_cursorY++;
     if ( tty_cursorY >= ttyHeight ) {
         // TODO : see if scroll -or- not
         ttyCLS();
     }
     // TODO : see if empty the current line
 }

 void ttyBKSP() {
     tty_cursorX--;
     if ( tty_cursorX < 0 ) {
        if ( tty_cursorY <= 0 ) {
            tty_cursorX = 0;
            tty_cursorY = 0;
            return;
        }
        tty_cursorY--;
        tty_cursorX = ttyWidth-1;
     }
 }

 void ttyChar(char ch) {
    if ( ch == '\b' ) {
        ttyBKSP();
    } else if ( ch == '\r' ) {
        // nothing
    } else if ( ch == '\n' ) {
        ttyBR();
    } else {
        if ( tty_cursorX >= ttyWidth ) {
            ttyBR();
        }
        int addr = (tty_cursorY*ttyHeight);
        tty_memseg[ addr+tty_cursorX ] = ch;
        tty_cursorX++;
        if ( addr+tty_cursorX <= TEXT_TTY_SIZE ) {
            tty_memseg[ addr+tty_cursorX ] = 0x00;
        }
    }
 }

 void ttyString(const char* str) {
     // TODO : tokenize by spe chars
     if ( str == NULL ) { return; }
     int len = strlen(str);
     for( int i=0; i < len; i++ ) {
         ttyChar( str[i] );
     }
 }

 void ttyString(char* str) {
     ttyString( (const char*) str );
 }

 void ttyInt(int val) {
     static char num[11+1];
     sprintf( num, "%d", val );
     ttyString( (const char*) num );
 }

 void ttyFloat(float val) {
     static char num[16+1];
     sprintf( num, "%f", val );
     ttyString( (const char*) num );
 }

 // can be (x,y,w,h,m,c)
 //        (x,y,r,m,c)
 bool storeAction(uint8_t type, uint16_t x=0, uint16_t y=0, uint16_t w=0, uint16_t h=0, uint16_t sx=0, uint16_t sy=0, char* filename=NULL, float val=-1.0) {
    if ( act_cursor >= ACTION_BUFF_SIZE ) {
        return false;
    }
    int base_addr = (ACTION_REC_SIZE*act_cursor);
    act_memseg[ base_addr + 0 ] = type;

    if ( type == SIG_SCR_CLEAR ) {
    } else if ( type == SIG_SCR_DRAW_RECT ) {
        int off = 1;
        act_memseg[ base_addr + (off++) ] = x>>8;
        act_memseg[ base_addr + (off++) ] = x%256;
        act_memseg[ base_addr + (off++) ] = y>>8;
        act_memseg[ base_addr + (off++) ] = y%256;
        act_memseg[ base_addr + (off++) ] = w>>8;
        act_memseg[ base_addr + (off++) ] = w%256;
        act_memseg[ base_addr + (off++) ] = h>>8;
        act_memseg[ base_addr + (off++) ] = h%256;
        act_memseg[ base_addr + (off++) ] = (uint8_t)sx; // mode
        act_memseg[ base_addr + (off++) ] = sy>>8;       // color
        act_memseg[ base_addr + (off++) ] = sy%256;
    } else if ( type == SIG_SCR_DRAW_CIRCLE ) {
        int off = 1;
        act_memseg[ base_addr + (off++) ] = x>>8;
        act_memseg[ base_addr + (off++) ] = x%256;
        act_memseg[ base_addr + (off++) ] = y>>8;
        act_memseg[ base_addr + (off++) ] = y%256;
        act_memseg[ base_addr + (off++) ] = w>>8;        // radius
        act_memseg[ base_addr + (off++) ] = w%256;
        act_memseg[ base_addr + (off++) ] = (uint8_t)sx; // mode
        act_memseg[ base_addr + (off++) ] = sy>>8;       // color
        act_memseg[ base_addr + (off++) ] = sy%256;
    } else if ( type == SIG_SCR_DRAW_LINE ) {
        int off = 1;
        act_memseg[ base_addr + (off++) ] = x>>8;
        act_memseg[ base_addr + (off++) ] = x%256;
        act_memseg[ base_addr + (off++) ] = y>>8;
        act_memseg[ base_addr + (off++) ] = y%256;
        act_memseg[ base_addr + (off++) ] = w>>8;        // x2
        act_memseg[ base_addr + (off++) ] = w%256;
        act_memseg[ base_addr + (off++) ] = h>>8;        // y2
        act_memseg[ base_addr + (off++) ] = h%256;
        act_memseg[ base_addr + (off++) ] = sx>>8;       // color
        act_memseg[ base_addr + (off++) ] = sx%256;
    } else if ( type == SIG_SCR_DRAW_PIX ) {
        int off = 1;
        act_memseg[ base_addr + (off++) ] = x>>8;
        act_memseg[ base_addr + (off++) ] = x%256;
        act_memseg[ base_addr + (off++) ] = y>>8;
        act_memseg[ base_addr + (off++) ] = y%256;
        act_memseg[ base_addr + (off++) ] = w>>8;        // color
        act_memseg[ base_addr + (off++) ] = w%256;
    } else if ( type == SIG_SCR_DRAW_BPP ) {
        int off = 1;
        act_memseg[ base_addr + (off++) ] = x>>8;
        act_memseg[ base_addr + (off++) ] = x%256;
        act_memseg[ base_addr + (off++) ] = y>>8;
        act_memseg[ base_addr + (off++) ] = y%256;
        
        // TODO : copy filename into act_memseg[]

    } else if ( type == SIG_SCR_PRINT_CH ) {
        // auto dispatch to TTY mem
        char ch = (char)x;
        ttyChar(ch);
        // quit w/o increment act_cursor !!!
        return true;
    } else if ( type == SIG_SCR_PRINT_STR ) {
        // auto dispatch to TTY mem
        ttyString( filename );
        // quit w/o increment act_cursor !!!
        return true;
    } else if ( type == SIG_SCR_PRINT_INT ) {
        // auto dispatch to TTY mem
        ttyInt( x );
        // quit w/o increment act_cursor !!!
        return true;
    } else if ( type == SIG_SCR_PRINT_NUM ) {
        // auto dispatch to TTY mem
        ttyFloat( val );
        // quit w/o increment act_cursor !!!
        return true;
    } else if ( type == SIG_SCR_CLEAR ) {
        // type is already stored...
        ttyCLS();
    } else if ( type == SIG_SCR_MODE ) {
        // NOT Subject to buffer Op
        __doBlitt();
        uint8_t mode = (uint8_t)x;
        mcu.getScreen()->setMode( mode );
        // auto dispatch to TTY mem
        tty_cursorX = 0; tty_cursorY = 0;
        if ( mode == SCREEN_MODE_128 ){
          ttyWidth = 21;
          ttyHeight = 8;
        } else if ( mode == SCREEN_MODE_160 ){
          ttyWidth = 21; // TODO : fix it
          ttyHeight = 16;
        } else if ( mode == SCREEN_MODE_320 ){
          ttyWidth = 45;
          ttyHeight = 30;
        }
        memset( tty_memseg, 0x00, TEXT_TTY_SIZE );
        // quit w/o increment act_cursor !!!
        return true;
    } else {
        // Oups
    }
    act_cursor++;
    return true;
 }





#endif