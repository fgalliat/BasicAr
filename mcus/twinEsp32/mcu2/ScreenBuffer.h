#ifndef __SCREEN_BUFFER__
#define __SCREEN_BUFFER__ 1

 // TYPE+COORDS+FILENAME+0x00
 #define ACTION_REC_SIZE  (1+12+13+1)
 // 50x27 -> 1350 bytes
 #define ACTION_BUFF_SIZE (50)

 #define MAX_TTY_WIDTH 45
 #define MAX_TTY_HEIGHT 30

 // 1350 bytes -- max size
 #define TEXT_TTY_SIZE   (MAX_TTY_WIDTH*MAX_TTY_HEIGHT)

 // not final !!! Cf screen mode
 int ttyWidth  = MAX_TTY_WIDTH;
 int ttyHeight = MAX_TTY_HEIGHT;

 char tty_memseg[TEXT_TTY_SIZE];

 unsigned char act_memseg[ACTION_BUFF_SIZE * ACTION_REC_SIZE];
 
 int act_cursor = 0;
 int tty_cursorX = 0;
 int tty_cursorY = 0;
 bool __textIsNotEmpty = false;

 void _b_doBlitt();
 void _b_doBlittText();

 void _b_blittIfNeeded() {
   if ( __screenBlittMode == SCREEN_BLITT_AUTO ) {
     _b_doBlitt();  
   }  
 }
 bool storeAction(uint8_t type, int val);

 void ttyCLS() {
     memset( tty_memseg, 0x00, ttyWidth*ttyHeight );
     tty_cursorX = 0;
     tty_cursorY = 0;
     __textIsNotEmpty = false;
 }

 void ttyBR() {
     if (tty_cursorX>=0 && tty_cursorY>=0) {
         tty_memseg[ (tty_cursorY*ttyWidth)+tty_cursorX ] = 0x00;
     }
     tty_cursorX = 0;
     tty_cursorY++;
     if ( tty_cursorY >= ttyHeight ) {
         // TODO : see if scroll -or- not
         // ttyCLS();
         storeAction( SIG_SCR_CLEAR, 0 );
     }
     // TODO : see if empty the current line
 }

 void ttyBKSP() {
     if (tty_cursorX>=0 && tty_cursorY>=0) {
         tty_memseg[ (tty_cursorY*ttyWidth)+tty_cursorX ] = 0x00;
     }
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
        // if ( tty_cursorX >= ttyWidth ) {
        //     ttyBR();
        // }
        int addr = (tty_cursorY*ttyWidth);
        tty_memseg[ addr+tty_cursorX ] = ch;
        tty_cursorX++;
        if ( tty_cursorX >= ttyWidth ) {
            ttyBR();
        } else if ( addr+tty_cursorX < TEXT_TTY_SIZE ) {
            tty_memseg[ addr+tty_cursorX ] = 0x00;
        }
    }
    __textIsNotEmpty = true;
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

 // used for setCursor ONLY, @ this time
 void ttyFillLine( int ttyY ) {
   //for(int i=0; i < ttyWidth; i++) {
   int addr = (tty_cursorY * ttyWidth);
   for(int i=0; i < tty_cursorX; i++) {
     if ( tty_memseg[ addr + i ] == 0x00 ) {
         tty_memseg[ addr + i ] = 0xFF;
     }
   }
 }



