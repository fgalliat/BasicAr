# BasicAr
Basic for Arduino'like MCUs (Teensy tested) (Xtase version)

**Initialy based on _Robin Edwards_ work (https://github.com/robinhedwards/ArduinoBASIC)**

Originally designed for **Teensy serie** (3.2 & 3.6 tested)

The hardware driver layout isn't finished & is provided as is (coded for the XtsUBasic board :><br/>
 https://hackaday.io/project/19223-xts-ubasic<br/>
 http://xtase.over-blog.com/2017/11/xts-ubasic-reborns-from-ashes.html)
![The XtsuBasic Computer boards](./docs/allBoards.jpg "The Xts-uBasic Computer...")

## Builtin hardware :
 * Teensy 3.6 180MHz 256KB ram MCU
 * Buzzer
 * SSD1306 OLED 128x64 I2C Display
 * HC-06 BT slave UART
 * HobbyTronics USB Keyboard UART interface
 * 3 LEDS
 * 3 push buttons
 * own-designed BUS to deal w/ peripheral cards

**Added comp.sh**<br/>
to compile on a g++ linux env. w/ ncurses+SDL2 installed,<br/>
so you can test BASIC directly on your computer (hardw support is still in progress...)<br/>
<br/>
Already have FS, Sound & VGAText emulated (see subComponent/RPIDisplay).
![The XtsuBasic Emulated](./docs/VideoCard/emulation/X11emul.png "The Xts-uBasic Emulated on X11...")
<br/>

**Added support Esp32+Oled module // XtsPocket console**<br/>
Running a small BASIC RayCaster..
![My new XtsuBasic Pocket Console](./docs/pocket/xtsPocket_gallery1.jpg "The Xts-uBasic Pocket Console...")



### Initial command set :
```
 AND,OR, NOT,MOD
 PRINT -----------------------> ADDED <?> shortcut
 CLS
 LIST,RUN
 GOTO,GOSUB,RETURN,
 REM, ------------------------> ADDED <'> shortcut
 STOP,CONT,PAUSE
 INPUT, INKEY$ // INKEY$ : non blocking key listening ....
 IF,THEN
 FOR,TO,STEP,NEXT
 VAL()
 INT(),RND() // int(1.5) -> rounds to 1 / rnd() -> returns between 0.00 & 1.00
 STR$()      // STR$(1) => "1" (allows string concat)
 LEN(),      // LEN("abc") => 3 
 RIGHT$(),MID$(),LEFT$(),
 POSITION, 
 LET,DIM,
 PIN, PINMODE,PINREAD,ANALOGRD
 SAVE,LOAD,NEW
 DIR, ----------------------> ADDED extension filter ex "*.BAS" TODO : FILES as DIR alias
 DELETE
```

### Extended command set :
```
' for REM
? for PRINT
MEM               : function to return free mem ammount ----------> <*> shortcut to display it
         
LOCATE <ROW>,<COL>

FS_SUPPORT for Teensy3.6 w/ SDCard
DIR/DIRARRAY      : DIRARRAY sends result to 'DIR$()' array variable
SAVE/LOAD/LLIST
CHAIN "random"    : will load & run program "RANDOM.BAS"

MUTE (toggle mute buzzer)
BEEP 100,2        : 1-48 produces some notes, else direct freq. is used, tones for '2' * 50msec
PLAY "ABC#"       : play some notes of OCTAVE_4
PLAYT5K & PLAYT53 : to play audio (beep) files

DRAWBPP           : display a 1bpp (B/W) picture to builtin screen
CIRCLE x,y,r      : draw a circle
LINE   x,y,x2,y2  : draw a line
PSET   x,y        : switch ON a pixel
PRESET x,y        : switch OFF a pixel
RECT x,y,w,h,c,m  : c : 0,1,2,3 (black,white,gray, darkGray) // m : 0,1 (draw,fill)
BLITT x           : 0 stop blitt / 1 trigger blitt / 2 auto blitt (default mode)
         
BTN(x)            : function get button state
LED [1..3], {0,1} : switch on/off a LED

CONSOLE <out>,<in>,<gfx> : set I/O + GFX main devices
ECHO {0;1}        : enable/disable console output

DELAY x           : wait x msec
SECS              : function that returns nb of seconds elapsed since boot
MILLIS            : function that returns nb of milli-seconds elapsed since boot

UPPER$(str)       : function that return UPPERCASE of str
STRING$(nbTimes, chr) : function that returns repeating string of nbTimes * CHR$(chr)
SPACE$(nbTimes)   : function that returns repeating string of nbTimes * CHR$(32)
ASC( str )        : returns ascii code of 1st char of str
INSTR( str, toFind ) : returns index of 'toFind' in 'str' (else 0)

ABS(x)            : return absolute value of number
COS(x)            : return cos( x in radians)
SIN(x)            : return sin( x in radians)
SQRT(x)           : return square-root of x
POW(x,y)          : return x pow y

EXEC "<extCmd>", "[<extArg>]" : to launch an extended command

BYE               : to reset the MCU

HALT              : to halt the MCU & optional connected RPI
```

**Builtin USB Keyboard support** (via HobbyTronic board) is now nearly complete !

**Board TTY VGAText support** is functional !<br/>
**Board MP3 DFPlayer support** is functional !

**Started computer enclosure**
![The XtsuBasic Computer](./docs/enclosure/xts_screen_mini.jpg "The Xts-uBasic Computer in its enclosure...")


(The code is a bit dirty @ this time, I know)<br/>
This work is still in progress ... (so see you @ anytime)
