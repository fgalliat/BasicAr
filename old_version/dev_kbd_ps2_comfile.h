#ifndef ps2key_h_
#define ps2key_h_ 1

#ifndef keyb_h_
 #error("do not include this file directly")
#endif

/**
  PS2 keyboard w/ 'comfile technology' board
*/


// RX pin 7
#define kbd Serial3

#define CHINESE_LAYOUT 1

#ifdef CHINESE_LAYOUT
int chineseMapTableLen = 20;
char chineseMapTable[] {
  ']' , '[',
  '}' , '{',
  '[' , '@',
  '{' , '\'',
  '\'', ':',
  '"' , '*',
  '=' , '^',
  ':' , '+',
  '+' , '~',
  '-' , '-',
  '_' , '=',
  '@' , '"',
  '^' , '&',
  '&' , '\'',
  '*' , '(',
  '(' , ')',
  '|' , '}',
  '\\', ']',
  '`' , '|', // not labeled
  '~' , '\\', // not labeled
};

char chineseMap(char ch) {
  for (int i = 0; i < chineseMapTableLen; i++) {
    if ( ch == chineseMapTable[i * 2] ) {
      ch = chineseMapTable[(i * 2) + 1];
      break;
    }
  }
  return ch;
}
#endif




void setup_kbd() {
  kbd.begin(4800);
}




char seq[3];
bool isCtrl = false;
bool isAlt  = false;


void poll_kbd() {


  if ( kbd.available() > 0 ) {

    kbd.readBytes(seq, 3);
    int kc = (int)seq[2];
    bool printable = false;

    struct KeyEvent ke;
    ke.printableChar = 0x00;


    // Shift/Caps auto modifies kc
    
    if ( kc == 146 )      {
      //Serial.print("Ctrl-");
      isCtrl = true;
    }
    else if ( kc == 148 ) {
      //Serial.print("Alt-");
      isAlt = true;
    }

    else if ( kc == 154 ) {
      Serial.print("Ctrl-Right");
    }
    else if ( kc == 151 ) {
      Serial.print("Alt-Right");
    }

    else if ( kc == 144 ) {
      Serial.print("Tab");
    }

    else if ( kc == 8 ) {
      Serial.print("BckSpc");
    }
    else if ( kc == 134 ) {
      Serial.print("Del");
    }

    else if ( kc == 27 ) {
      //Serial.print("Esc");
      ke.vkc = KEY_VIRT_ESC;
    }
    else if ( kc == 130 ) {
      //Serial.print("Break");
      ke.vkc = KEY_VIRT_BREAK;
    }

    else if ( kc == 137 + 0 ) {
      Serial.print("Up");
    }
    else if ( kc == 137 + 1 ) {
      Serial.print("Left");
    }
    else if ( kc == 137 + 2 ) {
      Serial.print("Down");
    }
    else if ( kc == 137 + 3 ) {
      Serial.print("Right");
    }

    else if ( kc == 13 ) {
      printable = true;
      ke.printableChar = (char)kc; /*Serial.print("Enter");*/
    }

    // Function keys
    else if ( kc >= 241 && kc <= 241 + 12 ) {
      Serial.print("F");
      Serial.print( (int)( kc - 241 + 1 ) );
    }
    else {
      printable = true;
      //Serial.print( kc );
      char ch = (char)kc;

#ifdef CHINESE_LAYOUT
      if (!( ( ch >= 'a' && ch <= 'z' ) || ( ch >= 'A' && ch <= 'Z' ) || ( ch >= '0' && ch <= '9' ) )) {
        ch = chineseMap(ch);
      }
#endif

      //Serial.print( (int)ch );
      ke.printableChar = ch;
      
      if ( isCtrl && isAlt) { printable = false; ke.modifier = MOD_CTRL_ALT; }
      else if ( isCtrl )    { printable = false; ke.modifier = MOD_CTRL; }
      else if ( isAlt )     { printable = false; ke.modifier = MOD_ALT; }
      
      isAlt = false;
      isCtrl = false;
    }
    if (!printable) {
      Serial.println();
    }

    ke.printable = printable;
    onKeyReceived( ke );
  }

}

#endif