bool storeAction(uint8_t type, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t x3, uint16_t y3, uint16_t sx, uint16_t sy, char* filename, float val) {
 // bool storeAction(uint8_t type, uint16_t x=0, uint16_t y=0, uint16_t w=0, uint16_t h=0, uint16_t x3=0, uint16_t y3=0, uint16_t sx=0, uint16_t sy=0, char* filename=NULL, float val=-1.0) {
    if ( act_cursor >= ACTION_BUFF_SIZE ) {
        //return false;
        _b_doBlitt();
        return true;
    }
    int base_addr = (ACTION_REC_SIZE*act_cursor);
    act_memseg[ base_addr + 0 ] = type;

    if ( type == SIG_SCR_DRAW_RECT ) {
        int off = 1;
        act_memseg[ base_addr + (off++) ] = (uint8_t)(x/256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(x%256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(y/256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(y%256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(w/256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(w%256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(h/256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(h%256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)sx;        // mode
        act_memseg[ base_addr + (off++) ] = (uint8_t)(sy/256);  // color
        act_memseg[ base_addr + (off++) ] = (uint8_t)(sy%256);
    } else if ( type == SIG_SCR_DRAW_TRIANGLE ) {
        int off = 1;
        act_memseg[ base_addr + (off++) ] = (uint8_t)(x/256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(x%256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(y/256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(y%256);

        act_memseg[ base_addr + (off++) ] = (uint8_t)(w/256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(w%256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(h/256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(h%256);

        act_memseg[ base_addr + (off++) ] = (uint8_t)(x3/256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(x3%256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(y3/256);
        act_memseg[ base_addr + (off++) ] = (uint8_t)(y3%256);

        act_memseg[ base_addr + (off++) ] = (uint8_t)sx;        // mode
        act_memseg[ base_addr + (off++) ] = (uint8_t)(sy/256);  // color
        act_memseg[ base_addr + (off++) ] = (uint8_t)(sy%256);
    } else if ( type == SIG_SCR_DRAW_CIRCLE ) {
        int off = 1;
        act_memseg[ base_addr + (off++) ] = x/256;
        act_memseg[ base_addr + (off++) ] = x%256;
        act_memseg[ base_addr + (off++) ] = y/256;
        act_memseg[ base_addr + (off++) ] = y%256;
        act_memseg[ base_addr + (off++) ] = w/256;        // radius
        act_memseg[ base_addr + (off++) ] = w%256;
        act_memseg[ base_addr + (off++) ] = (uint8_t)sx; // mode
        act_memseg[ base_addr + (off++) ] = sy/256;       // color
        act_memseg[ base_addr + (off++) ] = sy%256;
    } else if ( type == SIG_SCR_DRAW_SHAPE_BALL ) {
        int off = 1;
        act_memseg[ base_addr + (off++) ] = x/256;
        act_memseg[ base_addr + (off++) ] = x%256;
        act_memseg[ base_addr + (off++) ] = y/256;
        act_memseg[ base_addr + (off++) ] = y%256;
        act_memseg[ base_addr + (off++) ] = w/256;        // radius
        act_memseg[ base_addr + (off++) ] = w%256;
        act_memseg[ base_addr + (off++) ] = (uint8_t)sx; // mode
        act_memseg[ base_addr + (off++) ] = sy/256;       // color
        act_memseg[ base_addr + (off++) ] = sy%256;
    } else if ( type == SIG_SCR_DRAW_LINE ) {
        int off = 1;
        act_memseg[ base_addr + (off++) ] = x/256;
        act_memseg[ base_addr + (off++) ] = x%256;
        act_memseg[ base_addr + (off++) ] = y/256;
        act_memseg[ base_addr + (off++) ] = y%256;
        act_memseg[ base_addr + (off++) ] = w/256;        // x2
        act_memseg[ base_addr + (off++) ] = w%256;
        act_memseg[ base_addr + (off++) ] = h/256;        // y2
        act_memseg[ base_addr + (off++) ] = h%256;
        act_memseg[ base_addr + (off++) ] = sx/256;       // color
        act_memseg[ base_addr + (off++) ] = sx%256;
    } else if ( type == SIG_SCR_DRAW_PIX ) {
        int off = 1;
        act_memseg[ base_addr + (off++) ] = x/256;
        act_memseg[ base_addr + (off++) ] = x%256;
        act_memseg[ base_addr + (off++) ] = y/256;
        act_memseg[ base_addr + (off++) ] = y%256;
        act_memseg[ base_addr + (off++) ] = w/256;        // color
        act_memseg[ base_addr + (off++) ] = w%256;
    } else if ( type == SIG_SCR_DRAW_BPP ) {
        int off = 1;
        act_memseg[ base_addr + (off++) ] = x/256;
        act_memseg[ base_addr + (off++) ] = x%256;
        act_memseg[ base_addr + (off++) ] = y/256;
        act_memseg[ base_addr + (off++) ] = y%256;
        
        int len=strlen( filename );
        for(int i=0; i < len; i++) { 
            act_memseg[ base_addr + (off++) ] = filename[i];
        }
        act_memseg[ base_addr + (off++) ] = 0x00;

    } else if ( type == SIG_SCR_DRAW_PCT ) {
        int off = 1;
        act_memseg[ base_addr + (off++) ] = x/256;
        act_memseg[ base_addr + (off++) ] = x%256;
        act_memseg[ base_addr + (off++) ] = y/256;
        act_memseg[ base_addr + (off++) ] = y%256;
        
        int len=strlen( filename );
        for(int i=0; i < len; i++) { 
            act_memseg[ base_addr + (off++) ] = filename[i];
        }
        act_memseg[ base_addr + (off++) ] = 0x00;

    } else if ( type == SIG_SCR_DRAW_PCT_SPRITE ) {
        int off = 1;
        act_memseg[ base_addr + (off++) ] = x/256;
        act_memseg[ base_addr + (off++) ] = x%256;
        act_memseg[ base_addr + (off++) ] = y/256;
        act_memseg[ base_addr + (off++) ] = y%256;
        act_memseg[ base_addr + (off++) ] = w/256;
        act_memseg[ base_addr + (off++) ] = w%256;
        act_memseg[ base_addr + (off++) ] = h/256;
        act_memseg[ base_addr + (off++) ] = h%256;
        act_memseg[ base_addr + (off++) ] = sx/256;
        act_memseg[ base_addr + (off++) ] = sx%256;
        act_memseg[ base_addr + (off++) ] = sy/256;
        act_memseg[ base_addr + (off++) ] = sy%256;

        int len=strlen( filename );
        for(int i=0; i < len; i++) { 
            act_memseg[ base_addr + (off++) ] = filename[i];
        }
        act_memseg[ base_addr + (off++) ] = 0x00;

    } 
    else if ( type == SIG_SCR_PRINT_CH ) {
        // auto dispatch to TTY mem
        char ch = (char)x;
        ttyChar(ch);
        // quit w/o increment act_cursor !!!
        _b_blittIfNeeded();
        return true;
    } else if ( type == SIG_SCR_PRINT_STR ) {
        // auto dispatch to TTY mem
        ttyString( filename );
        // quit w/o increment act_cursor !!!
        _b_blittIfNeeded();
        return true;
    } else if ( type == SIG_SCR_PRINT_INT ) {
        // auto dispatch to TTY mem
        ttyInt( x );
        // quit w/o increment act_cursor !!!
        _b_blittIfNeeded();
        return true;
    } else if ( type == SIG_SCR_PRINT_NUM ) {
        // auto dispatch to TTY mem
        ttyFloat( val );
        // quit w/o increment act_cursor !!!
        _b_blittIfNeeded();
        return true;
    } else if ( type == SIG_SCR_CLEAR ) {
        // type is already stored... for screen clear ...
        ttyCLS();
    } else if ( type == SIG_SCR_CURSOR ) {
        if ( x < 0 || x >= ttyWidth || y < 0 || y >= ttyHeight ) {
            // quit w/o increment act_cursor !!!
            return true;
        }
        tty_cursorX = x;
        tty_cursorY = y;
        // else line will be ignored !!!
        ttyFillLine( tty_cursorY );
        // tty_memseg[ (tty_cursorY * ttyWidth) + 0 ] = ' ';
        
        // quit w/o increment act_cursor !!!
        // _b_blittIfNeeded();
        return true;
    } else if ( type == SIG_SCR_MODE ) {
        // NOT Subject to buffer Op
        _b_doBlitt();
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
        __blittIfNeeded();
        return true;
    } else {
        // Oups
    }

    act_cursor++;
    _b_blittIfNeeded();

    return true;
 }

 // can be (x,y,w,h,m,c)
 //        (x,y,r,m,c)
 bool storeAction(uint8_t type, uint16_t x=0, uint16_t y=0, uint16_t w=0, uint16_t h=0, uint16_t sx=0, uint16_t sy=0, char* filename=NULL, float val=-1.0) {
     return storeAction(type, x,y,w,h, 0,0, sx, sy, filename, val);
 }

 bool storeAction(uint8_t type, float val) {
     return storeAction(type, 0, 0, 0, 0, 0, 0, NULL, val);
 }

 bool storeAction(uint8_t type, int val) {
     return storeAction(type, val, 0, 0, 0, 0, 0, NULL, -1.0f);
 }


 bool storeAction(uint8_t type, char ch) {
     return storeAction(type, ch, 0, 0, 0, 0, 0, NULL, -1.0f);
 }

 bool storeAction(uint8_t type, char* str) {
     return storeAction(type, 0, 0, 0, 0, 0, 0, str, -1.0f);
 }

 uint16_t ___readACTU16(int addr) {
     return (uint16_t) (( (uint16_t) act_memseg[addr+0] ) * 256) + act_memseg[addr+1];
 }

 char* ___readACTFILEN(int addr) {
     static char filename[13+1];
     memset(filename, 0x00, 13+1);
     char ch;
     for(int i=0; i < 13; i++) {
        ch = act_memseg[ addr+i ];
        if ( ch == 0x00 ) { break; }
        filename[i] = ch;
     }
     return filename;
 }

 void _b_doBlitt() {
    // DOES THE WORK !!!!

    static GenericMCU_SCREEN* screen = mcu.getScreen();

    // mcu.println("_b_doBlitt()");

    for(int curAct=0; curAct < act_cursor; curAct++) {

        // mcu.println("_b_doBlitt() action loop");

        uint16_t x,y,w,h,sx,sy,color;
        uint8_t mode;
        char* filename;

        int base_addr = (ACTION_REC_SIZE*curAct);
        int sub_addr = 1;
        uint8_t type = act_memseg[base_addr];

        switch( type ) {
            case SIG_SCR_DRAW_BPP:
               x = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               y = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               filename = ___readACTFILEN( base_addr+sub_addr );
               if ( strlen(filename) == 0 ) {
                screen->drawPictureBPP( (char*)NULL, x, y );
               } else {
                screen->drawPictureBPP( filename, x, y );
               }
              break;

            case SIG_SCR_DRAW_PCT:
               x = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               y = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               filename = ___readACTFILEN( base_addr+sub_addr );
               if ( strlen(filename) == 0 ) {
                screen->drawPicture565( (char*)NULL, x, y );
               } else {
                screen->drawPicture565( filename, x, y );
               }
              break;

            case SIG_SCR_DRAW_PCT_SPRITE:
               x  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               y  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               w  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               h  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               sx = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               sy = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               filename = ___readACTFILEN( base_addr+sub_addr );
               if ( strlen(filename) == 0 ) {
                screen->drawPicture565Sprite( (char*)NULL, x, y, w, h, sx, sy );
               } else {
                screen->drawPicture565Sprite( filename, x, y, w, h, sx, sy );
               }
              break;

            case SIG_SCR_DRAW_RECT:
               x  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               y  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               w  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               h  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               mode = act_memseg[ base_addr+sub_addr ]; sub_addr+=1;
               color = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               screen->drawRect( x, y, w, h, mode, color );
              break;
            case SIG_SCR_DRAW_TRIANGLE:
               x  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               y  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               w  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2; // X2
               h  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               sx  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2; // X3
               sy  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               mode = act_memseg[ base_addr+sub_addr ]; sub_addr+=1;
               color = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               screen->drawTriangle( x, y, w, h, sx, sy, mode, color );
              break;

            case SIG_SCR_DRAW_CIRCLE:
               x  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               y  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               w  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               mode = act_memseg[ base_addr+sub_addr ]; sub_addr+=1;
               color = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               screen->drawCircle( x, y, w, mode, color );
              break;

            case SIG_SCR_DRAW_SHAPE_BALL:
               x  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               y  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               w  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2; // radius
               mode = act_memseg[ base_addr+sub_addr ]; sub_addr+=1;
               color = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;

               screen->drawCircle( x, y, w+1, 1, 0 ); // fill black
               screen->drawCircle( x, y, w, 0, color ); // draw white
               screen->drawCircle( x+(w/2)-1, y+(w/2)-1, w/2, 1, color ); // fill white

              break;

            case SIG_SCR_DRAW_LINE:
               x  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               y  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               // x2 & y2
               w  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               h  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               color = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               screen->drawLine( x, y, w, h, color );
              break;
            case SIG_SCR_DRAW_PIX:
               x  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               y  = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               color = ___readACTU16( base_addr+sub_addr ); sub_addr+=2;
               screen->drawPixel( x, y, color );
              return;

            case SIG_SCR_CLEAR:
               screen->clear();
              break;
        }

    }

    if ( __textIsNotEmpty ) {
        _b_doBlittText();
    }

    act_cursor = 0;
 }

 int array_copy(char* dest, int off0, char* source, int off1, int len, char stopOnChar, char skipChar, int &start ) {
    char ch; bool yet = false; int cpt=0;
    for(int i=0; i < len; i++) {
        ch = source[off1+i];
        if ( ch == skipChar && ! yet) {
            continue;
        }
        if ( !yet ) {
            start = i;
        }
        yet = true;
        if ( ch == stopOnChar ) { return i; }
        dest[off0+cpt] = ch; cpt++;
    }
    return len;
 }

 // DON'T FORGET the ttyMem blitt
 void _b_doBlittText() {
     static char line[MAX_TTY_WIDTH+1];
     int start = 0;
     for(int i=0; i < ttyHeight; i++) {
         start = 0;
         // int howMany = array_copy(line, 0, tty_memseg, (i*MAX_TTY_WIDTH), ttyWidth, 0x00);
         int howMany = array_copy(line, 0, tty_memseg, (i*ttyWidth), ttyWidth, 0x00, 0xFF, start);
         line[howMany-start] = 0x00;
         // see if emty line ...?
         if ( howMany > 0 ) {
            mcu.getScreen()->setCursor(start, i);
            mcu.getScreen()->print( line );
         }
     }
 }

#endif