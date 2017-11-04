#ifndef usbhtv1key_h_
#define usbhtv1key_h_ 1

#ifndef keyb_h_
 #error("do not include this file directly")
#endif

/**
  USB keyboard w/ 'hobytronic USBHOST-MINI v1' board
*/


// RX pin 7
#define kbd Serial3

// mini QWERTY usb Keyboard
//#define MINIKB_LAYOUT1 1

#ifdef MINIKB_LAYOUT1
// Fn '~' to have tilda
// Fn 'PAUSE' + by ctrl-C
// arrows by Fn
// no F8, F10 @ this time

static int mapTableLen = 4;
static char mapTable[] {
  '~' , '|',
  '"' , '@',
  '@' , '"',
  0x00, '~',
};

static char map(char ch) {
  for (int i = 0; i < mapTableLen; i++) {
    if ( ch == mapTable[i * 2] ) {
      ch = mapTable[(i * 2) + 1];
      break;
    }
  }
  return ch;
}
#endif




static void setup_kbd() {
  kbd.begin(9600);
}




static char seq[1];
static bool isCtrl = false;
static bool isAlt  = false;

static struct KeyEvent ke;

static void poll_kbd() {


  if ( kbd.available() > 0 ) {

    kbd.readBytes(seq, 1);
    int kc = (int)seq[0];
    bool printable = false;

#ifdef MINIKB_LAYOUT1
   if ( kc == 194 ) {
     kbd.readBytes(seq, 1);
     kc = (int)seq[0];
     if ( kc == 163 ) {
       kc = (int)'#'; // 35
     }
   } else if ( kc == 35 ) {
     kc = (int)'\\'; // 92
   } else if ( kc == 27 ) { // Esc / Fn
     //Serial.println( "mutillpe 1" );
     long t0=millis();
     while( kbd.available() == 0 ) {
      if (millis() - t0 > 150) { break; }
     }
     
     if ( kbd.available() > 0 ) {
      int subKc = kbd.read();
      //Serial.println( "mutillpe 2" );
           if ( subKc == 77 ) { kc = 137+3; } // right
      else if ( subKc == 75 ) { kc = 137+1; } // left
      else if ( subKc == 72 ) { kc = 137+0; } // Up
      else if ( subKc == 80 ) { kc = 137+2; } // Down
      
      else if ( subKc == 83 ) { kc = 134; } // DEL

      else if ( subKc == 49+10 ) { kc = 241+0; } // F1
      else if ( subKc == 50+10 ) { kc = 241+1; } // F2
      else if ( subKc == 51+10 ) { kc = 241+2; } // F3
      else if ( subKc == 52+10 ) { kc = 241+3; } // F4
      else if ( subKc == 53+10 ) { kc = 241+4; } // F5
      else if ( subKc == 54+10 ) { kc = 241+5; } // F6
      else if ( subKc == 71    ) { kc = 241+6; } // F7
      // else if ( subKc == 72 ) { kc = 241+7; } // F8 - DEAD - Fn Up
      else if ( subKc == 73    ) { kc = 241+8; } // F9
      //else if ( subKc == 48 ) { Serial.print("F10"); } // F10 - DEAD - *
      else if ( subKc == 87    ) { kc = 241+10; } // F11
      else if ( subKc == 88    ) { kc = 241+11; } // F12

      else if ( subKc == 0     ) { kc = 130; } // BREAK
      
      else { Serial.print( "fn combo " );Serial.println( subKc ); }
     }
   }
#endif


    //struct KeyEvent ke;
    ke.printableChar = 0x00;


    // Shift/Caps auto modifies kc
    
//    if ( kc == 146 )      {
//      Serial.print("Ctrl-");
//      isCtrl = true;
//    }
//    else if ( kc == 148 ) {
//      Serial.print("Alt-");
//      isAlt = true;
//    }

//    else if ( kc == 154 ) {
//      Serial.print("Ctrl-Right");
//    }
//    else if ( kc == 151 ) {
//      Serial.print("Alt-Right");
//    }

    /*else*/ if ( kc == 3 ) {
      Serial.print("Ctrl-C");
      ke.vkc = KEY_VIRT_CTRLC;
    }

    else if ( kc == 9 ) {
      Serial.print("Tab");
    }

    else if ( kc == 8 ) {
      Serial.print("BckSpc");
    }
    else if ( kc == 134 ) {
      Serial.print("Del");
    }

    else if ( kc == 27 ) {
      Serial.print("Esc");
      ke.vkc = KEY_VIRT_ESC;
    }
    else if ( kc == 130 ) {
      Serial.print("Break");
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

    else if ( kc == 10 ) { // not 13
      printable = true;
      ke.printableChar = (char)kc; /*Serial.print("Enter");*/
    }

    // Function keys
    else if ( kc >= 241 && kc <= 241 + 12 ) {
      Serial.print("F");
      Serial.print( (int)( kc - 241 + 1 ) );
    }

    // all other keys
    else {
      printable = true;
      //Serial.print( kc );
      char ch = (char)kc;

#ifdef MINIKB_LAYOUT1
      if (!( ( ch >= 'a' && ch <= 'z' ) || ( ch >= 'A' && ch <= 'Z' ) || ( ch >= '0' && ch <= '9' ) )) {
        ch = map(ch);
      }
#endif

      Serial.print( (int)ch );
      Serial.print( (char)ch );
      
      ke.printableChar = ch;
      
      // never happens w/ this board
      // if ( isCtrl && isAlt) { printable = false; ke.modifier = MOD_CTRL_ALT; }
      // else if ( isCtrl )    { printable = false; ke.modifier = MOD_CTRL; }
      // else if ( isAlt )     { printable = false; ke.modifier = MOD_ALT; }
      
      isAlt = false;
      isCtrl = false;
    }
    if (printable) {
      Serial.println();
    }

    ke.printable = printable;
    onKeyReceived( ke );
  }

}

#endif
